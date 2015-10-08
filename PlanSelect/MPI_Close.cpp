//*********************************************************
//	MPI_Close.cpp - MPI close files
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	MPI_Close
//---------------------------------------------------------

void PlanSelect::MPI_Close (void)
{
#ifdef MPI_EXE
	int i, lvalue [6], rank;

	if (Master ()) {
		if (new_plan_flag) {
			Show_Message (1);

			for (i=1; i < MPI_Size (); i++) {

				rank = Show_MPI_Message ();

				//---- retrieve the new plan statistics

				Get_MPI_Array (lvalue, 6, rank);

				new_plan_file->Add_Files (lvalue [0]);
				new_plan_file->Add_Records (lvalue [1]);
				new_plan_file->Add_Households (lvalue [2]);
				new_plan_file->Add_Persons (lvalue [3]);
				new_plan_file->Add_Tours (lvalue [4]);
				new_plan_file->Add_Trips (lvalue [5]);
			}
			new_plan_file->Close ();
		}

		//---- write the new selection file ----

		if (select_parts) {
			Show_Message (1);
			Write_Selections ();

			for (i=1; i < MPI_Size (); i++) {
				Show_MPI_Message ();
			}
		}
		Show_Message (1);

	} else {	//---- slave ----

		if (new_plan_flag) {
			if (Partition_Range ()) {
				Send_MPI_Message (String ("Slave %d Wrote %ss %d..%d") % MPI_Rank () % new_plan_file->File_Type () % First_Partition () % Last_Partition ());
			} else {
				Send_MPI_Message (String ("Slave %d Wrote %s %d") % MPI_Rank () % new_plan_file->File_Type () % First_Partition ());
			}

			//---- send new plan statistics ----

			lvalue [0] = new_plan_file->Num_Files ();
			lvalue [1] = new_plan_file->Num_Records ();
			lvalue [2] = new_plan_file->Num_Households ();
			lvalue [3] = new_plan_file->Num_Persons ();
			lvalue [4] = new_plan_file->Num_Tours ();
			lvalue [5] = new_plan_file->Num_Trips ();
			new_plan_file->Close ();

			Send_MPI_Array (lvalue, 6);
		}

		//---- write the new selection file ----

		if (select_parts) {
			Write_Selections ();

			Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);

			if (Partition_Range ()) {
				Send_MPI_Message (String ("Slave %d Wrote %ss %d..%d") % MPI_Rank () % file->File_Type () % First_Partition () % Last_Partition ());
			} else {
				Send_MPI_Message (String ("Slave %d Wrote %s %d") % MPI_Rank () % file->File_Type () % First_Partition ());
			}
		}
		Exit_Stat (DONE);
	}
#endif
}
