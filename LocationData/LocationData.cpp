//*********************************************************
//	LocationData.cpp - activity location data processing
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	LocationData constructor
//---------------------------------------------------------

LocationData::LocationData (void) : Data_Service ()
{
	Program ("LocationData");
	Version (3);
	Title ("Activity Location Data Processor");

	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, NEW_LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, ZONE, ACCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key locationdata_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_WALK_ACCESS_FIELD, "NEW_WALK_ACCESS_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ MAX_WALK_DISTANCE, "MAX_WALK_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "1000 meters", "10..3000 meters", NO_HELP },
		{ WALK_ACCESS_TIME_RANGE, "WALK_ACCESS_TIME_RANGE", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_USE_FLAG_FIELD, "NEW_USE_FLAG_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ LINK_USE_FLAG_TYPES, "LINK_USE_FLAG_TYPES", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SUBZONE_ZONE_FACTOR_FILE, "SUBZONE_ZONE_FACTOR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SUBZONE_FIELD, "NEW_SUBZONE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ MAX_SUBZONE_DISTANCE, "MAX_SUBZONE_DISTANCE", LEVEL1, OPT_KEY, INT_KEY, "1000 meters", "0, 10..10000 meters", NO_HELP },
		{ SUBZONE_DATA_FILE, "SUBZONE_DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SUBZONE_DATA_FORMAT, "SUBZONE_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ SUBZONE_DATA_FIELD, "SUBZONE_DATA_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SUBZONE_ZONE_FIELD, "SUBZONE_ZONE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_LOCATION_FIELD, "NEW_LOCATION_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ DATA_JOIN_FIELD, "DATA_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ LOCATION_JOIN_FIELD, "LOCATION_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ BOUNDARY_POLYGON_FILE, "BOUNDARY_POLYGON_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ZONE_LOCATION_MAP_FILE, "NEW_ZONE_LOCATION_MAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ MINIMUM_ZONE_LOCATIONS, "MINIMUM_ZONE_LOCATIONS", LEVEL0, OPT_KEY, INT_KEY, "4", "2..20", NO_HELP },
		{ ZONE_BOUNDARY_FILE, "ZONE_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_FIELD_NAME, "ZONE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ZONE_UPDATE_RANGE, "ZONE_UPDATE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"CHECK_ZONE_COVERAGE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (locationdata_keys);
	Report_List (reports);
	Location_XY_Flag (true);

	projection.Add_Keys ();

	copy_flag = false;
	walk_access_flag = walk_time_flag = false;
	walk_access_field = 0;
	walk_distance = 1000;

	num_data_files = 0;
	data_flag = false;

	zone_file_flag = subzone_map_flag = subzone_flag = false;
	num_subzone = 0;
	script_flag = zone_file_flag = range_flag = false;

	use_flag = false;

	boundary_flag = coverage_flag = zone_loc_flag = false;
	zone_field = 0;
	min_locations = 4;

	num_polygons = 0;
	polygon_flag = false;
}

//---------------------------------------------------------
//	LocationData destructor
//---------------------------------------------------------

LocationData::~LocationData (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LocationData *exe = new LocationData ();

	return (exe->Start_Execution (commands, control));
}
