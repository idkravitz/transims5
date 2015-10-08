//*********************************************************
//	Drive_Transit_Plan.cpp - build a park-&-ride plan
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Drive_Transit_Plan
//---------------------------------------------------------

int Path_Builder::Drive_Transit_Plan (int lot)
{
	int best;
	Trip_End_Array *from_ptr, *to_ptr;
	Path_End *to;
	bool out_flag;
	Problem_Type problem;

	if (param.mode == PNR_OUT_MODE || param.mode == PNR_IN_MODE) {
		problem = PARK_PROBLEM;
		if (exe->park_ride.size () == 0) return (problem);
	} else {
		problem = KISS_PROBLEM;
		if (param.use < HOV2) param.use = HOV2;
		if (exe->kiss_ride.size () == 0) return (problem);
	}
	out_flag = (param.mode == PNR_OUT_MODE || param.mode == KNR_OUT_MODE);

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
	}
	parking_lot = -1;
	Initialize_Transit ();

	//---- outbound trips (drive to transit) ----

	if (out_flag) {

		//---- walk to parking ----

		if (!Origin_Parking (from_ptr, &from_array, lot)) return (ACCESS_PROBLEM);

		//---- identify the best park-&-ride lots ----

		if (!Transit_Parking (from_ptr, to_ptr)) return (problem);

		//---- build the path between the origin and park ride lots ----

		if (Drive_Path (&from_array, &to_parking, false) < 0) return (Set_Drive_Error ());

		//---- update the parking legs ----

		if (Parking_Update () < 0) return (Set_Drive_Error ());

		//---- destination access ----

		if (!Destination_Location (to_ptr, &to_array, true)) return (ACCESS_PROBLEM);

		//---- build the path between the parking lot and destination access points ----
		
		max_imp = MAX_IMPEDANCE;
		imp_diff = 0;

		Transit_Path (&from_parking, &to_array, plan_flag);

	} else {	//---- inbound trips (transit to drive) ----
		
		//---- origin access ----

		if (!Origin_Location (from_ptr, &from_array, true)) return (ACCESS_PROBLEM);

		//---- identify the best parking lots ----

		if (!Transit_Parking (from_ptr, to_ptr, lot)) return (problem);

		//---- build the path between the origin and parking lot ----

		Transit_Path (&from_array, &to_parking, plan_flag);

		//---- update the parking legs ----

		if (Parking_Update () < 0) return (Set_Drive_Error ());

		//---- identify the destination parking lots ----

		if (!Destination_Parking (to_ptr, &to_array, lot)) return (ACCESS_PROBLEM);

		//---- build the path between the origin and destination parking lots ----
		
		max_imp = MAX_IMPEDANCE;
		imp_diff = 0;

		if (Drive_Path (&from_parking, &to_array, plan_flag) < 0) return (Set_Drive_Error ());
	}

	//---- complete the trips to destination ----

	if (Best_Destination (&to_array, to_ptr) < 0) return (Set_Transit_Error ());

	best = to_ptr->at (0).Best ();
	to = &to_array [best];

	if (plan_flag) {
		return (Trace_Path (&from_ptr->at (0), &from_array, to));
	} else {
		return (Save_Skims ());
	}
}
