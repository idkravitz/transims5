//*********************************************************
//	Walk_Access.cpp - generate walk access links
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Walk_Access
//---------------------------------------------------------

void TransitAccess::Walk_Access (void)
{
	bool flag;
	int zone, node, dist, mode;
	double x, y, dx, dy, len, max_len;
	String text;

	Zone_Map_Itr zone_itr;
	Zone_Data *zone_ptr;
	Node_Map_Itr node_itr;
	Node_Data *node_ptr;
	Dbl_Itr fac_itr;
	Int_Map_Itr station_itr;

	for (zone_itr = zone_map.begin (); zone_itr != zone_map.end (); zone_itr++) {
		flag = false;
		zone = zone_itr->first;
		zone_ptr = &zone_itr->second;

		if (zone_ptr->walk == 0) continue;

		x = zone_ptr->x_coord;
		y = zone_ptr->y_coord;

		for (fac_itr = walk_factors.begin (); fac_itr != walk_factors.end (); fac_itr++) {

			max_len = MIN ((*fac_itr * zone_ptr->distance * 5280.0), 5280.0);

			for (node_itr = node_map.begin (); node_itr != node_map.end (); node_itr++) {
				node = node_itr->first;
				node_ptr = &node_itr->second;

				if (node_ptr->use == 0) continue;

				//---- delete list ----

				dx = node_ptr->x_coord - x;
				dy = node_ptr->y_coord - y;

				len = sqrt (dx * dx + dy * dy);

				if (len > max_len) continue;

				flag = true;
				if (node_ptr->use == 2) {
					mode = 14;
					dist = DTOI (len / 52.8);

					if (station_acc_flag) {
						station_itr = station_access.find (node);
						if (station_itr != station_access.end ()) {
							dist += DTOI (station_itr->second * 3.0 / 60.0);
							if (dist < 1) dist = 1;
						}
					}
				} else {
					mode = 16;
					dist = DTOI (len * zone_ptr->weight / 52.8);
				}
				text (" N=%d-%d ONEWAY=F MODE=%d SPEED=3 DIST=%d ") % zone % node % mode % dist;

				walk_acc_file.File () << "SUPPORT" << text << endl;
				walk_link_file.File () << "SUPPLINK" << text << endl;
			}
			if (flag) break;
		}
	}
}
