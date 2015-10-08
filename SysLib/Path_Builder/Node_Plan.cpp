//*********************************************************
//	Node_Plan.cpp - build an all-walk or bike plan
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Node_Plan
//---------------------------------------------------------

int Path_Builder::Node_Plan ()
{
	int best;
	Trip_End_Array *from_ptr, *to_ptr;
	Path_End *to;

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
	}

	//---- origin access ----

	if (!Origin_Location (from_ptr, &from_array)) return (ACCESS_PROBLEM);

	//---- destination access ----

	if (!Destination_Location (to_ptr, &to_array)) return (ACCESS_PROBLEM);

	//---- build the path between the origin and destination access points ----

	if (Node_Path (&from_array, &to_array, plan_flag) < 0) return (Set_Node_Error ());
	
	//---- complete the trips to destination ----

	if (Best_Destination (&to_array, to_ptr) < 0) return (Set_Node_Error ());
	
	best = to_ptr->at (0).Best ();
	to = &to_array [best];

	if (plan_flag) {
		return (Trace_Path (&from_ptr->at (0), &from_array, to));
	} else {
		return (Save_Skims ());
	}
}

//---------------------------------------------------------
//	Set_Node_Error
//---------------------------------------------------------

int Path_Builder::Set_Node_Error ()
{
	if (length_flag) {
		return ((param.use == WALK) ? WALK_PROBLEM : BIKE_PROBLEM);
	} else if (time_flag) {
		return (TIME_PROBLEM);
	} else if (use_flag) {
		return (USE_PROBLEM);
	}
	return (PATH_PROBLEM);
}
