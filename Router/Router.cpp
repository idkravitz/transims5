//*********************************************************
//	Router.cpp - Network Path Building
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Router constructor
//---------------------------------------------------------

Router::Router (void) : Router_Service (), Select_Service ()
{
	Program ("Router");
	Version (54);
	Title ("Network Path Building");

	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
		POCKET, LANE_USE, TURN_PENALTY, PARKING, ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		HOUSEHOLD, SELECTION, TRIP, LINK_DELAY, VEHICLE, VEHICLE_TYPE, 
		PLAN, NEW_PLAN, NEW_PROBLEM, NEW_LINK_DELAY, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 
		SELECTION_PERCENTAGE, DELETION_FILE, DELETION_FORMAT, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, help ----
		{ UPDATE_PLAN_RECORDS, "UPDATE_PLAN_RECORDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ REROUTE_FROM_TIME_POINT, "REROUTE_FROM_TIME_POINT", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "", NO_HELP },
		{ PRINT_UPDATE_WARNINGS, "PRINT_UPDATE_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ MAXIMUM_NUMBER_OF_ITERATIONS, "MAXIMUM_NUMBER_OF_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ LINK_CONVERGENCE_CRITERIA, "LINK_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ TRIP_CONVERGENCE_CRITERIA, "TRIP_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ INITIAL_WEIGHTING_FACTOR, "INITIAL_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0, >= 0.5", NO_HELP },
		{ ITERATION_WEIGHTING_INCREMENT, "ITERATION_WEIGHTING_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..5.0", NO_HELP },
		{ MAXIMUM_WEIGHTING_FACTOR, "MAXIMUM_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "20.0", "0.0, >= 2.0", NO_HELP },
		{ NEW_LINK_CONVERGENCE_FILE, "NEW_LINK_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_CONVERGENCE_FILE, "NEW_TRIP_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TRAVELER_TYPE_SCRIPT",
		"TRAVELER_TYPE_STACK",
		"LINK_GAP_REPORT",
		"TRIP_GAP_REPORT",
		"ITERATION_PROBLEMS", 
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Router_Service_Keys ();
	Key_List (keys);
	Report_List (reports);
	
	Enable_Partitions (true);
#ifdef MPI_EXE
	Enable_MPI (true);
#endif
	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);

	reroute_time = 0;
	plan_set_flag = problem_set_flag = false;
	old_plan_flag = update_flag = warn_flag = false;
	select_flag = new_delay_flag = flow_flag = turn_flag = reroute_flag = false;
	
	link_gap_flag = trip_gap_flag = trip_gap_map_flag = iteration_flag = first_iteration = false;
	max_iteration = 1;
	num_trip_rec = num_trip_sel = 0;
	factor = increment = max_factor = link_gap = trip_gap = 0.0;

	num_file_sets = 1;
	total_records = num_time_updates = num_trip_parts = 0;
	trip_flag = plan_flag = new_plan_flag = problem_flag = false;

	trip_file = 0;
	plan_file = new_plan_file = 0;
	problem_file = 0;
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Router *exe = new Router ();

	return (exe->Start_Execution (commands, control));
}

