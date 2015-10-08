//*********************************************************
//	Zone_Locations.cpp - select locations within zones
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Zone_Locations
//---------------------------------------------------------

void PathSkim::Zone_Locations (void)
{
	int loc, zone, num_zone;

	Ints_Map all_org, all_des;
	Ints_Map_Itr map_itr;
	Ints_Map_Stat loc_stat;
	Integers locations, *int_ptr;
	Int_Itr from_itr, to_itr, int_itr;
	Int_Map_Itr loc_map_itr;
	Location_Itr loc_itr;
	Zone_Data *zone_ptr;

	//---- build a zone number list ----

	num_zone = Max_Zone_Number ();
	if (num_zone == 0) {
		Error ("No Zones were found in the Activity Location File");
	}
	if (zone_loc_flag) {
		map_itr = --zone_loc_map.end ();
		if (map_itr->first > num_zone) num_zone = map_itr->first;
	}
	num_zone++;
	warn_flag.assign (num_zone, 0);

	Show_Message (2, "Building the Location List -- Record");
	Set_Progress ();

	//---- process the location data ----

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
		Show_Progress ();
		if (zone_flag && loc_itr->Zone () >= 0) {
			zone_ptr = &zone_array [loc_itr->Zone ()];
			zone = zone_ptr->Zone ();
		} else {
			zone = loc_itr->Zone ();
		}
		if (zone <= 0) continue;
		loc = loc_itr->Location ();

		if (!sel_org_zone || org_zone_range.In_Range (zone)) {
			if (!select_org || org_range.In_Range (loc)) {
				loc_stat = all_org.insert (Ints_Map_Data (zone, locations));
				loc_stat.first->second.push_back (loc);
			}
		}
		if (!sel_des_zone || des_zone_range.In_Range (zone)) {
			if (!select_des || des_range.In_Range (loc)) {
				loc_stat = all_des.insert (Ints_Map_Data (zone, locations));
				loc_stat.first->second.push_back (loc);
			}
		}
	}
	if (zone_loc_flag) {
		for (map_itr = zone_loc_map.begin (); map_itr != zone_loc_map.end (); map_itr++) {
			Show_Progress ();
			zone = map_itr->first;
			int_ptr = &(map_itr->second);

			for (int_itr = int_ptr->begin (); int_itr != int_ptr->end (); int_itr++) {
				loc = *int_itr;

				if (!sel_org_zone || org_zone_range.In_Range (zone)) {
					if (!select_org || org_range.In_Range (loc)) {
						loc_stat = all_org.insert (Ints_Map_Data (zone, locations));
						loc_stat.first->second.push_back (loc);
					}
				}
				if (!sel_des_zone || des_zone_range.In_Range (zone)) {
					if (!select_des || des_range.In_Range (loc)) {
						loc_stat = all_des.insert (Ints_Map_Data (zone, locations));
						loc_stat.first->second.push_back (loc);
					}
				}
			}
		}
	}

	//---- select origin locations ----

	for (map_itr = all_org.begin (); map_itr != all_org.end (); map_itr++) {
		Show_Progress ();
		zone = map_itr->first;

		if (Location_List (zone, num_org, &map_itr->second, locations)) {
			if (zone_skim_flag) {
				org_zone_loc.insert (Ints_Map_Data (zone, locations));
				skim_file->Add_Org (zone);
			} else if (district_flag) {
				zone = zone_equiv.Zone_Group (zone);
				loc_stat = org_zone_loc.insert (Ints_Map_Data (zone, locations));

				if (!loc_stat.second) {
					int_ptr = &loc_stat.first->second;
					int_ptr->insert (int_ptr->end (), locations.begin (), locations.end ());
				} else {
					skim_file->Add_Org (zone);
				}
			} else {
				org_loc.insert (org_loc.end (), locations.begin (), locations.end ());
				if (skim_flag) {
					for (int_itr = locations.begin (); int_itr != locations.end (); int_itr++) {
						skim_file->Add_Org (*int_itr);
					}
				}
			}
		}
	}

	//---- select destination locations ----

	for (map_itr = all_des.begin (); map_itr != all_des.end (); map_itr++) {
		Show_Progress ();
		zone = map_itr->first;

		if (Location_List (zone, num_des, &map_itr->second, locations)) {
			if (zone_skim_flag) {
				des_zone_loc.insert (Ints_Map_Data (zone, locations));
				skim_file->Add_Des (zone);
			} else if (district_flag) {
				zone = zone_equiv.Zone_Group (zone);
				loc_stat = des_zone_loc.insert (Ints_Map_Data (zone, locations));

				if (!loc_stat.second) {
					int_ptr = &loc_stat.first->second;
					int_ptr->insert (int_ptr->end (), locations.begin (), locations.end ());
				} else {
					skim_file->Add_Des (zone);
				}
			} else {
				des_loc.insert (des_loc.end (), locations.begin (), locations.end ());
				if (skim_flag) {
					for (int_itr = locations.begin (); int_itr != locations.end (); int_itr++) {
						skim_file->Add_Des (*int_itr);
					}
				}
			}
		}
	}
	End_Progress ();
}
