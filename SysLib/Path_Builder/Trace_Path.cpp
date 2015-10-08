//*********************************************************
//	Trace_Path.cpp - save plan legs and skim travel times
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Trace_Path
//---------------------------------------------------------

int Path_Builder::Trace_Path (Trip_End *org, Path_End_Array *from, Path_End *to)
{
	int type, index, dir, in_index, out_index, flow_index;
	int imped, len, cost, mode, path, tot_len, prev_len;
	int from_index, from_type, from_path, from_dir, to_index, to_type, to_path, to_dir, prev_imp;
	bool ab_flag, park_flag;
	Dtime time, duration, prev_time, tod, tod2;
	double flow;

	Trip_End *parking_end;
	Path_End *from_end;
	Path_Data *from_ptr, *to_ptr;
	Path_Itr path_itr;
	Path_RItr path_ritr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Flow_Time_Array *link_delay_ptr, *turn_delay_ptr;
	Flow_Time_Data *flow_ptr;
	Int2_Map_Itr map2_itr;
	Plan_Leg_RItr leg_itr;
	Plan_Leg *prev_leg;
	Stop_Data *stop_ptr;
	Access_Data *access_ptr;

	mode = param.mode;
	park_flag = (mode == PNR_OUT_MODE || mode == PNR_IN_MODE || mode == KNR_OUT_MODE || mode == KNR_IN_MODE);
	flow_flag = (param.flow_flag && mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
				mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE && to->Index () >= 0);

	path_itr = to->begin ();
	from_end = &from->at (0);

	if (path_itr->Status () == 0) {
		plan_ptr->Clear_Plan ();
		return (0);
	}

	//---- save the path skim data ----

	plan_ptr->Activity (plan_ptr->Duration ());

	if (forward_flag) {
		if (!reroute_flag) plan_ptr->Depart (org->Time ().Round_Seconds ());
		plan_ptr->Arrive (path_itr->Time ().Round_Seconds ());

		if (plan_ptr->Arrive () > plan_ptr->End () && plan_ptr->Duration () > 0 &&
			plan_ptr->Constraint () != FIXED_TIME && plan_ptr->Constraint () != DURATION) {

			duration = DTOI (plan_ptr->Duration () * param.duration_factor [plan_ptr->Priority ()]);
			time = plan_ptr->End () + plan_ptr->Duration ();
			if (time > plan_ptr->Arrive () + duration.Round_Seconds ()) {
				plan_ptr->Activity (time - plan_ptr->Arrive ());
			} else {
				plan_ptr->Activity (duration.Round_Seconds ());
			}
		}
	} else {
		plan_ptr->Depart (path_itr->Time ().Round_Seconds ());
		plan_ptr->Arrive (org->Time ().Round_Seconds ());
	}
	plan_ptr->Impedance (Resolve (path_itr->Imped ()));
	plan_ptr->Length (path_itr->Length ());
	plan_ptr->Cost (path_itr->Cost ());

	//---- magic moves ----

	if (mode == RIDE_MODE || mode == OTHER_MODE) {
		plan_ptr->Other (plan_ptr->Arrive () - plan_ptr->Depart ());
		return (0);
	}

	//---- all walk path with no detail ----

	if (mode == WALK_MODE && !param.walk_detail) {
		plan_ptr->Walk (plan_ptr->Arrive () - plan_ptr->Depart ());
		return (0);
	}

	//---- add legs to the plan record ----

	if (forward_flag) {
		leg_ptr = &leg_array;
	} else {
		leg_ptr = plan_ptr;
	}
	if (!param.skim_only) leg_ptr->clear ();

	prev_time = 0;
	prev_imp = prev_len = tot_len = 0;
next_mode:
	from_ptr = to_ptr = &(*path_itr);
	
	//---- insert the destination access legs ----

	for (++path_itr; path_itr != to->end (); path_itr++) {
		from_ptr = &(*path_itr);
		type = to_ptr->Type ();

		//---- don't write parking ID twice ----

		if (to->End_Type () == PARKING_ID && type == PARKING_ID && 
			(param.mode == PNR_IN_MODE || param.mode == KNR_IN_MODE)) {
			to_ptr = from_ptr;
			continue;
		}
		
		//---- get leg attributes ----

		if (forward_flag) {
			time = to_ptr->Time () - from_ptr->Time ();
		} else {
			time = from_ptr->Time () - to_ptr->Time ();
		}
		len = to_ptr->Length () - from_ptr->Length ();
		imped = Resolve (to_ptr->Imped () - from_ptr->Imped ());
		cost = to_ptr->Cost () - from_ptr->Cost ();

		if (to_ptr->Mode () == WAIT_MODE && !forward_flag) {
			prev_time = time;
			prev_imp = imped;
			prev_len = len;
		} else {
			Add_Leg (to_ptr->Mode (), type, to_ptr->From (), to_ptr->Dir (), imped, time, len, cost);
		}
		tot_len += len;
		to_ptr = from_ptr;
	}
	to_index = from_index = to->Index ();
	to_type = from_type = to->Type ();	//TO_ID;
	to_path = from_path = to_ptr->Path ();
	to_dir = from_dir = to_ptr->Dir ();
	out_index = in_index = -1;

	//----- trace the path ----

	for (; from_type != FROM_ID; to_ptr = from_ptr) {
		if ((int) leg_ptr->size () > param.leg_check) {
			plan_ptr->Problem (TRACE_PROBLEM);
			return (0);
		}
		to_index = from_index;
		to_type = from_type;
		to_path = from_path;
		to_dir = from_dir;
		out_index = in_index;

		//---- retrieve the from record ----

		from_index = to_ptr->From ();
		from_type = to_ptr->Type ();
		from_path = to_ptr->Path ();
		from_dir = to_ptr->Dir ();
		mode = to_ptr->Mode ();

		if (from_type == DIR_ID) {
			from_ptr = &link_path [from_index];	
		} else if (from_type == FROM_ID) {
			if (park_flag) {
				from_end = &from_parking [from_index];
				park_flag = false;
			} else {
				from_end = &from->at (from_index);
			}
			path_ritr = from_end->rbegin ();
			from_ptr = &(*path_ritr);
		} else if (from_type == NODE_ID) {
			from_ptr = &node_path [from_path] [from_index];
		} else if (from_type == STOP_ID) {
			if (mode == WAIT_MODE) {
				from_ptr = &board_path [from_path] [from_index];
			} else {
				from_ptr = &alight_path [from_path] [from_index];
			}
		} else if (from_type == ROUTE_ID) {
			from_ptr = &wait_path [from_path] [to_index];
		} else if (from_type == ACCESS_ID) {
			access_ptr = &exe->access_array [from_index];

			if (param.walk_detail) {
				Add_Leg (mode, to_type, to_index);

				to_type = from_type;
				to_index = from_index;
				to_path = from_path;
				to_dir = from_dir;
			}
			from_type = access_ptr->Type ((from_dir == 0));
			from_index = access_ptr->ID ((from_dir == 0));

			if (from_type == NODE_ID) {
				from_ptr = &node_path [from_path] [from_index];
			} else if (from_type == STOP_ID) {
				from_ptr = &alight_path [from_path] [from_index];
			}
		} else {
			exe->Write (1, "From Type=") << from_type << "-" << from_index << " not implemented to=" << to_type << "-" << to_index;
			return (0);
		}

		//---- accumulate mode-specific travel time ---

		if (forward_flag) {
			time = to_ptr->Time () - from_ptr->Time ();
		} else {
			time = from_ptr->Time () - to_ptr->Time ();
		}
		len = to_ptr->Length () - from_ptr->Length ();
		imped = Resolve (to_ptr->Imped () - from_ptr->Imped ());
		cost = to_ptr->Cost () - from_ptr->Cost ();

		//---- save the plan leg or flow data ----

		if (!param.skim_only || (flow_flag && mode == DRIVE_MODE)) {
			if (forward_flag) {
				type = to_type;
				index = in_index = to_index;
				path = from_path;
				dir = to_dir;
			} else if (from_type == FROM_ID) {
				index = from_end->Index ();
				type = from_end->Type ();
				path = to_path;
				dir = from_dir;
			} else {
				index = to_index;
				type = to_type;
				path = to_path;
				dir = to_dir;
			}
			if (type == DIR_ID) {
				in_index = index;
			} else if (type == LINK_ID) {
				if (mode == DRIVE_MODE) {
					if (index < 0 || index >= (int) exe->link_array.size ()) return (0);
					link_ptr = &exe->link_array [index];
					if (dir == 0) {
						in_index = link_ptr->AB_Dir ();
					} else {
						in_index = link_ptr->BA_Dir ();
					}
				}
			} else if (type == NODE_ID) {
				if (from_type == NODE_ID) {
					int anode, bnode;

					if (index > from_index) {
						anode = from_index;
						bnode = index;
						ab_flag = forward_flag;
					} else {
						anode = index;
						bnode = from_index;
						ab_flag = !forward_flag;
					}
					Int2_Map_Itr itr = exe->ab_link.find (Int2_Key (anode, bnode));

					if (itr != exe->ab_link.end ()) {
						type = LINK_ID;
						index = itr->second;
						dir = (ab_flag) ? 0 : 1;
					} else {
						exe->Error (String ("Node Path Error %d-%d") % anode % bnode);
					}
				} else if (from_type == FROM_ID) {
					if (from_end->Type () == NODE_ID) {
						continue;	//---- ignore the extra node record ----
					} else if (from_end->Type () == LINK_ID) {
						if (from_end->Index () < 0) return (0);

						link_ptr = &exe->link_array [from_end->Index ()];
						ab_flag = (link_ptr->Bnode () == index) ? forward_flag : !forward_flag;

						type = LINK_ID;
						index = from_end->Index ();
						dir = (ab_flag) ? 0 : 1;
						in_index = (ab_flag) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();
					}
				} else if (from_type == STOP_ID) {
					if (from_index < 0) return (0);
					if (param.walk_detail) {
						stop_ptr = &exe->stop_array [from_index];
						link_ptr = &exe->link_array [stop_ptr->Link ()];

						ab_flag = (link_ptr->Bnode () == index) ? forward_flag : !forward_flag;

						type = LINK_ID;
						index = stop_ptr->Link ();
						dir = (ab_flag) ? 0 : 1;
					}
				} else {
					exe->Warning ("File_Type=") << from_type;
				}

			} else if (type == STOP_ID) {
				if (forward_flag) {
					if (param.walk_detail && (from_type == NODE_ID || from_type == FROM_ID)) {
						Add_Leg (mode, type, index);
						if (from_type == FROM_ID && time == 0) continue;

						stop_ptr = &exe->stop_array [index];

						type = LINK_ID;
						index = stop_ptr->Link ();
						dir = from_dir;
					} else {
						if (time == 0 && from_type == to_type && from_index == to_index) continue;
					}
				} else {
					if (mode == WAIT_MODE) {
						Add_Leg (TRANSIT_MODE, type, index, dir, prev_imp, prev_time, prev_len);
						prev_imp = imped;
						prev_time = time;
						prev_len = len;
						continue;
					}
					if (from_type == FROM_ID && time == 0) continue;
					if (time == 0 && from_type == to_type && from_index == to_index) continue;

					if (mode == WALK_MODE) {
						stop_ptr = &exe->stop_array [index];

						Add_Leg (mode, LINK_ID, stop_ptr->Link (), from_dir, imped, time, len, cost);

						imped = time = len = cost = 0;
					}
				}
			} else if (type == ROUTE_ID && !forward_flag) {
				prev_leg = &leg_ptr->back ();

				if (prev_leg->Type () != to_type) {
					Add_Leg (WALK_MODE, to_type, to_index);
				}
				Add_Leg (WAIT_MODE, type, index, dir, prev_imp, prev_time, prev_len);
				prev_imp = imped;
				prev_time = time;
				prev_len = len;
				continue;
			}

			//---- save the leg ----

			Add_Leg (mode, type, index, dir, imped, time, len, cost, path);

			//---- accumulate flow and turning movement data ----

			if (flow_flag && mode == DRIVE_MODE) {
				if (forward_flag) {
					tod = from_ptr->Time ();
					tod2 = to_ptr->Time ();
				} else {
					tod = to_ptr->Time ();
					tod2 = from_ptr->Time ();
				}
				link_delay_ptr = link_flow_ptr->Period_Ptr (tod);

				if (link_delay_ptr != 0) {
					if (path == 0) {
						flow_index = in_index;
					} else {
						dir_ptr = &exe->dir_array [in_index];
						flow_index = dir_ptr->Flow_Index ();
						if (flow_index < 0) flow_index = in_index;
					}
					flow_ptr = link_delay_ptr->Data_Ptr (flow_index);

					if (type == DIR_ID) {
						dir_ptr = &exe->dir_array [index];
						link_ptr = &exe->link_array [dir_ptr->Link ()];
					} else {
						link_ptr = &exe->link_array [abs (index)];
					}
					if (len >= link_ptr->Length ()) {
						flow = 1.0;
					} else {
						flow = (double) len / link_ptr->Length ();
						if (flow < 0.01) flow = 0.01;
					}
					flow_ptr->Add_Flow (flow);
				}

				if (out_index >= 0 && param.turn_flow_flag) {
					if (forward_flag) {
						map2_itr = exe->connect_map.find (Int2_Key (in_index, out_index));
					} else {
						map2_itr = exe->connect_map.find (Int2_Key (out_index, in_index));
					}
					if (map2_itr != exe->connect_map.end ()) {
						turn_delay_ptr = turn_flow_ptr->Period_Ptr (tod2);

						if (turn_delay_ptr != 0) {
							flow_ptr = turn_delay_ptr->Data_Ptr (map2_itr->second);
							flow_ptr->Add_Flow (1.0);
						}
					}
				}
			}
		}
		tot_len += len;
		dir = to_dir;
	}

	//---- end node ----

	for (++path_ritr; path_ritr != from_end->rend (); path_ritr++) {
		from_ptr = &(*path_ritr);
		mode = to_ptr->Mode ();

		if (forward_flag) {
			time = to_ptr->Time () - from_ptr->Time ();
		} else {
			time = from_ptr->Time () - to_ptr->Time ();
		}
		len = to_ptr->Length () - from_ptr->Length ();
		imped = Resolve (to_ptr->Imped () - from_ptr->Imped ());
		cost = to_ptr->Cost () - from_ptr->Cost ();

		Add_Leg (mode, to_ptr->Type (), to_ptr->From (), to_ptr->Dir (), imped, time, len, cost, to_ptr->Path ());

		tot_len += len;
		to_ptr = from_ptr;
	}
	if (from_end->End_Type () == PARKING_ID) {
		parking_end = &parking_lots [from_end->Trip_End ()];

		to = &to_parking [parking_end->Best ()];
		path_itr = to->begin ();

		if (to_ptr->Type () == PARKING_ID && (param.mode == PNR_OUT_MODE || param.mode == KNR_OUT_MODE)) goto next_mode;

		if (forward_flag) {
			time = to_ptr->Time () - path_itr->Time ();
		} else {
			time = path_itr->Time () - to_ptr->Time ();
		}
		len = to_ptr->Length () - path_itr->Length ();
		imped = Resolve (to_ptr->Imped () - path_itr->Imped ());
		cost = to_ptr->Cost () - path_itr->Cost ();
	} else {
		if (forward_flag) {
			time = to_ptr->Time () - org->Time ();
		} else {
			time = org->Time () - to_ptr->Time ();
		}
		len = to_ptr->Length ();
		imped = Resolve (to_ptr->Imped ());
		cost = to_ptr->Cost ();
	}
	Add_Leg (to_ptr->Mode (), to_ptr->Type (), to_ptr->From (), to_ptr->Dir (), imped, time, len, cost, to_ptr->Path ());

	if (from_end->End_Type () == PARKING_ID) goto next_mode;

	//---- check for early arrivals ----

	if (!forward_flag && prev_time > 0) {
		Add_Leg (WAIT_MODE, LOCATION_ID, -plan_ptr->Destination (), 0, prev_imp, prev_time);
	}

	plan_ptr->Length (tot_len);

	//---- reverse the legs and add to the plan record ----

	if (!param.skim_only && forward_flag) {
		if (reroute_flag) {
			leg_itr = leg_ptr->rbegin ();
			if (leg_itr != leg_ptr->rend ()) {
				Plan_Leg *leg = &plan_ptr->back ();
				leg->Time (leg->Time () + leg_itr->Time ());
				leg->Length (leg->Length () + leg_itr->Length ());
				leg->Cost (leg->Cost () + leg_itr->Cost ());
				leg->Impedance (leg->Impedance () + leg_itr->Impedance ());

				for (++leg_itr; leg_itr != leg_ptr->rend (); leg_itr++) {
					plan_ptr->push_back (*leg_itr);
				}
			}
		} else {
			for (leg_itr = leg_ptr->rbegin (); leg_itr != leg_ptr->rend (); leg_itr++) {
				plan_ptr->push_back (*leg_itr);
			}
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Add_Leg
//---------------------------------------------------------

bool Path_Builder::Add_Leg (int mode, int type, int index, int dir, int imped, int time, int len, int cost, int path)
{
	int id;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Plan_Leg plan_leg, *prev_leg;

	switch (mode) {
		case DRIVE_MODE:
			plan_ptr->Add_Drive (time);
			break;
		case TRANSIT_MODE:
			plan_ptr->Add_Transit (time);
			break;
		case WALK_MODE:
			plan_ptr->Add_Walk (time);
			break;
		case WAIT_MODE:
			plan_ptr->Add_Wait (time);
			break;
		default:
			plan_ptr->Add_Other (time);
			break;
	}

	//---- save the plan leg or flow data ----

	if (!param.skim_only || (flow_flag && mode == DRIVE_MODE)) {
		if (index < 0) {
			id = -index;
		} else {
			switch (type) {
				case DIR_ID:
					dir_ptr = &exe->dir_array [index];

					index = dir_ptr->Link ();
					dir = dir_ptr->Dir ();
				case LINK_ID:
					link_ptr = &exe->link_array [index];

					id = link_ptr->Link ();
					if (mode == DRIVE_MODE && path > 0) {
						type = (dir == 1) ? USE_BA : USE_AB;
					} else {
						type = (dir == 1) ? LINK_BA : LINK_AB;
					}
					if (len > link_ptr->Length ()) len = link_ptr->Length ();
					break;
				case NODE_ID:
					id = exe->node_array [index].Node ();
					break;
				case LOCATION_ID:
					id = exe->location_array [index].Location ();
					break;
				case PARKING_ID:
					if (forward_flag) {
						if (parking_lot < 0) parking_lot = index;
					} else {
						parking_lot = index;
					}
					id = exe->parking_array [index].Parking ();
					break;
				case ACCESS_ID:
					id = exe->access_array [index].Link ();
					type = (dir == 1) ? ACCESS_BA : ACCESS_AB;
					break;
				case STOP_ID:
					id = exe->stop_array [index].Stop ();
					break;
				case ROUTE_ID:
					id = exe->line_array [index].Route ();
					break;
				default:
					exe->Write (1, "ID Type=") << type << " not implemented";
					return (false);
			}
		}

		//---- accumulate walk data ----
				
		if (mode == WALK && !param.walk_detail && leg_ptr->size () > 0) {
			prev_leg = &leg_ptr->back ();

			if (prev_leg->Mode () == WALK_MODE) {
				if (!forward_flag) {
					prev_leg->Type (type);
					prev_leg->ID (id);
				}
				prev_leg->Add_Imped (imped);
				prev_leg->Add_Time (time);
				prev_leg->Add_Length (len);
				prev_leg->Add_Cost (cost);
				return (true);
			}
		}
		plan_leg.Type (type);
		plan_leg.ID (id);
		plan_leg.Mode (mode);
		plan_leg.Impedance (imped);
		plan_leg.Time (time);
		plan_leg.Length (len);
		plan_leg.Cost (cost);

		leg_ptr->push_back (plan_leg);
	}
	return (true);
}
