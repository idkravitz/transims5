//*********************************************************
//	Closest.cpp - Find the Closest Zone Centroid
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Closest_Zone
//---------------------------------------------------------

int Validate::Closest_Zone (int nx, int ny)
{
	int zone, z;
	double dx, dy, closest, distance;

	Zone_Itr itr;

	zone = -1;
	closest = 0;

	for (z=0, itr = zone_array.begin (); itr != zone_array.end (); itr++, z++) {
		dx = itr->X () - nx;
		dy = itr->Y () - ny;

		distance = dx * dx + dy * dy;

		if (zone < 0 || distance < closest) {
			zone = z;
			closest = distance;
		}
	}
	return (zone);
}
