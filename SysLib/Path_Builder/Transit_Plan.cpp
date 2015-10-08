//*********************************************************
//	Transit_Plan.cpp - Build a Transit Plan
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Transit_Plan
//---------------------------------------------------------

int Path_Builder::Transit_Plan (void)
{
	Trip_End_Array *from_ptr, *to_ptr;
	Trip_End *to;
	Path_End *path_end;

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
	}

	//---- allocate/initialize path building memory ----

	Initialize_Transit ();

	//---- origin access ----

	if (!Origin_Location (from_ptr, &from_array, true)) return (ACCESS_PROBLEM);

	//---- destination access ----

	if (!Destination_Location (to_ptr, &to_array, true)) return (ACCESS_PROBLEM);

	//---- build the path between the origin and destination access points ----

	Transit_Path (&from_array, &to_array, plan_flag);

	//---- complete the trips to destination ----

	if (Best_Destination (&to_array, to_ptr) < 0) return (Set_Transit_Error ());

	to = &to_ptr->at (0);
	path_end = &to_array [to->Best ()];

	if (plan_flag) {
		return (Trace_Path (&from_ptr->at (0), &from_array, path_end));
	} else {
		return (Save_Skims ());
	}
}

//---------------------------------------------------------
//	Initialize_Transit
//---------------------------------------------------------

void Path_Builder::Initialize_Transit (void)
{
	Path_Array *array_ptr;

	//---- allocate/initialize path building memory ----

	for (int i=0; i <= param.max_paths; i++) {
		array_ptr = &node_path [i];

		if (array_ptr->size () == 0) {
			array_ptr->resize (exe->node_array.size ());
		} else {
			array_ptr->Clear ();
		}
		if (i == param.max_paths) continue;

		array_ptr = &board_path [i];

		if (array_ptr->size () == 0) {
			array_ptr->resize (exe->stop_array.size ());
		} else {
			array_ptr->Clear ();
		}
		array_ptr = &wait_path [i];

		if (array_ptr->size () == 0) {
			array_ptr->resize (exe->stop_array.size ());
		} else {
			array_ptr->Clear ();
		}
		array_ptr = &alight_path [i];

		if (array_ptr->size () == 0) {
			array_ptr->resize (exe->stop_array.size ());
		} else {
			array_ptr->Clear ();
		}
	}
}

//---------------------------------------------------------
//	Set_Transit_Error
//---------------------------------------------------------

int Path_Builder::Set_Transit_Error (void)
{
	if (length_flag) {
		return (WALK_PROBLEM);
	} else if (wait_time_flag) {
		return (WAIT_PROBLEM);
	} else if (time_flag) {
		return (TIME_PROBLEM);
	} else if (use_flag) {
		return (LINK_PROBLEM);
	} else if (transfer_flag) {
		return (TRANSFER_PROBLEM);
	}
	return (PATH_PROBLEM);
}
