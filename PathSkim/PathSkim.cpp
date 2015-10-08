//*********************************************************
//	PathSkim.cpp - Build and Skim Network Paths
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	PathSkim constructor
//---------------------------------------------------------

PathSkim::PathSkim (void) : Router_Service ()
{
	Program ("PathSkim");
	Version (23);
	Title ("Build and Skim Network Paths");

	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
		ZONE, POCKET, LANE_USE, TURN_PENALTY, PARKING, ACCESS_LINK,
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		HOUSEHOLD, SELECTION, LINK_DELAY, VEHICLE, VEHICLE_TYPE, 
		NEW_SKIM, NEW_PLAN, NEW_PROBLEM, NEW_LINK_DELAY, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, SKIM_OD_UNITS, SKIM_TIME_PERIODS, 
		SKIM_TIME_INCREMENT, SKIM_TOTAL_TIME_FLAG, SKIM_TRAVEL_TIME_FORMAT, SKIM_TRIP_LENGTH_FORMAT, 
		NEAREST_NEIGHBOR_FACTOR, MERGE_TIME_PERIODS, SKIM_FILE_HEADERS, 
		ZONE_EQUIVALENCE_FILE, ZONE_LOCATION_MAP_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, help ----
		{ ROUTE_FROM_SPECIFIED_LOCATIONS, "ROUTE_FROM_SPECIFIED_LOCATIONS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "", NO_HELP },
		{ ROUTE_TO_SPECIFIED_LOCATIONS, "ROUTE_TO_SPECIFIED_LOCATIONS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "", NO_HELP },
		{ ROUTE_AT_SPECIFIED_TIMES, "ROUTE_AT_SPECIFIED_TIMES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP},
		{ ROUTE_BY_TIME_INCREMENT, "ROUTE_BY_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", MINUTE_RANGE, NO_HELP },
		{ ROUTE_WITH_TIME_CONSTRAINT, "ROUTE_WITH_TIME_CONSTRAINT", LEVEL0, OPT_KEY, TEXT_KEY, "START_TIME", CONSTRAINT_RANGE, NO_HELP },
		{ ROUTE_WITH_SPECIFIED_MODE, "ROUTE_WITH_SPECIFIED_MODE", LEVEL0, OPT_KEY, TEXT_KEY, "DRIVE", MODE_RANGE, NO_HELP },
		{ ROUTE_WITH_USE_RESTRICTION, "ROUTE_WITH_USE_RESTRICTION", LEVEL0, OPT_KEY, TEXT_KEY, "CAR", USE_RANGE, NO_HELP },
		{ ROUTE_USING_VEHICLE_TYPE, "ROUTE_USING_VEHICLE_TYPE", LEVEL0, OPT_KEY, INT_KEY, "1", "0..100", NO_HELP },
		{ ROUTE_USING_TRAVELER_TYPE, "ROUTE_USING_TRAVELER_TYPE", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ ROUTE_FROM_SPECIFIED_ZONES, "ROUTE_FROM_SPECIFIED_ZONES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "", NO_HELP },
		{ ROUTE_TO_SPECIFIED_ZONES, "ROUTE_TO_SPECIFIED_ZONES", LEVEL0, OPT_KEY, LIST_KEY, "ALL", "", NO_HELP },
		{ ORIGIN_LOCATIONS_PER_ZONE, "ORIGIN_LOCATIONS_PER_ZONE", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ DESTINATION_LOCATIONS_PER_ZONE, "DESTINATION_LOCATIONS_PER_ZONE", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ LOCATION_SELECTION_METHOD, "LOCATION_SELECTION_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "RANDOM", LOCATION_METHOD_RANGE, NO_HELP },
		{ ORIGIN_ZONE_FILE, "ORIGIN_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DESTINATION_ZONE_FILE, "DESTINATION_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ORIGIN_LOCATION_FILE, "ORIGIN_LOCATION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DESTINATION_LOCATION_FILE, "DESTINATION_LOCATION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ORIGIN_LOCATION_FILE, "NEW_ORIGIN_LOCATION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DESTINATION_LOCATION_FILE, "NEW_DESTINATION_LOCATION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"ZONE_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Router_Service_Keys ();
	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);
	Location_XY_Flag (true);

	mode_flag = select_org = select_des = select_time = skim_flag = zone_skim_flag = district_flag = false;
	zone_loc_flag = zone_flag = org_loc_flag = des_loc_flag = user_loc_flag = false;
	new_mode = nprocess = constraint = total_records = num_org = num_des = cells_out = 0;
	use_type = CAR;
	veh_type = 0;
	traveler_type = 0;
	method = RANDOM_LOC;
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PathSkim *exe = new PathSkim ();

	return (exe->Start_Execution (commands, control));
}

