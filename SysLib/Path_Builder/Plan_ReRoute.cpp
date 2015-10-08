//*********************************************************
//	Plan_ReRoute - Build a Path after a Specified Time
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_ReRoute
//---------------------------------------------------------

bool Path_Builder::Plan_ReRoute (Plan_Data *plan_data)
{
	int veh_id, lot, use, veh_type, stat, len, cost, imp, link, mode;
	Dtime tod, time, reroute_time;
	double factor;
	bool start_flag;
	
	Trip_End trip_end;
	Path_End path_end;
	Path_Data path_data;
	Int_Map_Itr map_itr;
	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Vehicle_Data *veh_ptr = 0;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Plan_Leg_Itr leg_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;

	parking_duration = plan_ptr->Duration ();
	forward_flag = true;
	reroute_flag = false;

	//---- initialize the plan ----
	
	trip_org.clear ();
	trip_des.clear ();

	reroute_time = plan_ptr->Arrive ();
	start_flag = false;
	tod = plan_ptr->Depart ();

	//---- find the path mode at reroute time ---

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		tod += leg_itr->Time ();
		if (leg_itr->Mode () == DRIVE_MODE) start_flag = true;

		if (tod >= reroute_time) {
			if (leg_itr->Mode () == DRIVE_MODE) break;

			if (start_flag) {
				if (param.flow_flag) {
					return (Plan_Flow (plan_data));
				} else {
					return (true);
				}
			} else {
				return (Plan_Build (plan_data));
			}
		}
	}

	//---- trace the path up to reroute_time ----

	tod = plan_ptr->Depart ();
	plan_ptr->Zero_Totals ();
	factor = 1.0;
	start_flag = false;

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		mode = leg_itr->Mode ();
		time = leg_itr->Time ();
		len = leg_itr->Length ();
		cost = leg_itr->Cost ();
		imp = Round (leg_itr->Impedance ());

		tod += time;
		if (tod > reroute_time && time > 0) {
			factor = (double) (reroute_time - tod + time) / time;

			time = DTOI (time * factor);
			len = DTOI (len * factor);
			cost = DTOI (cost * factor);
			imp = DTOI (imp * factor);

			leg_itr->Time (time);
			leg_itr->Length (len);
			leg_itr->Cost (cost);
			leg_itr->Impedance (Resolve (imp));
		}
		if (mode == DRIVE_MODE) {
			plan_ptr->Add_Drive (time);
			start_flag = true;
		} else if (mode == TRANSIT_MODE) {
			plan_ptr->Add_Transit (time);
		} else if (mode == WALK_MODE) {
			plan_ptr->Add_Walk (time);
		} else if (mode == WAIT_MODE) {
			plan_ptr->Add_Wait (time);
		} else {
			plan_ptr->Add_Other (time);
		}
		plan_ptr->Add_Length (len);
		plan_ptr->Add_Cost (cost);
		plan_ptr->Add_Impedance (Resolve (imp));

		//---- set the origin ----

		if (tod >= reroute_time) {
			path_end.Clear ();
			path_end.Trip_End (0);
			path_end.End_Type (DIR_ID);
			path_end.Type (DIR_ID);

			link = abs (leg_itr->ID ());

			map_itr = exe->link_map.find (link);
			if (map_itr != exe->link_map.end ()) {
				path_end.Index (map_itr->second);
				link_ptr = &exe->link_array [map_itr->second];

				if (leg_itr->ID () < 0) {
					path_end.Index (link_ptr->BA_Dir ());
				} else {
					path_end.Index (link_ptr->AB_Dir ());
				}
				path_end.Offset (DTOI (link_ptr->Length () * factor));
			}
			path_data.Time (reroute_time);
			path_data.Imped (Round ((int) plan_ptr->Impedance ()));
			path_data.Length (plan_ptr->Length ());
			path_data.Cost (plan_ptr->Cost ());
			path_data.Walk (plan_ptr->Walk ());
			path_data.Path (-1);

			path_end.push_back (path_data);

			to_array.push_back (path_end);
			break;
		}
	}
	time_limit = MAX_INTEGER;
	reroute_flag = true;

	//---- remove the remaining legs ----

	if (++leg_itr != plan_ptr->end ()) {
		plan_ptr->erase (leg_itr, plan_ptr->end ());
	}
	if (param.flow_flag) {
		Plan_Flow (plan_data);
	}

	//---- set the destination ----

	map_itr = exe->location_map.find (plan_ptr->Destination ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	loc_ptr = &exe->location_array [map_itr->second];

	trip_end.Type (LOCATION_ID);
	trip_end.Index (map_itr->second);
	trip_end.Time (plan_ptr->End ());

	trip_des.push_back (trip_end);

	path_end.Clear ();
	path_end.Trip_End (0);
	path_end.End_Type (LOCATION_ID);
	path_end.Type (LINK_ID);
	path_end.Index (loc_ptr->Link ());

	if (loc_ptr->Dir () == 1) {
		link_ptr = &exe->link_array [loc_ptr->Link ()];
		path_end.Offset (link_ptr->Length () - loc_ptr->Offset ());
	} else {
		path_end.Offset (loc_ptr->Offset ());
	}
	path_data.Time (plan_ptr->End ());

	if (forward_flag) {
		path_data.Imped (MAX_IMPEDANCE);
		path_data.Path (0);
	} else {
		path_data.Imped (1);
		path_data.Path (-1);
	}
	path_end.push_back (path_data);

	to_array.push_back (path_end);

	//---- get the vehicle record ----

	veh_id = plan_ptr->Vehicle ();

	if (veh_id <= 0) {
		lot = -1;
		grade_flag = false;
		op_cost_rate = 0.0;
		use = CAR;
		veh_type = -1;
	} else {
		if (vehicle_flag) {
			veh_index.Household (plan_ptr->Household ());
			veh_index.Vehicle (veh_id);

			veh_itr = exe->vehicle_map.find (veh_index);
			if (veh_itr == exe->vehicle_map.end ()) {
				plan_ptr->Problem (VEHICLE_PROBLEM);
				return (true);
			}
			veh_ptr = &exe->vehicle_array [veh_itr->second];
			lot = -1;
			veh_type = veh_ptr->Type ();
		} else {
			lot = -1;
			veh_type = veh_id;
		}
		if (veh_type_flag) {
			veh_type_ptr = &exe->veh_type_array [veh_type];
			use = veh_type_ptr->Use ();
			op_cost_rate = UnRound (veh_type_ptr->Op_Cost ());

			if (Metric_Flag ()) {
				op_cost_rate /= 1000.0;
			} else {
				op_cost_rate /= MILETOFEET;
			}
			grade_flag = param.grade_flag && veh_type_ptr->Grade_Flag ();
		} else {
			grade_flag = false;
			op_cost_rate = 0.0;
			use = CAR;
			veh_type = -1;
		}
	}
	stat = Build_Path (lot);
	
	if (veh_ptr != 0 && parking_lot >= 0) {
		veh_ptr->Parking (parking_lot);
	}
	if (stat < 0) return (false);
	if (stat > 0) {
		if (!param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
