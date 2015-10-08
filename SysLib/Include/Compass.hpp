//*********************************************************
//	Compass.hpp - compass orientation
//*********************************************************

#ifndef COMPASS_HPP
#define COMPASS_HPP

#include "APIDefs.hpp"

//---------------------------------------------------------
//	Compass_Points constructor
//---------------------------------------------------------

class SYSLIB_API Compass_Points
{
public:
	Compass_Points (int num_points = 8);

	void Set_Points (int num_points);
	int  Direction (double dx, double dy);
	int  Difference (int dir, int dir2);
	int  Change (int dir, int dir2);
	int  Flip (int dir);

	int  Bearing_Point (int bearing);

	const char *Direction_Text (double dx, double dy);
	const char *Point_Text (int dir, int num_points = 16);

	int  Num_Points (void)			{ return (compass_points); }

private:
	int  compass_points;
	int  compass_break;

	static const char *point_text [];
 };

#endif

