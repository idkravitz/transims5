//*********************************************************
//	TripPrep.cpp - trip processing utility
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	TripPrep constructor
//---------------------------------------------------------

TripPrep::TripPrep (void) : Data_Service (), Select_Service ()
{
	Program ("TripPrep");
	Version (11);
	Title ("Trip Processing Utility");

	System_File_Type required_files [] = {
		TRIP, END_FILE
	};
	System_File_Type optional_files [] = {
		SELECTION, VEHICLE, NEW_TRIP, NEW_VEHICLE, NEW_SELECTION, 
		NODE, LINK, PARKING, LOCATION, END_FILE
	};
	int data_service_keys [] = {
		TRIP_SORT_TYPE, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECTION_PERCENTAGE, 
		DELETION_FILE, DELETION_FORMAT, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MERGE_TRIP_FILE, "MERGE_TRIP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", PARTITION_RANGE, NO_HELP },
		{ MERGE_TRIP_FORMAT, "MERGE_TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MERGE_VEHICLE_FILE, "MERGE_VEHICLE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", PARTITION_RANGE, NO_HELP },
		{ MERGE_VEHICLE_FORMAT, "MERGE_VEHICLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MAXIMUM_SORT_SIZE, "MAXIMUM_SORT_SIZE", LEVEL0, OPT_KEY, INT_KEY, "0", "0, >=100000 trips", NO_HELP },
		{ UPDATE_TRIP_PARTITIONS, "UPDATE_TRIP_PARTITIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_TRAVELER_TYPE, "UPDATE_TRAVELER_TYPE", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ TRIP_PROCESSING_SCRIPT, "TRIP_PROCESSING_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NUMBER_OF_PARTITIONS, "NUMBER_OF_PARTITIONS", LEVEL0, OPT_KEY, INT_KEY, "8", "1..999", NO_HELP },
		{ SORT_HOUSEHOLD_TOURS, "SORT_HOUSEHOLD_TOURS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SYNTHESIZE_VEHICLES, "SYNTHESIZE_VEHICLES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ CHECK_ACTIVITY_PATTERNS, "CHECK_ACTIVITY_PATTERNS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"PROCESSING_SCRIPT",
		"PROCESSING_STACK",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

#ifdef BOOST_THREADS
	Enable_Threads (true);
#endif
#ifdef MPI_EXE
	Enable_MPI (true);
#endif
	trip_processing = 0;

	sort_size = check_count = error_count = 0;
	select_flag = merge_flag = combine_flag = output_flag = new_trip_flag = new_select_flag = false;
	update_flag = type_flag = script_flag = vehicle_flag = merge_veh_flag = veh_part_flag = false;
	sort_tours = make_veh_flag = check_flag = false;

	new_veh_file = 0;

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);
}

//---------------------------------------------------------
//	TripPrep destructor
//---------------------------------------------------------

TripPrep::~TripPrep (void)
{
	if (trip_processing != 0) {
		for (int i=0; i < Num_Threads (); i++) {
			delete trip_processing [i];
			trip_processing [i] = 0;
		}
		delete trip_processing;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TripPrep *exe = new TripPrep ();

	return (exe->Start_Execution (commands, control));
}

