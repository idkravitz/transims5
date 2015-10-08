//*********************************************************
//	PlanSum.cpp - travel plan summary utility
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	PlanSum constructor
//---------------------------------------------------------

PlanSum::PlanSum (void) : Data_Service (), Select_Service (), Flow_Time_Service ()
{
	Program ("PlanSum");
	Version (20);

	Title ("Travel Plan Summary Utility");

	System_File_Type required_files [] = {
		PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		NODE, LINK, CONNECTION, LANE_USE, LOCATION, PARKING, ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		SELECTION, VEHICLE_TYPE, LINK_DELAY, NEW_LINK_DELAY, NEW_RIDERSHIP, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, ZONE_EQUIVALENCE_FILE, 
		LINK_EQUIVALENCE_FILE, STOP_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, PERIOD_CONTROL_POINT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_LINKS, SELECT_NODES,
		SELECT_STOPS, SELECT_ROUTES, SELECT_SUBAREA_POLYGON, SELECTION_PERCENTAGE, 0
	};
	int flow_time_service_keys [] = {
		UPDATE_FLOW_RATES, CLEAR_INPUT_FLOW_RATES, UPDATE_TURNING_MOVEMENTS, UPDATE_TRAVEL_TIMES, 
		LINK_DELAY_FLOW_FACTOR, EQUATION_PARAMETERS, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_TRIP_TIME_FILE, "NEW_TRIP_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_VOLUME_FILE, "NEW_LINK_VOLUME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOP_100_V/C_RATIOS",
		"ALL_V/C_RATIOS_GREATER_THAN_*",
		"LINK_GROUP_V/C_RATIOS_*",
		"ZONE_EQUIVALENCE",
		"LINK_EQUIVALENCE",
		"STOP_EQUIVALENCE",
		"TRANSIT_RIDERSHIP_SUMMARY",
		"TRANSIT_STOP_SUMMARY",
		"TRANSIT_TRANSFER_SUMMARY",
		"TRANSIT_TRANSFER_DETAILS",
		"TRANSIT_STOP_GROUP_SUMMARY",
		"TRANSIT_PASSENGER_SUMMARY",
		"TRANSIT_LINK_GROUP_SUMMARY",
		"TRIP_TIME_REPORT",
		"TRAVEL_SUMMARY_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);
	Flow_Time_Service_Keys (flow_time_service_keys);

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
	num_process = max_rail = 0;

	select_flag = new_delay_flag = turn_flag = skim_flag = trip_flag = zone_flag = time_flag = travel_flag = false; 
	volume_flag = passenger_flag = transfer_flag = rider_flag = xfer_flag = xfer_detail = false;
	cap_factor = 0.25;
	minimum_vc = 1.0;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	PlanSum destructor
//---------------------------------------------------------

PlanSum::~PlanSum (void)
{
	if (plan_processing != 0) {
		for (int i=0; i < num_process; i++) {
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
	PlanSum *exe = new PlanSum ();

	return (exe->Start_Execution (commands, control));
}

