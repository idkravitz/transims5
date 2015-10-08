//*********************************************************
//	ZoneData.cpp - Zone Data Processing
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	ZoneData constructor
//---------------------------------------------------------

ZoneData::ZoneData (void) : Data_Service ()
{
	Program ("ZoneData");
	Version (5);
	Title ("Zone Data Processor");

	System_File_Type required_network [] = {
		ZONE, NEW_ZONE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key zonedata_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ CLEAR_ALL_FIELDS, "CLEAR_ALL_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ZONE_FILE_HEADER, "ZONE_FILE_HEADER", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_ZONE_FIELD, "NEW_ZONE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ DATA_JOIN_FIELD, "DATA_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ZONE_JOIN_FIELD, "ZONE_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ BOUNDARY_POLYGON_FILE, "BOUNDARY_POLYGON_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SUM_ATTRIBUTE_DISTANCE, "SUM_ATTRIBUTE_DISTANCE", LEVEL1, OPT_KEY, FLOAT_KEY, "2000 meters", "100..10000 meters", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Required_System_Files (required_network);
	File_Service_Keys (file_service_keys);
	Key_List (zonedata_keys);
	Report_List (reports);

	projection.Add_Keys ();

	num_data_files = num_polygons = num_sum_distance = 0;
	copy_flag = script_flag = data_flag = polygon_flag = sum_flag = false;
}

//---------------------------------------------------------
//	ZoneData destructor
//---------------------------------------------------------

ZoneData::~ZoneData (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ZoneData *exe = new ZoneData ();

	return (exe->Start_Execution (commands, control));
}
