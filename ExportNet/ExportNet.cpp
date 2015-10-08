//*********************************************************
//	ExportNet.cpp - export network data in various formats
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	ExportNet constructor
//---------------------------------------------------------

ExportNet::ExportNet (void) : Data_Service (), Select_Service ()
{
	Program ("ExportNet");
	Version (7);
	Title ("Export Network Data in Various Formats");

	System_File_Type optional_files [] = {
		LINK, NODE, SHAPE, POCKET, CONNECTION, LOCATION, PARKING, 
		SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, 
		NEW_LINK, NEW_NODE, NEW_SHAPE, NEW_CONNECTION, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		NEW_ROUTE_NODES, VEHICLE_TYPE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, 0
	};
	int select_service_keys [] = {
		SELECT_MODES, SELECT_TIME_OF_DAY, SELECT_ROUTES, SELECT_NODES, SELECT_SUBAREA_POLYGON, 0
	};
	Control_Key exportnet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_UTDF_NETWORK_FILE, "NEW_UTDF_NETWORK_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		{ NEW_NODE_MAP_FILE, "NEW_NODE_MAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		{ NEW_VISSIM_XML_FILE, "NEW_VISSIM_XML_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		{ INSERT_PARKING_LINKS, "INSERT_PARKING_LINKS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP  },
		{ WRITE_DWELL_FIELD, "WRITE_DWELL_FIELD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP  },
		{ WRITE_TIME_FIELD, "WRITE_TIME_FIELD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ WRITE_SPEED_FIELD, "WRITE_SPEED_FIELD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP  },
		{ MINIMUM_DWELL_TIME, "MINIMUM_DWELL_TIME", LEVEL0, OPT_KEY, TIME_KEY, "5 seconds", "0..120 seconds", NO_HELP },
		{ MAXIMUM_DWELL_TIME, "MAXIMUM_DWELL_TIME", LEVEL0, OPT_KEY, TIME_KEY, "30 seconds", "0..300 seconds", NO_HELP },
		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ PERIOD_TRAVEL_TIMES, "PERIOD_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP  },
		{ NEW_ZONE_LOCATION_FILE, "NEW_ZONE_LOCATION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (exportnet_keys);
	Report_List (reports);
	Location_XY_Flag (true);

	min_dwell = 5;			//---- 5 seconds ----
	max_dwell = 30;			//---- 30 seconds ----

	nroute = nnodes = num_periods = 0;
	utdf_flag = map_flag = vissim_flag = parking_flag = zone_loc_flag = false;
	route_flag = time_flag = dwell_flag = ttime_flag = speed_flag = false;
}

//---------------------------------------------------------
//	ExportNet destructor
//---------------------------------------------------------

ExportNet::~ExportNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ExportNet *exe = new ExportNet ();

	return (exe->Start_Execution (commands, control));
}
