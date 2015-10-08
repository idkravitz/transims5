//*********************************************************
//	MPI_Processing.cpp - MPI data transfer
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Router::MPI_Processing (void)
{
#ifdef MPI_EXE
	int i, num, lvalue [MAX_PROBLEM], rank;

	if (Master ()) {
		Write (1);

		for (i=1; i < MPI_Size (); i++) {
			rank = Show_MPI_Message ();

			//---- retrieve the data buffer ----

			Get_MPI_Buffer (rank);
			if (mpi_buffer.Size () == 0) continue;

			//---- retrieve the plan statistics

			if (trip_flag) {
				mpi_buffer.Get_Data (lvalue, 8 * sizeof (int));

				if (trip_file->Part_Flag ()) {
					trip_file->Add_Files (lvalue [0]);
					trip_file->Add_Records (lvalue [1]);
					trip_file->Add_Households (lvalue [2]);
					trip_file->Add_Persons (lvalue [3]);
					trip_file->Add_Tours (lvalue [4]);
					trip_file->Add_Trips (lvalue [5]);
				}
				num_trip_rec = MAX (lvalue [6], num_trip_rec);
				num_trip_sel += lvalue [7];
			}
			if (plan_flag) {
				mpi_buffer.Get_Data (lvalue, 6 * sizeof (int));

				plan_file->Add_Files (lvalue [0]);
				plan_file->Add_Records (lvalue [1]);
				plan_file->Add_Households (lvalue [2]);
				plan_file->Add_Persons (lvalue [3]);
				plan_file->Add_Tours (lvalue [4]);
				plan_file->Add_Trips (lvalue [5]);
			}
			if (new_plan_flag) {
				mpi_buffer.Get_Data (lvalue, 6 * sizeof (int));

				new_plan_file->Add_Files (lvalue [0]);
				new_plan_file->Add_Records (lvalue [1]);
				new_plan_file->Add_Households (lvalue [2]);
				new_plan_file->Add_Persons (lvalue [3]);
				new_plan_file->Add_Tours (lvalue [4]);
				new_plan_file->Add_Trips (lvalue [5]);
			}
			if (problem_flag) {
				mpi_buffer.Get_Data (lvalue, 6 * sizeof (int));

				problem_file->Add_Files (lvalue [0]);
				problem_file->Add_Records (lvalue [1]);
				problem_file->Add_Households (lvalue [2]);
				problem_file->Add_Persons (lvalue [3]);
				problem_file->Add_Tours (lvalue [4]);
				problem_file->Add_Trips (lvalue [5]);
			}
			mpi_buffer.Get_Data (lvalue, MAX_PROBLEM * sizeof (int));
			Add_Problems (lvalue);

			mpi_buffer.Get_Data (&num, sizeof (num));
			total_records += num;
			
			mpi_buffer.Get_Data (&num, sizeof (num));
			num_time_updates += num;
		}
		if (trip_flag) trip_file->Close ();
		if (plan_flag) plan_file->Close ();
		if (new_plan_flag) new_plan_file->Close ();
		if (problem_flag) problem_file->Close ();
		Show_Message (1);

	} else {	//---- slave ----

		if (new_plan_flag) {
			if (Partition_Range ()) {
				Send_MPI_Message (String ("Slave %d Wrote %ss %d..%d") % MPI_Rank () % new_plan_file->File_Type () % First_Partition () % Last_Partition ());
			} else {
				Send_MPI_Message (String ("Slave %d Wrote %s %d") % MPI_Rank () % new_plan_file->File_Type () % First_Partition ());
			}
		} else {
			if (Partition_Range ()) {
				Send_MPI_Message (String ("Slave %d Processed Partitions %d..%d") % MPI_Rank () % First_Partition () % Last_Partition ());
			} else {
				Send_MPI_Message (String ("Slave %d Processed Partition %d") % MPI_Rank () % First_Partition ());
			}
		}

		//---- gather run statistics ---

		mpi_buffer.Size (0);

		if (trip_flag) {
			lvalue [0] = trip_file->Num_Files ();
			lvalue [1] = trip_file->Num_Records ();
			lvalue [2] = trip_file->Num_Households ();
			lvalue [3] = trip_file->Num_Persons ();
			lvalue [4] = trip_file->Num_Tours ();
			lvalue [5] = trip_file->Num_Trips ();
			lvalue [6] = num_trip_rec;
			lvalue [7] = num_trip_sel;
			trip_file->Close ();

			mpi_buffer.Add_Data (lvalue, 8 * sizeof (int));
		}
		if (plan_flag) {
			lvalue [0] = plan_file->Num_Files ();
			lvalue [1] = plan_file->Num_Records ();
			lvalue [2] = plan_file->Num_Households ();
			lvalue [3] = plan_file->Num_Persons ();
			lvalue [4] = plan_file->Num_Tours ();
			lvalue [5] = plan_file->Num_Trips ();
			plan_file->Close ();

			mpi_buffer.Add_Data (lvalue, 6 * sizeof (int));
		}
		if (new_plan_flag) {
			lvalue [0] = new_plan_file->Num_Files ();
			lvalue [1] = new_plan_file->Num_Records ();
			lvalue [2] = new_plan_file->Num_Households ();
			lvalue [3] = new_plan_file->Num_Persons ();
			lvalue [4] = new_plan_file->Num_Tours ();
			lvalue [5] = new_plan_file->Num_Trips ();
			new_plan_file->Close ();

			mpi_buffer.Add_Data (lvalue, 6 * sizeof (int));
		}
		if (problem_flag) {
			lvalue [0] = problem_file->Num_Files ();
			lvalue [1] = problem_file->Num_Records ();
			lvalue [2] = problem_file->Num_Households ();
			lvalue [3] = problem_file->Num_Persons ();
			lvalue [4] = problem_file->Num_Tours ();
			lvalue [5] = problem_file->Num_Trips ();
			problem_file->Close ();

			mpi_buffer.Add_Data (lvalue, 6 * sizeof (int));
		}
		mpi_buffer.Add_Data (Get_Problems (), MAX_PROBLEM * sizeof (int));
		mpi_buffer.Add_Data (&total_records, sizeof (total_records));
		mpi_buffer.Add_Data (&num_time_updates, sizeof (num_time_updates));

		//---- send the data buffer ----

		Send_MPI_Buffer ();

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
