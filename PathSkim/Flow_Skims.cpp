//*********************************************************
//	Flow_Skims.cpp - build flow-only skims
//*********************************************************

#include "PathSkim.hpp"

#include "Skim_Processor.hpp"

//---------------------------------------------------------
//	Flow_Skims
//---------------------------------------------------------

void PathSkim::Flow_Skims (void)
{
	int mode, period, periods;
	size_t paths;
	Dtime low, high;

	One_To_Many *skim_ptr;
	Many_Data many_data;
	Many_Itr many_itr;

	Location_Itr loc_itr;
	Int_Itr org_itr, des_itr;
	
	Skim_Processor skim_processor;

	periods = route_periods.Num_Periods ();
	paths = org_loc.size () * des_loc.size () * periods;

	Break_Check (4);
	Print (1);
	Write (1, "Number of Selected Origin Locations = ") << org_loc.size ();
	Write (1, "Number of Selected Destinations     = ") << des_loc.size ();
	Write (1, "Number of Specified Time Periods    = ") << periods;
	Write (1, "Number of Potential Path Traces     = ") << paths;

	Show_Message (2, "Tracing Specified Paths -- Record");
	Set_Progress ();

	if (mode_flag) {
		mode = new_mode;
	} else {
		mode = DRIVE_MODE;
	}

	//---- initialize the skim processor ----

	skim_processor.Initialize (this, Num_Threads ());

	skim_processor.Start_Processing ();

	//---- loop through each time increment ----

	for (period = 0; period < periods; period++) {
		if (!route_periods.Period_Range (period, low, high)) continue;

		if (forward_flag) {

			//---- loop through each origin location ----

			for (org_itr = org_loc.begin (); org_itr != org_loc.end (); org_itr++) {
				skim_ptr = new One_To_Many ();

				skim_ptr->Location (*org_itr);
				skim_ptr->Time (low);
				skim_ptr->Mode (mode);
				skim_ptr->Use (use_type);
				skim_ptr->Veh_Type (veh_type);
				skim_ptr->Type (traveler_type);
				skim_ptr->Direction (constraint);

				//---- loop through each destination location ----

				for (des_itr = des_loc.begin (); des_itr != des_loc.end (); des_itr++) {
					if (*org_itr == *des_itr) continue;
					Show_Progress ();

					many_data.Clear ();
					many_data.Location (*des_itr);

					skim_ptr->push_back (many_data);
				}
				if (skim_ptr->size () > 0) {
					skim_processor.Skim_Build (skim_ptr);
				} else {
					delete skim_ptr;
				}
			}

		} else {

			//---- loop through each destination location ----

			for (des_itr = des_loc.begin (); des_itr != des_loc.end (); des_itr++) {
				skim_ptr = new One_To_Many ();

				skim_ptr->Location (*des_itr);
				skim_ptr->Time (high);
				skim_ptr->Mode (mode);
				skim_ptr->Use (use_type);
				skim_ptr->Veh_Type (veh_type);
				skim_ptr->Type (traveler_type);
				skim_ptr->Direction (constraint);

				//---- loop through each origin location ----

				for (org_itr = org_loc.begin (); org_itr != org_loc.end (); org_itr++) {
					if (*org_itr == *des_itr) continue;
					Show_Progress ();

					many_data.Clear ();
					many_data.Location (*org_itr);

					skim_ptr->push_back (many_data);
				}
				if (skim_ptr->size () > 0) {
					skim_processor.Skim_Build (skim_ptr);
				} else {
					delete skim_ptr;
				}
			}
		}
	}
	End_Progress ();
	skim_processor.Stop_Processing ();
}
