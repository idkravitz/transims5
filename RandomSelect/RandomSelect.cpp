//*********************************************************
//	RandomSelect.cpp - randomly distribute to partitions
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	RandomSelect constructor
//---------------------------------------------------------

RandomSelect::RandomSelect (void) : Data_Service (), Select_Service ()
{
	Program ("RandomSelect");
	Version (1);
	Title ("Randomly Distribute to Partitions");

	System_File_Type required_files [] = {
		NEW_SELECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		HOUSEHOLD, TRIP, END_FILE
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECTION_PERCENTAGE, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NUMBER_OF_PARTITIONS, "NUMBER_OF_PARTITIONS", LEVEL0, REQ_KEY, INT_KEY, "8", "1..999", NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);

	trip_flag = false;
	num_parts = 0;

	System_Data_Reserve (HOUSEHOLD, 0);
	System_Data_Reserve (TRIP, 0);
}

//---------------------------------------------------------
//	RandomSelect destructor
//---------------------------------------------------------

RandomSelect::~RandomSelect (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	RandomSelect *exe = new RandomSelect ();

	return (exe->Start_Execution (commands, control));
}

