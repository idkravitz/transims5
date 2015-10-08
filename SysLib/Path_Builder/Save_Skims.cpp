//*********************************************************
//	Save_Skims.cpp - save a skim data
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Save_Skims
//---------------------------------------------------------

int Path_Builder::Save_Skims ()
{
	int index, best;
	Dtime time;
	Trip_End_Array *from_ptr, *to_ptr;
	Trip_End_Itr from_itr, to_itr;
	Many_Data *many_ptr;
	Path_End *end_ptr;
	Path_Itr path_itr;

	//---- set the orientation ----

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
	}
	from_itr = from_ptr->begin ();

	for (index=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, index++) {
		many_ptr = &data_ptr->at (index);

		best = to_ptr->at (index).Best ();
		if (best < 0) {
			many_ptr->Clear_Totals ();
			if (many_ptr->Problem () == 0) {
				many_ptr->Problem (Set_Drive_Error ());
			}
			continue;
		}
		end_ptr = &to_array [best];
		path_itr = end_ptr->begin ();

		if (path_itr->Status () == 0) {
			many_ptr->Clear_Totals ();
			if (many_ptr->Problem () == 0) {
				many_ptr->Problem (PATH_PROBLEM);
			}
			continue;
		}
		if (param.skim_total_time && !param.flow_flag) {
			if (forward_flag) {
				time = path_itr->Time () - from_itr->Time ();
			} else {
				time = from_itr->Time () - path_itr->Time ();
			}
			many_ptr->Drive (time);
			many_ptr->Impedance (Resolve (path_itr->Imped ()));
			many_ptr->Length (path_itr->Length ());
			many_ptr->Cost (path_itr->Cost ());
		} else {
			plan_ptr->Clear_Plan ();

			Trace_Path (&(*from_itr), &from_array, end_ptr);

			//---- save the path skim data ----

			many_ptr->Drive (plan_ptr->Drive ());
			many_ptr->Transit (plan_ptr->Transit ());
			many_ptr->Walk (plan_ptr->Walk ());
			many_ptr->Wait (plan_ptr->Wait ());
			many_ptr->Other (plan_ptr->Other ());
			many_ptr->Impedance (plan_ptr->Impedance ());
			many_ptr->Length (plan_ptr->Length ());
			many_ptr->Cost (plan_ptr->Cost ());
		}
	}
	return (0);
}
