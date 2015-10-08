//*********************************************************
//	Network_Prep.cpp - prepare simulation network
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Network_Prep
//---------------------------------------------------------

void Sim_Method::Network_Prep (void)
{
	int i, dir_index, index, dir, record, next, lanes, use_code, min_lane, max_lane, num;
	int j, k, n, cells, cell_out, bnode, *list, bear1, bear2, change, capacity, first [20];
	double factor;
	bool flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *app_ptr;
	Node_Itr node_itr;
	Connect_Data *connect_ptr, *cross_ptr;
	Sim_Dir_Itr sim_dir_itr;
	Sim_Lane_Ptr sim_lane_ptr;
	Sim_Cap_Ptr sim_cap_ptr;
	Sim_Connection *sim_con_ptr;
	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;
	Pocket_Data *pocket_ptr;
	Lane_Map_Array lane_map;
	Lane_Map_Itr lane_itr;
	Turn_Pen_Data *turn_ptr;

	Integers node_link;
	Integers link_list;

	//---- initialize link dir data ----

	if (method == MESOSCOPIC) {
		node_link.assign (exe->node_array.size (), -1);
		link_list.assign (exe->dir_array.size (), -1);
	}

	use_code = Use_Code ("CAR/TRUCK/BUS/RAIL");

	for (dir_index=0, sim_dir_itr = exe->sim_dir_array.begin (); sim_dir_itr != exe->sim_dir_array.end (); sim_dir_itr++, dir_index++) {
		if (sim_dir_itr->To_Part () != partition && (sim_dir_itr->From_Part () != partition || !sim_dir_itr->Transfer ())) continue;

		dir_ptr = &exe->dir_array [dir_index];

		min_lane = dir_ptr->Left ();
		max_lane = dir_ptr->Lanes () + min_lane - 1;
	
		//---- link data by subarea method ----

		sim_dir_itr->Make_Data ();

		if (sim_dir_itr->Method () == MACROSCOPIC) {
			sim_cap_ptr = sim_dir_itr->Sim_Cap ();

			//---- calculate link pce capacity ----

			lanes = sim_dir_itr->Lanes ();
			cells = sim_dir_itr->Cells ();

			capacity = lanes * cells;

			cells--;
			cell_out = cells - sim_dir_itr->Out_Cell ();

			//----- add pocket lane cells ----

			for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &exe->pocket_array [index];

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == RIGHT_TURN) {
					k = cells;
					j = MIN ((pocket_ptr->Offset () / param.cell_size), cell_out);
					factor = param.turn_factor;
				} else if (pocket_ptr->Type () == LEFT_MERGE || pocket_ptr->Type () == RIGHT_MERGE) {
					j = 0;
					k = MIN (((pocket_ptr->Length () + param.cell_size - 1) / param.cell_size), cells);
					factor = param.merge_factor;
				} else {
					j = pocket_ptr->Offset () / param.cell_size;
					k = MIN (((pocket_ptr->Offset () + pocket_ptr->Length () + param.cell_size - 1) / param.cell_size), cells);
					factor = param.other_factor;
				}
				if (j < 0) j = 0;

				capacity += DTOI (pocket_ptr->Lanes () * (k - j + 1) * factor);
			}
			sim_cap_ptr->Max_Capacity (capacity);

			//---- set the link use at the start of the simulation ----

			sim_dir_itr->Reset_Use (min_lane, max_lane);

			index = dir_ptr->First_Lane_Use ();

			if (index >= 0) {
				for (period_ptr = &exe->use_period_array [index]; ; period_ptr = &exe->use_period_array [++index]) {
					if (period_ptr->Start () > exe->step) {
						if (period_ptr->Start () < use_update_time) {
							use_update_time = period_ptr->Start ();
						}
						continue;
					} else if (exe->step < period_ptr->End ()) {
						if (period_ptr->End () < use_update_time) {
							use_update_time = period_ptr->End ();
						}
						break;
					}
					if (period_ptr->Periods () == 0) goto exit_use;
				}
				num = period_ptr->Records ();
				index = period_ptr->Index ();

				for (i=0; i < num; i++, index++) {
					use_index = &exe->use_period_index [index];
					use_ptr = &exe->lane_use_array [use_index->Link ()];

					//---- must be a full length restriction ----
	
					if (use_ptr->Offset () > 0 || use_ptr->Length () > 0 || use_ptr->Type () == APPLY) continue;

					//---- apply to all main lanes ----

					if (use_ptr->Low_Lane () <= min_lane && use_ptr->High_Lane () >= max_lane) {

						if (use_ptr->Type () == LIMIT) {
							sim_cap_ptr->High_Use (use_ptr->Use ());
						} else if (use_ptr->Type () == PROHIBIT) {
							sim_cap_ptr->High_Use (ANY_USE_CODE ^ use_ptr->Use ());
						}
						sim_cap_ptr->High_Min_Lane (min_lane);
						sim_cap_ptr->High_Max_Lane (max_lane);

					} else {

						//---- split the capacity into two restriction groups ----

						sim_cap_ptr->High_Min_Lane ((use_ptr->Low_Lane () < min_lane) ? min_lane : use_ptr->Low_Lane ());
						sim_cap_ptr->High_Max_Lane ((use_ptr->High_Lane () > max_lane) ? max_lane : use_ptr->High_Lane ());

						lanes = sim_cap_ptr->High_Max_Lane () - sim_cap_ptr->High_Min_Lane () + 1;
						capacity = lanes * sim_dir_itr->Cells ();

						sim_cap_ptr->Low_Capacity (sim_cap_ptr->Max_Capacity () - capacity);
						sim_cap_ptr->High_Capacity (capacity);

						if (use_ptr->Type () == LIMIT) {
							sim_cap_ptr->High_Use (use_ptr->Use ());
						} else if (use_ptr->Type () == PROHIBIT) {
							sim_cap_ptr->High_Use (ANY_USE_CODE ^ use_ptr->Use ());
						}
						if (sim_cap_ptr->High_Min_Lane () == min_lane) {
							sim_cap_ptr->Low_Min_Lane (sim_cap_ptr->High_Max_Lane () + 1);
							sim_cap_ptr->Low_Max_Lane (max_lane);
						} else {
							sim_cap_ptr->Low_Min_Lane (min_lane);
							sim_cap_ptr->Low_Max_Lane (sim_cap_ptr->High_Min_Lane () - 1);
						}
					}
				}
			}

		} else if (sim_dir_itr->Method () == MESOSCOPIC) {
			
			link_ptr = &exe->link_array [dir_ptr->Link ()];

			bnode = (sim_dir_itr->Dir () == 0) ? link_ptr->Anode () : link_ptr->Bnode ();

			list = &node_link [bnode];
			link_list [dir_index] = *list;
			*list = dir_index;

			//---- initialize the pocket lanes and access restrictions ----

			cells = sim_dir_itr->Cells ();
			lanes = sim_dir_itr->Lanes ();

			for (i=0; i < lanes; i++) {
				sim_lane_ptr = sim_dir_itr->Lane (i);

				if (i < min_lane || i > max_lane) {
					sim_lane_ptr->assign (cells, -2);
				} else {
					sim_lane_ptr->assign (cells, -1);
				}
				sim_lane_ptr->Type (LIMIT);
				sim_lane_ptr->Use (link_ptr->Use ());
				sim_lane_ptr->Min_Veh_Type (-1);
				sim_lane_ptr->Max_Veh_Type (0);
				sim_lane_ptr->Min_Traveler (0);
				sim_lane_ptr->Max_Traveler (0);
				sim_lane_ptr->First_Use (-1);

				sim_lane_ptr->Thru_Lane (0);
				sim_lane_ptr->Thru_Link (-1);
			}

			//----- add pocket lane cells ----

			cells--;
			cell_out = cells - sim_dir_itr->Out_Cell ();

			for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &exe->pocket_array [index];

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == RIGHT_TURN) {
					k = cells;
					j = MIN ((pocket_ptr->Offset () / param.cell_size), cell_out);
				} else if (pocket_ptr->Type () == LEFT_MERGE || pocket_ptr->Type () == RIGHT_MERGE) {
					j = 0;
					k = MIN (((pocket_ptr->Length () + param.cell_size - 1) / param.cell_size), cells);
				} else {
					j = pocket_ptr->Offset () / param.cell_size;
					k = MIN (((pocket_ptr->Offset () + pocket_ptr->Length () + param.cell_size - 1) / param.cell_size), cells);
				}
				if (j < 0) j = 0;

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
					lanes = dir_ptr->Left () - pocket_ptr->Lanes ();
				} else {
					lanes = dir_ptr->Left () + dir_ptr->Lanes ();
				}

				for (n=0; n < pocket_ptr->Lanes (); n++, lanes++) {
					sim_lane_ptr = sim_dir_itr->Lane (lanes);

					for (i=j; i <= k; i++) {
						(*sim_lane_ptr) [i] = -1;
					}
				}
			}

			//---- initialize the connection array ----

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &exe->connect_array [index];
				sim_con_ptr = &exe->sim_connection [index];

				exe->Lane_Map (connect_ptr, *sim_con_ptr);
			}

			//---- set the thru link and lane for each entry lane ----

			for (index = dir_ptr->First_Connect_From (); index >= 0; index = connect_ptr->Next_From ()) {
				connect_ptr = &exe->connect_array [index];
				
				app_ptr = &exe->dir_array [connect_ptr->Dir_Index ()];
				if ((exe->link_array [app_ptr->Link ()].Use () & use_code) == 0) continue;

				exe->Lane_Map (connect_ptr, lane_map);

				for (lane_itr = lane_map.begin (); lane_itr != lane_map.end (); lane_itr++) {
					if (lane_itr->In_Thru () && lane_itr->Out_Thru ()) {
						sim_lane_ptr = sim_dir_itr->Lane (lane_itr->Out_Lane ());
						if (sim_lane_ptr->Thru_Link () < 0) {
							sim_lane_ptr->Thru_Lane (lane_itr->In_Lane ());
							sim_lane_ptr->Thru_Link (connect_ptr->Dir_Index ());
						}
					}
				}
			}

			//---- set the lane use at the start of the simulation ----

			memset (first, -1, sizeof (first));

			index = dir_ptr->First_Lane_Use ();

			if (index >= 0) {
				for (period_ptr = &exe->use_period_array [index]; ; period_ptr = &exe->use_period_array [++index]) {
					if (period_ptr->Start () > exe->step) {
						if (period_ptr->Start () < use_update_time) {
							use_update_time = period_ptr->Start ();
						}
						continue;
					} else if (exe->step < period_ptr->End ()) {
						if (period_ptr->End () < use_update_time) {
							use_update_time = period_ptr->End ();
						}
						break;
					}
					if (period_ptr->Periods () == 0) goto exit_use;
				}
				num = period_ptr->Records ();
				index = period_ptr->Index ();

				for (i=0; i < num; i++, index++) {
					use_index = &exe->use_period_index [index];
					use_ptr = &exe->lane_use_array [use_index->Link ()];

					if (use_ptr->Type () == REQUIRE) {
						lanes = sim_dir_itr->Lanes ();

						for (i=0; i < lanes; i++) {
							if (i < use_ptr->Low_Lane () || i > use_ptr->High_Lane ()) {
								sim_lane_ptr = sim_dir_itr->Lane (i);
								if (sim_lane_ptr->First_Use () >= 0) continue;

								if (first [i] >= 0) {
									sim_lane_ptr->First_Use (first [i]);
									sim_lane_ptr->Type (LIMIT);
									sim_lane_ptr->Use (link_ptr->Use ());
									sim_lane_ptr->Min_Veh_Type (-1);
									sim_lane_ptr->Max_Veh_Type (0);
									sim_lane_ptr->Min_Traveler (0);
									sim_lane_ptr->Max_Traveler (0);
								} else {
									first [i] = index;
									sim_lane_ptr->Use (sim_lane_ptr->Use () ^ use_ptr->Use ());
									sim_lane_ptr->Type (LIMIT);
									if (use_ptr->Min_Veh_Type () >= 0) {
										sim_lane_ptr->Min_Veh_Type (use_ptr->Min_Veh_Type ());
										sim_lane_ptr->Max_Veh_Type (use_ptr->Max_Veh_Type ());
									}
									if (use_ptr->Min_Traveler () > 0) {
										sim_lane_ptr->Min_Traveler (use_ptr->Min_Traveler ());
										sim_lane_ptr->Max_Traveler (use_ptr->Max_Traveler ());
									}
								}
							}
						}
					} else {
						for (i=use_ptr->Low_Lane (); i <= use_ptr->High_Lane (); i++) {
							sim_lane_ptr = sim_dir_itr->Lane (i);
							if (sim_lane_ptr->First_Use () >= 0) continue;

							if (first [i] >= 0) {
								sim_lane_ptr->First_Use (first [i]);
								sim_lane_ptr->Type (LIMIT);
								sim_lane_ptr->Use (link_ptr->Use ());
								sim_lane_ptr->Min_Veh_Type (-1);
								sim_lane_ptr->Max_Veh_Type (0);
								sim_lane_ptr->Min_Traveler (0);
								sim_lane_ptr->Max_Traveler (0);
							} else {
								first [i] = index;

								if (use_ptr->Type () == PROHIBIT || use_ptr->Type () == LIMIT) {
									if (use_ptr->Type () == PROHIBIT) {
										sim_lane_ptr->Use (sim_lane_ptr->Use () ^ use_ptr->Use ());
									} else {
										sim_lane_ptr->Use (use_ptr->Use ());
									}
									sim_lane_ptr->Type (use_ptr->Type ());
									if (use_ptr->Min_Veh_Type () >= 0) {
										sim_lane_ptr->Min_Veh_Type (use_ptr->Min_Veh_Type ());
										sim_lane_ptr->Max_Veh_Type (use_ptr->Max_Veh_Type ());
									}
									if (use_ptr->Min_Traveler () > 0) {
										sim_lane_ptr->Min_Traveler (use_ptr->Min_Traveler ());
										sim_lane_ptr->Max_Traveler (use_ptr->Max_Traveler ());
									}
								}
							}
						}
					}
				}
			}
		}
exit_use:
		//---- set the turn prohibition flag ----

		sim_dir_itr->Turn (false);

		for (index = dir_ptr->First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
			turn_ptr = &exe->turn_pen_array [index];

			if (turn_ptr->Penalty () != 0) continue;

			if (turn_ptr->Start () > exe->step) {
				if (turn_ptr->Start () < turn_update_time) {
					turn_update_time = turn_ptr->Start ();
				}
			} else if (exe->step < turn_ptr->End ()) {
				if (turn_ptr->End () < turn_update_time) {
					turn_update_time = turn_ptr->End ();
				}
				sim_dir_itr->Turn (true);
				break;
			}
		}
	}

	//---- initialize the traffic controls ----

	Traffic_Controls (true);

	if (method == MACROSCOPIC) return;

	//---- identify conflict links ----

	for (bnode = 0, node_itr = exe->node_array.begin (); node_itr != exe->node_array.end (); node_itr++, bnode++) {
		if (node_itr->Control () == -1 || node_itr->Partition () != partition) continue;

		flag = (node_itr->Control () >= 0);		//---- signal flag ----

		//---- process each link entering the node ----

		list = &node_link [bnode];

		for (index = *list; index >= 0; index = link_list [index]) {
			dir_ptr = &exe->dir_array [index];

			bear1 = dir_ptr->Out_Bearing ();
			if (flag) bear1 = exe->compass.Flip (bear1);

			//---- find connections with potential conflicts ----

			for (next = dir_ptr->First_Connect (); next >= 0; next = connect_ptr->Next_Index ()) {
				connect_ptr = &exe->connect_array [next];

				if (flag) {
					if (connect_ptr->Type () != LEFT) continue;
				} else {
					if (connect_ptr->Control () != STOP_GREEN) continue;
					if (connect_ptr->Type () == RIGHT || connect_ptr->Type () == UTURN) continue;
				}
				sim_con_ptr = &exe->sim_connection [next];
				n = sim_con_ptr->Max_Conflicts ();

				//---- search for conflicting approach links ----

				for (dir = *list; dir >= 0; dir = link_list [dir]) {
					if (dir == index) continue;

					app_ptr = &exe->dir_array [dir];

					bear2 = app_ptr->Out_Bearing ();
					change = exe->compass.Change (bear1, bear2);

					//---- check the angle ----

					if (change >= -45 && change <= 45) {
						if (!flag && connect_ptr->Type () != LEFT) continue;
						i = 0;
					} else if (change > -135 && change < -45) {
						if (flag || connect_ptr->Type () == LEFT) continue;
						i = 0;
					} else if (change > 45 && change < 135) {
						if (flag) continue;
						i = 1;
					} else {
						continue;
					}
					
					//---- find the thru movement ----
					
					for (record = app_ptr->First_Connect (); record >= 0; record = cross_ptr->Next_Index ()) {
						cross_ptr = &exe->connect_array [record];

						if (cross_ptr->Type () != THRU) continue;	// r_split l_split, r_merge, l_merge??

						if (sim_con_ptr->Conflict (i) < 0) {
							sim_con_ptr->Conflict (i, record);
						} else if (!flag && 
							((change >= -100 && change <= -80) || (change >= 80 && change <= 100))) {
							sim_con_ptr->Conflict (i, record);
						}
					}
				}
			}
		}
	}
}
