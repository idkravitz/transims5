//*********************************************************
//	Parking_Access.cpp - walk between location <-> parking
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Origin_Parking
//---------------------------------------------------------

bool Path_Builder::Origin_Parking (Trip_End_Array *org_ptr, Path_End_Array *from_ptr, int parking)
{
	int index, length, offset, diff, lot, org, imped, ttime, cost, acc;
	bool link_flag, ab_flag;

	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Parking_Data *lot_ptr;
	Park_Nest_Itr park_itr;
	Trip_End_Itr org_itr;
	Path_End path_end;
	Path_Data path_data;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	from_ptr->clear ();
	park_flag = walk_acc_flag = access_flag = false;

	for (org=0, org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++, org++) {
		if (org_itr->Index () < 0) continue;

		link_flag = false;
		loc_ptr = &exe->location_array [org_itr->Index ()];

		//---- add the access links to parking ----

		if (exe->access_link_flag) {
			acc = exe->loc_access [org_itr->Index ()].Next (forward_flag);

			//---- process each access link leaving the origin ----

			for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
				access_ptr = &exe->access_array [acc];
				acc_ptr = &exe->next_access (forward_flag) [acc];

				ab_flag = (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == org_itr->Index ());

				if (access_ptr->Type (!ab_flag) != PARKING_ID) continue;
				lot = access_ptr->ID (!ab_flag);

				if (forward_flag && parking >= 0 && parking != lot) continue;

				//---- save a parking connection ----

				lot_ptr = &exe->parking_array [lot];

				if (lot_ptr->Link () == loc_ptr->Link ()) link_flag = true;

				path_end.Clear ();
				path_end.Trip_End (org);
				path_end.End_Type (LOCATION_ID);
				path_end.Type (LINK_ID);
				path_end.Index (lot_ptr->Link ());

				if (lot_ptr->Dir () == 1) {
					link_ptr = &exe->link_array [lot_ptr->Link ()];
					path_end.Offset (link_ptr->Length () - lot_ptr->Offset ());
				} else {
					path_end.Offset (lot_ptr->Offset ());
				}
				path_data.Clear ();
				path_data.Imped (0);
				path_data.Time (org_itr->Time ());

				//--- save the access link attributes ----

				ttime = access_ptr->Time ();
				length = (int) (ttime * param.walk_speed + 0.5);
				cost = Round (access_ptr->Cost ());
				imped = DTOI (ttime * param.value_walk + cost * param.value_cost);

				path_data.Add_Imped (imped);
				path_data.Add_Time ((forward_flag) ? ttime : -ttime);
				path_data.Length (length);
				path_data.Cost (cost);
				path_data.Walk (length);
				path_data.Mode (WALK_MODE);
				path_data.From (acc);
				path_data.Type (ACCESS_ID);
				if (!ab_flag) path_data.Dir (1);
				path_data.Status (1);

				path_end.push_back (path_data);

				//---- calculate the parking attributes ----

				cost = ttime = 0;

				for (park_itr = lot_ptr->begin (); park_itr != lot_ptr->end (); park_itr++) {
					if (park_itr->Start () <= path_data.Time () && path_data.Time () <= park_itr->End ()) {
						if (park_itr->Use () == 0 || Use_Permission (park_itr->Use (), param.use)) {
							if (forward_flag) {
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
						} else {
							park_flag = true;
						}
					}
				}
				imped = DTOI (ttime * param.value_park + cost * param.value_cost);

				//---- add the parking penalty ----

				if (!forward_flag && param.park_pen_flag) {
					imped += exe->park_penalty [lot];
				}

				//---- save the parking lot attributes ----

				path_data.Add_Imped (imped);
				path_data.Add_Time ((forward_flag) ? ttime : -ttime);
				path_data.Add_Cost (cost);
				path_data.Mode (OTHER_MODE);
				path_data.From (lot);
				path_data.Type (PARKING_ID);
				path_data.Status (1);
				
				path_end.push_back (path_data);

				from_ptr->push_back (path_end);
			}
		}
		if (link_flag) continue;

		//---- find the closest parking lot on the link ----

		diff = MAX_INTEGER;
		lot = -1;
		index = loc_ptr->Link ();

		link_ptr = &exe->link_array [index];

		if (loc_ptr->Dir () == 1) {
			offset = link_ptr->Length () - loc_ptr->Offset ();
		} else {
			offset = loc_ptr->Offset ();
		}

		for (index = exe->link_parking [index]; index >= 0; index = exe->next_parking [index]) {
			if (forward_flag && parking >= 0 && parking != index) continue;

			lot_ptr = &exe->parking_array [index];

			if (lot_ptr->Dir () == 1) {
				length = abs (link_ptr->Length () - lot_ptr->Offset () - offset);
			} else {
				length = abs (lot_ptr->Offset () - offset);
			}
			if (length <= diff) {
				diff = length;
				lot = index;
				if ((forward_flag && parking >= 0) || 
					(length <= near_offset && loc_ptr->Location () == lot_ptr->Parking ())) break;
			}
		}
		if (lot < 0) continue;
		if (diff > near_offset && !Use_Permission (link_ptr->Use (), WALK)) {
			walk_acc_flag = true;
			continue;
		}

		//---- save a parking lot ----

		lot_ptr = &exe->parking_array [lot];

		path_end.Clear ();
		path_end.Trip_End (org);
		path_end.End_Type (LOCATION_ID);
		path_end.Type (LINK_ID);
		path_end.Index (lot_ptr->Link ());

		if (lot_ptr->Dir () == 1) {
			path_end.Offset (link_ptr->Length () - lot_ptr->Offset ());
		} else {
			path_end.Offset (lot_ptr->Offset ());
		}
		path_data.Clear ();
		path_data.Imped (0);
		path_data.Time (org_itr->Time ());

		//---- walk from the location to the link ----

		length = loc_ptr->Setback ();
		if (length < 1) length = 1;

		if (!param.walk_detail && diff > near_offset) length += diff;

		ttime = (int) (length / param.walk_speed + 0.5);
		imped = DTOI (ttime * param.value_walk);

		path_data.Add_Imped (imped);
		path_data.Add_Time ((forward_flag) ? ttime : -ttime);
		path_data.Add_Length (length);
		path_data.Add_Walk (length);
		path_data.Mode (WALK_MODE);
		path_data.From (org_itr->Index ());
		path_data.Type (LOCATION_ID);
		path_data.Status (1);

		path_end.push_back (path_data);

		//---- walk along the link ----

		if (param.walk_detail && diff > near_offset) {
			length = diff;

			ttime = (int) (length / param.walk_speed + 0.5);
			imped = DTOI (ttime * param.value_walk);

			path_data.Add_Imped (imped);
			path_data.Add_Time ((forward_flag) ? ttime : -ttime);
			path_data.Add_Length (length);
			path_data.Add_Walk (length);
			path_data.Mode (WALK_MODE);
			path_data.From (lot_ptr->Link ());
			path_data.Type (LINK_ID);

			if (forward_flag) {
				if (path_end.Offset () < offset) path_data.Dir (1);
			} else {
				if (path_end.Offset () > offset) path_data.Dir (1);
			}
			path_data.Status (1);

			path_end.push_back (path_data);
		}

		//---- calculate the parking attributes ----

		cost = ttime = 0;

		for (park_itr = lot_ptr->begin (); park_itr != lot_ptr->end (); park_itr++) {
			if (park_itr->Start () <= path_data.Time () && path_data.Time () <= park_itr->End ()) {
				if (park_itr->Use () == 0 || Use_Permission (park_itr->Use (), param.use)) {
					if (forward_flag) {
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
				} else {
					park_flag = true;
				}
			}
		}
		imped = DTOI (ttime * param.value_park + cost * param.value_cost);

		//---- add the parking penalty ----

		if (!forward_flag && param.park_pen_flag) {
			imped += exe->park_penalty [lot];
		}

		//---- save the parking lot attributes ----

		path_data.Add_Imped (imped);
		path_data.Add_Time ((forward_flag) ? ttime : -ttime);
		path_data.Add_Cost (cost);
		path_data.Mode (OTHER_MODE);
		path_data.From (lot);
		path_data.Type (PARKING_ID);
		path_data.Status (1);
				
		path_end.push_back (path_data);

		from_ptr->push_back (path_end);
	}
	access_flag = (from_ptr->size () == 0);
	return (!access_flag);
}

//---------------------------------------------------------
//	Destination_Parking
//---------------------------------------------------------

bool Path_Builder::Destination_Parking (Trip_End_Array *des_ptr, Path_End_Array *to_ptr, int parking)
{
	int index, length, offset, diff, lot, des, imped, ttime, cost, acc, low_imp, hi_imp, tot_imp, tod;
	bool link_flag, ab_flag;

	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Parking_Data *lot_ptr;
	Park_Nest_Itr park_itr;
	Trip_End_Itr des_itr;
	Path_End path_end;
	Path_Data path_data;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	to_ptr->clear ();
	low_imp = MAX_INTEGER;
	hi_imp = 0;

	park_flag = walk_acc_flag = access_flag = false;

	for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {
		if (des_itr->Index () < 0) continue;
		
		link_flag = false;
		loc_ptr = &exe->location_array [des_itr->Index ()];

		//---- add the access links to parking ----

		if (exe->access_link_flag) {
			acc = exe->loc_access [des_itr->Index ()].Next (!forward_flag);

			//---- process each access link to the destination ----

			for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
				access_ptr = &exe->access_array [acc];
				acc_ptr = &exe->next_access (!forward_flag) [acc];

				ab_flag = (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == des_itr->Index ());
				
				if (access_ptr->Type (!ab_flag) != PARKING_ID) continue;
				lot = access_ptr->ID (!ab_flag);
				
				if (!forward_flag && parking >= 0 && parking != lot) continue;

				//---- save a parking connection ----

				lot_ptr = &exe->parking_array [lot];

				if (lot_ptr->Link () == loc_ptr->Link ()) link_flag = true;

				path_end.Clear ();
				path_end.Trip_End (des);
				path_end.End_Type (LOCATION_ID);
				path_end.Type (LINK_ID);
				path_end.Index (lot_ptr->Link ());

				if (lot_ptr->Dir () == 1) {
					link_ptr = &exe->link_array [lot_ptr->Link ()];
					path_end.Offset (link_ptr->Length () - lot_ptr->Offset ());
				} else {
					path_end.Offset (lot_ptr->Offset ());
				}
				path_data.Clear ();	
				tot_imp = 0;
				tod = des_itr->Time ();
				
				//--- save the access link attributes ----

				ttime = access_ptr->Time ();
				length = (int) (ttime * param.walk_speed + 0.5);
				cost = Round (access_ptr->Cost ());
				imped = DTOI (ttime * param.value_walk + cost * param.value_cost);

				tot_imp += imped;
				tod += ((!forward_flag) ? ttime : -ttime);

				path_data.Imped (imped);
				path_data.Time (ttime);
				path_data.Length (length);
				path_data.Cost (cost);
				path_data.Walk (length);
				path_data.Mode (WALK_MODE);
				path_data.From (acc);
				path_data.Type (ACCESS_ID);
				if (ab_flag) path_data.Dir (1);

				path_end.push_back (path_data);

				//---- calculate the parking attributes ----

				cost = ttime = 0;

				for (park_itr = lot_ptr->begin (); park_itr != lot_ptr->end (); park_itr++) {
					if (park_itr->Start () <= tod && tod <= park_itr->End ()) {
						if (park_itr->Use () == 0 || Use_Permission (park_itr->Use (), param.use)) {
							if (!forward_flag) {
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
						} else {
							park_flag = true;
						}
					}
				}
				imped = DTOI (ttime * param.value_park + cost * param.value_cost);

				//---- add the parking penalty ----

				if (forward_flag && param.park_pen_flag) {
					imped += exe->park_penalty [lot];
				}
				tot_imp += imped;
				tod += (!forward_flag) ? ttime : -ttime;

				//---- save the parking lot attributes ----

				path_data.Clear ();				
				path_data.Imped (imped);
				path_data.Time (ttime);
				path_data.Cost (cost);
				path_data.Mode (OTHER_MODE);
				path_data.From (lot);
				path_data.Type (PARKING_ID);

				path_end.push_back (path_data);

				//---- check the impedance range ----

				if (tot_imp < low_imp) low_imp = tot_imp;
				if (tot_imp > hi_imp) hi_imp = tot_imp;

				path_data.Clear ();

				path_end.push_back (path_data);
				to_ptr->push_back (path_end);
			}
		}
		if (link_flag) continue;

		//---- find the closest parking lot on the link ----

		diff = MAX_INTEGER;
		lot = -1;
		index = loc_ptr->Link ();

		link_ptr = &exe->link_array [index];

		if (loc_ptr->Dir () == 1) {
			offset = link_ptr->Length () - loc_ptr->Offset ();
		} else {
			offset = loc_ptr->Offset ();
		}

		for (index = exe->link_parking [index]; index >= 0; index = exe->next_parking [index]) {
			if (!forward_flag && parking >= 0 && parking != index) continue;

			lot_ptr = &exe->parking_array [index];

			if (lot_ptr->Dir () == 1) {
				length = abs (link_ptr->Length () - lot_ptr->Offset () - offset);
			} else {
				length = abs (lot_ptr->Offset () - offset);
			}
			if (length <= diff) {
				diff = length;
				lot = index;
				if ((!forward_flag && parking >= 0) || 
					(length <= near_offset && loc_ptr->Location () == lot_ptr->Parking ())) break;
			}
		}
		if (lot < 0) continue;
		if (diff > near_offset && !Use_Permission (link_ptr->Use (), WALK)) {
			walk_acc_flag = true;
			continue;
		}

		//---- save a parking lot ----

		lot_ptr = &exe->parking_array [lot];

		path_end.Clear ();
		path_end.Trip_End (des);
		path_end.End_Type (LOCATION_ID);
		path_end.Type (LINK_ID);
		path_end.Index (lot_ptr->Link ());

		if (lot_ptr->Dir () == 1) {
			path_end.Offset (link_ptr->Length () - lot_ptr->Offset ());
		} else {
			path_end.Offset (lot_ptr->Offset ());
		}
		path_data.Clear ();
		tot_imp = 0;
		tod = des_itr->Time ();

		//---- walk from the location to the link ----

		length = loc_ptr->Setback ();
		if (length < 1) length = 1;

		if (!param.walk_detail && diff > near_offset) length += diff;

		ttime = (int) (length / param.walk_speed + 0.5);
		imped = DTOI (ttime * param.value_walk);

		tot_imp += imped;
		tod += (!forward_flag) ? ttime : -ttime;		

		path_data.Imped (imped);
		path_data.Time (ttime);
		path_data.Length (length);
		path_data.Walk (length);
		path_data.Mode (WALK_MODE);
		path_data.From (des_itr->Index ());
		path_data.Type (LOCATION_ID);

		path_end.push_back (path_data);

		//---- walk along the link ----

		if (param.walk_detail && diff > near_offset) {
			length = diff;

			ttime = (int) (length / param.walk_speed + 0.5);
			imped = DTOI (ttime * param.value_walk);

			tot_imp += imped;
			tod += (!forward_flag) ? ttime : -ttime;		

			path_data.Imped (imped);
			path_data.Time (ttime);
			path_data.Length (length);
			path_data.Walk (length);
			path_data.Mode (WALK_MODE);
			path_data.From (lot_ptr->Link ());
			path_data.Type (LINK_ID);
			if (!forward_flag) {
				if (path_end.Offset () < offset) path_data.Dir (1);
			} else {
				if (path_end.Offset () > offset) path_data.Dir (1);
			}
			path_end.push_back (path_data);
		}

		//---- calculate the parking attributes ----

		cost = ttime = 0;

		for (park_itr = lot_ptr->begin (); park_itr != lot_ptr->end (); park_itr++) {
			if (park_itr->Start () <= tod && tod <= park_itr->End ()) {
				if (park_itr->Use () == 0 || Use_Permission (park_itr->Use (), param.use)) {
					if (!forward_flag) {
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
				} else {
					park_flag = true;
				}
			}
		}
		imped = DTOI (ttime * param.value_park + cost * param.value_cost);

		//---- add the parking penalty ----

		if (forward_flag && param.park_pen_flag) {
			imped += exe->park_penalty [lot];
		}
		tot_imp += imped;

		//---- save the parking lot attributes ----
		
		path_data.Clear ();
		path_data.Imped (imped);
		path_data.Time (ttime);
		path_data.Cost (cost);
		path_data.Mode (OTHER_MODE);
		path_data.From (lot);
		path_data.Type (PARKING_ID);
		
		path_end.push_back (path_data);

		//---- check the impedance range ----

		if (tot_imp < low_imp) low_imp = tot_imp;
		if (tot_imp > hi_imp) hi_imp = tot_imp;

		path_data.Clear ();

		path_end.push_back (path_data);

		to_ptr->push_back (path_end);
	}
	if (low_imp < MAX_INTEGER) {
		imp_diff = hi_imp - low_imp;
	} else {
		imp_diff = 0;
	}
	access_flag = (to_ptr->size () == 0);
	return (!access_flag);
}
