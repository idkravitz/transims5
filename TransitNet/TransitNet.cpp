//*********************************************************
//	TransitNet.cpp - Transit Conversion Utility
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	TransitNet constructor
//---------------------------------------------------------

TransitNet::TransitNet (void) : Data_Service ()
{
	Program ("TransitNet");
	Version (6);
	Title ("Transit Network Conversion Utility");
	
	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, VEHICLE_TYPE, ROUTE_NODES, NEW_TRANSIT_STOP, 
		NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER, END_FILE
	};
	System_File_Type optional_files [] = {
		ZONE, SHAPE, LOCATION, PARKING, ACCESS_LINK, NEW_PARKING, NEW_ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, ZONE_EQUIVALENCE_FILE, 0
	};
	Control_Key transimsnet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ PARK_AND_RIDE_FILE, "PARK_AND_RIDE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STOP_SPACING_BY_AREA_TYPE, "STOP_SPACING_BY_AREA_TYPE", LEVEL1, OPT_KEY, LIST_KEY, "200 meters", "37.5..3200 meters", NO_HELP },
		{ STOP_FACILITY_TYPE_RANGE, "STOP_FACILITY_TYPE_RANGE", LEVEL0, OPT_KEY, TEXT_KEY, "PRINCIPAL..FRONTAGE", "FREEWAY..EXTERNAL", NO_HELP },
		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ TRANSIT_TRAVEL_TIME_FACTORS, "TRANSIT_TRAVEL_TIME_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..3.0", NO_HELP },
		{ MINIMUM_DWELL_TIME, "MINIMUM_DWELL_TIME", LEVEL0, OPT_KEY, TIME_KEY, "5 seconds", "0..300 seconds", NO_HELP },
		{ INTERSECTION_STOP_TYPE, "INTERSECTION_STOP_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "NEARSIDE", "NEARSIDE, FARSIDE, MIDBLOCK", NO_HELP },
		{ INTERSECTION_STOP_OFFSET, "INTERSECTION_STOP_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 meters", "0..100 meters", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"ZONE_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (transimsnet_keys);
	Report_List (reports);

	AB_Map_Flag (true);

	parkride_flag = equiv_flag = dwell_flag = time_flag = speed_flag = at_flag = access_flag = false;

	memset (facility_flag, '\0', sizeof (facility_flag));

	naccess = nlocation = nparking = 0;
	line_edit = route_edit = schedule_edit = driver_edit = 0;
	max_parking = max_access = max_location = max_stop = nparkride = 0;
	nstop = nroute = nschedule = ndriver = end_warnings = parking_warnings = 0;
	min_dwell = 5;
	stop_type = NEARSIDE;

	left_turn = compass.Num_Points () * -70 / 360;
	bus_code = Use_Code ("BUS");
	rail_code = Use_Code ("RAIL");
	stop_offset = 10;
}

//---------------------------------------------------------
//	TransitNet destructor
//---------------------------------------------------------

TransitNet::~TransitNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TransitNet *exe = new TransitNet ();

	return (exe->Start_Execution (commands, control));
}
