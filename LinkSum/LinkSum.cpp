//*********************************************************
//	LinkSum.cpp - Summarize Link Performance Data
//*********************************************************

#include "LinkSum.hpp"

int	LinkSum::percent_break [NUM_PERCENTILES] = {50, 65, 70, 75, 80, 85, 90, 95, 99};

//---------------------------------------------------------
//	LinkSum constructor
//---------------------------------------------------------

LinkSum::LinkSum (void) : Data_Service (), Select_Service ()
{
	Program ("LinkSum");
	Version (12);
	Title ("Summarize Link Performance Data");

	System_File_Type required_files [] = {
		NODE, LINK, PERFORMANCE, END_FILE
	};
	System_File_Type optional_files [] = {
		CONNECTION, LANE_USE, LOCATION, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, LINK_EQUIVALENCE_FILE, ZONE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_FACILITY_TYPES, SELECT_SUBAREA_POLYGON, 0
	};
	Control_Key linksum_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COMPARE_PERFORMANCE_FILE, "COMPARE_PERFORMANCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COMPARE_PERFORMANCE_FORMAT, "COMPARE_PERFORMANCE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ MINIMUM_LINK_FLOW, "MINIMUM_LINK_FLOW", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0", ">= 0", NO_HELP },
		{ SELECT_BY_LINK_GROUP, "SELECT_BY_LINK_GROUP", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_ACTIVITY_FILE, "NEW_LINK_ACTIVITY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_ACTIVITY_FORMAT, "NEW_LINK_ACTIVITY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ COPY_LOCATION_FIELDS, "COPY_LOCATION_FIELDS", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_ZONE_TRAVEL_FILE, "NEW_ZONE_TRAVEL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ZONE_TRAVEL_FORMAT, "NEW_ZONE_TRAVEL_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_GROUP_TRAVEL_FILE, "NEW_GROUP_TRAVEL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_GROUP_TRAVEL_FORMAT, "NEW_GROUP_TRAVEL_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMTED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FILE, "NEW_LINK_DIRECTION_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FORMAT, "NEW_LINK_DIRECTION_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FIELD, "NEW_LINK_DIRECTION_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", PERF_FIELD_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_INDEX, "NEW_LINK_DIRECTION_INDEX", LEVEL1, OPT_KEY, BOOL_KEY, "", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FILE, "NEW_LINK_DATA_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FORMAT, "NEW_LINK_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FIELD, "NEW_LINK_DATA_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", PERF_FIELD_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_DATA_FILE, "NEW_PERFORMANCE_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_DATA_FORMAT, "NEW_PERFORMANCE_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_FILE, "NEW_TURN_VOLUME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_FORMAT, "NEW_TURN_VOLUME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ TURN_NODE_RANGE, "TURN_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },	
		END_CONTROL
	};
	const char *reports [] = {
		"TOP_100_LINK_FLOWS",
		"TOP_100_LANE_FLOWS",
		"TOP_100_PERIOD_FLOWS",
		"TOP_100_SPEED_REDUCTIONS",
		"TOP_100_TRAVEL_TIME_RATIOS",
		"TOP_100_VOLUME_CAPACITY_RATIOS",
		"TOP_100_TRAVEL_TIME_CHANGES",
		"TOP_100_VOLUME_CHANGES",
		"LINK_VOLUME_GREATER_THAN_*",
		"GROUP_VOLUME_GREATER_THAN_*",
		"LINK_EQUIVALENCE",
		"ZONE_EQUIVALENCE",
		"TRAVEL_TIME_DISTRIBUTION",
		"VOLUME_CAPACITY_RATIOS",
		"TRAVEL_TIME_CHANGES",
		"VOLUME_CHANGES",
		"LINK_GROUP_TRAVEL_TIME",
		"NETWORK_PERFORMANCE_SUMMARY",
		"NETWORK_PERFORMANCE_DETAILS",
		"RELATIVE_GAP_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Sum_Flow_Flag (true);
	Key_List (linksum_keys);
	Report_List (reports);

	compare_flag = group_select = detail_flag = turn_vol_flag = false;
	select_flag = activity_flag = zone_flag = group_flag = false;

	nerror = 0;
	minimum_flow = 2.0;
	increment = 900;
	num_inc = 96;
	cap_factor = 0.25;
	congested_ratio = 3.0;
}

//---------------------------------------------------------
//	LinkSum destructor
//---------------------------------------------------------

LinkSum::~LinkSum (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkSum *exe = new LinkSum ();

	return (exe->Start_Execution (commands, control));
}

