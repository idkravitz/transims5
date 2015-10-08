//*********************************************************
//	Magic_Move.cpp - move traveler to he destination
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Magic_Mode
//---------------------------------------------------------

int Path_Builder::Magic_Move ()
{
	Dtime ttime;
	unsigned imped;

	Trip_End *from_ptr;
	Path_End path_end;
	Path_Data path_data;

	if (forward_flag) {
		from_ptr = &trip_org [0];
	} else {
		from_ptr = &trip_des [0];
	}
	parking_lot = -1;
	from_array.clear ();

	path_end.End_Type (LOCATION_ID);

	path_data.Clear ();
	path_data.Imped (0);
	path_data.Time (from_ptr->Time ());

	path_data.Mode (param.mode);
	path_data.Type (LOCATION_ID);
	path_data.Status (1);

	path_end.push_back (path_data);

	from_array.push_back (path_end);

	//---- identify the destination parking lots ----

	to_array.clear ();

	path_end.Clear ();
	path_end.End_Type (LOCATION_ID);

	ttime = plan_ptr->End () - plan_ptr->Start ();
	imped = DTOI (ttime * param.value_time);

	if (forward_flag) {
		ttime += from_ptr->Time ();
	} else {
		ttime = from_ptr->Time () - ttime;
		if (ttime < time_limit) return (TIME_PROBLEM);
	}
	path_data.Clear ();	
	path_data.Imped (imped);
	path_data.Time (ttime);
	path_data.Mode (param.mode);
	path_data.Type (LOCATION_ID);
	path_data.Status (1);

	path_end.push_back (path_data);

	if (plan_flag) {
		return (Trace_Path (from_ptr, &from_array, &path_end));
	}
	return (-1);
}
