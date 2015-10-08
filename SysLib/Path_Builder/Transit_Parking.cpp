//*********************************************************
//	Transit_Parking.cpp - access to park-&-ride lots
//*********************************************************

#include "Path_Builder.hpp"

#include <math.h>

//---------------------------------------------------------
//	Transit_Parking 
//---------------------------------------------------------

int Path_Builder::Transit_Parking (Trip_End_Array *org_ptr, Trip_End_Array *des_ptr, int lot_num)
{
	int index, lot, org_x, org_y, des_x, des_y, des_tod, tod, cost, imped, ttime;
	int dist, dist1, dist2, max_dist, ratio, max_ratio, parking, acc, offset, type, length;
	bool ab_flag, park_flag, out_flag;
	double dx, dy;

	Trip_End_Itr org_itr, des_itr;	
	Trip_End trip_end;	
	Path_End path_end;
	Path_Data path_data;
	Path_End_Itr to_itr;
	Location_Data *loc_ptr;
	Parking_Data *park_ptr;
	Park_Nest_Itr park_itr;
	Link_Data *link_ptr;
	Lot_XY_Array *lot_array;
	Lot_XY_Itr lot_itr;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	parking_lots.clear ();
	from_parking.clear ();
	to_parking.clear ();

	if (param.mode == PNR_OUT_MODE || param.mode == PNR_IN_MODE) {
		lot_array = &exe->park_ride;
		max_ratio = param.max_parkride;
		park_flag = true;
		out_flag = (param.mode == PNR_OUT_MODE && forward_flag);
	} else if (param.mode == KNR_OUT_MODE || param.mode == KNR_IN_MODE) {
		lot_array = &exe->kiss_ride;
		max_ratio = param.max_kissride;
		park_flag = false;
		out_flag = (param.mode == KNR_OUT_MODE && forward_flag);
	} else {
		return (0);
	}
	if (lot_array->size () == 0) return (0);

	org_itr = org_ptr->begin ();
	des_itr = des_ptr->begin ();
	
	loc_ptr = &exe->location_array [org_itr->Index ()];

	org_x = loc_ptr->X ();
	org_y = loc_ptr->Y ();

	des_tod = des_itr->Time ();

	loc_ptr = &exe->location_array [des_itr->Index ()];

	des_x = loc_ptr->X ();
	des_y = loc_ptr->Y ();

	dx = org_x - des_x;
	dy = org_y - des_y;

	max_dist = (int) (2.0 * sqrt (dx * dx + dy * dy) + 0.5);

	//---- process each park-&-ride lot ----

	for (lot_itr = lot_array->begin (); lot_itr != lot_array->end (); lot_itr++) {
		if (lot_num >= 0 && lot_itr->Lot () != lot_num) continue;

		//---- calculate the distance ----

		dx = org_x - lot_itr->X ();
		dy = org_y - lot_itr->Y ();

		dist1 = (int) (sqrt (dx * dx + dy * dy) + 0.5);

		dx = des_x - lot_itr->X ();
		dy = des_y - lot_itr->Y ();

		dist2 = (int) (sqrt (dx * dx + dy * dy) + 0.5);

		dist = dist1 + dist2;

		if (dist == 0 || dist > max_dist) continue;

		if (out_flag) {
			ratio = 100 * dist1 / dist;
		} else {
			ratio = 100 * dist2 / dist;
		}
		if (ratio > max_ratio) continue;

		parking = lot_itr->Lot ();

		trip_end.Type (PARKING_ID);
		trip_end.Index (parking);
		lot = (int) parking_lots.size ();

		parking_lots.push_back (trip_end);

		//---- create access records to the parking lot ----

		park_ptr = &exe->parking_array [parking];

		if (park_ptr->Dir () == 1) {
			link_ptr = &exe->link_array [park_ptr->Link ()];
			offset = link_ptr->Length () - park_ptr->Offset ();
		} else {
			offset = park_ptr->Offset ();
		}
		path_end.Clear ();
		path_end.Trip_End (lot);
		path_end.End_Type (PARKING_ID);
		path_end.Type (LINK_ID);
		path_end.Index (park_ptr->Link ());
		path_end.Offset (offset);
		
		//---- calculate the parking attributes ----
		
		imped = cost = ttime = 0;

		if (park_flag) {
			tod = des_itr->Time ();

			for (park_itr = park_ptr->begin (); park_itr != park_ptr->end (); park_itr++) {
				if (park_itr->Start () <= tod && tod <= park_itr->End ()) {
					if (park_itr->Use () == 0 || Use_Permission (park_itr->Use (), CAR)) {
						if (!out_flag) {
							ttime = park_itr->Time_Out ();
						} else {
							cost = DTOI (park_itr->Hourly () * parking_duration.Hours ());

							if (cost > park_itr->Daily ()) {
								cost = park_itr->Daily ();
							}
							ttime = park_itr->Time_In ();
							cost = Round (cost);
						}
						break;
					}
				}
			}
			imped = DTOI (ttime * param.value_park + cost * param.value_cost);

			//---- add the parking penalty ----

			if (out_flag && param.park_pen_flag) {
				imped += exe->park_penalty [lot_itr->Lot ()];
			}
		}

		//---- save the parking lot attributes ----
	
		path_data.Clear ();
		path_data.Imped (imped);
		path_data.Time (ttime);
		path_data.Cost (cost);
		path_data.Mode (OTHER_MODE);
		path_data.From (parking);
		path_data.Type (PARKING_ID);

		path_end.push_back (path_data);

		if (out_flag) {		
			path_data.Clear ();
			path_end.push_back (path_data);

			to_parking.push_back (path_end);
		} else {
			from_parking.push_back (path_end);
		}

		//---- create access records from the parking lot ----

		if (exe->access_link_flag) {

			acc = exe->park_access [parking].Next (forward_flag);

			//---- process each access link leaving the parking lot ----

			for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
				access_ptr = &exe->access_array [acc];
				acc_ptr = &exe->next_access (forward_flag) [acc];

				ab_flag = (access_ptr->From_Type () == PARKING_ID && access_ptr->From_ID () == parking);

				type = access_ptr->Type (!ab_flag);
				if (type != NODE_ID && type != STOP_ID) continue;

				index = access_ptr->ID (!ab_flag);

				//---- save the node/stop connection ----

				path_end.Clear ();
				path_end.Trip_End (lot);
				path_end.End_Type (PARKING_ID);
				path_end.Type (type);
				path_end.Index (index);
				path_end.Offset (0);

				//---- add a minimum wait ----

				path_data.Clear ();

				if (!forward_flag && param.min_wait > 0) {
					ttime = param.min_wait;
					imped = DTOI (ttime * param.value_wait);

					path_data.Imped (imped);
					path_data.Time (-ttime);
					path_data.Length (0);
					path_data.Walk (0);
					path_data.Mode (WAIT_MODE);
					path_data.From (parking);
					path_data.Type (PARKING_ID);
					path_data.Status (1);

					path_end.push_back (path_data);
				}

				//--- save the access link attributes ----

				ttime = access_ptr->Time ();
				length = (int) (ttime * param.walk_speed + 0.5);
				imped = DTOI (ttime * param.value_walk);

				path_data.Clear ();
				path_data.Imped (imped);
				path_data.Time ((forward_flag) ? ttime : -ttime);
				path_data.Length (length);
				path_data.Walk (length);
				path_data.Mode (WALK_MODE);
				path_data.From (acc);
				path_data.Type (ACCESS_ID);
				if (!ab_flag) path_data.Dir (1);
				path_data.Status (1);

				path_end.push_back (path_data);

				if (out_flag) {
					from_parking.push_back (path_end);
				} else {
					path_data.Clear ();
					path_end.push_back (path_data);

					to_parking.push_back (path_end);
				}
			}
		}

		//---- walk from the parking offset ----

		path_end.Clear ();
		path_end.Trip_End (lot);
		path_end.End_Type (PARKING_ID);
		path_end.Type (LINK_ID);
		path_end.Index (park_ptr->Link ());
		path_end.Offset (offset);

		//---- add a minimum wait ----

		path_data.Clear ();

		if (!forward_flag && param.min_wait > 0) {
			ttime = param.min_wait;
			imped = DTOI (ttime * param.value_wait);

			path_data.Imped (imped);
			path_data.Time (-ttime);
			path_data.Length (0);
			path_data.Walk (0);
			path_data.Mode (WAIT_MODE);
			path_data.From (parking);
			path_data.Type (PARKING_ID);
			path_data.Status (1);

			path_end.push_back (path_data);
		}
		path_data.Imped (0);
		path_data.Mode (OTHER_MODE);
		path_data.From (parking);
		path_data.Type (PARKING_ID);
		path_data.Status (1);

		path_end.push_back (path_data);

		if (out_flag) {
			from_parking.push_back (path_end);
		} else {
			path_data.Clear ();
			path_end.push_back (path_data);

			to_parking.push_back (path_end);
		}
	}

	//---- set the destination flags ----

	if (param.mode == PNR_IN_MODE || param.mode == KNR_IN_MODE) {
		link_to_flag = node_to_flag = stop_to_flag = false;

		link_to.assign (exe->link_array.size (), 0);
		node_to.assign (exe->node_array.size (), 0);
		stop_to.assign (exe->stop_array.size (), 0);

		for (to_itr = to_parking.begin (); to_itr != to_parking.end (); to_itr++) {
			if (to_itr->Type () == LINK_ID) {
				link_to [to_itr->Index ()] = 1;
				link_to_flag = true;
			}
			if (to_itr->Type () == NODE_ID) {
				node_to [to_itr->Index ()] = 1;
				node_to_flag = true;
			}
			if (to_itr->Type () == STOP_ID) {
				stop_to [to_itr->Index ()] = 1;
				stop_to_flag = true;
			}
		}
	}
	return ((int) parking_lots.size ());
}
