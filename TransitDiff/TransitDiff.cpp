//*********************************************************
//	TransitDiff.cpp - compare two transit networks
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	TransitDiff constructor
//---------------------------------------------------------

TransitDiff::TransitDiff (void) : Data_Service ()
{
	Program ("TransitDiff");
	Version (0);
	Title ("Compare Two Transit Networks");

	System_File_Type required_files [] = {
		NODE, LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, VEHICLE_TYPE, END_FILE
	};
	System_File_Type optional_files [] = {
		TRANSIT_DRIVER, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, help ----
		{ COMPARE_NODE_FILE, "COMPARE_NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_NODE_FORMAT, "COMPARE_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ COMPARE_LINK_FILE, "COMPARE_LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_LINK_FORMAT, "COMPARE_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ COMPARE_TRANSIT_STOP_FILE, "COMPARE_TRANSIT_STOP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_TRANSIT_STOP_FORMAT, "COMPARE_TRANSIT_STOP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ COMPARE_TRANSIT_ROUTE_FILE, "COMPARE_TRANSIT_ROUTE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_TRANSIT_ROUTE_FORMAT, "COMPARE_TRANSIT_ROUTE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ COMPARE_TRANSIT_SCHEDULE_FILE, "COMPARE_TRANSIT_SCHEDULE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_TRANSIT_SCHEDULE_FORMAT, "COMPARE_TRANSIT_SCHEDULE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ COMPARE_TRANSIT_DRIVER_FILE, "COMPARE_TRANSIT_DRIVER_FILE", LEVEL0, OPT_KEY, IN_KEY, "", "", NO_HELP },
		{ COMPARE_TRANSIT_DRIVER_FORMAT, "COMPARE_TRANSIT_DRIVER_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		{ NEW_TRANSIT_DIFFERENCE_FILE, "NEW_TRANSIT_DIFFERENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", "", NO_HELP },
		{ NEW_TRANSIT_DIFFERENCE_FORMAT, "NEW_TRANSIT_DIFFERENCE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", "", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"FIRST_REPORT",
		"SECOND_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (keys);
	Report_List (reports);

	driver_flag = false;
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TransitDiff *exe = new TransitDiff ();

	return (exe->Start_Execution (commands, control));
}

