//*********************************************************
//	Process_Origin.cpp - process each origin
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Process_Origin
//---------------------------------------------------------

void TransitAccess::Process_Origin (Zone_Map_Itr &zone_itr)
{
	int org, des, org1, des1, zone1, speed, dist, node;
	double ttime, distance, dx, dy, cbd_dist, stop_dist, ratio;
	bool default_flag, missing_flag;
	String text;
	Station_Itr stop_itr;
	Mode_Itr mode_itr;
	Zone_Data *org_ptr, *des_ptr;
	Zone_Map_Itr des_itr;
	Skim_Data *skim_ptr;

	//---- get the origin zone data ----

	org = zone_itr->first;
	org1 = org - 1;
	org_ptr = &zone_itr->second;

	dx = org_ptr->x_coord - cbd_x;
	dy = org_ptr->y_coord - cbd_y;

	cbd_dist = sqrt (dx * dx + dy * dy);
	if (cbd_dist < 1.0) cbd_dist = 1.0;

	//---- consider each station ----

	for (stop_itr = station_array.begin (); stop_itr != station_array.end (); stop_itr++) {

		//---- check the destination zone and barrier ----

		des_itr = zone_map.find (stop_itr->zone);

		if (des_itr == zone_map.end ()) continue;
		des_ptr = &des_itr->second;
		zone1 = stop_itr->zone - 1;

		if (barriers.find (Int2_Key (org_ptr->barrier, des_ptr->barrier)) != barriers.end ()) continue;	

		//---- check the diversion constraint ----

		dx = org_ptr->x_coord - stop_itr->x_coord;
		dy = org_ptr->y_coord - stop_itr->y_coord;

		stop_dist = sqrt (dx * dx + dy * dy);

		distance = stop_dist + stop_itr->cbd_dist - cbd_dist;
		ratio = (stop_dist + stop_itr->cbd_dist) / cbd_dist;

		if (ratio > max_ratio || distance > max_dist) continue;

		//---- set the destination skim ----

		des = stop_itr->skim;
		if (des == 0) {
			des = stop_itr->zone;
		}
		if (des == 0) continue;
		des1 = des - 1;
		missing_flag = false;

		//---- find the mode group ----

		for (mode_itr = mode_array.begin (); mode_itr != mode_array.end (); mode_itr++) {

			if (mode_itr->skim < 0) continue;
			if (stop_itr->mode != mode_itr->mode) continue;
			if (mode_itr->pnr && !stop_itr->pnr) continue;

			skim_ptr = &skim_array [mode_itr->skim];

			ttime = skim_ptr->time [org1] [des1];
			distance = skim_ptr->distance [org1] [des1] / 10.0;

			if (ttime == 0.0) {
				ttime = skim_ptr->time [org1] [zone1];
				distance = skim_ptr->distance [org1] [zone1] / 10.0;
				missing_flag = true;
			}
			if (ttime > 0) {
				speed = DTOI (60.0 * distance / ttime);
				dist = MAX (DTOI (distance * 100), 10);
				default_flag = false;
			} else {
				speed = 25;
				dist = 50;
				default_flag = true;
			}

			//---- connection type ----

			if (stop_itr->type == slug_type) {
				if (!slug_flag || org_ptr->slug == 0) continue;
				node = stop_itr->stop;
			} else {

				//---- check the distance criteria ----

				if (stop_itr->type >= (int) mode_itr->max_dist.size ()) continue;
				if (stop_dist > mode_itr->max_dist [stop_itr->type]) continue;

				if (mode_itr->connect == PARKING_NODE) {
					node = stop_itr->park;
				} else if (mode_itr->connect == STOP_NODE) {
					node = stop_itr->stop;
				} else {
					node = stop_itr->node;
				}
				if (node == 0) continue;
			}

			//---- write the link and access records ----

			text (" N=%d-%d DIST=%d ONEWAY=T MODE=11 SPEED=%d") % org % node % dist % speed;

			mode_itr->mode_file->File () << "SUPPORT" << text << endl;
			mode_itr->mode_file->File () << "SUPPLINK" << text << endl;
			mode_itr->count += 2;

			if (default_flag) num_default++;
			if (missing_flag) {
				stop_itr->missing = true;
				missing_skims++;
			}
		}
	}
}
