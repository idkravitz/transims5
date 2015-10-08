//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void IntControl::Execute (void)
{

	//---- read the network files ----

	Data_Service::Execute ();

	//---- create the node link list ----

	Node_Links ();

	//---- delete intersection data ----

	if (delete_flag) {
		Read_Deletes ();
	}

	//---- read the turn volumes ----

	if (turn_flag) {
		Read_Turn ();
	}

	//---- read the signal file ----

	if (input_signal_flag) {
		Read_Signal ();
	}

	//---- read the sign file ----

	if (input_sign_flag) {
		Read_Sign ();
	}

	//---- process the sign data ----

	if (sign_flag) {
		Write_Signs ();
	}

	//---- process the signal data ----

	if (signal_flag) {
		if (num_update > 0 || regen_flag) {
			Update_Signals ();
		}
		if (num_new > 0) {
			Create_Signals ();
		}
		if (System_File_Flag (NEW_SIGNAL)) Write_Signals ();
		if (System_File_Flag (NEW_TIMING_PLAN)) Write_Timing_Plans ();
		if (System_File_Flag (NEW_PHASING_PLAN)) Write_Phasing_Plans ();
		if (System_File_Flag (NEW_DETECTOR)) Write_Detectors ();
	}

	//---- processing results ----

	if (sign_flag) {
		Write (2, "Number of Sign Changes = ") << nsign;
	}
	if (signal_flag) {
 		if (update_flag && !delete_flag) {
			Write (2, "Number of Timing Plan Changes = ") << ntiming;
		} else {
			Break_Check (5);
			Write (2, "Number of Signal Changes = ") << nsignal;
			Write (1, "Number of Timing Plan Changes = ") << ntiming;
			Write (1, "Number of Phasing Plan Changes = ") << nphasing;
			Write (1, "Number of Detector Changes = ") << ndetector;
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void IntControl::Page_Header (void)
{
	switch (Header_Number ()) {
		case TIMING_UPDATE:			//---- Signal Timing Update ----
			Update_Header ();
			break;
		default:
			break;
	}
}
