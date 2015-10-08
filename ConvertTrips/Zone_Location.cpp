//*********************************************************
//	Zone_Location.cpp - check the zone data
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Zone_Location
//---------------------------------------------------------

void ConvertTrips::Zone_Location (void)
{
	int loc, count, *zone;
	bool warn_flag = false;

	Location_Itr loc_itr;
	Convert_Location *loc_ptr;
	Int_Map_Itr zone_itr;
	Integers *int_ptr;

	num_zone = Max_Zone_Number ();

	if (num_zone == 0) {
		Error ("No Zones were found in the Activity Location File");
	}
	num_zone = (int) zone_array.size ();

	Show_Message ("Checking Zone Locations -- Record");
	Set_Progress ();

	//---- allocate zone based memory ----

	zone_loc.assign (num_zone, -1);

	//---- process the location data ----

	for (loc = 0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, loc++) {
		Show_Progress ();

		if (loc_itr->Zone () >= 0) {
			zone = &zone_loc [loc_itr->Zone ()];
			loc_ptr = &convert_array [loc];

			loc_ptr->Zone_List (*zone);
			*zone = loc;
		}
	}
	End_Progress ();

	//---- search for locations without a zone number ----

	count = 0;

	for (zone_itr = zone_map.begin (); zone_itr != zone_map.end (); zone_itr++) {
		Show_Progress ();

		if (zone_loc [zone_itr->second] >= 0) continue;
		if (zone_loc_map.Locations (zone_itr->second) != 0) continue;

		if (count == 0) {
			Print (2, "No Trip End Location for Zones...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, String ("%d ") % zone_itr->first);
	}
	if (count) {
		Warning (String ("%d Zone%s No Trip End Locations") % count % ((count > 1) ? "s have" : " has"));
		warn_flag = true;
	}

	//---- search for locations with only one zone number ----

	count = 0;

	for (zone_itr = zone_map.begin (); zone_itr != zone_map.end (); zone_itr++) {
		Show_Progress ();

		loc = zone_loc [zone_itr->second];
		if (loc < 0) continue;

		loc_ptr = &convert_array [loc];
		if (loc_ptr->Zone_List () >= 0) continue;

		int_ptr = zone_loc_map.Locations (zone_itr->second);
		if (int_ptr == 0) continue;

		if (int_ptr->size () > 1) continue;

		if (count == 0) {
			Print (2, "Only One Trip End Location for Zones...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, String ("%d ") % zone_itr->first);
	}
	if (count) {
		Warning (String ("%d Zone%s only One Trip End Location") % count % ((count > 1) ? "s have" : " has"));
		warn_flag = true;
	}
	if (warn_flag) Show_Message (1);
}
