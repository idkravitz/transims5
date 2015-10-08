//*********************************************************
//	NewFormat.cpp - convert data files to version 4.2 format
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	NewFormat constructor
//---------------------------------------------------------

NewFormat::NewFormat (void) : Data_Service ()
{
	Program ("NewFormat");
	Version (20);
	Title ("Convert Data Files to Version 5.0 Format");

	String v4_range ("[project_directory]Version_4_filename");
	String v4_parts = v4_range + ".*";

	System_File_Type optional_files [] = {
		NODE, ZONE, LINK, POCKET, SHAPE, LANE_USE, CONNECTION, 
		TURN_PENALTY, PARKING, LOCATION, ACCESS_LINK, SIGN, 
		SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		SELECTION, HOUSEHOLD, LINK_DELAY, PERFORMANCE, RIDERSHIP, 
		VEHICLE_TYPE, VEHICLE, TRIP, PROBLEM, PLAN, SKIM,
		NEW_NODE, NEW_ZONE, NEW_LINK, NEW_SHAPE, NEW_POCKET, NEW_LANE_USE,
		NEW_CONNECTION, NEW_TURN_PENALTY, NEW_PARKING, NEW_LOCATION, 
		NEW_ACCESS_LINK, NEW_SIGN, NEW_SIGNAL, NEW_TIMING_PLAN, 
		NEW_PHASING_PLAN, NEW_DETECTOR, 
		NEW_TRANSIT_STOP, NEW_TRANSIT_FARE, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER, 
		NEW_ROUTE_NODES,
		NEW_SELECTION, NEW_HOUSEHOLD, NEW_LINK_DELAY, NEW_PERFORMANCE, NEW_RIDERSHIP,
		NEW_VEHICLE_TYPE, NEW_VEHICLE, NEW_TRIP, NEW_PROBLEM, NEW_PLAN, NEW_SKIM, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, FLOW_UNITS, SAVE_LANE_USE_FLOWS, SKIM_TOTAL_TIME_FLAG, 0
	};
	int data_service_keys [] = {
		TRIP_SORT_TYPE, DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, CONGESTED_TIME_RATIO, 0
	};
	Control_Key newformat_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ FLATTEN_OUTPUT_FLAG, "FLATTEN_OUTPUT_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP},
		{ TOLL_FILE, "TOLL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", v4_range, NO_HELP },
		{ ACTIVITY_FILE, "ACTIVITY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", v4_range, NO_HELP },
		{ PERSON_FILE, "PERSON_FILE", LEVEL0, OPT_KEY, IN_KEY, "", v4_range, NO_HELP },
		{ SNAPSHOT_FILE, "SNAPSHOT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_FILE, "NEW_SNAPSHOT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_FORMAT, "NEW_SNAPSHOT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_SNAPSHOT_COMPRESSION, "NEW_SNAPSHOT_COMPRESSION", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ VERSION4_TIME_FORMAT, "VERSION4_TIME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "SECONDS", TIME_FORMAT_RANGE, NO_HELP },
		{ VERSION4_PLAN_FILE, "VERSION4_PLAN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", v4_parts, NO_HELP },
		{ VERSION4_PLAN_FORMAT, "VERSION4_PLAN_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "VERSION3", "VERSION3, BINARY", NO_HELP },
		{ NODE_LIST_PATHS, "NODE_LIST_PATHS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP},
		{ TRAVELER_SCALING_FACTOR, "TRAVELER_SCALING_FACTOR", LEVEL0, OPT_KEY, INT_KEY, "100", "1..100", NO_HELP},
		{ VERSION4_ROUTE_HEADER, "VERSION4_ROUTE_HEADER", LEVEL0, OPT_KEY, IN_KEY, "", v4_range, NO_HELP },
		{ VERSION4_ROUTE_NODES, "VERSION4_ROUTE_NODES", LEVEL0, OPT_KEY, IN_KEY, "", v4_range, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (newformat_keys);
	Report_List (reports);

#ifdef BOOST_THREADS
	Enable_Threads (true);
#endif
	plan_processing = 0;

	toll_flag = activity_flag = person_flag = plan_flag = route_flag = copy_flag = false;

	num_new_trip = max_trip_part = num_new_skim = max_skim_part = num_new_act = route_periods = 0;
	scale = 100;
	time_units = SECONDS;

	new_loc_file = 0;
	new_zone_file = 0;
}

//---------------------------------------------------------
//	NewFormat destructor
//---------------------------------------------------------

NewFormat::~NewFormat (void)
{
	if (plan_processing != 0) {
		for (int i=0; i < Num_Threads (); i++) {
			delete plan_processing [i];
			plan_processing [i] = 0;
		}
		delete plan_processing;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	NewFormat *exe = new NewFormat ();

	return (exe->Start_Execution (commands, control));
}

