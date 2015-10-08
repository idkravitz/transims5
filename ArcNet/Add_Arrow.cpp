//*********************************************************
//	Add_Arrow.cpp - additonal an arrow to a link
//*********************************************************

#include "ArcNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Arrow
//---------------------------------------------------------

void ArcNet::Add_Arrow (Points &points)
{
	XYZ_Point point, end_point;
	Points_RItr itr;

	double dx, dy, dz, length, arrow, side;

	side = arrow_side;
	arrow = arrow_length;
	dx = dy = dz = 0.0;

	itr = points.rbegin ();
	end_point = *itr;

	for (++itr; itr != points.rend (); itr++) {
		dx = end_point.x - itr->x;
		dy = end_point.y - itr->y;
		dz = end_point.z - itr->z;

		length = dx * dx + dy * dy;

		if (length >= arrow) {
			length = sqrt (length);
			dx /= length;
			dy /= length;
			dz /= length;
			break;
		}
	}
	if (itr == points.rend ()) return;
	
	point.x = end_point.x - arrow * dx + side * dy;
	point.y = end_point.y - arrow * dy - side * dx;
	point.z = end_point.z - arrow * dz;

	points.push_back (point);

	point.x = end_point.x - arrow * dx - side * dy;
	point.y = end_point.y - arrow * dy + side * dx;

	points.push_back (point);
	points.push_back (end_point);
}
