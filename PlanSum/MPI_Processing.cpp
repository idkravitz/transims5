//*********************************************************
//	MPI_Processing.cpp - MPI data transfer
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void PlanSum::MPI_Processing (void)
{
#ifdef MPI_EXE
	int i, p, num, lvalue [6], periods, num_link, num_turn, link_size, turn_size, period_size, rank;
	bool extend_flag = false;

	Flow_Time_Data *data_ptr, data_rec;
	Flow_Time_Itr data_itr;
	Flow_Time_Array link_flow_array, turn_flow_array;
	Flow_Time_Period_Itr period_itr;

	if (new_delay_flag) {
		if (System_File_Flag (NEW_LINK_DELAY)) {
			Link_Delay_File *new_file = (Link_Delay_File *) System_File_Base (NEW_LINK_DELAY);
			extend_flag = (!new_file->Extend_Flag () && !new_file->Part_Flag ());
		} else {
			extend_flag = true;
		}
	}
	num_link = link_delay_array.Num_Records ();
	link_size = num_link * sizeof (data_rec);

	periods = sum_periods.Num_Periods ();
	period_size = periods * sizeof (double);

	if (turn_flag) {
		num_turn = (int) connect_array.size ();
		turn_size = num_turn * sizeof (data_rec);
	} else {
		num_turn = turn_size = 0;
	}

	if (Master ()) {
		Write (1);
		if (extend_flag) {
			if (num_link) {
				link_flow_array.assign (num_link, data_rec);
			}
			if (num_turn) {
				turn_flow_array.assign (num_turn, data_rec);
			}
		}

		for (i=1; i < MPI_Size (); i++) {
			rank = Show_MPI_Message ();

			//---- retrieve the data buffer ----

			Get_MPI_Buffer (rank);
			if (mpi_buffer.Size () == 0) continue;

			//---- retrieve the plan statistics

			mpi_buffer.Get_Data (lvalue, sizeof (lvalue));
			
			plan_file->Add_Files (lvalue [0]);
			plan_file->Add_Records (lvalue [1]);
			plan_file->Add_Households (lvalue [2]);
			plan_file->Add_Persons (lvalue [3]);
			plan_file->Add_Tours (lvalue [4]);
			plan_file->Add_Trips (lvalue [5]);

			//---- add the trip time data ----

			if (periods > 0 && time_flag) {
				Doubles temp;
				temp.assign (periods, 0);
				double *ptr = &temp [0];

				mpi_buffer.Get_Data (ptr, period_size);				

				for (p=0; p < periods; p++) {
					start_time [p] += ptr [p];
				}

				mpi_buffer.Get_Data (ptr, period_size);

				for (p=0; p < periods; p++) {
					mid_time [p] += ptr [p];
				}

				mpi_buffer.Get_Data (ptr, period_size);

				for (p=0; p < periods; p++) {
					end_time [p] += ptr [p];
				}
			}

			//---- combine summary report data ----

			if (travel_flag) {
				Trip_Sum_Data temp;
				temp.Replicate (trip_sum_data);

				temp.UnPack (mpi_buffer);
				trip_sum_data.Merge_Data (temp);
			}
			if (passenger_flag) {
				Trip_Sum_Data temp;
				temp.Replicate (pass_sum_data);

				temp.UnPack (mpi_buffer);
				pass_sum_data.Merge_Data (temp);
			}
				
			//---- add the volume data ----

			if (extend_flag && num_link > 0) {
				for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {
					Get_MPI_Buffer (rank);
					if (mpi_buffer.Size () == 0) continue;
					
					data_ptr = link_flow_array.Data_Ptr (0);
					mpi_buffer.Get_Data (data_ptr, link_size);

					for (num=0, data_itr = link_flow_array.begin (); data_itr != link_flow_array.end (); data_itr++, num++) {
						data_ptr = period_itr->Data_Ptr (num);
						data_ptr->Add_Flow (data_itr->Flow ());
					}
				}
			}
			if (extend_flag && num_turn > 0) {
				for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {
					Get_MPI_Buffer (rank);
					if (mpi_buffer.Size () == 0) continue;
					
					data_ptr = turn_flow_array.Data_Ptr (0);
					mpi_buffer.Get_Data (data_ptr, turn_size);

					for (num=0, data_itr = turn_flow_array.begin (); data_itr != turn_flow_array.end (); data_itr++, num++) {
						data_ptr = period_itr->Data_Ptr (num);
						data_ptr->Add_Flow (data_itr->Flow ());
					}
				}
			}
		}
		Show_Message (1);

	} else {	//---- slave ----

		if (Partition_Range ()) {
			Send_MPI_Message (String ("Slave %d Read %ss %d..%d") % MPI_Rank () % plan_file->File_Type () % First_Partition () % Last_Partition ());
		} else {
			Send_MPI_Message (String ("Slave %d Read %s %d") % MPI_Rank () % plan_file->File_Type () % First_Partition ());
		}
		lvalue [0] = plan_file->Num_Files ();
		lvalue [1] = plan_file->Num_Records ();
		lvalue [2] = plan_file->Num_Households ();
		lvalue [3] = plan_file->Num_Persons ();
		lvalue [4] = plan_file->Num_Tours ();
		lvalue [5] = plan_file->Num_Trips ();

		mpi_buffer.Data (lvalue, sizeof (lvalue));

		//---- transfer the trip time data ----

		if (periods > 0 && time_flag) {
			mpi_buffer.Add_Data (&start_time [0], period_size);
			mpi_buffer.Add_Data (&mid_time [0], period_size);
			mpi_buffer.Add_Data (&end_time [0], period_size);
		}

		//---- combine summary report data ----

		if (travel_flag) {
			trip_sum_data.Pack (mpi_buffer);
		}
		if (passenger_flag) {
			pass_sum_data.Pack (mpi_buffer);
		}

		//---- send the data buffer ----

		Send_MPI_Buffer ();

		//---- transfer the volume data ----

		if (extend_flag && num_link > 0) {
			for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {
				mpi_buffer.Data (period_itr->Data_Ptr (0), link_size);

				Send_MPI_Buffer ();
			}
		}
		if (extend_flag && num_turn > 0) {
			for (period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++) {
				mpi_buffer.Data (period_itr->Data_Ptr (0), turn_size);

				Send_MPI_Buffer ();
			}
		}

		//---- remove the link delay file ----

		if (System_File_Flag (NEW_LINK_DELAY)) {
			Link_Delay_File *new_file = (Link_Delay_File *) System_File_Base (NEW_LINK_DELAY);
				
			new_file->Close ();
			string message = new_file->Filename ();
			remove (message.c_str ());
			message += ".def";
			remove (message.c_str ());
		}
		Exit_Stat (DONE);
	}
#endif
}
