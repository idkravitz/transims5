//*********************************************************
//	MPI_Link_Delay.cpp - MPI link data transfer
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	MPI_Link_Delay
//---------------------------------------------------------

double Router::MPI_Link_Delay (bool last_flag)
{
	double gap = 1.0;
	if (last_flag) gap = 1.0;

#ifdef MPI_EXE
	int i, num, rank, num_link, num_turn, periods, link_size, turn_size;
	
	Flow_Time_Data *data_ptr, data_rec;
	Flow_Time_Itr data_itr;
	Flow_Time_Period_Itr period_itr;

	num_link = link_delay_array.Num_Records ();
	link_size = num_link * sizeof (data_rec);

	num_turn = (int) connect_array.size ();
	turn_size = num_turn * sizeof (data_rec);
	
	periods = link_delay_array.periods->Num_Periods ();
			
	if (!flow_flag || periods == 0 || num_link == 0) return (gap);

	if (link_flow_array.size () == 0) {
		link_flow_array.assign (num_link, data_rec);
	}
	if (turn_flag && num_turn > 0 && turn_flow_array.size () == 0) {
		turn_flow_array.assign (num_turn, data_rec);
	}

	//---- gather flows and distribute travel times ----

	if (Master ()) {

		for (i=1; i < MPI_Size (); i++) {
			rank = Get_MPI_Array (&rank, 1);

			//---- combine link flows ----

			for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {

				//---- retrieve the data buffer ----

				Get_MPI_Buffer (rank);
				if (mpi_buffer.Size () == 0) continue;

				data_ptr = link_flow_array.Data_Ptr (0);

				mpi_buffer.Get_Data (data_ptr, link_size);

				//---- add the volume data ----

				for (num=0, data_itr = link_flow_array.begin (); data_itr != link_flow_array.end (); data_itr++, num++) {
					if (data_itr->Flow () > 0.0) {
						data_ptr = period_itr->Data_Ptr (num);
						data_ptr->Add_Flow (data_itr->Flow ());
					}
				}
			}

			//---- combine turning movements ----

			if (turn_flag && num_turn > 0) {
				for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {

					//---- retrieve the data buffer ----

					Get_MPI_Buffer (rank);
					if (mpi_buffer.Size () == 0) continue;

					data_ptr = turn_flow_array.Data_Ptr (0);

					mpi_buffer.Get_Data (data_ptr, turn_size);

					//---- add the turning movements ----

					for (num=0, data_itr = turn_flow_array.begin (); data_itr != turn_flow_array.end (); data_itr++, num++) {
						if (data_itr->Flow () > 0.0) {
							data_ptr = period_itr->Data_Ptr (num);
							data_ptr->Add_Flow (data_itr->Flow ());
						}
					}
				}
			}
		}

		//--- update the travel times ----

		if (Time_Updates ()) {
			Update_Travel_Times ();
			gap = Merge_Delay (factor, !last_flag);	
		}

		//---- send the convergence result ----

		mpi_buffer.Data (&gap, sizeof (double));

		for (i=1; i < MPI_Size (); i++) {
			Send_MPI_Buffer (i);
		}

		//---- send the updated travel times to each slave ----
	
		if (Time_Updates () && !last_flag) {

			//---- send the link flows ----

			for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {

				mpi_buffer.Data (period_itr->Data_Ptr (0), link_size);

				for (i=1; i < MPI_Size (); i++) {
					Send_MPI_Buffer (i);
				}
			}

			//---- send the turning movements ----

			if (turn_flag && num_turn > 0) {
				for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {

					mpi_buffer.Data (period_itr->Data_Ptr (0), turn_size);

					for (i=1; i < MPI_Size (); i++) {
						Send_MPI_Buffer (i);
					}
				}
			}
		}

	} else {	//---- slave ----

		rank = MPI_Rank ();

		Send_MPI_Array (&rank, 1);

		//---- send the link flows ----

		for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {

			mpi_buffer.Data (period_itr->Data_Ptr (0), link_size);

			Send_MPI_Buffer ();
		}

		//---- send the turning movements ----

		if (turn_flag && num_turn > 0) {
			for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {

				mpi_buffer.Data (period_itr->Data_Ptr (0), turn_size);

				Send_MPI_Buffer ();
			}
		}

		//---- retrieve the convergence result ----

		Get_MPI_Buffer ();
		mpi_buffer.Get_Data (&gap, sizeof (double));

		//---- get the updated travel times ----

		if (Time_Updates () && !last_flag) {

			for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {

				//---- retrieve the data buffer ----

				Get_MPI_Buffer ();
				if (mpi_buffer.Size () == 0) continue;

				mpi_buffer.Get_Data (period_itr->Data_Ptr (0), link_size);
			}

			//---- update turn penalties ----

			if (turn_flag && num_turn > 0) {

				for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {

					//---- retrieve the data buffer ----

					Get_MPI_Buffer ();
					if (mpi_buffer.Size () == 0) continue;

					mpi_buffer.Get_Data (period_itr->Data_Ptr (0), turn_size);
				}
			}
		}
	}
#endif
	return (gap);
}
