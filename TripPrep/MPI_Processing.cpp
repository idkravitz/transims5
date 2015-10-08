//*********************************************************
//	MPI_Processing.cpp - MPI data transfer
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void TripPrep::MPI_Processing (void)
{
#ifdef MPI_EXE
	int i, lvalue [6], rank;

	if (Master ()) {
		Write (1);

		for (i=1; i < MPI_Size (); i++) {

			rank = Show_MPI_Message ();

			//---- retrieve the data buffer ----

			Get_MPI_Buffer (rank);
			
			if (mpi_buffer.Size () == 0) continue;

			//---- retrieve the trip statistics

			mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

			trip_file->Add_Files (lvalue [0]);
			trip_file->Add_Records (lvalue [1]);
			trip_file->Add_Households (lvalue [2]);
			trip_file->Add_Persons (lvalue [3]);
			trip_file->Add_Tours (lvalue [4]);
			trip_file->Add_Trips (lvalue [5]);

			//---- retrieve the merge statistics

			if (merge_flag) {
				mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

				merge_file.Add_Files (lvalue [0]);
				merge_file.Add_Records (lvalue [1]);
				merge_file.Add_Households (lvalue [2]);
				merge_file.Add_Persons (lvalue [3]);
				merge_file.Add_Tours (lvalue [4]);
				merge_file.Add_Trips (lvalue [5]);
			}

			//---- retrieve the new trip statistics

			if (new_trip_flag) {
				mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

				new_trip_file->Add_Files (lvalue [0]);
				new_trip_file->Add_Records (lvalue [1]);
				new_trip_file->Add_Households (lvalue [2]);
				new_trip_file->Add_Persons (lvalue [3]);
				new_trip_file->Add_Tours (lvalue [4]);
				new_trip_file->Add_Trips (lvalue [5]);
			}
		}
		Show_Message (1);

		trip_file->Close ();
		if (merge_flag) merge_file.Close ();
		if (new_trip_flag) new_trip_file->Close ();

		if (combine_flag || output_flag) {
			Combine_Trips (true);
		}

	} else {	//---- slave ----

		if (Partition_Range ()) {
			Send_MPI_Message (String ("Slave %d Read %ss %d..%d") % MPI_Rank () % trip_file->File_Type () % First_Partition () % Last_Partition ());
		} else {
			Send_MPI_Message (String ("Slave %d Read %s %d") % MPI_Rank () % trip_file->File_Type () % First_Partition ());
		}

		//---- send trip statistics ----

		lvalue [0] = trip_file->Num_Files ();
		lvalue [1] = trip_file->Num_Records ();
		lvalue [2] = trip_file->Num_Households ();
		lvalue [3] = trip_file->Num_Persons ();
		lvalue [4] = trip_file->Num_Tours ();
		lvalue [5] = trip_file->Num_Trips ();
		trip_file->Close ();

		mpi_buffer.Data (lvalue, sizeof (lvalue));

		//---- send merge statistics ----

		if (merge_flag) {
			lvalue [0] = merge_file.Num_Files ();
			lvalue [1] = merge_file.Num_Records ();
			lvalue [2] = merge_file.Num_Households ();
			lvalue [3] = merge_file.Num_Persons ();
			lvalue [4] = merge_file.Num_Tours ();
			lvalue [5] = merge_file.Num_Trips ();
			merge_file.Close ();

			mpi_buffer.Add_Data (lvalue, sizeof (lvalue));
		}

		//---- send new trip statistics ----

		if (new_trip_flag) {
			lvalue [0] = new_trip_file->Num_Files ();
			lvalue [1] = new_trip_file->Num_Records ();
			lvalue [2] = new_trip_file->Num_Households ();
			lvalue [3] = new_trip_file->Num_Persons ();
			lvalue [4] = new_trip_file->Num_Tours ();
			lvalue [5] = new_trip_file->Num_Trips ();
			new_trip_file->Close ();

			mpi_buffer.Add_Data (lvalue, sizeof (lvalue));
		}

		//---- send the data buffer ----

		Send_MPI_Buffer ();

		Exit_Stat (DONE);
	}
#endif
}
