//*********************************************************
//	MPI_Processing.cpp - MPI data transfer
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void PlanPrep::MPI_Processing (void)
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

			//---- retrieve the plan statistics

			mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

			plan_file->Add_Files (lvalue [0]);
			plan_file->Add_Records (lvalue [1]);
			plan_file->Add_Households (lvalue [2]);
			plan_file->Add_Persons (lvalue [3]);
			plan_file->Add_Tours (lvalue [4]);
			plan_file->Add_Trips (lvalue [5]);

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

			//---- retrieve the new plan statistics

			if (new_plan_flag) {
				mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

				new_plan_file->Add_Files (lvalue [0]);
				new_plan_file->Add_Records (lvalue [1]);
				new_plan_file->Add_Households (lvalue [2]);
				new_plan_file->Add_Persons (lvalue [3]);
				new_plan_file->Add_Tours (lvalue [4]);
				new_plan_file->Add_Trips (lvalue [5]);
			}
		}
		Show_Message (1);

		plan_file->Close ();
		if (merge_flag) merge_file.Close ();
		if (new_plan_flag) new_plan_file->Close ();

		if (combine_flag || output_flag) {
			Combine_Plans (true);
		}

	} else {	//---- slave ----

		if (Partition_Range ()) {
			Send_MPI_Message (String ("Slave %d Read %ss %d..%d") % MPI_Rank () % plan_file->File_Type () % First_Partition () % Last_Partition ());
		} else {
			Send_MPI_Message (String ("Slave %d Read %s %d") % MPI_Rank () % plan_file->File_Type () % First_Partition ());
		}

		//---- send plan statistics ----

		lvalue [0] = plan_file->Num_Files ();
		lvalue [1] = plan_file->Num_Records ();
		lvalue [2] = plan_file->Num_Households ();
		lvalue [3] = plan_file->Num_Persons ();
		lvalue [4] = plan_file->Num_Tours ();
		lvalue [5] = plan_file->Num_Trips ();
		plan_file->Close ();

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

		//---- send new plan statistics ----

		if (new_plan_flag) {
			lvalue [0] = new_plan_file->Num_Files ();
			lvalue [1] = new_plan_file->Num_Records ();
			lvalue [2] = new_plan_file->Num_Households ();
			lvalue [3] = new_plan_file->Num_Persons ();
			lvalue [4] = new_plan_file->Num_Tours ();
			lvalue [5] = new_plan_file->Num_Trips ();
			new_plan_file->Close ();

			mpi_buffer.Add_Data (lvalue, sizeof (lvalue));
		}

		//---- send the data buffer ----

		Send_MPI_Buffer ();

		Exit_Stat (DONE);
	}
#endif
}
