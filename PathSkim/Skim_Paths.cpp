//*********************************************************
//	Skim_Paths.cpp - process the path skimming commands
//*********************************************************

#include "PathSkim.hpp"

#include "Skim_Processor.hpp"

//---------------------------------------------------------
//	Skim_Paths
//---------------------------------------------------------

void PathSkim::Skim_Paths (void)
{
	int mode, period, periods, out_period, last_period;
	int org_locs, des_locs, num_periods, num_cells;
	Dtime low, high;

	One_To_Many *skim_ptr;
	Many_Data many_data;
	Many_Itr many_itr;

	Location_Itr loc_itr;
	Int_Itr org_itr, des_itr;
	
	Skim_Processor skim_processor;

	org_locs = (int) org_loc.size ();
	des_locs = (int) des_loc.size ();

	if (Single_Partition ()) {
		num_periods = 1;
	} else {
		num_periods = skim_file->Num_Periods ();
		if (First_Partition () >= 0 && num_periods > Num_Partitions ()) num_periods = Num_Partitions ();
		if (num_periods > Num_Partitions ()) num_periods = Num_Partitions ();
	}
	num_cells = org_locs * des_locs * num_periods;
	cells_out = 0;

	Break_Check (5);
	Print (1);
	Write (1, "Number of Selected Origin Locations = ") << org_locs;
	Write (1, "Number of Selected Destinations     = ") << des_locs;
	Write (1, "Number of Specified Time Periods    = ") << num_periods;
	Write (1, "Number of Potential Skim Cells      = ") << num_cells;

	Show_Message (2, "Skimming Specified Paths -- Record");
	Set_Progress ();

	if (mode_flag) {
		mode = new_mode;
	} else {
		mode = DRIVE_MODE;
	}
	periods = route_periods.Num_Periods ();
	last_period = -1;

	//---- initialize the skim processor ----

	skim_processor.Initialize (this, Num_Threads ());

	skim_processor.Start_Processing ();

	//---- loop through each time increment ----

	for (period = 0; period < periods; period++) {
		if (!route_periods.Period_Range (period, low, high)) continue;

		out_period = skim_file->Period ((forward_flag) ? low : (high - 1));
		if (out_period < 0) continue;
		if (Partition_Index (out_period) < 0) continue;

		//---- save the last time period ----

		if (out_period != last_period) {
			if (last_period >= 0) {
				End_Progress ();
				skim_processor.Stop_Processing ();

				Output_Skims (last_period);

				Show_Message ("Skimming Specified Paths -- Record");
				Set_Progress ();
				skim_processor.Start_Processing ();
			}
			last_period = out_period;
		}

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

	if (last_period >= 0) {
		Output_Skims (last_period);
	}
	Break_Check (5);
	Print (1);
	Write (1, "Number of Output Origin Locations = ") << org_out.size ();
	if (org_locs > 0) Write (0, String (" (%.1lf%%)") % (100.0 * org_out.size () / org_locs) % FINISH);
	Write (1, "Number of Output Destinations     = ") << des_out.size ();
	if (des_locs > 0) Write (0, String (" (%.1lf%%)") % (100.0 * des_out.size () / des_locs) % FINISH);
	Write (1, "Number of Output Time Periods     = ") << per_out.size ();
	if (num_periods > 0) Write (0, String (" (%.1lf%%)") % (100.0 * per_out.size () / num_periods) % FINISH);
	Write (1, "Number of Output Skim Cells       = ") << cells_out;
	if (num_cells > 0) Write (0, String (" (%.1lf%%)") % (100.0 * cells_out / num_cells) % FINISH);
}
