//*********************************************************
//	Check_Ahead.cpp - Check Speed Constraints on Path Ahead
//*********************************************************

#include "Sim_Method.hpp"

#define NUM_OUT_LANES	8

//---------------------------------------------------------
//	Check_Ahead
//---------------------------------------------------------

bool Sim_Method::Check_Ahead (Travel_Step &step)
{
	int i, c, l, cells, lanes, speed, veh_speed, first_cell, last_cell, end_cell, control;
	int dir_index, cell, lane, from_lane, vehicle, min_speed, max_speed, max_cells, pce;
	int thru_dir, from_dir, index, low, high, lane_change;
	int weight, sum, lane_wt [NUM_OUT_LANES], out_lane [NUM_OUT_LANES];
	bool park_flag, stop_flag, change_flag, last_flag, flag;
	double gap, reaction, slow_down;

	Cell_Data cell_rec;
	Sim_Dir_Ptr sim_dir_ptr, next_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg;
	Connect_Data *connect_ptr, *conflict_ptr;
	Sim_Connection *sim_con_ptr = 0;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;
	Sim_Veh_Ptr sim_veh_ptr;
	Lane_Map_Itr map_itr;
	Dir_Data *dir_ptr;
	Turn_Pen_Data *turn_ptr;

	//---- get the current location ----

	cell_rec = step.sim_veh_ptr->front ();
	from_lane = cell_rec.Lane ();

	if (step.size () > 0) {
		cell_rec = step.back ();
		last_flag = true;
	} else {
		last_flag = false;
	}
	dir_index = cell_rec.Index ();
	lane = cell_rec.Lane ();
	cell = cell_rec.Cell ();

	if (dir_index == step.Dir_Index ()) {
		if (step.sim_dir_ptr == 0) {
			step.sim_dir_ptr = &exe->sim_dir_array [dir_index];
		}
		sim_dir_ptr = step.sim_dir_ptr;
	} else {
		sim_dir_ptr = &exe->sim_dir_array [dir_index];
	}
#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (1, "\tCheck=") << exe->link_array [exe->dir_array [dir_index].Link ()].Link () << "-" << lane << "-" << cell << " last=" << last_flag << " partition=" << sim_dir_ptr->Partition (cell); END_LOCK
#endif
	first_cell = sim_dir_ptr->In_Cell ();
	last_cell = sim_dir_ptr->Cell_Out ();

	speed = step.sim_veh_ptr->Speed ();

	if (speed > 0) {
		min_speed = speed - step.veh_type_ptr->Max_Decel ();
	} else {
		min_speed = 0;
	}
	max_speed = speed + step.veh_type_ptr->Max_Accel ();

	if (max_speed > step.veh_type_ptr->Max_Speed ()) {
		max_speed = step.veh_type_ptr->Max_Speed ();
	}
	if (max_speed > sim_dir_ptr->Speed ()) {
		max_speed = sim_dir_ptr->Speed ();
	}
	if (!boundary_flag && sim_dir_ptr->Boundary () && sim_dir_ptr->To_Part () != partition) {
		end_cell = sim_dir_ptr->Split_Cell ();

		if (cell < end_cell && cell + max_speed >= end_cell) {
			max_speed = end_cell - cell - 1;
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " max_speed=") << max_speed << " cell=" << cell << " split=" << end_cell; END_LOCK
#endif
			if (max_speed <= 0) goto exit;
		}
	}
	connect_ptr = 0;

	//---- get the plan data ----

	plan_ptr = step.sim_traveler_ptr->plan_ptr;
	if (plan_ptr == 0) goto exit;

	leg_itr = plan_ptr->begin ();
	if (leg_itr == plan_ptr->end ()) goto exit;

	end_cell = sim_dir_ptr->Cells () - 1;
	park_flag = stop_flag = false;

#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (0, " end=") << end_cell; END_LOCK
#endif

	if (leg_itr->Connect () < 0) {
		next_leg = leg_itr + 1;
		if (next_leg == plan_ptr->end ()) goto exit;

		if (next_leg->Type () == PARKING_ID) {
			park_flag = true;
			sim_park_ptr = &exe->sim_park_array [next_leg->Index ()];

			if (sim_dir_ptr->Dir () == 0) {
				end_cell = sim_park_ptr->Cell_AB ();
			} else {
				end_cell = sim_park_ptr->Cell_BA ();
			}
			if (cell == end_cell) {
				if (param.parking_lanes && step.sim_veh_ptr->Priority () < 2 && 
					(lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ())) {

					if (lane < leg_itr->Out_Lane_Low ()) {
						lane_change = 1;
					} else {
						lane_change = -1;
					}
					step.sim_veh_ptr->Change (lane_change);

					if (Lane_Change (step)) {
						lane += lane_change;
						if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) goto exit;
					} else {
						if (step.size () == 0) {
							cell_rec.Lane_Change (lane_change);
							Reserve_Cell (cell_rec, step);
						}
						goto exit;
					}
				}
				cell_rec.Lane (-1);
				step.push_back (cell_rec);
				step.Parking (next_leg->Index ());
				step.Veh_Speed (next_leg->Max_Speed ());
				goto exit;
			}
		} else if (next_leg->Type () == STOP_ID) {
			stop_flag = true;
			sim_stop_ptr = &exe->sim_stop_array [next_leg->Index ()];
			end_cell = sim_stop_ptr->Cell ();

			if (cell == end_cell) {
				if (step.sim_veh_ptr->Priority () < 2 && 
					(lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ())) {

					if (lane < leg_itr->Out_Lane_Low ()) {
						lane_change = 1;
					} else {
						lane_change = -1;
					}
					step.sim_veh_ptr->Change (lane_change);

					if (Lane_Change (step)) {
						lane += lane_change;
						if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) goto exit;
					} else {
						if (step.size () == 0) {
							cell_rec.Lane_Change (lane_change);
							Reserve_Cell (cell_rec, step);
						}
						goto exit;
					}
				}
				step.Stop (next_leg->Index ());
				step.Veh_Speed (0);
				goto exit;
			}
		} else {
			goto exit;
		}
	}

	//---- set plan following / lane change flags ----

	lanes = 0;
	max_cells = end_cell - cell;
	change_flag = (step.sim_veh_ptr->Change () != 0);

	if (max_cells < param.plan_follow && (lane < leg_itr->Out_Best_Low () || lane > leg_itr->Out_Best_High ())) {
		flag = true;
		lane_change = 0;

		if (lane < leg_itr->Out_Best_Low ()) {
			lane_change = 1;
			lanes = leg_itr->Out_Best_Low () - lane;
			if (lanes > 1 && leg_itr->Out_Lane_Low () < leg_itr->Out_Best_Low ()) lanes--;
		} else {
			lane_change = -1;
			lanes = lane - leg_itr->Out_Best_High ();
			if (lanes > 1 && leg_itr->Out_Lane_High () > leg_itr->Out_Best_High ()) lanes--;
		}
		lanes = (param.lane_change_levels * max_cells / lanes + param.plan_follow / 2) / param.plan_follow; 

		if (lanes > param.change_priority) {
			flag = Check_Cell (Cell_Data (dir_index, lane + lane_change, cell), step);
		} else {
			if (max_speed > 1) {
				if (lanes > 0) {
					if (Check_Cell (Cell_Data (dir_index, lane + lane_change, cell), step)) {
						max_cells = sim_dir_ptr->Speed () * lanes / param.lane_change_levels + 1;
						if (max_speed > max_cells) {
#ifdef DEBUG_KEYS
							if (exe->debug) MAIN_LOCK exe->Write (0, " slowchange"); END_LOCK
#endif
							max_speed--;
						}
					}
				} else {
					max_speed--;
#ifdef DEBUG_KEYS
					if (exe->debug) MAIN_LOCK exe->Write (0, " slowchange"); END_LOCK
#endif
				}
			}
			lanes = param.change_priority - lanes + 1;
			if (lanes > step.sim_veh_ptr->Priority ()) step.sim_veh_ptr->Priority (lanes);
		}
		if (flag && !change_flag) {
			step.sim_veh_ptr->Change (lane_change);
		}
	}

	//---- apply random slow down ----

	if (max_speed > 0 && step.sim_veh_ptr->Priority () < 1 && param.slow_down_flag) {
		slow_down = param.slow_down_prob [sim_dir_ptr->Type ()];
		if (param.traveler_flag) slow_down *= param.traveler_fac.Best (step.sim_traveler_ptr->Type ());

		if (slow_down > step.sim_traveler_ptr->random.Probability ()) {
			double percent = param.slow_down_percent [sim_dir_ptr->Type ()];
			if (param.traveler_flag) percent *= param.traveler_fac.Best (step.sim_traveler_ptr->Type ());

			if (percent > 0.0) {
				double spd = max_speed * percent;
				int slow_down = (int) spd;
				spd -= slow_down;

				if (spd > 0.0 && spd > step.sim_traveler_ptr->random.Probability ()) slow_down++;

				max_speed -= slow_down;
				if (slow_down > 0) {
					stats.num_slow_down++;
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " slowdown=") << slow_down; END_LOCK
#endif
				}
			} else {
				max_speed--;
				stats.num_slow_down++;
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " slowdown=") << slow_down; END_LOCK
#endif
			}
			if (max_speed < 0) max_speed = 0;
		}
	}
	if (param.comfort_flag && max_speed > 1) {
		double comfort = param.comfort_speed.Best (step.sim_traveler_ptr->Type ());
		if (comfort > 0.0) {
			double spd = max_speed * UnRound (param.cell_size);

			if (spd > comfort) {
				spd -= comfort;
				comfort *= 0.25;
				if (spd < comfort) {
					comfort = (comfort - spd) / comfort;
					if (step.sim_traveler_ptr->random.Probability () > comfort) {
						max_speed--;
						stats.num_slow_down++;
					}
				} else {
					max_speed--;
					stats.num_slow_down++;
				}
			}
		}
	}
#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (0, " maxspd=") << max_speed << " change=" << step.sim_veh_ptr->Change (); END_LOCK
#endif

	if (max_speed == 0 && !change_flag) goto exit;

	//---- attempt to move the vehicle ----

#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (0, " lane_ptr=") << lane; END_LOCK
#endif
	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	if (leg_itr->Connect () >= 0) {
		connect_ptr = &exe->connect_array [leg_itr->Connect ()];
		control = connect_ptr->Control ();
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " control=") << control; END_LOCK
#endif
	} else {
		connect_ptr = 0;
		control = UNCONTROLLED;
	}

	//---- try to move ----

	for (speed=0; speed < max_speed; ) {

		//---- attempt a lane change ----

		if (change_flag) {
			lane_change = step.sim_veh_ptr->Change ();

			if (Lane_Change (step)) {
				from_lane = lane;
				lane += lane_change;
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " lane_ptr1=") << lane; END_LOCK
#endif
				sim_lane_ptr = sim_dir_ptr->Lane (lane);
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " newlane=") << lane; END_LOCK
#endif
				change_flag = false;
				last_flag = true;
			}
		}

		//---- check the intersection approach lane ----

		if (cell >= last_cell) {

			//---- check the exit lanes ----

			if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) {
				change_flag = true;
				if (lane < leg_itr->Out_Lane_Low ()) {
					step.sim_veh_ptr->Change (1);
				} else {
					step.sim_veh_ptr->Change (-1);
				}
				if (speed > 1 && !last_flag) goto slow_down;
				step.Problem (LANE_PROBLEM);
				break;
			}

			//---- find the connection options ----

			if (connect_ptr > 0) {

				//---- check for turn prohibition ----
				
				if (sim_dir_ptr->Turn ()) {
					dir_ptr = &exe->dir_array [dir_index];

					for (index = dir_ptr->First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
						turn_ptr = &exe->turn_pen_array [index];

						if (turn_ptr->Penalty () != 0 || turn_ptr->To_Index () != connect_ptr->To_Index ()) continue;

						if (turn_ptr->Start () <= exe->step && exe->step < turn_ptr->End ()) {
							int veh_type = step.sim_veh_ptr->Type ();

							if (veh_type < 0 || turn_ptr->Min_Veh_Type () < 0 || 
								(turn_ptr->Min_Veh_Type () <= veh_type && veh_type <= turn_ptr->Max_Veh_Type ())) {
								
								if (Use_Permission (turn_ptr->Use (), step.veh_type_ptr->Use ())) {
									step.Problem (CONNECT_PROBLEM);
									goto exit;
								}
							}
						}
					}
				}

				//---- set the output lanes ----

				lanes = 0;
				sim_con_ptr = &exe->sim_connection [leg_itr->Connect ()];
				
				for (map_itr = sim_con_ptr->begin (); map_itr != sim_con_ptr->end (); map_itr++) {
					if (map_itr->In_Lane () == lane) {
						lane_wt [lanes] = map_itr->In_Thru () + map_itr->Out_Thru ();
						out_lane [lanes++] = map_itr->Out_Lane ();
						if (lanes == NUM_OUT_LANES) break;
					}
				}
				if (lanes == 0) break;
			}

			//---- check the traffic control ----

			if (control != UNCONTROLLED && cell == last_cell) {

				//---- check stop controls ----

				if (control == RED_LIGHT || (step.sim_veh_ptr->Speed () > 0 && (control == STOP_SIGN || control == STOP_GREEN))) {
					if (speed > 1 && !last_flag) goto slow_down;

					if (control == RED_LIGHT) {
						if (speed < min_speed) step.Problem (CONTROL_PROBLEM);
						step.sim_veh_ptr->Next_Event (Next_Signal_Event (dir_index));
					}
					step.Veh_Speed (0);		//---- stop the vehicle ----
					break;

				} else if (step.sim_veh_ptr->Priority () < 2) {		//---- not desperate yet -----

					//---- don't block the box ----

					if (connect_ptr > 0) {
						if (cell < end_cell) {
							vehicle = sim_lane_ptr->at (cell + 1);
							if (vehicle >= 0 && exe->veh_status [vehicle] != 0) break;
						}
						next_dir_ptr = &exe->sim_dir_array [connect_ptr->To_Index ()];
						if (!next_dir_ptr->Active ()) break;

						for (l=0; l < lanes; l++) {
							vehicle = next_dir_ptr->Cell (out_lane [l], 0);
							if (vehicle == -1) break;

							sim_veh_ptr = exe->sim_veh_array [vehicle];
							if (sim_veh_ptr == 0) continue;
							if (sim_veh_ptr->Speed () > 0 && sim_veh_ptr->Next_Event () <= exe->step && sim_veh_ptr->Priority () == 0) break;
						}
						if (l >= lanes) break;
					}

					//---- yellow decision ----
										
					if (control == YELLOW_LIGHT && speed > step.sim_veh_ptr->Speed ()) {
						if (speed > 1 && !last_flag) goto slow_down;

						step.Veh_Speed (0);		//---- stop the vehicle ----
						step.sim_veh_ptr->Next_Event (Next_Signal_Event (dir_index));
						break;
					}

					//---- check conflicts ----

					if (control != PROTECTED_GREEN && connect_ptr > 0) {

						for (i=0; i < sim_con_ptr->Max_Conflicts (); i++) {
							index = sim_con_ptr->Conflict (i);
							if (index < 0) continue;

							conflict_ptr = &exe->connect_array [index];

							if (i == 0) {
								from_dir = conflict_ptr->Dir_Index ();
								low = conflict_ptr->Low_Lane ();
								high = conflict_ptr->High_Lane ();
								c = exe->sim_dir_array [from_dir].Cells () - 1;
							} else {
								from_dir = conflict_ptr->To_Index ();
								low = conflict_ptr->To_Low_Lane ();
								high = conflict_ptr->To_High_Lane ();
								c = 0;
							}
							cells = end_cell - cell + (high - low + 1) / 2;
#ifdef DEBUG_KEYS
							if (exe->debug) MAIN_LOCK exe->Write (0, " yield=") << exe->link_array [exe->dir_array [from_dir].Link ()].Link () << "-" << low << ":" << high << "-" << c; END_LOCK
#endif
							for (l=low; l <= high; l++) {
								if (!Check_Behind (Cell_Data (from_dir, l, c), step, cells)) {
									if (speed > 1 && !last_flag) goto slow_down;
									step.Veh_Speed (0);
#ifdef DEBUG_KEYS
									if (exe->debug) MAIN_LOCK exe->Write (0, " NO=") << exe->sim_dir_array.Cell (from_dir, l, c); END_LOCK
#endif
									goto exit;
								}
							}
						}
					}
				}
			}
		}

		//---- try to move forward ----

		cell++;

#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " next=") << lane << "-" << cell; END_LOCK
#endif
		//---- try to enter the next link ----

		if (cell > end_cell) {
			if (connect_ptr == 0) break;

			from_dir = dir_index;
			dir_index = connect_ptr->To_Index ();

			sim_dir_ptr = &exe->sim_dir_array [dir_index];
			if (!sim_dir_ptr->Active ()) break;

			if (sim_dir_ptr->Method () != MESOSCOPIC)  {
				if (sim_dir_ptr->Method () == MACROSCOPIC) {

					//---- try the restricted lanes first ----

					flag = false;
					pce = (int) step.sim_veh_ptr->size ();

					Sim_Cap_Ptr sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

					if (Use_Permission (sim_cap_ptr->High_Use (), step.veh_type_ptr->Use ())) {
						if (sim_cap_ptr->High_Volume () + pce <= sim_cap_ptr->High_Capacity ()) {
							flag = true;
							sim_cap_ptr->Add_High (pce);
							step.sim_veh_ptr->Restricted (true);
						}
					}

					//---- try the general purpose lanes ----

					if (!flag && sim_cap_ptr->Low_Volume () + pce <= sim_cap_ptr->Low_Capacity ()) {
						flag = true;
						sim_cap_ptr->Add_Low (pce);
						step.sim_veh_ptr->Restricted (false);
					}
					if (!flag) break;

					if (sim_cap_ptr->Last_Veh () >= 0) {
						sim_veh_ptr = exe->sim_veh_array [sim_cap_ptr->Last_Veh ()];
						if (sim_veh_ptr != 0) sim_veh_ptr->Follower (step.Vehicle ());
						step.sim_veh_ptr->Leader (sim_cap_ptr->Last_Veh ());
						sim_cap_ptr->Last_Veh (step.Vehicle ());
					} else {
						sim_cap_ptr->First_Veh (step.Vehicle ());
						sim_cap_ptr->Last_Veh (step.Vehicle ());
						step.sim_veh_ptr->Leader (-1);
					}
					step.sim_veh_ptr->Follower (-1);

					++leg_itr;
					lane = (leg_itr->In_Best_Low () + leg_itr->In_Best_High ()) / 2;
				} else {
					lane = (connect_ptr->To_Low_Lane () + connect_ptr->To_High_Lane ()) / 2;
				}
				step.push_back (Cell_Data (dir_index, lane, 0));
				break;
			}
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " new=") <<  exe->link_array [exe->dir_array [dir_index].Link ()].Link (); END_LOCK
#endif
			first_cell = sim_dir_ptr->In_Cell ();
			last_cell = sim_dir_ptr->Cell_Out ();

			index = leg_itr->Connect ();
			if (index < 0) break;

			if (++leg_itr == plan_ptr->end ()) break;

			//---- find the best connection ----

			if (lanes > 1) {
				flag = false;

				for (l=0; l < lanes; l++) {
#ifdef DEBUG_KEYS
					if (exe->debug) MAIN_LOCK exe->Write (0, " lane_ptr2=") << out_lane [l]; END_LOCK
#endif
					sim_lane_ptr = sim_dir_ptr->Lane (out_lane [l]);
#ifdef DEBUG_KEYS
					if (exe->debug) MAIN_LOCK exe->Write (0, " cell=") << sim_lane_ptr->at (0); END_LOCK
#endif
					if (sim_lane_ptr->at (0) != -1) continue;

					Cell_Data new_cell (dir_index, out_lane [l], 0);

					thru_dir = sim_lane_ptr->Thru_Link ();
					if (thru_dir >= 0 && from_dir != thru_dir && lane != sim_lane_ptr->Thru_Lane ()) {
						if (!Check_Behind (new_cell, step)) continue;
					}
					weight = lane_wt [l];

					if (param.look_ahead > 0) {
						sum = Sum_Path (new_cell, step);
					} else {
						sum = 0;
					}
					if (sum <= 0) {
						if (out_lane [l] >= leg_itr->In_Best_Low () && out_lane [l] <= leg_itr->In_Best_High ()) {
							weight += param.connect_lane_weight;
						}
						if (Cell_Use (sim_lane_ptr, out_lane [l], first_cell, step)) {
							weight += param.lane_use_weight;
						}
					} else {
						weight += sum;
					}
					if (weight <= 0) continue;

					lane_wt [l] = weight;
					flag = true;
				}
				if (!flag) break;

				weight = 0;
				lane = -1;

				for (l=0; l < lanes; l++) {
					weight += lane_wt [l];
				}
				i = DTOI (weight * step.sim_traveler_ptr->random.Probability ());
				weight = 0;

				for (l=0; l < lanes; l++) {
					if (lane_wt [l] > 0) {
						weight += lane_wt [l];
						if (weight >= i) {
							lane = out_lane [l];
							break;
						}
					}
				}
			} else {
				lane = out_lane [0];
			}
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " lane=") << lane; END_LOCK
#endif
			if (lane < 0) break;
			cell = 0;
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " lane_ptr3=") << lane; END_LOCK
#endif
			sim_lane_ptr = sim_dir_ptr->Lane (lane);

			if (sim_lane_ptr->at (0) != -1) {
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " occupied=") << sim_lane_ptr->at (0); END_LOCK
#endif
				break;
			}
			end_cell = sim_dir_ptr->Cells () - 1;
			park_flag = stop_flag = false;

			if (leg_itr->Connect () < 0) {
				control = UNCONTROLLED;
				connect_ptr = 0;

				next_leg = leg_itr + 1;
				if (next_leg == plan_ptr->end ()) goto exit;

				if (next_leg->Type () == PARKING_ID) {
					park_flag = true;
					sim_park_ptr = &exe->sim_park_array [next_leg->Index ()];

					if (sim_dir_ptr->Dir () == 0) {
						end_cell = sim_park_ptr->Cell_AB ();
					} else {
						end_cell = sim_park_ptr->Cell_BA ();
					}
				} else if (next_leg->Type () == STOP_ID) {
					stop_flag = true;
					sim_stop_ptr = &exe->sim_stop_array [next_leg->Index ()];
					end_cell = sim_stop_ptr->Cell ();
				} else {
					goto exit;
				}
			} else {
				connect_ptr = &exe->connect_array [leg_itr->Connect ()];
				control = connect_ptr->Control ();
			}

			//---- set lane change flags ----

			change_flag = false;
			step.sim_veh_ptr->Change (0);

			if (end_cell < param.plan_follow && (lane < leg_itr->Out_Best_Low () || lane > leg_itr->Out_Best_High ())) {
				if (lane < leg_itr->Out_Best_Low ()) {
					lane_change = 1;
					lanes = leg_itr->Out_Best_Low () - lane;
					if (lanes > 1 && leg_itr->Out_Lane_Low () < leg_itr->Out_Best_Low ()) lanes--;
				} else {
					lane_change = -1;
					lanes = lane - leg_itr->Out_Best_High ();
					if (lanes > 1 && leg_itr->Out_Lane_High () > leg_itr->Out_Best_High ()) lanes--;
				}
				if (Check_Cell (Cell_Data (dir_index, lane + lane_change, 0), step)) {
					change_flag = true;
					step.sim_veh_ptr->Change (lane_change);
				}
				lanes = (param.lane_change_levels * end_cell / lanes + param.plan_follow / 2) / param.plan_follow; 
				lanes = param.change_priority - lanes + 1;

				if (lanes > step.sim_veh_ptr->Priority ()) step.sim_veh_ptr->Priority (lanes);
			}

			//---- adjust the max speed ----

			if (max_speed > sim_dir_ptr->Speed ()) {
				max_speed = sim_dir_ptr->Speed ();
			}
		}

		//---- check the cell availability -----

		vehicle = sim_lane_ptr->at (cell);

#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " veh=") << vehicle; END_LOCK
#endif
		if (vehicle == -2) {

			//---- pocket lane ----

			if (cell > 0) {
				cell--;

				//---- attempt a lane change ----

				if (last_flag) {
					lane_change = lane - from_lane;
				} else {
					cell_rec.Location (dir_index, lane + 1, cell);

					if (Check_Behind (cell_rec, step)) {
						step.push_back (cell_rec);
						sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
						from_lane = lane;
						lane++;
						stats.num_change++;
						change_flag = false;
						last_flag = true;
						continue;
					}
					lane_change = -1;
				}
				cell_rec.Lane (lane + lane_change);

				if (Check_Behind (cell_rec, step)) {
					step.push_back (cell_rec);
					sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
					from_lane = lane;
					lane = cell_rec.Lane ();
					stats.num_change++;
					change_flag = false;
					last_flag = true;
					continue;
				}
			}
			change_flag = true;
			if (last_flag) {
				step.sim_veh_ptr->Change (lane - from_lane);
			} else {
				if (lane < 1) {
					step.sim_veh_ptr->Change (1);
				} else {
					step.sim_veh_ptr->Change (-1);
				}
			}
			if (speed > 1 && !last_flag) goto slow_down;
			step.Veh_Speed (0);
			step.Problem (LINK_ACCESS_PROBLEM);
			break;

		} else if (vehicle >= 0) {

			//---- check the vehicle spacing ----

			sim_veh_ptr = exe->sim_veh_array [vehicle];
			if (sim_veh_ptr == 0) break;

			veh_speed = sim_veh_ptr->Speed ();

			if (veh_speed <= speed && speed > 0 && !last_flag) {
				if (param.reaction_flag) {
					max_cells = speed - veh_speed + 1;

					gap = 0.0;
					reaction = param.reaction_time [sim_dir_ptr->Type ()];

					if (param.traveler_flag) {
						reaction *= param.traveler_fac.Best (step.sim_traveler_ptr->Type ());
					}
					for (c=1; c <= max_cells; c++) {
						gap += reaction / c;
					}
					max_cells = (int) gap;
					gap -= max_cells;

					if (gap > step.sim_traveler_ptr->random.Probability ()) max_cells++;
					if (max_cells > (int) step.size ()) max_cells = (int) step.size ();

					if (max_cells > 1) goto slow_down;

					//if (max_cells > 1 && step.Lane_Change () >= 2) max_cells = 1;

					//for (c=0; c < max_cells; c++) {
					//	step.pop_back ();			
					//	step.Veh_Speed (--speed);
					//}
				}
			} else if (veh_speed == 0 && speed == 0 && !change_flag && exe->veh_status [vehicle] >= 0 &&
				step.size () == 0 && step.sim_veh_ptr->Leader () < 0) {
				
				cell_rec.Location (dir_index, lane, cell);
				Reserve_Cell (cell_rec, step);
			}
			if (speed < min_speed) step.Problem (SPACING_PROBLEM);
			break;
		}

		//---- check the end cell ----

		if (cell == end_cell) {

			//---- check the lane position for parking the vehicle ----

			if (park_flag) {
				if (param.parking_lanes && (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ())) {
					change_flag = true;
					if (lane < leg_itr->Out_Lane_Low ()) {
						step.sim_veh_ptr->Change (1);
					} else {
						step.sim_veh_ptr->Change (-1);
					}
					break;
				}
				if (speed < leg_itr->Max_Speed ()) {
					cell_rec.Location (dir_index, lane, cell);
					step.push_back (cell_rec);
					sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
					step.Parking (next_leg->Index ());
					step.Veh_Speed (++speed);

					if (speed <= leg_itr->Max_Speed ()) {
						cell_rec.Location (dir_index, -1, cell);
						step.push_back (cell_rec);
					}
					goto exit;
				} else {
					if (leg_itr->Max_Speed () > 0) {
						step.Veh_Speed (leg_itr->Max_Speed ());
					} else {
						step.Veh_Speed (1);
					}
					goto exit;
				}
			}

			//---- check the lane position for transit stop ----

			if (stop_flag) {
				if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) {
					step.Problem (STOP_PROBLEM);
					change_flag = true;
					if (lane < leg_itr->Out_Lane_Low ()) {
						step.sim_veh_ptr->Change (1);
					} else {
						step.sim_veh_ptr->Change (-1);
					}
					if (leg_itr->Max_Speed () > 0) {
						step.Veh_Speed (leg_itr->Max_Speed ());
					} else {
						step.Veh_Speed (1);
					}
					break;
				}
				if (speed < leg_itr->Max_Speed ()) {
					cell_rec.Location (dir_index, lane, cell);
					step.push_back (cell_rec);
					sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
					step.Stop (next_leg->Index ());
					step.Veh_Speed (0);
					plan_ptr->Next_Leg ();
					goto exit;
				} else {
					if (leg_itr->Max_Speed () > 0) {
						step.Veh_Speed (leg_itr->Max_Speed ());
					} else {
						step.Veh_Speed (1);
					}
					goto exit;
				}
			}
		}

		//---- check the use restrictions ----

		step.Delay (0);

		if (cell >= first_cell && cell < last_cell && !Cell_Use (sim_lane_ptr, lane, cell, step)) {

			//---- check if lane change is possible ----

			if (cell > 0) {
				cell--;

				//---- attempt a lane change ----

				if (last_flag) {
					lane_change = lane - from_lane;
				} else {
					cell_rec.Location (dir_index, lane + 1, cell);

					if (Check_Behind (cell_rec, step)) {
						step.push_back (cell_rec);
						sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
						from_lane = lane;
						lane++;
						stats.num_change++;
						change_flag = false;
						last_flag = true;
						continue;
					}
					lane_change = -1;
				}
				cell_rec.Lane (lane + lane_change);

				if (Check_Behind (cell_rec, step)) {
					step.push_back (cell_rec);
					sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
					from_lane = lane;
					lane = cell_rec.Lane ();
					stats.num_change++;
					change_flag = false;
					last_flag = true;
					continue;
				}
			}
			change_flag = true;
			if (last_flag) {
				step.sim_veh_ptr->Change (lane - from_lane);
			} else {

				//---- try to move toward an unrestricted lane ----

				lanes = sim_dir_ptr->Lanes ();
				for (l=0; l < lanes; l++) {
					if (Cell_Use (sim_dir_ptr->Lane (l), l, cell, step)) break;
				}
				if (l < lanes) {
					if (l < lane) {
						cell_rec.Lane (lane - 1);
					} else {
						cell_rec.Lane (lane + 1);
					}
					if (Check_Behind (cell_rec, step, 1, true)) {
						step.push_back (cell_rec);
						sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
						from_lane = lane;
						lane = cell_rec.Lane ();
						stats.num_change++;
						change_flag = false;
						last_flag = true;
						continue;
					}
				}
				if (lane < 1) {
					step.sim_veh_ptr->Change (1);
				} else {
					step.sim_veh_ptr->Change (-1);
				}
			}
			if (speed > 1 && !last_flag) goto slow_down;
			step.Veh_Speed (0);
			step.Problem (USE_PROBLEM);
			break;
		}

		//---- add the cell to the path ----

		cell_rec.Location (dir_index, lane, cell);
		step.push_back (cell_rec);
		sim_dir_ptr->Cell (cell_rec, step.Vehicle ());

		//---- set the random delay 

		if (step.Delay () > 0) {
			step.sim_veh_ptr->Next_Event (exe->step + step.Delay ());
			step.Veh_Speed (0);		//---- stop the vehicle ----
			break;
		}
		step.Veh_Speed (++speed);
		last_flag = false;

		//---- check speed constraints ----

		if ((cell + 1) == end_cell && speed > leg_itr->Max_Speed ()) {
			step.Veh_Speed (leg_itr->Max_Speed () + 1);
			break;
		}
	}

	//---- attempt a lane change ----

	if (change_flag) {
		if (!Lane_Change (step)) {
			if (step.size () == 0 && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 0) {
				cell_rec = step.sim_veh_ptr->front ();
				cell_rec.Lane_Change (step.sim_veh_ptr->Change ());
				Reserve_Cell (cell_rec, step);
			}
		} else {
			step.Problem (0);
		}
	}
exit:
	return (step.size () > 0);

	//---- remove the last cell from the current move ----

slow_down:
	cell_rec = step.back ();
	exe->sim_dir_array.Cell (cell_rec, -1);

	step.pop_back ();		
	step.Veh_Speed (--speed);
	return (step.size () > 0);
}
