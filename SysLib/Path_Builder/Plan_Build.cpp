//*********************************************************
//	Plan_Build - Build a Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Build
//---------------------------------------------------------

bool Path_Builder::Plan_Build (Plan_Data *plan_data)
{
	int veh_id, lot, stat;
	
	Trip_End trip_end;	
	Int_Map_Itr map_itr;
	Vehicle_Data *veh_ptr = 0;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	if (!initialized) {
		cout << "\tPath Building Requires TRANSIMS Router Services" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;
	stat = plan_ptr->Depart ();
	plan_ptr->Clear_Plan ();
	plan_ptr->Depart (stat);

	if (plan_ptr->Mode () >= MAX_MODE || !mode_path_flag [plan_ptr->Mode ()]) return (false);

	//---- set the traveler parameters ----

	exe->Set_Parameters (param, plan_ptr->Type ());

	param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = (plan_ptr->Constraint () != END_TIME);
	time_limit = (forward_flag) ? MAX_INTEGER : 0;
	reroute_flag = false;

	//---- initialize the plan ----

	trip_org.clear ();
	trip_des.clear ();

	//---- set the origin ----

	map_itr = exe->location_map.find (plan_ptr->Origin ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	trip_end.Type (LOCATION_ID);
	trip_end.Index (map_itr->second);

	if (plan_ptr->Depart () > 0) {
		trip_end.Time (plan_ptr->Depart ());
	} else {
		trip_end.Time (plan_ptr->Start ());
	}
	trip_org.push_back (trip_end);

	//---- set the destination ----

	map_itr = exe->location_map.find (plan_ptr->Destination ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	trip_end.Type (LOCATION_ID); 
	trip_end.Index (map_itr->second);

	if (plan_ptr->Arrive () > 0) {
		trip_end.Time (plan_ptr->Arrive ());
	} else {
		trip_end.Time (plan_ptr->End ());
	}
	trip_des.push_back (trip_end);

	//---- get the vehicle record ----

	veh_id = plan_ptr->Vehicle ();

	if (veh_id <= 0) {
		lot = -1;
		grade_flag = false;
		op_cost_rate = 0.0;
		param.use = CAR;
		param.veh_type = -1;
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
			lot = (param.ignore_veh_flag) ? -1 : veh_ptr->Parking ();
			param.veh_type = veh_ptr->Type ();
		} else {
			if (veh_type_flag && veh_id >= (int) exe->veh_type_array.size ()) {
				veh_id = 0;
			}
			lot = -1;
			param.veh_type = veh_id;
		}
		if (veh_type_flag) {
			veh_type_ptr = &exe->veh_type_array [param.veh_type];
			param.use = veh_type_ptr->Use ();
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
			param.use = CAR;
			param.veh_type = -1;
		}
	}
	if (param.use == CAR && plan_ptr->Passengers () > 0) {
		param.use = (Use_Type) (SOV + MIN (plan_ptr->Passengers (), 3));
	}

	//---- build the path -----

	stat = Build_Path (lot);

	if (stat < 0) return (false);

	if (lot >= 0 && parking_lot >= 0) {
		veh_ptr->Parking (parking_lot);
	}
	if (stat > 0) {
		if (!param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
