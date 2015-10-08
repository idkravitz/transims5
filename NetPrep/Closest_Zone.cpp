//*********************************************************
//	Closest_Zone.cpp - find the closest zone
//*********************************************************

#include "NetPrep.hpp"

#include <math.h>

//---------------------------------------------------------
//	Closest_Zone
//---------------------------------------------------------

int NetPrep::Closest_Zone (int node)
{
	int z, zone = -1;
	Zone_Itr itr;
	Node_Data *node_ptr = &node_array [node];
	double dx, dy, dist, best = 0.0;

	for (z=0, itr = zone_array.begin (); itr != zone_array.end (); itr++, z++) {
		dx = itr->X () - node_ptr->X ();
		dy = itr->Y () - node_ptr->Y ();

		dist = sqrt (dx * dx + dy * dy);
		if (zone < 0 || dist < best) {
			best = dist;
			zone = z;
		}
	}
	return (zone);
}
