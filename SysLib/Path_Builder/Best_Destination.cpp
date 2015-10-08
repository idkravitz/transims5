//*********************************************************
//	Best_Destination.cpp - select the best destination
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Best_Destination
//---------------------------------------------------------

int Path_Builder::Best_Destination (Path_End_Array *to_ptr, Trip_End_Array *des_ptr)
{
	int best_des, des, to_length, to_cost, to_walk;
	Dtime to_time;
	unsigned to_imped, best_imped;

	Path_RItr path_ritr;
	Path_End_Itr end_itr;
	Path_End *end_ptr;
	Trip_End *des_end;

	//---- identify the best combination ----

	best_des = -1;
	best_imped = MAX_IMPEDANCE;

	for (des=0, end_itr = to_ptr->begin (); end_itr != to_ptr->end (); end_itr++, des++) {
		path_ritr = end_itr->rbegin ();

		//---- check for a completed path ----

		if (path_ritr->Status () == 0) continue;

		//---- calculate cumulative values ----

		to_imped = path_ritr->Imped ();
		to_time = path_ritr->Time ();
		to_length = path_ritr->Length ();
		to_cost = path_ritr->Cost ();
		to_walk = path_ritr->Walk ();

		for (++path_ritr; path_ritr != end_itr->rend (); path_ritr++) {
			to_imped += path_ritr->Imped ();
			if (forward_flag) {
				to_time = to_time + path_ritr->Time ();
			} else {
				to_time = to_time - path_ritr->Time ();
			}
			to_length += path_ritr->Length ();
			to_cost += path_ritr->Cost ();
			to_walk += path_ritr->Walk ();

			if (walk_flag && to_walk > param.max_walk) {
				to_imped = MAX_IMPEDANCE;
				length_flag = true;
				break;
			}
			path_ritr->Imped (to_imped);
			path_ritr->Time (to_time);
			path_ritr->Length (to_length);
			path_ritr->Cost (to_cost);
			path_ritr->Walk (to_walk);
			path_ritr->Status (1);
		}
		if (des_ptr != 0) {
			des_end = &des_ptr->at (end_itr->Trip_End ());

			if (des_end->Best () >= 0) {
				end_ptr = &to_ptr->at (des_end->Best ());
				if (end_ptr->begin ()->Imped () > to_imped) {
					des_end->Time (to_time);
					des_end->Best (des);
				}
			} else {
				des_end->Time (to_time);
				des_end->Best (des);
			}
			if (to_imped < best_imped) {
				best_des = des;
				best_imped = to_imped;
			}
		} else {
			best_des = des;
		}
	}
	return (best_des);
}

