//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void EventSum::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();
	
	select_flag = System_File_Flag (SELECTION);
	trip_file_flag = System_File_Flag (TRIP);
	new_select_flag = System_File_Flag (NEW_SELECTION);
	update_flag = System_File_Flag (PLAN) || System_File_Flag (NEW_PLAN);

	if (update_flag && (!System_File_Flag (PLAN) || !System_File_Flag (NEW_PLAN))) {
		Error ("Input and Output Plan Files are Required for Plan Updates");
	}
	num_inc = sum_periods.Num_Periods ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the compare travel time file ----

	key = Get_Control_String (TRAVEL_TIME_FILE);

	if (!key.empty ()) {
		if (Check_Control_Key (TRAVEL_TIME_FORMAT)) {
			time_in_file.Dbase_Format (Get_Control_String (TRAVEL_TIME_FORMAT));
		}
		time_in_file.Open (Project_Filename (key));
		time_in_flag = true;
	}

	//---- get the travel time file ----

	key = Get_Control_String (NEW_TRAVEL_TIME_FILE);

	if (!key.empty ()) {
		if (Check_Control_Key (NEW_TRAVEL_TIME_FORMAT)) {
			travel_time_file.Dbase_Format (Get_Control_String (NEW_TRAVEL_TIME_FORMAT));
		}
		travel_time_file.Create (Project_Filename (key));
		time_file_flag = true;
	}

	//---- get the time summary file ----

	key = Get_Control_String (NEW_TIME_SUMMARY_FILE);

	if (!key.empty ()) {
		time_sum_file.File_Type ("New Time Summary File");

		if (Check_Control_Key (NEW_TIME_SUMMARY_FORMAT)) {
			time_sum_file.Dbase_Format (Get_Control_String (NEW_TIME_SUMMARY_FORMAT));
		}
		time_sum_file.Create (Project_Filename (key));
		time_sum_flag = true;

		//---- get the minimum schedule variance ----

		min_variance = Get_Control_Time (MINIMUM_SCHEDULE_VARIANCE);
	}

	//---- get the link event file ----

	key = Get_Control_String (NEW_LINK_EVENT_FILE);

	if (!key.empty ()) {
		link_event_file.File_Type ("New Link Event File");

		if (Check_Control_Key (NEW_LINK_EVENT_FORMAT)) {
			link_event_file.Dbase_Format (Get_Control_String (NEW_LINK_EVENT_FORMAT));
		}
		link_event_file.Create (Project_Filename (key));
		link_event_flag = true;
	}

	//---- new time distribution file ----

	time_diff.Open_Distribution (Get_Control_String (NEW_TIME_DISTRIBUTION_FILE));

	//---- new trip time gap file ----

	time_gap.Open_Trip_Gap_File (Get_Control_String (NEW_TRIP_TIME_GAP_FILE));

	//---- read report types ----

	List_Reports ();

	//---- set report flags ----

	time_flag = time_diff.Report_Flags (Report_Flag (TOTAL_TIME), Report_Flag (PERIOD_TIME), 
										Report_Flag (TOT_TIME_SUM), Report_Flag (PER_TIME_SUM));

	if (time_diff.Output_Flag () || Report_Flag (PERIOD_TIME) || Report_Flag (PER_TIME_SUM)) {
		time_diff.Set_Periods (sum_periods);
		time_flag = time_diff_flag = true;
	}
	if (time_gap.Output_Flag () || Report_Flag (TRP_TIME_GAP)) {
		time_gap.Report_Flags (Report_Flag (TRP_TIME_GAP));
		time_gap.Set_Periods (sum_periods);
		time_gap_flag = true;
	}

	//---- set the compare flag ----

	compare_flag = (time_in_flag || trip_file_flag);
} 
