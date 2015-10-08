//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkDelay::Program_Control (void)
{
	String key;

	//---- open network files ----

	Data_Service::Program_Control ();

	Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
	turn_flag = file->Turn_Flag ();
	
	Print (2, String ("%s Control Keys:") % Program ());	

	//---- open the merge link delay ----
	
	key = Get_Control_String (MERGE_LINK_DELAY_FILE);

	if (!key.empty ()) {
		merge_file.File_Type ("Merge Link Delay File");
		Print (1);

		if (Check_Control_Key (MERGE_LINK_DELAY_FORMAT)) {
			merge_file.Dbase_Format (Get_Control_String (MERGE_LINK_DELAY_FORMAT));
		}
		merge_file.Open (Project_Filename (key));
		merge_flag = true;

		//---- processing method ----

		method = Combine_Code (Get_Control_Text (PROCESSING_METHOD));

		if (method == WEIGHTED_LINK_AVG || method == REPLACE_AVERAGE) {

			//---- merge weighting factor ----

			factor = Get_Control_Double (MERGE_WEIGHTING_FACTOR);
		}

		//---- check the turning movements ----

		if (turn_flag != merge_file.Turn_Flag ()) {
			if (turn_flag) {
				Warning ("Link Delay Files are Not Turning Movement Compatible");
				file->Turn_Flag (false);

				file = (Link_Delay_File *) System_File_Handle (NEW_LINK_DELAY);

				file->Clear_Fields ();
				file->Turn_Flag (false);
				file->Create_Fields ();
				file->Write_Header ();
			}
			turn_flag = false;
		}
	}
	Print (1);

	//---- read the smoothing parameters ----

	smooth_flag = smooth_data.Read_Control ();

	if (smooth_flag) {
		if (!smooth_data.Num_Input (time_periods.Num_Periods ())) {
			Error ("Smooth Parameters are Illogical");
		}
	}

	//---- max travel time ratio ----

	if (Check_Control_Key (MAX_TRAVEL_TIME_RATIO)) {
		Print (1);
		time_ratio = Get_Control_Double (MAX_TRAVEL_TIME_RATIO);
		ratio_flag = (time_ratio > 1.0);
	}

	//---- set min travel time ----

	min_time_flag = Get_Control_Flag (SET_MIN_TRAVEL_TIME);
}
