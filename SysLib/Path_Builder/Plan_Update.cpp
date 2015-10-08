//*********************************************************
//	Plan_Update - Update Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Update
//---------------------------------------------------------

bool Path_Builder::Plan_Update (Plan_Data *plan_data)
{
	int i, mode, index, dir_index, nlegs, len, cost, imp, veh_id, veh_type, flow_index, group;
	Dtime time, ttime, delay;
	bool first_park, ab_flag;
	double factor;

	Plan_Leg *leg_ptr;
	Flow_Time_Array *link_delay_ptr, *turn_delay_ptr;
	Flow_Time_Data *data_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Data *connect_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Use_Type use;
	Vehicle_Data *veh_ptr = 0;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;

	exe->Set_Parameters (param, plan_ptr->Type ());

	random_flag = (param.random_imped > 0);
	turn_flag = (param.left_imped > 0 || param.right_imped > 0 || param.uturn_imped > 0);
	link_delay_ptr = turn_delay_ptr = 0;

	parking_duration = plan_ptr->Duration ();
	forward_flag = (plan_ptr->Constraint () != END_TIME);
	reroute_flag = false;

	mode = plan_ptr->Mode ();
	param.mode = (Mode_Type) mode;

	if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
		mode != TRANSIT_MODE && mode != OTHER_MODE) {

		//---- get the vehicle record ----

		veh_id = plan_ptr->Vehicle ();

		if (veh_id <= 0) {
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
				veh_type = veh_ptr->Type ();
			} else {
				veh_type = veh_id;
			}
			if (veh_type_flag) {
				veh_type_ptr = &exe->veh_type_array [veh_type];
				use = (Use_Type) veh_type_ptr->Use ();
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
		group = 0;

		//---- trace the path ----

		if (forward_flag) {
			time = plan_ptr->Start ();
			plan_ptr->Depart (time);
		} else {
			time = plan_ptr->End ();
			plan_ptr->Arrive (time);
		}
		nlegs = (int) plan_ptr->size ();

		plan_ptr->Zero_Totals ();
		dir_index = -1;
		first_park = true;

		for (i=0; i < nlegs; i++) {
			if (forward_flag) {
				leg_ptr = &plan_ptr->at (i);
			} else {
				leg_ptr = &plan_ptr->at (nlegs - i - 1);
			}
			mode = leg_ptr->Mode ();
			ttime = leg_ptr->Time ();
			len = leg_ptr->Length ();
			cost = leg_ptr->Cost ();
			imp = leg_ptr->Impedance ();

			if (mode == DRIVE_MODE) {
				if (leg_ptr->Link_Type ()) {
					link_delay_ptr = exe->link_delay_array.Period_Ptr (time);

					if (link_delay_ptr != 0) {
						index = leg_ptr->Link_ID ();
						map_itr = exe->link_map.find (index);

						if (map_itr != exe->link_map.end ()) {
							link_ptr = &exe->link_array [map_itr->second];
							if (len > link_ptr->Length ()) len = link_ptr->Length ();

							if (leg_ptr->Link_Dir () == 1) {
								index = link_ptr->BA_Dir ();
								ab_flag = false;
							} else {
								index = link_ptr->AB_Dir ();
								ab_flag = true;
							}
							dir_ptr = &exe->dir_array [index];

							if (!Best_Lane_Use (index, time, ttime, delay, cost, group)) {
								//---- problem ----
							}
							cost += DTOI (op_cost_rate * len);

							factor = (double) len / link_ptr->Length ();
							ttime = DTOI (ttime * factor);

							if (param.flow_flag) {
								if (group == 1) {
									flow_index = dir_ptr->Flow_Index (); 
								} else {
									flow_index = index;
								}
								data_ptr = link_delay_ptr->Data_Ptr (flow_index);
								data_ptr->Add_Flow (factor);
							}
							imp = 0;

							if (grade_flag && link_ptr->Grade (ab_flag) > 0) {
								ttime = ttime / veh_type_ptr->Grade (link_ptr->Grade (ab_flag));
							}
							ttime += delay;

							if (dir_index >= 0) {
								if (forward_flag) {
									map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));
								} else {
									map2_itr = exe->connect_map.find (Int2_Key (index, dir_index));
								}
								if (map2_itr != exe->connect_map.end ()) {
									if (param.turn_delay_flag) {
										turn_delay_ptr = exe->turn_delay_array.Period_Ptr (time);
										ttime += turn_delay_ptr->Time (map2_itr->second);
										if (ttime < 1) ttime = 1;
									}
									if (param.turn_flow_flag) {
										turn_delay_ptr = turn_flow_ptr->Period_Ptr (time);
										data_ptr = turn_delay_ptr->Data_Ptr (map2_itr->second);
										data_ptr->Add_Flow (1.0);
									}
									if (turn_flag) {
										connect_ptr = &exe->connect_array [map2_itr->second];

										if (connect_ptr->Type () == LEFT) {
											imp += param.left_imped;
										} else if (connect_ptr->Type () == RIGHT) {
											imp += param.right_imped;
										} else if (connect_ptr->Type () == UTURN) {
											imp += param.uturn_imped;
										}
									}
								}
							}
							imp += Resolve (ttime * param.value_time + len * param.value_dist + cost * param.value_cost);

							if (link_ptr->Type () == FREEWAY) {
								imp = DTOI (imp * param.freeway_fac);
							} else if (link_ptr->Type () == EXPRESSWAY) {
								imp = DTOI (imp * param.express_fac);
							}
							dir_index = index;

							if (random_flag) {
								imp = DTOI (imp * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
							}
						}
					}
				}
				plan_ptr->Add_Drive (ttime);
			} else if (mode == TRANSIT_MODE) {
				plan_ptr->Add_Transit (ttime);
			} else if (mode == WALK_MODE) {
				if (leg_ptr->Type () == ACCESS_ID) {
					index = leg_ptr->ID ();
					map_itr = exe->access_map.find (index);

					if (map_itr != exe->access_map.end ()) {
						Access_Data *access_ptr;

						access_ptr = &exe->access_array [map_itr->second];

						ttime = access_ptr->Time ();
						cost = Round (access_ptr->Cost ());
						imp = Resolve (ttime * param.value_walk + cost * param.value_cost);
					}
				} else {
					imp = Resolve (ttime * param.value_walk);
				}
				if (random_flag) {
					imp = DTOI (imp * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
				}
				plan_ptr->Add_Walk (ttime);
			} else if (mode == WAIT_MODE) {
				plan_ptr->Add_Wait (ttime);
			} else {
				if (leg_ptr->Type () == PARKING_ID) {
					index = leg_ptr->ID ();

					map_itr = exe->parking_map.find (index);

					if (map_itr != exe->parking_map.end ()) {
						Parking_Data *parking_ptr;
						Park_Nest_Itr park_itr;
						ttime = 0;
						cost = 0;

						if (!first_park && veh_ptr != 0) {
							veh_ptr->Parking (map_itr->second);
						}
						parking_ptr = &exe->parking_array [map_itr->second];

						if (parking_ptr->size () > 0) {
							for (park_itr = parking_ptr->begin (); park_itr != parking_ptr->end (); park_itr++) {
								if (exe->Use_Permission (park_itr->Use (), use) &&
									park_itr->Start () <= time && time < park_itr->End ()) {

									if (forward_flag && first_park) {
										ttime = park_itr->Time_Out ();
									} else {
										ttime = park_itr->Time_In ();
										cost = DTOI (park_itr->Hourly () * parking_duration.Hours ());
										if (cost > park_itr->Daily ()) cost = park_itr->Daily ();
										cost = Round (cost);
									}
									break;
								}
							}
						}
						imp = Resolve (ttime * param.value_park + cost * param.value_cost);

						if (random_flag) {
							imp = DTOI (imp * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
						}
					}
					first_park = !first_park;
				}
				plan_ptr->Add_Other (ttime);
			}
			leg_ptr->Time (ttime);
			leg_ptr->Length (len);
			leg_ptr->Cost (cost);
			leg_ptr->Impedance (imp);

			plan_ptr->Add_Length (len);
			plan_ptr->Add_Cost (cost);
			plan_ptr->Add_Impedance (imp);

			if (forward_flag) {
				time += ttime;
			} else {
				time -= ttime;
			}
		}
		plan_ptr->Activity (plan_ptr->Duration ());

		if (forward_flag) {
			plan_ptr->Arrive (time.Round_Seconds ());

			if (plan_ptr->Arrive () > plan_ptr->End () && plan_ptr->Duration () > 0 && 
				plan_ptr->Constraint () != FIXED_TIME && plan_ptr->Constraint () != DURATION) {

				delay = DTOI (plan_ptr->Duration () * param.duration_factor [plan_ptr->Priority ()]);
				time = plan_ptr->End () + plan_ptr->Duration ();
				if (time > plan_ptr->Arrive () + delay.Round_Seconds ()) {
					plan_ptr->Activity (time - plan_ptr->Arrive ());
				} else {
					plan_ptr->Activity (delay.Round_Seconds ());
				}
			}
		} else {
			plan_ptr->Depart (time.Round_Seconds ());
		}
	}
	plan_ptr->Problem (0);

	return (true);
}
