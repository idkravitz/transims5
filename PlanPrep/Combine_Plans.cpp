//*********************************************************
//	Combine_Plans.cpp - combine temporary plans to output plans
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Combine_Plans
//---------------------------------------------------------

void PlanPrep::Combine_Plans (bool mpi_flag)
{
	int num_temp;

	//---- initialize the file handles ----

	new_plan_file->File_Type ("New Plan File");
	new_plan_file->Clear_Fields ();
	new_plan_file->Reset_Counters ();

	if (MPI_Size () > 1 && !mpi_flag) {
		new_plan_file->Dbase_Format (BINARY);
	} else {
		new_plan_file->Dbase_Format ((Format_Type) new_format);
		if (mpi_flag) {
			pathname.erase (pathname.size () - 2);
		}
	}
	new_plan_file->Part_Flag (false);
	new_plan_file->Pathname (pathname);
	new_plan_file->First_Open (false);

	new_plan_file->Open (0);

	Plan_File temp_file;

	temp_file.Part_Flag (true);
	if (mpi_flag) {
		temp_file.Pathname (pathname);
	} else {
		temp_file.Pathname (pathname + ".temp");
	}
	temp_file.First_Open (false);

	num_temp = temp_file.Num_Parts ();

	if (num_temp == 0) {
		Error ("No Plan Files to Combine");
	}

	//---- process the plan files ----

	if (Trip_Sort () == TIME_SORT) {
		Time_Combine (&temp_file, num_temp);
	} else {
		Trip_Combine (&temp_file, num_temp);
	}
	new_plan_file->Close ();
}
