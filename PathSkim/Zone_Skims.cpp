//*********************************************************
//	Zone_Skims.cpp - aggregate zone skim data
//*********************************************************

#include "PathSkim.hpp"

#include "Skim_Processor.hpp"

//---------------------------------------------------------
//	Zone_Skims
//---------------------------------------------------------

void PathSkim::Zone_Skims (void)
{
	int mode, period, periods, out_period, last_period, loc, zone;
	int org_zones, des_zones, num_periods, num_cells;
	Dtime low, high;

	Path_Loc_Array path_locations;
	Path_Loc_Itr   path_loc_itr;
	Path_Locations *path_loc_ptr, path_loc_rec;

	Ints_Map_Itr org_itr, des_itr;
	Int_Itr from_itr, to_itr, int_itr;

	One_To_Many *skim_ptr;
	Many_Data many_data;
	Many_Itr many_itr;

	Skim_Processor skim_processor;

	org_zones = (int) org_zone_loc.size ();
	des_zones = (int) des_zone_loc.size ();

	if (org_zones == 0 && des_zones == 0) {
		Error ("No Zone Locations were Selected");
	}
	if (org_zones == 0) {
		Error ("No Origin Zone Locations were Selected");
	}
	if (des_zones == 0) {
		Error ("No Destination Zone Locations were Selected");
	}
	if (!skim_file->Set_Table (org_zones, des_zones)) {
		Error (String ("Insufficient Memory for %dx%d Skim Matrix") % org_zones % des_zones);
	}
	skim_file->Write_Header ();

	if (Single_Partition () || skim_file->Merge_Flag ()) {
		num_periods = 1;
	} else {
		num_periods = skim_file->Num_Periods ();

		if (First_Partition () >= 0 && num_periods > Num_Partitions ()) num_periods = Num_Partitions ();
	}
	num_cells = org_zones * des_zones * num_periods;
	cells_out = 0;

	Break_Check (5);
	Print (1);
	Write (1, "Number of Selected Origin Zones  = ") << org_zones;
	Write (1, "Number of Selected Destinations  = ") << des_zones;
	Write (1, "Number of Specified Time Periods = ") << num_periods;
	Write (1, "Number of Potential Skim Cells   = ") << num_cells;

	//---- select activity locations within each zone ----

	Show_Message (2, "Building the Zone Path List -- Record");
	Set_Progress ();
	path_loc_rec.from_zone = 0;

	if (forward_flag) {
		path_locations.assign (org_zones, path_loc_rec);

		for (loc=0, org_itr = org_zone_loc.begin (); org_itr != org_zone_loc.end (); org_itr++, loc++) {
			path_loc_ptr = &path_locations [loc];
			zone = org_itr->first;

			path_loc_ptr->from_zone = zone;
			path_loc_ptr->from_loc.assign (org_itr->second.begin (), org_itr->second.end ());

			for (des_itr = des_zone_loc.begin (); des_itr != des_zone_loc.end (); des_itr++) {
				Show_Progress ();
				zone = des_itr->first;

				for (int_itr = des_itr->second.begin (); int_itr != des_itr->second.end (); int_itr++) {
					path_loc_ptr->to_loc.push_back (*int_itr);
					path_loc_ptr->to_zone.push_back (zone);
				}
			}
		}
	} else {
		path_locations.assign (des_zones, path_loc_rec);

		for (loc=0, des_itr = des_zone_loc.begin (); des_itr != des_zone_loc.end (); des_itr++, loc++) {
			path_loc_ptr = &path_locations [loc];
			zone = des_itr->first;
			
			path_loc_ptr->from_zone = zone;
			path_loc_ptr->from_loc.assign (des_itr->second.begin (), des_itr->second.end ());

			for (org_itr = org_zone_loc.begin (); org_itr != org_zone_loc.end (); org_itr++) {
				Show_Progress ();
				zone = org_itr->first;

				for (int_itr = org_itr->second.begin (); int_itr != org_itr->second.end (); int_itr++) {
					path_loc_ptr->to_loc.push_back (*int_itr);
					path_loc_ptr->to_zone.push_back (zone);
				}
			}
		}
	}
	End_Progress ();

	//---- initialize the data ----

	Show_Message ("Aggregating Zone Skims -- Record");
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

		if (out_period != last_period) {
			if (last_period >= 0) {
				End_Progress ();
				//skim_processor.Stop_Processing ();
				skim_processor.Complete_Work ();

				Output_Skims (last_period);

				Show_Message ("Aggregating Zone Skims -- Record");
				Set_Progress ();
				//skim_processor.Start_Processing ();
				skim_processor.Start_Work ();
			}
			last_period = out_period;
		}

		//---- build paths from each location ----

		for (path_loc_itr = path_locations.begin (); path_loc_itr != path_locations.end (); path_loc_itr++) {
			if (path_loc_itr->from_zone == 0) break;

			//---- loop through each from location ----

			for (from_itr = path_loc_itr->from_loc.begin (); from_itr != path_loc_itr->from_loc.end (); from_itr++) {
				skim_ptr = new One_To_Many ();

				skim_ptr->Location (*from_itr);
				skim_ptr->Time ((forward_flag) ? low : high);
				skim_ptr->Mode (mode);
				skim_ptr->Use (use_type);
				skim_ptr->Veh_Type (veh_type);
				skim_ptr->Type (traveler_type);
				skim_ptr->Direction (constraint);
				skim_ptr->Zone (path_loc_itr->from_zone);
				skim_ptr->Period (0);

				//---- loop through each destination location ----

				for (loc=0, to_itr = path_loc_itr->to_loc.begin (); to_itr != path_loc_itr->to_loc.end (); to_itr++, loc++) {
					Show_Progress ();

					//??? intrazonal ???

					many_data.Clear ();
					many_data.Location (*to_itr);
					many_data.Zone (path_loc_itr->to_zone [loc]);

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
	Write (1, "Number of Output Origin Zones = ") << org_out.size ();
	if (org_zones > 0) Write (0, String (" (%.1lf%%)") % (100.0 * org_out.size () / org_zones) % FINISH);
	Write (1, "Number of Output Destinations = ") << des_out.size ();
	if (des_zones > 0) Write (0, String (" (%.1lf%%)") % (100.0 * des_out.size () / des_zones) % FINISH);
	Write (1, "Number of Output Time Periods = ") << per_out.size ();
	if (num_periods > 0) Write (0, String (" (%.1lf%%)") % (100.0 * per_out.size () / num_periods) % FINISH);
	Write (1, "Number of Output Skim Cells   = ") << cells_out;
	if (num_cells > 0) Write (0, String (" (%.1lf%%)") % (100.0 * cells_out / num_cells) % FINISH);
}
