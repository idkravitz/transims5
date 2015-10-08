//*********************************************************
//	Parking_Update.cpp - update the parking impedances
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Parking_Update
//---------------------------------------------------------

int Path_Builder::Parking_Update (void)
{
	int best_lot, lot, to_length, to_cost, to_walk, to_path;
	Dtime to_time;
	unsigned to_imped, best_imped;

	Path_RItr path_ritr;
	Path_Itr path_itr;
	Path_End_Itr end_itr;
	Path_End *end_ptr;
	Trip_End *park_end;

	//---- identify the best combination ----

	best_lot = -1;
	best_imped = MAX_IMPEDANCE;

	for (lot=0, end_itr = to_parking.begin (); end_itr != to_parking.end (); end_itr++, lot++) {
		path_ritr = end_itr->rbegin ();

		//---- check for a completed path ----

		if (path_ritr->Status () == 0) continue;

		//---- calculate cumulative values ----

		to_imped = path_ritr->Imped ();
		to_time = path_ritr->Time ();
		to_length = path_ritr->Length ();
		to_cost = path_ritr->Cost ();
		to_walk = path_ritr->Walk ();
		to_path = path_ritr->Path ();

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
			path_ritr->Path (to_path);
			path_ritr->Status (1);
		}
		park_end = &parking_lots [end_itr->Trip_End ()];

		if (park_end->Best () >= 0) {
			end_ptr = &to_parking [park_end->Best ()];
			if (end_ptr->begin ()->Imped () > to_imped) {
				park_end->Time (to_time);
				park_end->Best (lot);
			}
		} else {
			park_end->Time (to_time);
			park_end->Best (lot);
		}
		if (to_imped < best_imped) {
			best_lot = lot;
			best_imped = to_imped;
		}
	}

	//---- update the impedance for the departure options ----

	for (end_itr = from_parking.begin (); end_itr != from_parking.end (); end_itr++) {
		park_end = &parking_lots [end_itr->Trip_End ()];

		if (park_end->Best () >= 0) {
			end_ptr = &to_parking [park_end->Best ()];

			path_itr = end_ptr->begin ();

			to_imped = path_itr->Imped ();
			to_time = path_itr->Time ();
			to_length = path_itr->Length ();
			to_cost = path_itr->Cost ();
			to_walk = path_itr->Walk ();
			to_path = path_itr->Path ();
		} else {
			to_imped = MAX_IMPEDANCE;
			to_time = 0;
			to_length = to_cost = to_walk = to_path = 0;
		}
		for (path_itr = end_itr->begin (); path_itr != end_itr->end (); path_itr++) {
			if (to_imped < MAX_IMPEDANCE) {
				to_imped += path_itr->Imped ();
				if (forward_flag) {
					to_time = to_time + path_itr->Time ();
				} else {
					to_time = to_time - path_itr->Time ();
				}
				to_length += path_itr->Length ();
				to_cost += path_itr->Cost ();
				to_walk += path_itr->Walk ();

				if (walk_flag && to_walk > param.max_walk) {
					to_imped = MAX_IMPEDANCE;
					length_flag = true;
				}
			}
			path_itr->Imped (to_imped);
			path_itr->Time (to_time);
			path_itr->Length (to_length);
			path_itr->Cost (to_cost);
			path_itr->Walk (to_walk);
			path_itr->Path (to_path);
			path_itr->Status ((to_imped < MAX_IMPEDANCE) ? 1 : 0);
		}
	}
	return (best_lot);
}

