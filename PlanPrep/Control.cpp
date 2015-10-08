//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanPrep::Program_Control (void)
{
	String key;

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	plan_file = (Plan_File *) System_File_Handle (PLAN);
	if (!plan_file->Part_Flag ()) Num_Threads (1);

	if (System_File_Flag (NEW_PLAN)) {
		new_plan_file = (Plan_File *) System_File_Handle (NEW_PLAN);
		if (Trip_Sort () == UNKNOWN_SORT) {
			new_plan_file->Sort_Type (plan_file->Sort_Type ());
		} else {
			new_plan_file->Sort_Type (Trip_Sort ());
		}
		new_plan_file->Close ();
		new_plan_flag = true;

		if (plan_file->Part_Flag () && !new_plan_file->Part_Flag ()) {
			pathname = new_plan_file->Pathname ();
			new_format = new_plan_file->Dbase_Format ();

			if (MPI_Size () > 1 && Master ()) {
				pathname += ".0";
			}
			if (Trip_Sort () == TRAVELER_SORT || Trip_Sort () == TIME_SORT) {
				combine_flag = true;
				new_plan_file->File_Type ("Temporary File");
				new_plan_file->Dbase_Format (BINARY);
				new_plan_file->Part_Flag (true);
				new_plan_file->Pathname (pathname + ".temp");
			} else {
				output_flag = true;
				new_plan_file->Pathname (pathname);
				if (MPI_Size () > 1) {
					new_plan_file->Dbase_Format (BINARY);
				}
				Num_Threads (1);
			}
		}
	}
	select_flag = System_File_Flag (SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- merge plan file ----

	key = Get_Control_Text (MERGE_PLAN_FILE);

	if (!key.empty ()) {
		merge_file.File_Type ("Merge Plan File");
		merge_file.Filename (Project_Filename (key));
		merge_flag = true;

		key = Get_Control_String (MERGE_PLAN_FORMAT);
		if (!key.empty ()) {
			merge_file.Dbase_Format (key);
		}
		merge_file.Open (0);

		if (Num_Threads () > 1 && !merge_file.Part_Flag ()) {
			Error ("Partitioned Input Plans require Partitioned Merge Plans");
		}
	}

	//---- maximum sort size ----

	if (Trip_Sort () == TRAVELER_SORT || Trip_Sort () == TIME_SORT) {
		sort_size = Get_Control_Integer (MAXIMUM_SORT_SIZE);
	}
}
