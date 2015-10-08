//*********************************************************
//	Path_Builder.cpp - Network Path Building
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Path_Builder constructor
//---------------------------------------------------------

Path_Builder::Path_Builder (Router_Service *exe) : Static_Service ()
{
	Initialize (exe);
}
#ifdef BOOST_THREADS

Path_Builder::Path_Builder (Plan_Queue *queue, Router_Service *exe) : Static_Service ()
{
	if (queue == 0) {
		cout << "\tPlan Queue is Zero" << endl;
		initialized = false;
	} else {
		Initialize (exe);
		plan_queue = queue;
		param.one_to_many = false;
		plan_flag = true;
	}
}
Path_Builder::Path_Builder (Skim_Queue *queue, Router_Service *exe) : Static_Service ()
{
	if (queue == 0) {
		cout << "\tSkim Queue is Zero" << endl;
		initialized = false;
	} else {
		Initialize (exe);
		skim_queue = queue;
		param.one_to_many = true;
		plan_flag = false;
	}
}

//---------------------------------------------------------
//	Save_Flows
//---------------------------------------------------------

void Path_Builder::Save_Flows (void)
{
	MAIN_LOCK
	int index, period;

	Flow_Time_Period_Itr period_itr;
	Flow_Time_Array *period_ptr;
	Flow_Time_Itr data_itr;
	Flow_Time_Data *data_ptr;

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {
		period_ptr = &exe->link_delay_array [period];

		for (index=0, data_itr = period_itr->begin (); data_itr != period_itr->end (); data_itr++, index++) {
			if (data_itr->Flow () > 0.0) {
				data_ptr = &period_ptr->at (index);
				data_ptr->Add_Flow (data_itr->Flow ());
				data_itr->Flow (0);
			}
		}
	}
	if (param.turn_flow_flag) {
		for (period=0, period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++, period++) {
			period_ptr = &exe->turn_delay_array [period];

			for (index=0, data_itr = period_itr->begin (); data_itr != period_itr->end (); data_itr++, index++) {
				if (data_itr->Flow () > 0.0) {
					data_ptr = &period_ptr->at (index);
					data_ptr->Add_Flow (data_itr->Flow ());
					data_itr->Flow (0);
				}
			}
		}
	}
	END_LOCK
}
#endif

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Path_Builder::Initialize (Router_Service *_exe)
{
	if (_exe == 0) {
		initialized = false;
		return;
	}
	exe = _exe;
	initialized = plan_flag = forward_flag = true;
	reroute_flag = link_to_flag = node_to_flag = stop_to_flag = false;

	if (exe->Service_Level () < ROUTER_SERVICE) {
		exe->Error ("Path Building Requires Router Services");
	}
	max_imp = MAX_IMPEDANCE;
	imp_diff = 0;
	time_limit = min_time_limit = 0;
	data_ptr = 0;
	parking_duration = 0;
	op_cost_rate = 0.0;
	grade_flag = false;
	veh_type_ptr = 0;
	parking_lot = -1;

	mode_path_flag [WALK_MODE] = exe->walk_path_flag;
	mode_path_flag [BIKE_MODE] = exe->bike_path_flag;
	mode_path_flag [DRIVE_MODE] = exe->drive_path_flag;
	mode_path_flag [RIDE_MODE] = true;
	mode_path_flag [TRANSIT_MODE] = exe->transit_path_flag;
	mode_path_flag [PNR_OUT_MODE] = exe->parkride_path_flag;
	mode_path_flag [PNR_IN_MODE] = exe->parkride_path_flag;
	mode_path_flag [KNR_OUT_MODE] = exe->kissride_path_flag;
	mode_path_flag [KNR_IN_MODE] = exe->kissride_path_flag;
	mode_path_flag [TAXI_MODE] = exe->drive_path_flag;
	mode_path_flag [OTHER_MODE] = true;
	mode_path_flag [HOV2_MODE] = exe->drive_path_flag;
	mode_path_flag [HOV3_MODE] = exe->drive_path_flag;
	mode_path_flag [HOV4_MODE] = exe->drive_path_flag;

	near_offset = Round (Internal_Units (10.0, FEET));	

	vehicle_flag = exe->System_Data_Flag (VEHICLE);
	veh_type_flag = exe->System_Data_Flag (VEHICLE_TYPE);

	exe->Set_Parameters (param);
	plan_flag = !param.one_to_many;

	if (param.flow_flag) {
		if (exe->Num_Threads () < 2) {
			link_flow_ptr = &exe->link_delay_array;
			turn_flow_ptr = &exe->turn_delay_array;
		} else {
#ifdef BOOST_THREADS
			link_flow_ptr = &link_delay_array;
			turn_flow_ptr = &turn_delay_array;

			link_flow_ptr->Replicate (exe->link_delay_array);

			if (param.turn_flow_flag) {
				turn_flow_ptr->Replicate (exe->turn_delay_array);
			}
#endif
		}
	}
}

//---------------------------------------------------------
//	Transit_Path_Index operator
//---------------------------------------------------------

bool operator == (Transit_Path_Index left, Transit_Path_Index right)
{
	return (left.Index () == right.Index () &&
		left.Type () == right.Type () && 
		left.Path () == right.Path ());
}
