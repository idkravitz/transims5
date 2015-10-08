//*********************************************************
//	PlanCompare.cpp - travel plan comparison utility
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	PlanCompare constructor
//---------------------------------------------------------

PlanCompare::PlanCompare (void) : Data_Service (), Select_Service ()
{
	Program ("PlanCompare");
	Version (13);
	Title ("Travel Plan Comparison Utility");

	System_File_Type required_files [] = {
		PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		SELECTION, NEW_SELECTION, NEW_PLAN, END_FILE
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, PERIOD_CONTROL_POINT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_LINKS, SELECT_NODES, SELECT_SUBAREA_POLYGON, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, PERCENT_COST_DIFFERENCE,
		MINIMUM_COST_DIFFERENCE, MAXIMUM_COST_DIFFERENCE, SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, 
		DELETION_FILE, DELETION_FORMAT, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COMPARE_PLAN_FILE, "COMPARE_PLAN_FILE", LEVEL0, REQ_KEY, IN_KEY, "", PARTITION_RANGE, NO_HELP },
		{ COMPARE_PLAN_FORMAT, "COMPARE_PLAN_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ COMPARE_GENERALIZED_COSTS, "COMPARE_GENERALIZED_COSTS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SELECTION_METHOD, "SELECTION_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "RANDOM", SELECT_METHOD_RANGE, NO_HELP },
		{ MERGE_PLAN_FILES, "MERGE_PLAN_FILES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_TIME_DISTRIBUTION_FILE, "NEW_TIME_DISTRIBUTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_COST_DISTRIBUTION_FILE, "NEW_COST_DISTRIBUTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TIME_GAP_FILE, "NEW_TRIP_TIME_GAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_COST_GAP_FILE, "NEW_TRIP_COST_GAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_MATCH_FILE, "NEW_TRIP_MATCH_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", PARTITION_RANGE, NO_HELP },
		{ NEW_TRIP_MATCH_FORMAT, "NEW_TRIP_MATCH_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOTAL_TIME_DISTRIBUTION",
		"PERIOD_TIME_DISTRIBUTIONS",
		"TOTAL_TIME_SUMMARY",
		"PERIOD_TIME_SUMMARY",
		"TOTAL_COST_DISTRIBUTION",
		"PERIOD_COST_DISTRIBUTIONS",
		"TOTAL_COST_SUMMARY",
		"PERIOD_COST_SUMMARY",
		"TRIP_TIME_GAP_REPORT",
		"TRIP_COST_GAP_REPORT",
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
	plan_output = 0;

	select_flag = cost_flag = time_sum_flag = cost_sum_flag = time_gap_flag = cost_gap_flag = merge_flag = false;
	select_parts = time_sort = match_flag = false;
	method = RANDOM_METHOD;
	num_trips = num_select = 0;

	cost_diff.Cost_Flag (true);
	cost_gap.Cost_Flag (true);

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	PlanCompare destructor
//---------------------------------------------------------

PlanCompare::~PlanCompare (void)
{
	if (plan_processing != 0) {
		for (int i=0; i < Num_Threads (); i++) {
			delete plan_processing [i];
			plan_processing [i] = 0;
		}
		delete plan_processing;
	}
	if (plan_output != 0) {
		for (int i=0; i < Num_Threads (); i++) {
			delete plan_output [i];
			plan_output [i] = 0;
		}
		delete plan_output;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanCompare *exe = new PlanCompare ();

	return (exe->Start_Execution (commands, control));
}

