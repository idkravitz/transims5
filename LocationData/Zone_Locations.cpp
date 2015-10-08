//*********************************************************
//	Zone_Locations.cpp - check the zone location data
//*********************************************************

#include "LocationData.hpp"
#include "Compass.hpp"

#include <math.h>

//---------------------------------------------------------
//	Zone_Locations
//---------------------------------------------------------

void LocationData::Zone_Locations (void)
{
	int z, x, y, num, dir, distance, total;
	double dx, dy;

	Zone_Data *zone_ptr;
	Location_Itr location_itr;
	Integers zone, *loc_ptr, loc_rec, best_loc, best_dist;
	Int_Map_Itr map_itr;
	Ints_Map_Stat map_stat;

	//---- allocate zone memory ----

	zone.assign (Max_Zone_Number () + 1, 0);

	//---- process the location data ----

	for (map_itr = loc_zone_map.begin (); map_itr != loc_zone_map.end (); map_itr++) {
		if (map_itr->second >= 0) {
			zone [map_itr->second]++;
		}
	}
	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		z = map_itr->first;

		if (range_flag && !zone_range.In_Range (z)) continue;

		if (zone [z] >= min_locations) continue;

		map_stat = zone_loc_map.insert (Ints_Map_Data (z, loc_rec));
		if (!map_stat.second) {
			Warning ("Duplcate Zone Number = ") << z;
		}
		loc_ptr = &map_stat.first->second;

		zone_ptr = &zone_array [map_itr->second];
		x = zone_ptr->X ();
		y = zone_ptr->Y ();

		num = (min_locations - zone [z]) * 2;
		compass.Set_Points (num);

		best_loc.assign (num, 0);
		best_dist.assign (num, MAX_INTEGER);

		for (location_itr = location_array.begin (); location_itr != location_array.end (); location_itr++) {
			if (location_itr->Zone () == map_itr->second) continue;

			dx = UnRound (location_itr->X () - x);
			dy = UnRound (location_itr->Y () - y);

			distance = DTOI (sqrt (dx * dx + dy * dy));

			dir = compass.Direction (dx, dy);

			if (distance < best_dist [dir]) {
				best_loc [dir] = location_itr->Location ();
				best_dist [dir] = distance;
			}
		}

		//---- select the best group ----

		total = 0;
		distance = 0;

		for (dir=0; dir < num; dir++) {
			if ((dir % 2) == 1) {
				distance += best_dist [dir];
			} else {
				total += best_dist [dir];
			}
		}
		if (distance < total) {
			dir = 1;
		} else {
			dir = 0;
		}
		for (; dir < num; dir += 2) {
			if (best_loc [dir] > 0) {
				loc_ptr->push_back (best_loc [dir]);
			}
		}
	}
	System_Data_False (ZONE);
	System_Data_False (LOCATION);
}
