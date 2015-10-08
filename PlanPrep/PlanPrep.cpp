//*********************************************************
//	PlanPrep.cpp - travel plan processing utility
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	PlanPrep constructor
//---------------------------------------------------------

PlanPrep::PlanPrep (void) : Data_Service (), Select_Service ()
{
	Program ("PlanPrep");
	Version (9);
	Title ("Travel Plan Processing Utility");

	System_File_Type required_files [] = {
		PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		SELECTION, NEW_PLAN, END_FILE
	};
	int data_service_keys [] = {
		TRIP_SORT_TYPE, PLAN_SORT_TYPE, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_LINKS, SELECT_NODES, 
		SELECT_SUBAREA_POLYGON, SELECTION_PERCENTAGE, DELETION_FILE, DELETION_FORMAT, 
		DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MERGE_PLAN_FILE, "MERGE_PLAN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", PARTITION_RANGE, NO_HELP },
		{ MERGE_PLAN_FORMAT, "MERGE_PLAN_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MAXIMUM_SORT_SIZE, "MAXIMUM_SORT_SIZE", LEVEL0, OPT_KEY, INT_KEY, "0", "0, >=100000 trips", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		//"FIRST_REPORT",
		//"SECOND_REPORT",
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
	plan_processing = 0;

	sort_size = 0;
	select_flag = merge_flag = combine_flag = output_flag = new_plan_flag = false;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	PlanPrep destructor
//---------------------------------------------------------

PlanPrep::~PlanPrep (void)
{
	if (plan_processing != 0) {
		for (int i=0; i < Num_Threads (); i++) {
			delete plan_processing [i];
			plan_processing [i] = 0;
		}
		delete plan_processing;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanPrep *exe = new PlanPrep ();

	return (exe->Start_Execution (commands, control));
}

