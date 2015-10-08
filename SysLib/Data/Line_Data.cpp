//*********************************************************
//	Line_Data.cpp - network transit line data classes
//*********************************************************

#include "Line_Data.hpp"

//---------------------------------------------------------
//	Vehicle_ID
//---------------------------------------------------------

int Line_Array::Vehicle_ID (int vehicles)
{
	int high_route, runs;
	Line_Itr itr;

	runs = 1;

	while (vehicles > 10) {
		vehicles /= 10;
		runs *= 10;
	}
	vehicles = (vehicles + 1) * runs;

	//---- find the highest route number ----

	high_route = num_runs = max_runs = 0;

	for (itr = begin (); itr != end (); itr++) {
		if (itr->Route () > high_route) high_route = itr->Route ();

		runs = (int) itr->begin ()->size ();

		num_runs += runs;
		if (runs > max_runs) max_runs = runs;
	}
	offset = 1;
	runs = max_runs;

	while (runs > 0) {
		runs /= 10;
		offset *= 10;
	}
	runs = 1;

	while (high_route > 0) {
		high_route /= 10;
		runs *= 10;
	}
	runs *= offset;

	if (runs > vehicles) {
		veh_id = runs;
	} else {
		veh_id = vehicles;
	}
	return (veh_id);
}
