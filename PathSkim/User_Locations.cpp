//*********************************************************
//	User_Locations.cpp - read the user location files
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	User_Locations
//---------------------------------------------------------

void PathSkim::User_Locations (void)
{
	int num, zone, n1, n2, count;
	double low, high;

	String text;
	Strings ranges;
	Str_Itr range_itr;
	Ints_Map_Stat zone_loc_stat;
	Integers locations, *loc_ptr;

	Show_Message (2, "Reading User Location Files -- Record");
	Set_Progress ();

	loc_ptr = 0;
	count = 0;

	//---- read the origin locations ----

	while (user_org_file.Read ()) {
		Show_Progress ();

		text = user_org_file.Record_String ();
		if (text.empty ()) continue;

		text.Parse (ranges);

		for (range_itr = ranges.begin (); range_itr != ranges.end (); range_itr++) {

			//---- create the zone index ----

			if (range_itr == ranges.begin ()) {
				zone = range_itr->Integer ();
				if (zone == 0) break;
				if (sel_org_zone && !org_zone_range.In_Range (zone)) break;

				if (zone_skim_flag || district_flag) {
					if (district_flag) zone = zone_equiv.Zone_Group (zone);
					skim_file->Add_Org (zone);

					zone_loc_stat = org_zone_loc.insert (Ints_Map_Data (zone, locations));
					loc_ptr = &zone_loc_stat.first->second;
				}
				count = 0;
				continue;
			}

			//---- parse location records ----

			if (!range_itr->Range (low, high)) continue;
			n1 = (int) low;
			n2 = (int) high;

			if (n1 == 0 && n2 == 0) continue;

			if (n1 > n2) {
				Warning (String ("Location Range %d..%d is Illogical") % n1 % n2); 
				continue;
			}
			for (num = n1; num <= n2; num++) {
				if (num_org > 0 && count > num_org) break;
				count++;

				if (zone_skim_flag || district_flag) {
					loc_ptr->push_back (num);
				} else {
					org_loc.push_back (num);
					if (skim_flag) skim_file->Add_Org (num);
				}
			}
			if (num_org > 0 && count > num_org) break;
		}
	}
	user_org_file.Close ();

	//---- read the destination locations ----

	while (user_des_file.Read ()) {
		Show_Progress ();

		text = user_des_file.Record_String ();
		if (text.empty ()) continue;

		text.Parse (ranges);

		for (range_itr = ranges.begin (); range_itr != ranges.end (); range_itr++) {

			//---- create the zone index ----

			if (range_itr == ranges.begin ()) {
				zone = range_itr->Integer ();
				if (zone == 0) break;
				if (sel_des_zone && !des_zone_range.In_Range (zone)) break;

				if (zone_skim_flag || district_flag) {
					if (district_flag) zone = zone_equiv.Zone_Group (zone);
					skim_file->Add_Des (zone);

					zone_loc_stat = des_zone_loc.insert (Ints_Map_Data (zone, locations));
					loc_ptr = &zone_loc_stat.first->second;
				}
				count = 0;
				continue;
			}

			//---- parse location records ----

			if (!range_itr->Range (low, high)) continue;
			n1 = (int) low;
			n2 = (int) high;

			if (n1 == 0 && n2 == 0) continue;

			if (n1 > n2) {
				Warning (String ("Location Range %d..%d is Illogical") % n1 % n2); 
				continue;
			}
			for (num = n1; num <= n2; num++) {
				if (num_org > 0 && count > num_org) break;
				count++;

				if (zone_skim_flag || district_flag) {
					loc_ptr->push_back (num);
				} else {
					des_loc.push_back (num);
					if (skim_flag) skim_file->Add_Des (num);
				}
			}
			if (num_des > 0 && count > num_des) break;
		}
	}
	user_des_file.Close ();
	End_Progress ();
}
