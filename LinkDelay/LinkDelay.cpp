//*********************************************************
//	LinkDelay.cpp - manipulate the link delay file
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	LinkDelay constructor
//---------------------------------------------------------

LinkDelay::LinkDelay (void) : Data_Service ()
{
	Program ("LinkDelay");
	Version (4);
	Title ("Manipulate the Link Delay File");

	System_File_Type required_files [] = {
		NODE, LINK, LINK_DELAY, NEW_LINK_DELAY, END_FILE
	};
	System_File_Type optional_files [] = {
		CONNECTION, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key linkdelay_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MERGE_LINK_DELAY_FILE, "MERGE_LINK_DELAY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_LINK_DELAY_FORMAT, "MERGE_LINK_DELAY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ PROCESSING_METHOD, "PROCESSING_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "SIMPLE_AVERAGE", COMBINE_RANGE, NO_HELP },
		{ MERGE_WEIGHTING_FACTOR, "MERGE_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0, >= 0.5", NO_HELP },
		{ MAX_TRAVEL_TIME_RATIO, "MAX_TRAVEL_TIME_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, > 1.0", NO_HELP },
		{ SET_MIN_TRAVEL_TIME, "SET_MIN_TRAVEL_TIME", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (linkdelay_keys);

	smooth_data.Add_Keys ();

	smooth_flag = merge_flag = ratio_flag = min_time_flag = false;

	method = SIMPLE_LINK_AVG;
	factor = 1.0;
	time_ratio = 0.0;
}

//---------------------------------------------------------
//	LinkDelay destructor
//---------------------------------------------------------

LinkDelay::~LinkDelay (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkDelay *exe = new LinkDelay ();

	return (exe->Start_Execution (commands, control));
}
