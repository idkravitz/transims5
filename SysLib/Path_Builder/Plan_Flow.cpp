//*********************************************************
//	Plan_Flow - update link/turn flows
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Flow
//---------------------------------------------------------

bool Path_Builder::Plan_Flow (Plan_Data *plan_data)
{
	int mode, index, dir_index, flow_index, len;
	Dtime time, ttime;
	double factor;

	Plan_Leg_Itr leg_itr;
	Flow_Time_Array *link_delay_ptr, *turn_delay_ptr;
	Flow_Time_Data *data_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;

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

	//---- set the traveler parameters ----

	exe->Set_Parameters (param, plan_ptr->Type ());

	link_delay_ptr = turn_delay_ptr = 0;
	reroute_flag = false;

	mode = plan_ptr->Mode ();
	param.mode = (Mode_Type) mode;

	if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
		mode != TRANSIT_MODE && mode != OTHER_MODE) {

		//---- trace the path ----

		time = plan_ptr->Depart ();
		dir_index = -1;

		for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
			mode = leg_itr->Mode ();
			ttime = leg_itr->Time ();

			if (mode == DRIVE_MODE && leg_itr->Link_Type ()) {
				link_delay_ptr = link_flow_ptr->Period_Ptr (time);

				if (link_delay_ptr != 0) {
					index = leg_itr->Link_ID ();
					map_itr = exe->link_map.find (index);

					if (map_itr != exe->link_map.end ()) {
						link_ptr = &exe->link_array [map_itr->second];

						len = leg_itr->Length ();
						if (len > link_ptr->Length ()) len = link_ptr->Length ();

						if (leg_itr->Link_Dir () == 1) {
							flow_index = index = link_ptr->BA_Dir ();
							if (leg_itr->Type () == USE_BA && exe->Lane_Use_Flows ()) {
								flow_index = exe->dir_array [index].Flow_Index ();
								if (flow_index < 0) {
									flow_index = index;
								}
							}
						} else {
							flow_index = index = link_ptr->AB_Dir ();
							if (leg_itr->Type () == USE_AB && exe->Lane_Use_Flows ()) {
								flow_index = exe->dir_array [index].Flow_Index ();
								if (flow_index < 0) {
									flow_index = index;
								}
							}
						}
						if (param.flow_flag) {
							data_ptr = link_delay_ptr->Data_Ptr (flow_index);

							factor = (double) len / link_ptr->Length ();
							data_ptr->Add_Flow (factor);
						}
						if (dir_index >= 0 && param.turn_flow_flag) {
							map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));

							if (map2_itr != exe->connect_map.end ()) {
								turn_delay_ptr = turn_flow_ptr->Period_Ptr (time);
								data_ptr = turn_delay_ptr->Data_Ptr (map2_itr->second);
								data_ptr->Add_Flow (1.0);
							}
						}
						dir_index = index;
					}
				}
			}
			time += ttime;
		}
	}
	return (true);
}
