//*********************************************************
//	Drive_Plan.cpp - build a drive (SOV/HOV/Truck) plan
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Drive_Plan
//---------------------------------------------------------

int Path_Builder::Drive_Plan (int lot)
{
	int best;
	Trip_End_Array *from_ptr, *to_ptr;

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
	}
	parking_lot = -1;

	//---- re-route from a link offset ----

	if (reroute_flag) {

		from_array.clear ();
	//	from_array.push_back (from_ptr->at (0));

	} else {

		//---- walk to parking ----

		if (!Origin_Parking (from_ptr, &from_array, lot)) return (Set_Drive_Error ());
	}

	//---- identify the destination parking lots ----

	if (!Destination_Parking (to_ptr, &to_array, lot)) return (Set_Drive_Error ());

	//---- build the path between the origin and destination parking lots ----

	if (Drive_Path (&from_array, &to_array, plan_flag) < 0) return (Set_Drive_Error ());

	//---- complete the trips to destination ----

	if (Best_Destination (&to_array, to_ptr) < 0) return (Set_Drive_Error ());

	if (plan_flag) {
		best = to_ptr->at (0).Best ();
		if (best < 0) return (false);

		return (Trace_Path (&from_ptr->at (0), &from_array, &to_array [best]));
	} else {
		return (Save_Skims ());
	}
}

//---------------------------------------------------------
//	Set_Drive_Error
//---------------------------------------------------------

int Path_Builder::Set_Drive_Error (void)
{
	if (zero_flag) {
		return (ZERO_PROBLEM);
	} else if (time_flag) {
		return (TIME_PROBLEM);
	} else if (dist_flag) {
		return (DIST_PROBLEM);
	} else if (length_flag) {
		return (WALK_PROBLEM);
	} else if (use_flag) {
		return (USE_PROBLEM);
	} else if (local_acc_flag) {
		return (LOCAL_PROBLEM); 
	} else if (walk_acc_flag) {
		return (WALK_PROBLEM);
	} else if (park_flag) {
		return (PARK_USE_PROBLEM);
	} else if (access_flag) {
		return (ACCESS_PROBLEM);
	}
	return (PATH_PROBLEM);
}
