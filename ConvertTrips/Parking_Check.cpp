//*********************************************************
//	Parking_Check.cpp - check for missing parking lots
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Parking_Check
//---------------------------------------------------------

void ConvertTrips::Parking_Check (void)
{
	int loc, count, best, best_diff, diff, offset;

	Convert_Loc_Itr loc_itr;
	Location_Data *loc_ptr;
	Parking_Data *park_ptr;
	Parking_Itr park_itr;
	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	
	Show_Message ("Assigning Parking Locations -- Record");
	Set_Progress ();

	count = 0;

	for (loc=0, loc_itr = convert_array.begin (); loc_itr != convert_array.end (); loc_itr++, loc++) { 
		Show_Progress ();

		if (loc_itr->Org_Parking () > 0 && loc_itr->Des_Parking () > 0) continue;

		loc_ptr = &location_array [loc];
		if (loc_ptr->Zone () < 0) continue;

		//---- look for a direct index match -----

		map_itr = parking_map.find (loc_ptr->Location ());

		if (map_itr != parking_map.end ()) {
			park_ptr = &parking_array [map_itr->second];

			if (park_ptr->Link_Dir () == loc_ptr->Link_Dir () && 
				park_ptr->Offset () == loc_ptr->Offset ()) {

				if (loc_itr->Org_Parking () <= 0) loc_itr->Org_Parking (map_itr->first);
				if (loc_itr->Des_Parking () <= 0) loc_itr->Des_Parking (map_itr->first);
				continue;
			}
		}

		//---- look for parking lots on the same link -----

		best = best_diff = 0;
		link_ptr = &link_array [loc_ptr->Link ()];

		if (Use_Permission (link_ptr->Use (), CAR)) {
			offset = link_ptr->Length () - loc_ptr->Offset ();

			for (park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++) {
				if (park_itr->Link () != loc_ptr->Link ()) continue;

				if (park_itr->Dir () == loc_ptr->Dir ()) {
					if (park_itr->Offset () == loc_ptr->Offset ()) {
						best = park_itr->Parking ();
						break;
					}
					diff = abs (park_itr->Offset () - loc_ptr->Offset ());
				} else {
					if (park_itr->Offset () == offset) {
						best = park_itr->Parking ();
						best_diff = 0;
						continue;
					} else {
						diff = abs (park_itr->Offset () - offset);
					}
				}
				if (best <= 0 || diff < best_diff) {
					best = park_itr->Parking ();
					best_diff = diff;
				}
			}
		}
		if (best > 0) {
			if (loc_itr->Org_Parking () <= 0) loc_itr->Org_Parking (best);
			if (loc_itr->Des_Parking () <= 0) loc_itr->Des_Parking (best);
			continue;
		}
		if (count == 0) {
			Print (2, "No Parking Lot for Activity Locations...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, String ("%d ") % loc_ptr->Location ());
	}
	if (count) {
		Warning (String ("%d Activity Locations have No Parking Lot") % count);
	}
	End_Progress ();
}
