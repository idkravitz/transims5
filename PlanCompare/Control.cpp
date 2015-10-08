//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanCompare::Program_Control (void)
{
	String key;
	
	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	plan_file = (Plan_File *) System_File_Handle (PLAN);
	if (!plan_file->Part_Flag ()) Num_Threads (1);
	time_sort = plan_file->Time_Sort ();

	new_plan_flag = System_File_Flag (NEW_PLAN);
	if (new_plan_flag) {
		new_plan_file = (Plan_File *) System_File_Handle (NEW_PLAN);
		new_plan_file->Sort_Type (plan_file->Sort_Type ());
		new_plan_file->Close ();

		if (Num_Threads () > 1 && !new_plan_file->Part_Flag ()) {
			Error ("Partitioned Input Plans require Partitioned Output Plans");
		}
	}
	select_flag = System_File_Flag (SELECTION);
	if (plan_file->Part_Flag () && System_File_Flag (NEW_SELECTION)) {
		Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);
		
		if (!file->Part_Flag ()) {
			if (Master ()) {
				file->Clear_Fields ();
				file->Partition_Flag (true);
				file->Create_Fields ();
				file->Write_Header ();
			} else {

				//---- remove the new selection file ----
				
				file->Close ();
				key = file->Filename ();
				remove (key.c_str ());
				key += ".def";
				remove (key.c_str ());
			}
		} else {
			select_parts = true;
		}
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- compare plan file ----

	key = Get_Control_String (COMPARE_PLAN_FILE);

	if (!key.empty ()) {
		compare_file.File_Type ("Compare Plan File");
		compare_file.Filename (Project_Filename (key));

		key = Get_Control_String (COMPARE_PLAN_FORMAT);
		if (!key.empty ()) {
			compare_file.Dbase_Format (key);
		}
		compare_file.Open (0);

		if (Num_Threads () > 1 && !compare_file.Part_Flag ()) {
			Error ("Partitioned Input Plans require Partitioned Compare Plans");
		}
		if ((time_sort && !compare_file.Time_Sort ()) ||
			(!time_sort && compare_file.Time_Sort ())) {
			Error ("Plan and Compare Plan Files do not have the Same Sort");
		}
	}
	Print (1);

	//---- compare generalized costs ----

	cost_flag = Get_Control_Flag (COMPARE_GENERALIZED_COSTS);

	//---- selection method ----

	method = Select_Method_Code (Get_Control_Text (SELECTION_METHOD));

	//---- merge flag ----

	merge_flag = Get_Control_Flag (MERGE_PLAN_FILES);

	//---- new time distribution file ----

	time_diff.Open_Distribution (Get_Control_String (NEW_TIME_DISTRIBUTION_FILE));

	//---- new cost distribution file ----

	cost_diff.Open_Distribution (Get_Control_String (NEW_COST_DISTRIBUTION_FILE));

	//---- new trip time gap file ----

	time_gap.Open_Trip_Gap_File (Get_Control_String (NEW_TRIP_TIME_GAP_FILE));

	//---- new trip cost gap file ----

	cost_gap.Open_Trip_Gap_File (Get_Control_String (NEW_TRIP_COST_GAP_FILE));

	//---- new trip match file ----

	key = Get_Control_String (NEW_TRIP_MATCH_FILE);

	if (!key.empty ()) {
		Print (1);
		match_file.File_Type ("New Trip Match File");
		match_file.Filename (Project_Filename (key));
		match_file.File_Access (CREATE);
		if (!match_file.Part_Flag () && plan_file->Part_Flag ()) {
			match_file.Partition_Flag (true);
		}
		key = Get_Control_String (NEW_TRIP_MATCH_FORMAT);
		if (!key.empty ()) {
			match_file.Dbase_Format (key);
		}
		match_file.Open (0);
		match_flag = true;
	}

	//---- write the report names ----

	List_Reports ();

	//---- set report flags ----

	time_sum_flag = time_diff.Report_Flags (Report_Flag (TOTAL_TIME), Report_Flag (PERIOD_TIME), 
											Report_Flag (TOT_TIME_SUM), Report_Flag (PER_TIME_SUM));

	cost_sum_flag = cost_diff.Report_Flags (Report_Flag (TOTAL_COST), Report_Flag (PERIOD_COST), 
											Report_Flag (TOT_COST_SUM), Report_Flag (PER_COST_SUM));

	if (time_diff.Output_Flag () || Report_Flag (PERIOD_TIME) || Report_Flag (PER_TIME_SUM)) {
		time_diff.Set_Periods (sum_periods);
		time_sum_flag = true;
	}
	if (cost_diff.Output_Flag () || Report_Flag (PERIOD_COST) || Report_Flag (PER_COST_SUM)) {
		cost_diff.Set_Periods (sum_periods);
		cost_sum_flag = true;
	}
	if (time_gap.Output_Flag () || Report_Flag (TRP_TIME_GAP)) {
		time_gap.Report_Flags (Report_Flag (TRP_TIME_GAP));
		time_gap.Set_Periods (sum_periods);
		time_gap_flag = true;
	}
	if (cost_gap.Output_Flag () || Report_Flag (TRP_COST_GAP)) {
		cost_gap.Report_Flags (Report_Flag (TRP_COST_GAP));
		cost_gap.Set_Periods (sum_periods);
		cost_gap_flag = true;
	}
}
