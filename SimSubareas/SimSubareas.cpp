//*********************************************************
//	SimSubareas.cpp - generate geographic subareas
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	SimSubareas constructor
//---------------------------------------------------------

SimSubareas::SimSubareas (void) : Data_Service ()
{
	Program ("SimSubareas");
	Version (4);
	Title ("Generate Geographic Subareas");

	System_File_Type required_files [] = {
		NODE, NEW_NODE, END_FILE
	};
	System_File_Type optional_files [] = {
		END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NUMBER_OF_SUBAREAS, "NUMBER_OF_SUBAREAS", LEVEL0, REQ_KEY, INT_KEY, "8", "1..1000", NO_HELP },
		{ PARTITIONS_PER_SUBAREA, "PARTITIONS_PER_SUBAREA", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000", NO_HELP },
		{ CENTER_NODE_NUMBER, "CENTER_NODE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "0", ">=0", NO_HELP },
		{ SUBAREA_BOUNDARY_FILE, "SUBAREA_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (keys);

	num_subareas = num_parts = center = 0;
	subarea_field = -1;
	boundary_flag = false;
}

//---------------------------------------------------------
//	SimSubareas destructor
//---------------------------------------------------------

SimSubareas::~SimSubareas (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SimSubareas *exe = new SimSubareas ();

	return (exe->Start_Execution (commands, control));
}

