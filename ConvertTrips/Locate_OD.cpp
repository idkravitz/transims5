//*********************************************************
//	Locate_OD.cpp - assign zones to activity locations
//*********************************************************

#include "ConvertTrips.hpp"

#include "math.h"

//---------------------------------------------------------
//	Locate_OD
//---------------------------------------------------------

bool ConvertTrips::Locate_OD (File_Group *group, int &org, int &des, int &stop, int &park, double &dist1, double &dist2)
{
	int loc, last, x, y, x2, y2, mode, num;
	double org_wt, des_wt, stop_wt, prob, cum_wt, dx, dy, share, max_dist;
	bool org_flag, des_flag, stop_flag, org_loc_flag, des_loc_flag, stop_loc_flag;

	Convert_Location *loc_ptr;
	Location_Data *location_ptr;
	Integers *org_ptr, *des_ptr, *stop_ptr;
	Int_Itr int_itr;
	Int_Map_Itr map_itr;

	//---- set the vehicle access flags ----

	if (org == 0 || des == 0) return (false);
	mode = group->Mode ();

	org_flag = (mode != DRIVE_MODE && mode != PNR_OUT_MODE && mode != KNR_OUT_MODE &&
				mode != HOV2_MODE && mode != HOV3_MODE && mode != HOV4_MODE && mode != TAXI_MODE);
	des_flag = (mode != DRIVE_MODE && mode != PNR_IN_MODE && mode != KNR_IN_MODE &&
				mode != HOV2_MODE && mode != HOV3_MODE && mode != HOV4_MODE && mode != TAXI_MODE);
	stop_flag = org_flag && des_flag;

	org_loc_flag = (org < 0);
	des_loc_flag = (des < 0);
	stop_loc_flag = (stop < 0);
	org = abs (org);
	des = abs (des);
	stop = abs (stop);

	dist1 = dist2 = max_dist = org_wt = 0.0;
	org_ptr = des_ptr = stop_ptr = 0;
	loc_ptr = 0;

	//---- calculate the origin weight ----

	if (!org_loc_flag) {
		map_itr = zone_map.find (org);

		if (map_itr == zone_map.end ()) {
			Warning (String ("Origin Zone %d is Not in the Zone File") % org);
			return (false);
		}
		org = map_itr->second;

		org_wt = 0.0;
		num = 0;

		if (zone_loc_flag) {
			org_ptr = zone_loc_map.Locations (org);
		}
		for (loc = zone_loc [org]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (org_flag || loc_ptr->Org_Parking () > 0) {
				org_wt += loc_ptr->Weight (group->Org_Wt ());
				num++;
			}
		}
		if (org_ptr != 0) {
			for (int_itr = org_ptr->begin (); int_itr != org_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];

				if (org_flag || loc_ptr->Org_Parking () > 0) {
					org_wt += loc_ptr->Weight (group->Org_Wt ());
					num++;
				}
			}
		}
		if (org_wt == 0.0) {
			if (num == 0) return (false);

			loc = map_itr->first;
			Warning (String ("No Location Weights were Provided for Origin Zone %d Group %d") % loc % group->Group ());

			org_wt = 1.0;
			share = 1.0 / num;

			for (loc = zone_loc [org]; loc >= 0; loc = loc_ptr->Zone_List ()) {
				loc_ptr = &convert_array [loc];	

				if (org_flag || loc_ptr->Org_Parking () > 0) {
					(*loc_ptr) [group->Org_Wt ()] = share;
				}
			}
			if (org_ptr != 0) {
				for (int_itr = org_ptr->begin (); int_itr != org_ptr->end (); int_itr++) {
					loc_ptr = &convert_array [*int_itr];

					if (org_flag || loc_ptr->Org_Parking () > 0) {
						(*loc_ptr) [group->Org_Wt ()] = share;
					}
				}
			}
		}
	}

	//---- check the destination weight ----
	
	if (!des_loc_flag) {
		map_itr = zone_map.find (des);

		if (map_itr == zone_map.end ()) {
			Warning (String ("Destination Zone %d is Not in the Zone File") % des);
			return (false);
		}
		des = map_itr->second;

		des_wt = 0.0;
		num = 0;

		if (zone_loc_flag) {
			des_ptr = zone_loc_map.Locations (des);
		}
		for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (des_flag || loc_ptr->Des_Parking () > 0) {
				des_wt += loc_ptr->Weight (group->Des_Wt ());
				num++;
			}
		}
		if (des_ptr != 0) {
			for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];

				if (des_flag || loc_ptr->Des_Parking () > 0) {
					des_wt += loc_ptr->Weight (group->Des_Wt ());
					num++;
				}
			}
		}
		if (des_wt == 0.0) {
			if (num == 0) return (false);

			loc = map_itr->first;
			Warning (String ("No Location Weights were Provided for Destination Zone %d Group %d") % loc % group->Group ());

			des_wt = 1.0;
			share = 1.0 / num;

			for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
				loc_ptr = &convert_array [loc];	

				if (des_flag || loc_ptr->Des_Parking () > 0) {
					(*loc_ptr) [group->Des_Wt ()] = share;
				}
			}
			if (des_ptr != 0) {
				for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
					loc_ptr = &convert_array [*int_itr];

					if (des_flag || loc_ptr->Des_Parking () > 0) {
						(*loc_ptr) [group->Des_Wt ()] = share;
					}
				}
			}
		}
	}

	//---- check the stop weight ----
	
	stop_wt = 0.0;
	
	if (!stop_loc_flag && stop > 0) {
		map_itr = zone_map.find (stop);

		if (map_itr == zone_map.end ()) {
			Warning (String ("Stop Zone %d is Not in the Zone File") % stop);
			return (false);
		}
		stop = map_itr->second;
		num = 0;

		if (zone_loc_flag) {
			stop_ptr = zone_loc_map.Locations (stop);
		}
		for (loc = zone_loc [stop]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (stop_flag || loc_ptr->Des_Parking () > 0) {
				stop_wt += loc_ptr->Weight (group->Stop_Wt ());
				num++;
			}
		}
		if (stop_ptr != 0) {
			for (int_itr = stop_ptr->begin (); int_itr != stop_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];	

				if (stop_flag || loc_ptr->Des_Parking () > 0) {
					stop_wt += loc_ptr->Weight (group->Stop_Wt ());
					num++;
				}
			}
		}
		if (stop_wt == 0.0) {
			if (num == 0) return (false);

			loc = map_itr->first;
			Warning (String ("No Location Weights were Provided for Stop Zone %d Group %d") % loc % group->Group ());

			stop_wt = 1.0;
			share = 1.0 / num;

			for (loc = zone_loc [stop]; loc >= 0; loc = loc_ptr->Zone_List ()) {
				loc_ptr = &convert_array [loc];	

				if (stop_flag || loc_ptr->Des_Parking () > 0) {
					(*loc_ptr) [group->Stop_Wt ()] = share;
				}
			}
			if (stop_ptr != 0) {
				for (int_itr = stop_ptr->begin (); int_itr != stop_ptr->end (); int_itr++) {
					loc_ptr = &convert_array [*int_itr];

					if (stop_flag || loc_ptr->Des_Parking () > 0) {
						(*loc_ptr) [group->Stop_Wt ()] = share;
					}
				}
			}
		}
	}

	//---- locate the trip origin ----

	if (!org_loc_flag) {
		prob = org_wt * random_org.Probability ();

		last = -1;
		cum_wt = 0.0;

		for (loc = zone_loc [org]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (org_flag || loc_ptr->Org_Parking () > 0) {
				share = loc_ptr->Weight (group->Org_Wt ());

				if (share != 0.0) {
					cum_wt += share;
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc < 0 && org_ptr != 0) {
			for (int_itr = org_ptr->begin (); int_itr != org_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];

				if (org_flag || loc_ptr->Org_Parking () > 0) {
					share = loc_ptr->Weight (group->Org_Wt ());

					if (share != 0.0) {
						cum_wt += share;
						loc = *int_itr;
						if (prob < cum_wt) {
							loc = *int_itr;
							break;
						}
						last = *int_itr;
					}
				}
			}
		}
		if (loc < 0) {
			if (last < 0) return (false);
			loc_ptr = &convert_array [last];	
		}
		park = loc_ptr->Org_Parking ();

		location_ptr = &location_array [loc];

		org = location_ptr->Location ();
	} else {
		map_itr = location_map.find (org);

		location_ptr = &location_array [map_itr->second];
	}
	x = location_ptr->X ();
	y = location_ptr->Y ();
	
	//---- calculate the destination weight ----

	if (!des_loc_flag) {
		des_wt = 0.0;

		for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	
			location_ptr = &location_array [loc];

			if (location_ptr->Location () != org && (des_flag || loc_ptr->Des_Parking () > 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {

					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;
					
						des_wt += share * dist1;
					} else {
						des_wt += share;
					}
				}
			}
		}
		if (des_ptr != 0) {
			for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];	
				location_ptr = &location_array [*int_itr];

				if (location_ptr->Location () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {

						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = location_ptr->X () - x;
							dy = location_ptr->Y () - y;

							dist1 = sqrt (dx * dx + dy * dy);
							if (dist1 == 0.0) dist1 = 0.01;
						
							des_wt += share * dist1;
						} else {
							des_wt += share;
						}
					}
				}
			}
		}
		if (des_wt == 0.0) return (false);

		//---- locate the destination ----

		prob = des_wt * random_des.Probability ();
		
		last = -1;
		cum_wt = 0.0;

		for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	
			location_ptr = &location_array [loc];

			if (location_ptr->Location () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {
					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;
					
						cum_wt += share * dist1;
					} else {
						cum_wt += share;
					}
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc < 0 && des_ptr != 0) {
			for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];
				location_ptr = &location_array [*int_itr];

				if (location_ptr->Location () != org && (des_flag || loc_ptr->Des_Parking () != 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {
						if (group->Dist_Wt ()) {

							//--- apply the distance weight ----

							dx = location_ptr->X () - x;
							dy = location_ptr->Y () - y;

							dist1 = sqrt (dx * dx + dy * dy);
							if (dist1 == 0.0) dist1 = 0.01;
						
							cum_wt += share * dist1;
						} else {
							cum_wt += share;
						}
						if (prob < cum_wt) {
							loc = *int_itr;
							break;
						}
						last = *int_itr;
					}
				}
			}
		}
		if (loc < 0) {
			if (last < 0) return (false);
			loc = last;
		}
		location_ptr = &location_array [loc];

		des = location_ptr->Location ();
	} else {
		map_itr = location_map.find (des);

		location_ptr = &location_array [map_itr->second];
	}
	x2 = location_ptr->X ();
	y2 = location_ptr->Y ();

	dx = x2 - x;
	dy = y2 - y;

	dist1 = sqrt (dx * dx + dy * dy);

	if (stop == 0) return (org != des && des != 0);

	//---- calculate the stop weight ----

	if (!stop_loc_flag) {

		//---- find the maximum distance ----

		max_dist = 0.0;

		for (loc = zone_loc [stop]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	
			location_ptr = &location_array [loc];

			//--- apply the distance weight ----

			dx = location_ptr->X () - x;
			dy = location_ptr->Y () - y;

			dist1 = sqrt (dx * dx + dy * dy);
			if (dist1 == 0.0) dist1 = 0.01;

			dx = location_ptr->X () - x2;
			dy = location_ptr->Y () - y2;

			dist2 = sqrt (dx * dx + dy * dy);
			if (dist2 == 0.0) dist2 = 0.01;

			if ((dist1 + dist2) > max_dist) {
				max_dist = dist1 + dist2 + 1.0;
			}
		}
		if (stop_ptr != 0) {
			for (int_itr = stop_ptr->begin (); int_itr != stop_ptr->end (); int_itr++) {
				location_ptr = &location_array [*int_itr];

				//--- apply the distance weight ----

				dx = location_ptr->X () - x;
				dy = location_ptr->Y () - y;

				dist1 = sqrt (dx * dx + dy * dy);
				if (dist1 == 0.0) dist1 = 0.01;

				dx = location_ptr->X () - x2;
				dy = location_ptr->Y () - y2;

				dist2 = sqrt (dx * dx + dy * dy);
				if (dist2 == 0.0) dist2 = 0.01;

				if ((dist1 + dist2) > max_dist) {
					max_dist = dist1 + dist2 + 1.0;
				}
			}
		}
		if (max_dist == 0.0) return (false);

		//---- calculate the stop weight ----

		stop_wt = 0.0;

		for (loc = zone_loc [stop]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (stop_flag || loc_ptr->Des_Parking () != 0) {
				share = loc_ptr->Weight (group->Stop_Wt ());

				if (share != 0.0) {
					location_ptr = &location_array [loc];

					//--- apply the distance weight ----

					dx = location_ptr->X () - x;
					dy = location_ptr->Y () - y;

					dist1 = sqrt (dx * dx + dy * dy);
					if (dist1 == 0.0) dist1 = 0.01;

					dx = location_ptr->X () - x2;
					dy = location_ptr->Y () - y2;

					dist2 = sqrt (dx * dx + dy * dy);
					if (dist2 == 0.0) dist2 = 0.01;

					if ((dist1 + dist2) < max_dist) {
						stop_wt += share * (max_dist - dist1 - dist2);
					}
				}
			}
		}
		if (stop_ptr != 0) {
			for (int_itr = stop_ptr->begin (); int_itr != stop_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];

				if (stop_flag || loc_ptr->Des_Parking () != 0) {
					share = loc_ptr->Weight (group->Stop_Wt ());

					if (share != 0.0) {
						location_ptr = &location_array [*int_itr];

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;

						dx = location_ptr->X () - x2;
						dy = location_ptr->Y () - y2;

						dist2 = sqrt (dx * dx + dy * dy);
						if (dist2 == 0.0) dist2 = 0.01;

						if ((dist1 + dist2) < max_dist) {
							stop_wt += share * (max_dist - dist1 - dist2);
						}
					}
				}
			}
		}
		if (stop_wt == 0.0) return (false);

		//---- locate the stop ----

		prob = stop_wt * random_stop.Probability ();
		
		last = 0;
		cum_wt = 0.0;

		for (loc = zone_loc [stop]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (stop_flag || loc_ptr->Des_Parking () != 0) {
				share = loc_ptr->Weight (group->Stop_Wt ());

				if (share != 0.0) {
					location_ptr = &location_array [loc];

					//--- apply the distance weight ----

					dx = location_ptr->X () - x;
					dy = location_ptr->Y () - y;

					dist1 = sqrt (dx * dx + dy * dy);
					if (dist1 == 0.0) dist1 = 0.01;

					dx = location_ptr->X () - x2;
					dy = location_ptr->Y () - y2;

					dist2 = sqrt (dx * dx + dy * dy);
					if (dist2 == 0.0) dist2 = 0.01;

					if ((dist1 + dist2) < max_dist) {
						cum_wt += share * (max_dist - dist1 - dist2);

						if (prob < cum_wt) break;
						last = loc;
					}
				}
			}
		}
		if (loc < 0 && stop_ptr != 0) {
			for (int_itr = stop_ptr->begin (); int_itr != stop_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];	

				if (stop_flag || loc_ptr->Des_Parking () != 0) {
					share = loc_ptr->Weight (group->Stop_Wt ());

					if (share != 0.0) {
						location_ptr = &location_array [*int_itr];

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist1 = sqrt (dx * dx + dy * dy);
						if (dist1 == 0.0) dist1 = 0.01;

						dx = location_ptr->X () - x2;
						dy = location_ptr->Y () - y2;

						dist2 = sqrt (dx * dx + dy * dy);
						if (dist2 == 0.0) dist2 = 0.01;

						if ((dist1 + dist2) < max_dist) {
							cum_wt += share * (max_dist - dist1 - dist2);

							if (prob < cum_wt) {
								loc = *int_itr;
								break;
							}
							last = *int_itr;
						}
					}
				}
			}
		}
		if (loc < 0) {
			if (last < 0) return (false);
			loc = last;
		}
		location_ptr = &location_array [loc];
		
		stop = location_ptr->Location ();
	} else {
		map_itr = location_map.find (stop);

		location_ptr = &location_array [map_itr->second];
	}
	dx = location_ptr->X () - x;
	dy = location_ptr->Y () - y;
	dist1 = sqrt (dx * dx + dy * dy);

	dx = location_ptr->X () - x2;
	dy = location_ptr->Y () - y2;
	dist2 = sqrt (dx * dx + dy * dy);

	return (org != des && des != 0);
}
