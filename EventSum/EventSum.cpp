//*********************************************************
//	EventSum.cpp - Summarize Event File Data
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	EventSum constructor
//---------------------------------------------------------

EventSum::EventSum (void) : Data_Service (), Select_Service ()
{
	Program ("EventSum");
	Version (0);
	Title ("Summarize Event File Data");

	System_File_Type required_files [] = {
		EVENT, END_FILE
	};
	System_File_Type optional_files [] = {
		NODE, LINK, LOCATION, SELECTION, TRIP, PLAN, NEW_SELECTION, NEW_PLAN, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, TIME_EQUIVALENCE_FILE, ZONE_EQUIVALENCE_FILE, ZONE_LOCATION_MAP_FILE, 0
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, PERIOD_CONTROL_POINT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_START_TIMES, SELECT_END_TIMES, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, 0
	};
	Control_Key eventsum_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TRAVEL_TIME_FILE, "TRAVEL_TIME_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAVEL_TIME_FORMAT, "TRAVEL_TIME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TRAVEL_TIME_FILE, "NEW_TRAVEL_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRAVEL_TIME_FORMAT, "NEW_TRAVEL_TIME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TIME_SUMMARY_FILE, "NEW_TIME_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TIME_SUMMARY_FORMAT, "NEW_TIME_SUMMARY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MINIMUM_SCHEDULE_VARIANCE, "MINIMUM_SCHEDULE_VARIANCE", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", "0..100 minutes", NO_HELP },
		{ NEW_LINK_EVENT_FILE, "NEW_LINK_EVENT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_EVENT_FORMAT, "NEW_LINK_EVENT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TIME_DISTRIBUTION_FILE, "NEW_TIME_DISTRIBUTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TIME_GAP_FILE, "NEW_TRIP_TIME_GAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOTAL_TIME_DISTRIBUTION",
		"PERIOD_TIME_DISTRIBUTIONS",
		"TOTAL_TIME_SUMMARY",
		"PERIOD_TIME_SUMMARY",
		"TRIP_TIME_GAP_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (eventsum_keys);
	Report_List (reports);
	
	Location_XY_Flag (true);
	Time_Table_Flag (true);
	
	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);

	System_Read_False (EVENT);
	System_Data_Reserve (EVENT, 0);

	link_event_flag = trip_flag = travel_flag = group_flag = time_in_flag = false;
	time_file_flag = compare_flag = new_select_flag = false;
	trip_file_flag = distrib_flag = time_flag = diff_file_flag = false;
	select_flag = update_flag = time_sum_flag = time_diff_flag = time_gap_flag = false;
	min_variance = 0;

	increment = 900;
	num_inc = 96;

	max_time = 60;
	min_time = 1;

	percent_diff = 0.0;
	percent = 100.0;
	max_percent = 100.0;
}

//---------------------------------------------------------
//	EventSum destructor
//---------------------------------------------------------

EventSum::~EventSum (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	EventSum *exe = new EventSum ();

	return (exe->Start_Execution (commands, control));
}
