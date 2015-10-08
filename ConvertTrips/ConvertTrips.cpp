//*********************************************************
//	ConvertTrips.cpp - Trip Table Conversion Utility
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	ConvertTrips constructor
//---------------------------------------------------------

ConvertTrips::ConvertTrips (void) : Data_Service (), Select_Service ()
{
	Program ("ConvertTrips");
	Version (9);
	Title ("Trip Table Conversion Utility");

	System_File_Type required_files [] = {
		NODE, LINK, ZONE, PARKING, LOCATION, VEHICLE_TYPE, 
		NEW_TRIP, NEW_HOUSEHOLD, NEW_VEHICLE, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, ACCESS_LINK, SKIM, TRIP, HOUSEHOLD, VEHICLE, NEW_PROBLEM, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, TIME_EQUIVALENCE_FILE, ZONE_EQUIVALENCE_FILE, ZONE_LOCATION_MAP_FILE, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 0
	};
	Control_Key converttrips_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ FIRST_HOUSEHOLD_NUMBER, "FIRST_HOUSEHOLD_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000000000", NO_HELP },
		{ ADDITIONAL_TRAVEL_TIME, "ADDITIONAL_TRAVEL_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", "0..30 minutes", NO_HELP },
		{ TOUR_DATA_FILE, "TOUR_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TOUR_DATA_FORMAT, "TOUR_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TOUR_HOUSEHOLD_FIELD, "TOUR_HOUSEHOLD_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_PERSON_FIELD, "TOUR_PERSON_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_NUMBER_FIELD, "TOUR_NUMBER_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_PURPOSE_FIELD, "TOUR_PURPOSE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_MODE_FIELD, "TOUR_MODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_ORIGIN_FIELD, "TOUR_ORIGIN_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_DESTINATION_FIELD, "TOUR_DESTINATION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_STOP_OUT_FIELD, "TOUR_STOP_OUT_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_STOP_IN_FIELD, "TOUR_STOP_IN_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_START_FIELD, "TOUR_START_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_RETURN_FIELD, "TOUR_RETURN_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_GROUP_FIELD, "TOUR_GROUP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TOUR_GROUP_SCRIPT, "TOUR_GROUP_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_TABLE_FILE, "TRIP_TABLE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_TABLE_FORMAT, "TRIP_TABLE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TRIP_SCALING_FACTOR, "TRIP_SCALING_FACTOR", LEVEL1, OPT_KEY, FLOAT_KEY, "1.0", "0.001..100.0", NO_HELP },
		{ TIME_DISTRIBUTION_FILE, "TIME_DISTRIBUTION_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TIME_DISTRIBUTION_FORMAT, "TIME_DISTRIBUTION_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TIME_DISTRIBUTION_SCRIPT, "TIME_DISTRIBUTION_SCRIPT", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TIME_DISTRIBUTION_FIELD, "TIME_DISTRIBUTION_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TIME_DISTRIBUTION_TYPE, "TIME_DISTRIBUTION_TYPE", LEVEL1, OPT_KEY, TEXT_KEY, "TRIP_START", "TRIP_START, TRIP_END, MID_TRIP", NO_HELP },
		{ TIME_PERIOD_RANGE, "TIME_PERIOD_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ TIME_SCHEDULE_CONSTRAINT, "TIME_SCHEDULE_CONSTRAINT", LEVEL1, OPT_KEY, TEXT_KEY, "START_TIME", CONSTRAINT_RANGE, NO_HELP },
		{ ORIGIN_WEIGHT_FIELD, "ORIGIN_WEIGHT_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DESTINATION_WEIGHT_FIELD, "DESTINATION_WEIGHT_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DISTANCE_WEIGHT_FLAG, "DISTANCE_WEIGHT_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ STOP_WEIGHT_FIELD, "STOP_WEIGHT_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ TRAVELER_TYPE_SCRIPT, "TRAVELER_TYPE_SCRIPT", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAVELER_TYPE_CODE, "TRAVELER_TYPE_CODE", LEVEL1, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ TRIP_PURPOSE_CODE, "TRIP_PURPOSE_CODE", LEVEL1, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ TRAVEL_MODE_CODE, "TRAVEL_MODE_CODE", LEVEL1, OPT_KEY, TEXT_KEY, "DRIVE", MODE_RANGE, NO_HELP },
		{ RETURN_TRIP_FLAG, "RETURN_TRIP_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ACTIVITY_DURATION, "ACTIVITY_DURATION", LEVEL1, OPT_KEY, TIME_KEY, "0.0 hours", "0.0, 0.25..24.0 hours", NO_HELP },
		{ TRIP_STOP_DURATION, "TRIP_STOP_DURATION", LEVEL1, OPT_KEY, TIME_KEY, "5 minutes", "0..180 minutes", NO_HELP },
		{ TRIP_PRIORITY_CODE, "TRIP_PRIORITY_CODE", LEVEL1, OPT_KEY, TEXT_KEY, "MEDIUM", "LOW, MEDIUM, HIGH, CRITICAL", NO_HELP },
		{ VEHICLE_TYPE_CODE, "VEHICLE_TYPE_CODE", LEVEL1, OPT_KEY, INT_KEY, "1", "0..100", NO_HELP },
		{ VEHICLE_PASSENGERS, "VEHICLE_PASSENGERS", LEVEL1, OPT_KEY, INT_KEY, "0", "0..500", NO_HELP },
		{ AVERAGE_TRAVEL_SPEED, "AVERAGE_TRAVEL_SPEED", LEVEL1, OPT_KEY, FLOAT_KEY, "30.0 kph", "3.0..100.0 kph", NO_HELP },
		{ TRIP_FACTOR_FILE, "TRIP_FACTOR_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_FACTOR_FORMAT, "TRIP_FACTOR_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TIME_DISTRIBUTION_FILE, "NEW_TIME_DISTRIBUTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOUR_GROUP_SCRIPT",
		"TOUR_GROUP_STACK",
		"TIME_DISTRIBUTION_SCRIPT",
		"TIME_DISTRIBUTION_STACK",
		"TIME_EQUIVALENCE",
		"ZONE_EQUIVALENCE",
		"TRAVELER_TYPE_SCRIPT",
		"TRAVELER_TYPE_STACK",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (converttrips_keys);
	Report_List (reports);
	Location_XY_Flag (true);
	Time_Table_Flag (true);

	equiv_flag = skim_flag = zone_loc_flag = tour_flag = script_flag = diurnal_flag = problem_flag = false;

	num_zone = additional_time = 0;
	tot_tours = tot_trips = tot_errors = 0;
	new_trips = new_hholds = new_persons = new_vehicles = 0;
	tot_add = tot_del = 0.0;
	group_number = 0;
	skim_ptr = 0;

	trip_copy = hhold_copy = veh_copy = 0;
	max_hh_in = max_veh_in = 0;

	midnight = Dtime (MIDNIGHT, SECONDS);
	minute.Minutes (1);
	two_minutes.Minutes (2);
	five_minutes.Minutes (5);
	ten_minutes.Minutes (10);
	one_hour.Hours (1);
	max_start = midnight - minute;
}

//---------------------------------------------------------
//	ConvertTrips destructor
//---------------------------------------------------------

ConvertTrips::~ConvertTrips (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ConvertTrips *exe = new ConvertTrips ();

	return (exe->Start_Execution (commands, control));
}
