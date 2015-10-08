//*********************************************************
//	Simulator.cpp - Simulate Travel Plans
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Simulator constructor
//---------------------------------------------------------

Simulator::Simulator (void) : Simulator_Base ()
{
	Program ("Simulator");
	Version (49);
	Title ("Simulate Travel Plans");

	System_File_Type required_files [] = {
		NODE, LINK, POCKET, CONNECTION, PARKING, LOCATION, 
		VEHICLE_TYPE, VEHICLE, PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		ACCESS_LINK, LANE_USE, TURN_PENALTY, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		HOUSEHOLD, NEW_PROBLEM, END_FILE
	};
	Control_Key msim_keys [] = { //--- code, key, level, status, type, help ----
#ifdef DEBUG_KEYS
		{ DEBUG_TIME_RANGE, "DEBUG_TIME_RANGE", LEVEL0, OPT_KEY, TIME_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ DEBUG_VEHICLE_LIST, "DEBUG_VEHICLE_LIST", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ DEBUG_LINK_LIST, "DEBUG_LINK_LIST", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
#endif
		END_CONTROL
	};
	const char *reports [] = {
		"HOUSEHOLD_TYPE_SCRIPT",
		"HOUSEHOLD_TYPE_STACK",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);

	Router_Service_Keys ();
	Simulator_Service_Keys ();
	Key_List (msim_keys);
	Report_List (reports);

#ifdef MPI_EXE
	Enable_MPI (true);
#endif
	step_flag = false;
	count_flag = true;

	lane_change_levels = 10;
	max_vehicles = 0;
	step = max_time = 0;
	signal_update_time = timing_update_time = transit_update_time = 0;
	one_second = Dtime (1, SECONDS);
	one_minute = Dtime (60, SECONDS);
	one_hour = Dtime (3600, SECONDS);

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
	Transit_Veh_Flag (true);
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Simulator *exe = new Simulator ();

	return (exe->Start_Execution (commands, control));
}


