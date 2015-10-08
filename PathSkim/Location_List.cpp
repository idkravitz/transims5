//*********************************************************
//	Location_List.cpp - select locations within a zone
//*********************************************************

#include "PathSkim.hpp"

#include <math.h>

//---------------------------------------------------------
//	Location_List
//---------------------------------------------------------

bool PathSkim::Location_List (int zone, int num_sel, Integers *locations, Integers &selected)
{
	int num_loc;
	Int_Itr int_itr;
	Location_Data *location_ptr;

	//---- set the selection count ----

	selected.clear ();
	num_loc = (int) locations->size ();

	if (num_loc == 0) {
		if (warn_flag [zone] == 0) {
			Warning ("No Locations for Zone ") << zone;
			warn_flag [zone] = 1;
		}
		return (false);
	}
	if (num_sel == 0 || num_sel > num_loc) {
		num_sel = num_loc;
	}

	//---- apply the selection method ----

	if (num_sel < num_loc) {

		if (method == RANDOM_LOC) {

			//---- select a random percentage ----

			int num = 0;
			double share = (double) num_sel / num_loc;

			while (num < num_sel) {
				for (int_itr = locations->begin (); int_itr != locations->end (); int_itr++) {
					if (random_loc.Probability () <= share) {
						selected.push_back (*int_itr);
						if (++num == num_sel) break;
					}
				}
			}

		} else if (method == CENTROID_LOC) {

			//---- select locations near the zone centroid ----

			double dx, dy, dist;
			Int_Map_Itr map_itr;
			Zone_Data *zone_ptr;
			NInt_Map dist_sort;
			NInt_Map_Itr sort_itr;

			map_itr = zone_map.find (zone);

			if (map_itr == zone_map.end ()) {
				if (warn_flag [zone] == 0) {
					Warning ("No Zone Centroid for Zone ") << zone;
					warn_flag [zone] = 1;
				}
				return (false);
			}
			zone_ptr = &zone_array [map_itr->second];

			for (int_itr = locations->begin (); int_itr != locations->end (); int_itr++) {
				map_itr = location_map.find (*int_itr);
				if (map_itr == location_map.end ()) continue;

				location_ptr = &location_array [map_itr->second];

				dx = location_ptr->X () - zone_ptr->X ();
				dy = location_ptr->Y () - zone_ptr->Y ();

				dist = sqrt (dx * dx + dy * dy);

				dist_sort.insert (NInt_Map_Data (Round (dist), *int_itr));
			}
			for (sort_itr = dist_sort.begin (); sort_itr != dist_sort.end (); sort_itr++) {
				selected.push_back (sort_itr->second);
				if (--num_sel <= 0) break;
			}

		} else if (method == DISTRIB_LOC) {

			//---- select spatially distributed locations ----

			int dir, num [4];
			double dx, dy, dist;
			Int_Map_Itr map_itr;
			Zone_Data *zone_ptr;
			NInt_Map dist_sort [4], order;
			NInt_Map_Itr sort_itr, order_itr;

			map_itr = zone_map.find (zone);

			if (map_itr == zone_map.end ()) {
				if (warn_flag [zone] == 0) {
					Warning ("No Zone Centroid for Zone ") << zone;
					warn_flag [zone] = 1;
				}
				return (false);
			}
			zone_ptr = &zone_array [map_itr->second];

			compass.Set_Points (4);

			for (int_itr = locations->begin (); int_itr != locations->end (); int_itr++) {
				map_itr = location_map.find (*int_itr);
				if (map_itr == location_map.end ()) continue;

				location_ptr = &location_array [map_itr->second];

				dx = location_ptr->X () - zone_ptr->X ();
				dy = location_ptr->Y () - zone_ptr->Y ();

				dist = sqrt (dx * dx + dy * dy);

				dir = compass.Direction (dx, dy);

				dist_sort [dir].insert (NInt_Map_Data (Round (dist), *int_itr));
			}
			for (dir=0; dir < 4; dir++) {
				order.insert (NInt_Map_Data ((int) dist_sort [dir].size (), dir));
				num [dir] = 0;
			}
			while (num_sel > 0) {
				for (order_itr = order.begin (); order_itr != order.end (); order_itr++) {
					if (order_itr->first > num [order_itr->second]) {
						num [order_itr->second]++;
						if (--num_sel <= 0) break;
					}
				}
			}
			for (order_itr = order.begin (); order_itr != order.end (); order_itr++) {
				dir = order_itr->second;
				if (num [dir] > 0) {
					dist = 0.0;
					dx = (double) num [dir] / order_itr->first;

					for (sort_itr = dist_sort [dir].begin (); sort_itr != dist_sort [dir].end (); sort_itr++) {
						if ((int) dist != (int) (dist + dx)) {
							selected.push_back (sort_itr->second);
						}
						dist += dx;
					}
				}
			}
		} else {	//---- user ----

			int num = 0;

			for (int_itr = locations->begin (); int_itr != locations->end () && num < num_sel; int_itr++, num++) {
				selected.push_back (*int_itr);
			}
		}

	} else {

		//---- select all activity locations within the zone ----

		selected.insert (selected.begin (), locations->begin (), locations->end ());
	}
	return ((selected.size () > 0));
}

