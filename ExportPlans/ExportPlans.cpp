//*********************************************************
//	ExportPlans.cpp - export plan data in various formats
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	ExportPlans constructor
//---------------------------------------------------------

ExportPlans::ExportPlans (void) : Data_Service (), Select_Service ()
{
	Program ("ExportPlans");
	Version (7);
	Title ("Export Plan Data in Various Formats");

	System_File_Type required_files [] = {
		PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		LINK, NODE, LOCATION, VEHICLE_TYPE, VEHICLE, END_FILE
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_MODES, SELECT_NODES, SELECT_SUBAREA_POLYGON, SELECT_VEHICLE_TYPES, 0
	};
	Control_Key exportplans_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_VISSIM_XML_FILE, "NEW_VISSIM_XML_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		{ LOCATION_ZONE_MAP_FILE, "LOCATION_ZONE_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP  },
		{ SUBZONE_WEIGHT_FILE, "SUBZONE_WEIGHT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP  },
		{ NEW_ZONE_LOCATION_FILE, "NEW_ZONE_LOCATION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP  },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (exportplans_keys);
	Report_List (reports);
	
	Enable_Partitions (true);
	Location_XY_Flag (true);

	vissim_flag = select_flag = map_flag = zone_loc_flag = subzone_flag = false;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	ExportPlans destructor
//---------------------------------------------------------

ExportPlans::~ExportPlans (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ExportPlans *exe = new ExportPlans ();

	return (exe->Start_Execution (commands, control));
}
