//*********************************************************
//	Relocate.cpp - Update activity locations
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Relocate constructor
//---------------------------------------------------------

Relocate::Relocate (void) : Data_Service ()
{
	Program ("Relocate");
	Version (0);
	Title ("Update Activity Locations");
	
	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, PARKING, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, ACCESS_LINK, TRIP, VEHICLE, PLAN, NEW_TRIP, NEW_VEHICLE, NEW_PLAN, NEW_SELECTION, END_FILE
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ OLD_NODE_FILE, "OLD_NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_NODE_FORMAT, "OLD_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OLD_SHAPE_FILE, "OLD_SHAPE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_SHAPE_FORMAT, "OLD_SHAPE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OLD_LINK_FILE, "OLD_LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_LINK_FORMAT, "OLD_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OLD_LOCATION_FILE, "OLD_LOCATION_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_LOCATION_FORMAT, "OLD_LOCATION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OLD_PARKING_FILE, "OLD_PARKING_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_PARKING_FORMAT, "OLD_PARKING_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OLD_ACCESS_FILE, "OLD_ACCESS_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OLD_ACCESS_FORMAT, "OLD_ACCESSE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_LOCATION_MAP_FILE, "NEW_LOCATION_MAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PARKING_MAP_FILE, "NEW_PARKING_MAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);

	Key_List (keys);
	Enable_Partitions (true);
	Location_XY_Flag (true);

	trip_flag = plan_flag = vehicle_flag = select_flag = false;
	loc_map_flag = park_map_flag = shape_flag = access_flag = false;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);

	System_Read_False (VEHICLE);
	System_Data_Reserve (VEHICLE, 0);
}

//---------------------------------------------------------
//	Relocate destructor
//---------------------------------------------------------

Relocate::~Relocate (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Relocate *exe = new Relocate ();

	return (exe->Start_Execution (commands, control));
}
