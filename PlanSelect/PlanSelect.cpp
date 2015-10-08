//*********************************************************
//	PlanSelect.cpp - travel plan selection utility
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	PlanSelect constructor
//---------------------------------------------------------

PlanSelect::PlanSelect (void) : Data_Service (), Select_Service ()
{
	Program ("PlanSelect");
	Version (12);
	Title ("Travel Plan Selection Utility");

	System_File_Type required_files [] = {
		PLAN, NEW_SELECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		NODE, LINK, LANE_USE, CONNECTION, LOCATION, VEHICLE_TYPE, LINK_DELAY, NEW_PLAN, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_FACILITY_TYPES, 
		SELECT_LINKS, SELECT_NODES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, SELECT_SUBAREA_POLYGON, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, DELETION_FILE, DELETION_FORMAT, 
		DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SELECT_PARKING_LOTS, "SELECT_PARKING_LOTS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ SELECT_VC_RATIOS, "SELECT_VC_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, >1.0", NO_HELP },
		{ SELECT_TIME_RATIOS, "SELECT_TIME_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, >1.0", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		//"FIRST_REPORT",
		//"SECOND_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
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

	delay_flag = turn_flag = select_parking, select_vc = select_ratio = new_plan_flag = select_parts = false; 
	num_trips = num_select = 0;
	vc_ratio = time_ratio = 0.0;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	PlanSelect destructor
//---------------------------------------------------------

PlanSelect::~PlanSelect (void)
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
	PlanSelect *exe = new PlanSelect ();

	return (exe->Start_Execution (commands, control));
}

