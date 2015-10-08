//*********************************************************
//	Check_Queue.cpp - Check Link Exit Queue
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Check_Queue
//---------------------------------------------------------

bool Sim_Method::Check_Queue (Travel_Step &step)
{
	int dir_index, lane, cell, to_cell, control, end_cell, first_cell, last_cell, max_cell, mid_cell;
	int speed, max_speed, pce, index, veh_type;
	double capacity;
	bool next_flag, restricted_flag, park_flag, method_flag = false;
	Use_Type use;

	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Dir_Ptr sim_dir_ptr, next_dir_ptr = 0;
	Sim_Cap_Ptr sim_cap_ptr, next_cap_ptr = 0;
	Sim_Lane_Ptr next_lane_ptr;
	Cell_Data cell_rec;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg;
	Connect_Data *connect_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Dir_Data *dir_ptr;
	Turn_Pen_Data *turn_ptr;

	//---- get the current location ----

	if (step.size () > 0) {
		cell_rec = step.back ();
	} else {
		cell_rec = step.sim_veh_ptr->front ();
	}
	dir_index = cell_rec.Index ();
	cell = cell_rec.Cell ();

	if (dir_index == step.Dir_Index ()) {
		if (step.sim_dir_ptr == 0) {
			step.sim_dir_ptr = &exe->sim_dir_array [dir_index];
		}
		sim_dir_ptr = step.sim_dir_ptr;
	} else {
		sim_dir_ptr = &exe->sim_dir_array [dir_index];
	}
	if (!sim_dir_ptr->Active ()) return (step.size () > 0);

	veh_type = step.sim_veh_ptr->Type ();
	use = (Use_Type) step.veh_type_ptr->Use ();

	//---- find the end of the queue ----

	sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

	capacity = sim_cap_ptr->High_Capacity () + sim_cap_ptr->Low_Capacity ();
	max_cell = (int) (sim_dir_ptr->Cells () * (1.0 - (step.Position () + 1) / capacity));

	pce = (int) step.sim_veh_ptr->size ();

	//---- calculate the maximum move ----

	max_speed = step.sim_veh_ptr->Speed () + step.veh_type_ptr->Max_Accel ();

	if (max_speed > step.veh_type_ptr->Max_Speed ()) {
		max_speed = step.veh_type_ptr->Max_Speed ();
	}
	if (max_speed > sim_dir_ptr->Speed ()) {
		max_speed = sim_dir_ptr->Speed ();
	}

	//---- apply random slow down ----

	if (max_speed > 0 && step.sim_veh_ptr->Priority () < 1 && param.slow_down_flag) {
		double slow_down = param.slow_down_prob [sim_dir_ptr->Type ()];
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
		if (max_speed == 0) {
			step.Veh_Speed (0);
			return (false);
		}
	}

	//---- check the limits ----

	first_cell = sim_dir_ptr->In_Cell ();
	last_cell = sim_dir_ptr->Cell_Out ();
	end_cell = sim_dir_ptr->Cells () - 1;
	mid_cell = sim_dir_ptr->Cells () / 2;

	to_cell = cell + max_speed;

	if (to_cell >= max_cell && to_cell <= last_cell) {
		max_speed = max_cell - cell;

		if (max_speed <= 0) {
			step.Veh_Speed (0);
			goto exit;
		}

		//---- apply reaction time ---

		if (param.reaction_flag) {
			double gap, reaction;

			gap = 0.0;
			reaction = param.reaction_time [sim_dir_ptr->Type ()];

			if (param.traveler_flag) {
				reaction *= param.traveler_fac.Best (step.sim_traveler_ptr->Type ());
			}
			for (int c=1; c <= max_speed; c++) {
				gap += reaction / c;
			}
			max_speed = (int) gap;
			gap -= max_speed;

			if (gap > step.sim_traveler_ptr->random.Probability ()) max_speed++;

			if (max_speed <= 0) {
				step.Veh_Speed (0);
				goto exit;
			}
		}
	}

#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (1, "Vehicle=") << step.Vehicle () << " Cell=" << cell << " vs " << end_cell << " vs " << max_cell << " link=" << exe->link_array [exe->dir_array [dir_index].Link ()].Link () << "-" << cell_rec.Lane () << "-" << cell_rec.Cell () << " leader=" << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower (); END_LOCK
#endif
	//---- get the plan data ----

	plan_ptr = step.sim_traveler_ptr->plan_ptr;
	if (plan_ptr == 0) goto exit;

	leg_itr = plan_ptr->begin ();
	if (leg_itr == plan_ptr->end ()) goto exit;

	//---- parking position ----

	if (leg_itr->Connect () < 0) {
		leg_itr++;
		if (leg_itr->Type () == PARKING_ID) {
			sim_park_ptr = &exe->sim_park_array [leg_itr->Index ()];

			if (sim_dir_ptr->Dir () == 0) {
				end_cell = sim_park_ptr->Cell_AB ();
			} else {
				end_cell = sim_park_ptr->Cell_BA ();
			}
		}
		park_flag = true;
	} else {
		park_flag = false;
	}

#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (0, " Max_Speed=") << max_speed; END_LOCK
#endif
	//---- try to move forward ----

	for (speed = 0; speed <= max_speed; speed++) {

		//---- check the intersection ----

		if (cell == last_cell && !park_flag) {
			connect_ptr = &exe->connect_array [leg_itr->Connect ()];
			control = connect_ptr->Control ();

			//---- check traffic controls ----

			if (control == RED_LIGHT || control == YELLOW_LIGHT) {
				step.sim_veh_ptr->Next_Event (Next_Signal_Event (dir_index));
				speed = 0;
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " Control=") << control; END_LOCK
#endif
				break;
			} else if (step.sim_veh_ptr->Speed () > 0 && (control == STOP_SIGN || control == STOP_GREEN)) {
				speed = 0;
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " Stop Sign"); END_LOCK
#endif
				break;
			}

			//---- check for turn prohibition ----
			
			if (sim_dir_ptr->Turn ()) {
				dir_ptr = &exe->dir_array [dir_index];

				for (index = dir_ptr->First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
					turn_ptr = &exe->turn_pen_array [index];

					if (turn_ptr->Penalty () != 0 || turn_ptr->To_Index () != connect_ptr->To_Index ()) continue;

					if (turn_ptr->Start () <= exe->step && exe->step < turn_ptr->End ()) {
						if (veh_type < 0 || turn_ptr->Min_Veh_Type () < 0 || 
							(turn_ptr->Min_Veh_Type () <= veh_type && veh_type <= turn_ptr->Max_Veh_Type ())) {
							
							if (Use_Permission (turn_ptr->Use (), use)) {
								step.Problem (CONNECT_PROBLEM);
								goto exit;
							}
						}
					}
				}
			}
		}

		//---- move forward ----

		if (cell < end_cell) {
			cell_rec.Cell (++cell);

			//---- set the lane number ----

			if (cell >= last_cell - 1) {
				cell_rec.Lane ((leg_itr->Out_Best_Low () + leg_itr->Out_Best_High ()) / 2);
			} else if (cell == first_cell + 1) {
				if (step.sim_veh_ptr->Restricted ()) {
					if (cell_rec.Lane () < sim_cap_ptr->High_Min_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->High_Min_Lane ());
					} else if (cell_rec.Lane () > sim_cap_ptr->High_Max_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->High_Max_Lane ());
					}
				} else {
					if (cell_rec.Lane () < sim_cap_ptr->Low_Min_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->Low_Min_Lane ());
					} else if (cell_rec.Lane () > sim_cap_ptr->Low_Max_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->Low_Max_Lane ());
					}
				}
			} else if (cell == mid_cell) {
				cell_rec.Lane ((leg_itr->Out_Best_Low () + leg_itr->Out_Best_High ()) / 2);

				if (step.sim_veh_ptr->Restricted ()) {
					if (cell_rec.Lane () < sim_cap_ptr->High_Min_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->High_Min_Lane ());
					} else if (cell_rec.Lane () > sim_cap_ptr->High_Max_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->High_Max_Lane ());
					}
				} else {
					if (cell_rec.Lane () < sim_cap_ptr->Low_Min_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->Low_Min_Lane ());
					} else if (cell_rec.Lane () > sim_cap_ptr->Low_Max_Lane ()) {
						cell_rec.Lane (sim_cap_ptr->Low_Max_Lane ());
					}
				}
			}
			step.push_back (cell_rec);

			if (cell == max_cell && cell <= last_cell) {
				step.Veh_Speed (0);
				goto exit;
			}
			continue;
		}
		restricted_flag = step.sim_veh_ptr->Restricted ();

		//---- park the vehicle ----

		if (park_flag) {
			cell_rec.Lane (-1);
			step.push_back (cell_rec);
			step.Parking (leg_itr->Index ());

		} else {

			//---- check the next link ----

			connect_ptr = &exe->connect_array [leg_itr->Connect ()];

			dir_index = connect_ptr->To_Index ();
			next_dir_ptr = &exe->sim_dir_array [dir_index];
			if (!next_dir_ptr->Active ()) break;
			next_cap_ptr = 0;

#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (1, "New Link=") << exe->link_array [exe->dir_array [dir_index].Link ()].Link () << "-" << cell_rec.Lane () << "-" << cell_rec.Cell () << " leader=" << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower (); END_LOCK
#endif
			leg_itr++;
			cell_rec.Index (dir_index);
			cell = 0;
			cell_rec.Cell (0);
			cell_rec.Lane ((leg_itr->In_Best_Low () + leg_itr->In_Best_High ()) / 2);

			if (next_dir_ptr->Method () == NO_SIMULATION) {
				next_flag = method_flag = true;
			} else {
				next_flag = method_flag = false;

				if (next_dir_ptr->Method () == MACROSCOPIC) {

					//---- try the restricted lanes first ----

					next_cap_ptr = next_dir_ptr->Sim_Cap ();

					if (Use_Permission (next_cap_ptr->High_Use (), use)) {
						if ((next_cap_ptr->High_Volume () + pce) <= next_cap_ptr->High_Capacity ()) {
							next_flag = true;
							next_cap_ptr->Add_High (pce);
							step.sim_veh_ptr->Restricted (true);
						}
					}

					//---- try the general purpose lanes ----

					if (!next_flag && (next_cap_ptr->Low_Volume () + pce) <= next_cap_ptr->Low_Capacity ()) {
						next_flag = true;
						next_cap_ptr->Add_Low (pce);
						step.sim_veh_ptr->Restricted (false);
					}

				} else if (next_dir_ptr->Method () == MESOSCOPIC) {
					method_flag = true;

					for (lane=leg_itr->In_Best_Low (); lane <= leg_itr->In_Best_High (); lane++) {
						next_lane_ptr = next_dir_ptr->Lane (lane);
						if (next_lane_ptr->at (0) == -1) {
							next_flag = true;
							break;
						}
					}
					if (!next_flag) {
						for (lane=leg_itr->In_Lane_Low (); lane <= leg_itr->In_Lane_High (); lane++) {
							next_lane_ptr = next_dir_ptr->Lane (lane);
							if (next_lane_ptr->at (0) == -1) {
								next_flag = true;
								break;
							}
						}
					}
					if (next_flag) {
						cell_rec.Lane (lane);
					}
				}
			}
			if (!next_flag) {
#ifdef DEBUG_KEYS
				if (exe->debug && next_dir_ptr->Method () == MACROSCOPIC) MAIN_LOCK exe->Write (0, " High=") << next_cap_ptr->High_Volume () << " vs " << next_cap_ptr->High_Capacity () << " Low=" << next_cap_ptr->Low_Volume () << " vs " << next_cap_ptr->Low_Capacity (); END_LOCK
#endif
				break;
			}
		}

		//---- remove the vehicle from the current link ----

		if (restricted_flag) {
			sim_cap_ptr->Add_High (-pce);
		} else {
			sim_cap_ptr->Add_Low (-pce);
		}
		step.Exit_Link (pce);

		//---- update the vehicle list ----

		if (step.sim_veh_ptr->Follower () >= 0) {
			sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Follower ()];
			if (sim_veh_ptr != 0) sim_veh_ptr->Leader (step.sim_veh_ptr->Leader ());
		} else {
			sim_cap_ptr->Last_Veh (step.sim_veh_ptr->Leader ());
		}
		if (step.sim_veh_ptr->Leader () >= 0) {
			sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
			if (sim_veh_ptr != 0) sim_veh_ptr->Follower (step.sim_veh_ptr->Follower ());
		} else {
			sim_cap_ptr->First_Veh (step.sim_veh_ptr->Follower ());
		}
		if (park_flag) break;

		//---- place on the link ----

		sim_dir_ptr = next_dir_ptr;

		if (method_flag) {
			step.sim_veh_ptr->Leader (-1);
			step.sim_veh_ptr->Follower (-1);
			step.push_back (cell_rec);
			break;
		}
		sim_cap_ptr = next_cap_ptr;

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
		step.push_back (cell_rec);

#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (1, "New Link=") << exe->link_array [exe->dir_array [dir_index].Link ()].Link () << "-" << cell_rec.Lane () << "-" << cell_rec.Cell () << " leader=" << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower (); END_LOCK
#endif
		first_cell = sim_dir_ptr->In_Cell ();
		last_cell = sim_dir_ptr->Cell_Out ();
		end_cell = sim_dir_ptr->Cells () - 1;
		mid_cell = sim_dir_ptr->Cells () / 2;

		max_cell = sim_cap_ptr->High_Volume () + sim_cap_ptr->Low_Volume ();
		capacity = sim_cap_ptr->High_Capacity () + sim_cap_ptr->Low_Capacity ();
		max_cell = (int) (sim_dir_ptr->Cells () * (1.0 - max_cell / capacity));

		//---- parking position ----

		if (leg_itr->Connect () < 0) {
			leg_itr++;
			if (leg_itr->Type () == PARKING_ID) {
				sim_park_ptr = &exe->sim_park_array [leg_itr->Index ()];

				if (sim_dir_ptr->Dir () == 0) {
					end_cell = sim_park_ptr->Cell_AB ();
				} else {
					end_cell = sim_park_ptr->Cell_BA ();
				}
			}
			park_flag = true;
		} else {
			park_flag = false;
		}
	}
	step.Veh_Speed (speed);
exit:
	return ((step.size () > 0));
}
