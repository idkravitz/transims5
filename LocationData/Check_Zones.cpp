//*********************************************************
//	Check_Zones.cpp - check the zone data
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Check_Zones
//---------------------------------------------------------

void LocationData::Check_Zones (void)
{
	int z, count;

	Integers zone;
	Int_Map_Itr map_itr;
	Ints_Map_Itr zone_itr;

	Show_Message (1, "Checking Zone Coverage");

	//---- allocate zone memory ----

	zone.assign (Max_Zone_Number () + 1, 0);

	//---- process the location data ----

	for (map_itr = loc_zone_map.begin (); map_itr != loc_zone_map.end (); map_itr++) {
		if (map_itr->second >= 0) {
			zone [map_itr->second]++;
		}
	}
	if (zone_loc_flag) {
		for (zone_itr = zone_loc_map.begin (); zone_itr != zone_loc_map.end (); zone_itr++) {
			zone [zone_itr->first] += (int) zone_itr->second.size ();
		}
	}

	//---- search for locations without a zone number ----

	count = 0;

	for (z=1; z <= Max_Zone_Number (); z++) {
		if (range_flag && !zone_range.In_Range (z)) continue;

		if (zone [z] == 0) {
			if (count == 0) {
				Print (2, "No Activity Locations for Zones...");
			}
			if (!(count % 10)) {
				Print (1, "\t");
			}
			count++;
			Print (0, String ("%d ") % z);
		}
	}
	if (count) {
		Warning (String ("%d Zones have No Activity Locations") % count);
	}

	//---- search for locations with only one zone number ----

	count = 0;

	for (z=1; z <= Max_Zone_Number (); z++) {
		if (range_flag && !zone_range.In_Range (z)) continue;

		if (zone [z] != 1) continue;

		if (count == 0) {
			Print (2, "Only One Activity Location for Zones...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, String ("%d ") % z);
	}
	if (count) {
		Warning (String ("%d Zones have only One Activity Location") % count);
	}

	//---- location with a zone number of zero ----

	if (zone [0] > 0) {
		Print (1);
		Warning (String ("%d Activity Locations have No Zone Number") % zone [0]);
	}
}
