//*********************************************************
//	Match_Locations.cpp - map activity locations
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Match_Locations
//---------------------------------------------------------

void Relocate::Match_Locations (void)
{
	int location, parking, x, y, best_loc, xmin, xmax, ymin, ymax, diff, min_diff, near_offset;
	double dx, dy, best_dist;

	Int_Map old_loc_park, new_loc_park;
	Int_Map_Itr map_itr, new_map_itr;
	Location_Itr loc_itr, old_itr;
	Location_Data *loc_ptr;
	Parking_Itr park_itr;
	Parking_Data *park_ptr;
	Access_Itr acc_itr;

	for (old_itr = old_loc_array.begin (); old_itr != old_loc_array.end (); old_itr++) {

		x = old_itr->X ();
		y = old_itr->Y ();

		xmin = x - MIN_DIFF;
		xmax = x + MIN_DIFF;
		ymin = y - MIN_DIFF;
		ymax = y + MIN_DIFF;

		//---- check for exact match ----

		map_itr = location_map.find (old_itr->Location ());

		if (map_itr != location_map.end ()) {
			loc_ptr = &location_array [map_itr->second];

			if (loc_ptr->Link_Dir () == old_itr->Link_Dir () && loc_ptr->Offset () == old_itr->Offset () &&
				loc_ptr->X () <= xmax && loc_ptr->X () >= xmin && loc_ptr->Y () <= ymax && loc_ptr->Y () >= ymin) {

				old_new_loc.insert (Int_Map_Data (old_itr->Location (), loc_ptr->Location ()));
				continue;
			}
		}
		best_loc = 0;
		best_dist = 0.0;

		for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
			if (loc_itr->X () <= xmax && loc_itr->X () >= xmin && loc_itr->Y () <= ymax && loc_itr->Y () >= ymin) {

				dx = (double) loc_itr->X () - x;
				dy = (double) loc_itr->Y () - y;

				dx = dx * dx + dy * dy;

				if (best_loc == 0 || best_dist > dx) {
					best_loc = loc_itr->Location ();
					best_dist = dx;
					if (best_dist == 0.0) break;
				}
			}
		}
		old_new_loc.insert (Int_Map_Data (old_itr->Location (), best_loc));
	}

	if (loc_map_flag) {
		for (map_itr = old_new_loc.begin (); map_itr != old_new_loc.end (); map_itr++) {
			location = map_itr->first;
			new_loc_file.Put_Field (0, location);
			new_loc_file.Put_Field (1, map_itr->second);

			if (!new_loc_file.Write ()) {
				Error (String ("Writing %s") % new_loc_file.File_Type ());
			}
		}
		new_loc_file.Close ();
	}

	if (!vehicle_flag && !plan_flag) return;

	//---- process access links ----

	if (access_flag) {
		for (acc_itr = old_acc_array.begin (); acc_itr != old_acc_array.end (); acc_itr++) {
			if (acc_itr->From_Type () == LOCATION_ID && acc_itr->To_Type () == PARKING_ID) {

				location = acc_itr->From_ID ();
				loc_ptr = &old_loc_array [location];

				parking = acc_itr->To_ID ();
				park_ptr = &old_park_array [parking];
				
				old_loc_park.insert (Int_Map_Data (loc_ptr->Location (), park_ptr->Parking ()));
			}
		}
		for (acc_itr = access_array.begin (); acc_itr != access_array.end (); acc_itr++) {
			if (acc_itr->From_Type () == LOCATION_ID && acc_itr->To_Type () == PARKING_ID) {

				location = acc_itr->From_ID ();
				loc_ptr = &location_array [location];

				parking = acc_itr->To_ID ();
				park_ptr = &parking_array [parking];
				
				new_loc_park.insert (Int_Map_Data (loc_ptr->Location (), park_ptr->Parking ()));
			}
		}
	}

	//---- process link-based relationships ----
		
	near_offset = Round (Internal_Units (10.0, FEET));	

	for (loc_itr = old_loc_array.begin (); loc_itr != old_loc_array.end (); loc_itr++) {
		map_itr = old_loc_park.find (loc_itr->Location ());
		if (map_itr != old_loc_park.end ()) continue;

		map_itr = old_park_map.find (loc_itr->Location ());
		if (map_itr != old_park_map.end ()) {
			park_ptr = &old_park_array [map_itr->second];

			if (park_ptr->Link_Dir () == loc_itr->Link_Dir () && abs (park_ptr->Offset () - loc_itr->Offset ()) <= near_offset) {
				old_loc_park.insert (Int_Map_Data (loc_itr->Location (), park_ptr->Parking ()));
				continue;
			}
		}
		min_diff = MAX_INTEGER; 
		parking = -1;

		for (park_itr = old_park_array.begin (); park_itr != old_park_array.end (); park_itr++) {
			if (park_itr->Link_Dir () == loc_itr->Link_Dir ()) {
				diff = abs (park_itr->Offset () - loc_itr->Offset ());

				if (diff < min_diff) {
					min_diff = diff;
					parking = park_itr->Parking ();
					if (diff <= near_offset) break;
				}
			}
		}
		if (parking >= 0) {
			old_loc_park.insert (Int_Map_Data (loc_itr->Location (), parking));
		}
	}

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
		map_itr = new_loc_park.find (loc_itr->Location ());
		if (map_itr != new_loc_park.end ()) continue;

		map_itr = parking_map.find (loc_itr->Location ());
		if (map_itr != parking_map.end ()) {
			park_ptr = &parking_array [map_itr->second];

			if (park_ptr->Link_Dir () == loc_itr->Link_Dir () && abs (park_ptr->Offset () - loc_itr->Offset ()) <= near_offset) {
				new_loc_park.insert (Int_Map_Data (loc_itr->Location (), park_ptr->Parking ()));
				continue;
			}
		}
		min_diff = MAX_INTEGER; 
		parking = -1;

		for (park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++) {
			if (park_itr->Link_Dir () == loc_itr->Link_Dir ()) {
				diff = abs (park_itr->Offset () - loc_itr->Offset ());

				if (diff < min_diff) {
					min_diff = diff;
					parking = park_itr->Parking ();
					if (diff <= near_offset) break;
				}
			}
		}
		if (parking >= 0) {
			new_loc_park.insert (Int_Map_Data (loc_itr->Location (), parking));
		}
	}

	//---- map the parking lots based on the location map ----

	for (map_itr = old_loc_park.begin (); map_itr != old_loc_park.end (); map_itr++) {
		location = map_itr->first;

		new_map_itr = old_new_loc.find (location);
		if (new_map_itr != old_new_loc.end ()) {
			old_new_park.insert (Int_Map_Data (map_itr->second, new_map_itr->second));
		}
	}
	
	if (park_map_flag) {
		for (map_itr = old_new_park.begin (); map_itr != old_new_park.end (); map_itr++) {
			location = map_itr->first;
			new_park_file.Put_Field (0, location);
			new_park_file.Put_Field (1, map_itr->second);

			if (!new_park_file.Write ()) {
				Error (String ("Writing %s") % new_park_file.File_Type ());
			}
		}
		new_park_file.Close ();
	}
}
