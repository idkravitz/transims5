//*********************************************************
//	IntControl.cpp - Intersection Control Utility
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	IntControl constructor
//---------------------------------------------------------

IntControl::IntControl (void) : Data_Service ()
{
	Program ("IntControl");
	Version (3);
	Title ("Intersection Traffic Controls");

	System_File_Type required_files [] = {
		NODE, LINK, POCKET, CONNECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, LANE_USE, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR,
		NEW_SIGN, NEW_SIGNAL, NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR, 
		LINK_DELAY, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key intcontrol_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ INPUT_SIGN_FILE, "INPUT_SIGN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_SIGNAL_FILE, "INPUT_SIGNAL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_NODE_CONTROL_FILE, "DELETE_NODE_CONTROL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PRINT_SIGN_WARNINGS, "PRINT_SIGN_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PRINT_MERGE_WARNINGS, "PRINT_MERGE_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SIGNAL_TYPE_CODE, "SIGNAL_TYPE_CODE", LEVEL1, OPT_KEY, LIST_KEY, "TIMED", "TIMED or ACTUATED", NO_HELP },
		{ NUMBER_OF_RINGS, "NUMBER_OF_RINGS", LEVEL1, OPT_KEY, LIST_KEY, "1", "1..4", NO_HELP },		
		{ SIGNAL_TIME_BREAKS, "SIGNAL_TIME_BREAKS", LEVEL1, OPT_KEY, LIST_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ SIGNAL_CYCLE_LENGTH, "SIGNAL_CYCLE_LENGTH", LEVEL1, OPT_KEY, LIST_KEY, "60 seconds", "30..360 seconds", NO_HELP },
		{ MINIMUM_PHASE_TIME, "MINIMUM_PHASE_TIME", LEVEL1, OPT_KEY, LIST_KEY, "5 seconds", "0..30 seconds", NO_HELP },
		{ YELLOW_PHASE_TIME, "YELLOW_PHASE_TIME", LEVEL1, OPT_KEY, LIST_KEY, "3 seconds", "0..6 seconds", NO_HELP },
		{ RED_CLEAR_PHASE_TIME, "RED_CLEAR_PHASE_TIME", LEVEL1, OPT_KEY, LIST_KEY, "0 seconds", "0..4 seconds", NO_HELP },
		{ SIGNAL_SPLIT_METHOD, "SIGNAL_SPLIT_METHOD", LEVEL1, OPT_KEY, LIST_KEY, "CAPACITY", "LANES, CAPACITY, VOLUME, UPDATE", NO_HELP },
		{ MINIMUM_LANE_CAPACITY, "MINIMUM_LANE_CAPACITY", LEVEL1, OPT_KEY, INT_KEY, "500", "0..1000 vphpl", NO_HELP },
		{ MAXIMUM_LANE_CAPACITY, "MAXIMUM_LANE_CAPACITY", LEVEL1, OPT_KEY, INT_KEY, "1500", "100..3000 vphpl", NO_HELP },
		{ POCKET_LANE_FACTOR, "POCKET_LANE_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "0.5", "0.2..1.0", NO_HELP },
		{ SHARED_LANE_FACTOR, "SHARED_LANE_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "0.5", "0.2..1.0", NO_HELP },
		{ TURN_MOVEMENT_FACTOR, "TURN_MOVEMENT_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "0.9", "0.2..1.0", NO_HELP },
		{ PERMITTED_LEFT_FACTOR, "PERMITTED_LEFT_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "0.5", "0.2..1.0", NO_HELP },
		{ GENERAL_GREEN_FACTOR, "GENERAL_GREEN_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.7..1.0", NO_HELP },
		{ EXTENDED_GREEN_FACTOR, "EXTENDED_GREEN_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "0.5", "0.1..1.0", NO_HELP },
		{ MAXIMUM_GREEN_FACTOR, "MAXIMUM_GREEN_FACTOR", LEVEL1, OPT_KEY, LIST_KEY, "2.0", "0.2..10.0", NO_HELP },
		{ SIGNAL_DETECTOR_LENGTH, "SIGNAL_DETECTOR_LENGTH", LEVEL1, OPT_KEY, FLOAT_KEY, "20 meters", "5..50 meters", NO_HELP },
		{ TURN_VOLUME_FILE, "TURN_VOLUME_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"SIGNAL_TIMING_UPDATES",
		""
	};

	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (intcontrol_keys);
	Report_List (reports);

	Sum_Flow_Flag (true);

	delay_flag = range_flag = no_control_flag = signal_flag = sign_flag = delete_flag = update_flag = false;
	detector_flag = coord_flag = update_report = regen_flag = volume_flag = false;
	merge_flag = warning_flag = input_sign_flag = input_signal_flag = time_flag = turn_flag = false;
	num_new = num_update = nsign = ntiming = nsignal = nphasing = ndetector = 0;
}

//---------------------------------------------------------
//	IntControl destructor
//---------------------------------------------------------

IntControl::~IntControl (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	IntControl *exe = new IntControl ();

	return (exe->Start_Execution (commands, control));
}

