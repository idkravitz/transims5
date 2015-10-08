//*********************************************************
//	Output_Step.cpp - output the travel step directives
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Output_Step
//---------------------------------------------------------

bool Sim_Method::Output_Step (Travel_Step &step)
{
	int cell_offset, index, cell, lane, to_index, to_cell, to_lane, last_index, pce;
	int first_cell, last_cell, to_part, from_index;
	bool reload_flag, bound_flag, remove_flag = false;
	bool transfer_flag = false;

	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Cap_Ptr sim_cap_ptr = 0;
	Sim_Veh_Ptr sim_veh_ptr;
	Cell_Itr cell_itr, veh_cell_itr, end_itr;
	Problem_Data problem_data;

	if (step.Vehicle () >= 0 && step.sim_veh_ptr == 0) {
		step.sim_veh_ptr = exe->sim_veh_array [step.Vehicle ()];
	}
	plan_ptr = step.sim_traveler_ptr->plan_ptr;

	//---- check for lost problems ----

	if (step.Status () == 1 || step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM) {
		if (plan_ptr->Person () == 0) {
			stats.num_run_lost++;
		} else {
			stats.num_veh_lost++;

			//---- check the lost vehicle event ----

			if (step.sim_veh_ptr != 0) {
				if (exe->Check_Event (VEH_LOST_EVENT, plan_ptr->Mode (), subarea)) {
					Event_Data event_data;

					event_data.Household (plan_ptr->Household ());
					event_data.Person (plan_ptr->Person ());
					event_data.Tour (plan_ptr->Tour ());
					event_data.Trip (plan_ptr->Trip ());
					event_data.Mode (plan_ptr->Mode ());

					if (step.Problem () == ARRIVAL_PROBLEM) {
						event_data.Schedule (plan_ptr->Arrive ());
					} else if (step.Problem () == DEPARTURE_PROBLEM) {
						event_data.Schedule (plan_ptr->Depart ());
					} else {
						event_data.Schedule (plan_ptr->Schedule ());
					}
					event_data.Actual (exe->step);
					event_data.Event (VEH_LOST_EVENT);

					veh_cell_itr = step.sim_veh_ptr->begin ();
					veh_cell_itr->Location (&to_index, &to_lane, &to_cell);

					event_data.Dir_Index (to_index);

					if (to_index >= 0 || to_cell > 0) {
						cell_offset = to_cell * param.cell_size + (param.cell_size >> 1);

						event_data.Offset (UnRound (cell_offset));
					} else {
						event_data.Offset (0);
					}
					event_data.Lane (to_lane);
					event_data.Route (-1);

					exe->Output_Event (event_data);
				}
			}
		}
	}

	//---- process the problem message ----

	if (step.Problem () > 0) {
		if (step.Status () == 1 || step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM || 
			param.count_warnings || (exe->select_problems && exe->problem_range.In_Range (step.Problem ()))) {

			step.sim_traveler_ptr->Problem (1);
			exe->Set_Problem ((Problem_Type) step.Problem ());
		}
		if (!exe->select_problems || exe->problem_range.In_Range (step.Problem ())) {
			if (param.problem_flag && step.sim_traveler_ptr->Active ()) {
				if ((int) step.size () > 0) {
					cell_itr = --step.end ();
					cell_itr->Location (&to_index, &to_lane, &to_cell);
				} else if (step.sim_veh_ptr != 0) {
					veh_cell_itr = step.sim_veh_ptr->begin ();
					veh_cell_itr->Location (&to_index, &to_lane, &to_cell);
				} else {
					to_index = step.Dir_Index ();
					to_lane = to_cell = 0;
				}
				problem_data.Household (plan_ptr->Household ());
				problem_data.Person (plan_ptr->Person ());
				problem_data.Tour (plan_ptr->Tour ());
				problem_data.Trip (plan_ptr->Trip ());

				problem_data.Start (plan_ptr->Depart ());
				problem_data.End (plan_ptr->Arrive ());
				problem_data.Duration (plan_ptr->Duration ());

				problem_data.Origin (plan_ptr->Origin ());
				problem_data.Destination (plan_ptr->Destination ());

				problem_data.Purpose (plan_ptr->Purpose ());
				problem_data.Mode (plan_ptr->Mode ());
				problem_data.Constraint (plan_ptr->Constraint ());
				problem_data.Priority (plan_ptr->Priority ());

				if (step.Vehicle () >= 0) {
					problem_data.Vehicle (exe->vehicle_array [step.Vehicle ()].Vehicle ());
				} else {
					problem_data.Vehicle (0);
				}
				problem_data.Passengers (plan_ptr->Passengers ());
				problem_data.Problem (step.Problem ());
				problem_data.Time (exe->step);

				problem_data.Dir_Index (to_index);

				if (to_index >= 0 || to_cell > 0) {
					cell_offset = to_cell * param.cell_size + (param.cell_size >> 1);

					problem_data.Offset (UnRound (cell_offset));
				} else {
					problem_data.Offset (0);
				}
				problem_data.Lane (to_lane);
				problem_data.Route (-1);

				exe->Output_Problem (problem_data);
			}
		}
	}
	if (!step.sim_traveler_ptr->Active ()) {
		exe->veh_status [step.Vehicle ()] = 3;
		return (false);
	}

	//---- move the vehicle forward ----

	if (step.sim_veh_ptr != 0) {

		Cell_Data front = step.sim_veh_ptr->front ();

		//---- process output files ----

		if (front.Lane () >= 0) {
			exe->Check_Output (step);
		}

		//---- update the vehicle status ----

		step.sim_veh_ptr->Speed (step.Veh_Speed ());

		from_index = last_index = front.Index ();
		end_itr = step.sim_veh_ptr->end ();

		sim_dir_ptr = &exe->sim_dir_array [last_index];
		bound_flag = sim_dir_ptr->Boundary ();

#ifdef DEBUG_KEYS
		int from_cell = front.Cell ();
		int from_part = sim_dir_ptr->Partition (front.Cell ());
#endif
		for (cell_itr = step.begin (); cell_itr != step.end (); cell_itr++) {
			cell_itr->Location (&to_index, &to_lane, &to_cell);

			//---- check for a link change ----

			if (to_index != last_index) {
				plan_ptr->Next_Leg ();
				last_index = to_index;
			}

			//---- move each vehicle cell ----

			for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
				veh_cell_itr->Location (&index, &lane, &cell);

				if (index >= 0 && lane >= 0) {
					if (to_index == index && to_lane == lane && to_cell == cell) break;
					exe->sim_dir_array.Cell (index, lane, cell, -1);
				}
				veh_cell_itr->Location (to_index, to_lane, to_cell);

				if (to_index >= 0 && to_lane >= 0) {
#ifdef DEBUG_KEYS
					int v = exe->sim_dir_array.Cell (to_index, to_lane, to_cell);
					if (v != -1 && v != step.Vehicle ()) {
						MAIN_LOCK
						exe->Write (1, "override=") << v << " with=" << step.Vehicle () << " boundary=" << exe->sim_dir_array [to_index].Boundary ();
						exe->Write (0, " front=") << exe->link_array [exe->dir_array [front.Index ()].Link ()].Link () << "-" << front.Lane () << "-" << front.Cell ();
						exe->Write (0, " to=") << exe->link_array [exe->dir_array [to_index].Link ()].Link () << "-" << to_lane << "-" << to_cell;
						END_LOCK
					}
#endif
					exe->sim_dir_array.Cell (to_index, to_lane, to_cell, step.Vehicle ());
				}
				to_index = index;
				to_lane = lane;
				to_cell = cell;
			}
		}

		//---- check the partition status ----

		sim_dir_ptr = &exe->sim_dir_array [last_index];
		front = step.sim_veh_ptr->front ();
#ifdef DEBUG_KEYS
		if (front.Lane () >= 0 && sim_dir_ptr->Cell (front.Lane (), front.Cell ()) != step.Vehicle ()) {
			MAIN_LOCK exe->Write (1, " MOVE_LOC: vehicle=") << step.Vehicle () << " cell=" << front.Index () << "-" << front.Lane () << "-" << front.Cell () << " dir=" << last_index << "=" << sim_dir_ptr->Cell (front.Lane (), front.Cell ()); END_LOCK
		}
#endif
		to_part = sim_dir_ptr->Partition (front.Cell ());

		//---- output traveler record ----

		if (exe->Output_Traveler_Flag ()) {
			Traveler_Data traveler_data;

			traveler_data.Household (plan_ptr->Household ());
			traveler_data.Person (plan_ptr->Person ());
			traveler_data.Time (exe->step);
			traveler_data.Mode (plan_ptr->Mode ());

			if (exe->Check_Traveler (traveler_data)) {
				traveler_data.Tour (plan_ptr->Tour ());
				traveler_data.Trip (plan_ptr->Trip ());
				traveler_data.Distance ((int) step.size () * param.cell_size);
				traveler_data.Speed (step.Veh_Speed () * param.cell_size);
				traveler_data.Dir_Index (front.Index ());
				traveler_data.Lane (front.Lane ());
				traveler_data.Offset ((front.Cell () + 1) * param.cell_size);

				exe->Output_Traveler (traveler_data);
			}
		}

#ifdef DEBUG_KEYS
if (!boundary_flag && from_part != to_part) {
	MAIN_LOCK exe->Write (1, "BOUND=") << step.Vehicle () << " from=" << from_part << " to=" << to_part << " part=" << partition << " from=" << from_cell << " to=" << front.Cell () << " split=" << sim_dir_ptr->Split_Cell () << " step=" << exe->step; END_LOCK
}
		if (exe->debug) MAIN_LOCK exe->Write (0, " part=") << to_part << " vs " << partition << " cell=" << front.Cell () << " vs " << sim_dir_ptr->Cells () << " spd=" << sim_dir_ptr->Speed (); END_LOCK
#endif
		if (bound_flag && to_part != partition) {
			transfer_flag = true;
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " TRANSFER=") << step.Vehicle () << " from=" << partition << " to=" << to_part; END_LOCK
#endif
		}
		if (sim_dir_ptr->Boundary ()) {

			last_cell = sim_dir_ptr->Split_Cell ();
			first_cell = last_cell - sim_dir_ptr->Speed ();

			cell = front.Cell ();

			if (cell >= first_cell && cell < last_cell) {
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " BOUNDARY=") << step.Vehicle (); END_LOCK
#endif
				boundary.push_back (step.Vehicle ());
			} else if (cell >= last_cell) {
				if (to_part != partition) {
					transfer_flag = true;
				} else if ((cell - last_cell) < step.sim_veh_ptr->Num_Cells ()) {
					first_list.push_back (step.Vehicle ());
#ifdef DEBUG_KEYS
					if (exe->debug) MAIN_LOCK exe->Write (0, " FIRST=") << step.Vehicle (); END_LOCK
#endif
				}
			}
		}

		//---- process a transit stop ----

		if (step.Stop () >= 0) {
			leg_itr = plan_ptr->begin ();
			step.sim_veh_ptr->Next_Event (exe->step + leg_itr->Time ());
			plan_ptr->Next_Leg ();
			if (plan_ptr->size () == 0) {
				stats.num_run_end++;
				step.Status (1);
				remove_flag = true;
			}
		}

		//---- park the vehicle -----

		if (step.Parking () >= 0 || step.Status () > 0) {

			//---- remove the vehicle from the network ----

			sim_dir_ptr = &exe->sim_dir_array [last_index];

			if (step.sim_traveler_ptr->Status () >= 0) {
				for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
					veh_cell_itr->Location (&index, &lane, &cell);

					if (index >= 0 && lane >= 0) {
						exe->sim_dir_array.Cell (index, lane, cell, -1);
#ifdef DEBUG_KEYS
						if (exe->debug) {
							MAIN_LOCK exe->Write (1, " remove veh") << step.Vehicle () << " cell=" << index << "-" << lane << "-" << cell << " last=" << last_index << " status=" << step.sim_traveler_ptr->Status (); END_LOCK
						}
#endif
					}
				}
				if (step.Status () > 0 && sim_dir_ptr->Method () == MACROSCOPIC) {
					sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

					//---- remove the vehicle from the current link ----

					pce = (int) step.sim_veh_ptr->size ();

					if (step.sim_veh_ptr->Restricted ()) {
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
				}
			}
			if (sim_dir_ptr->Method () == MESOSCOPIC) {
				if (step.sim_veh_ptr->Follower () >= 0) {
					sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Follower ()];
					if (sim_veh_ptr != 0) {
						sim_veh_ptr->Leader (-1);
						sim_veh_ptr->Next_Event (0);
					}
				}
				if (step.sim_veh_ptr->Leader () >= 0) {
					sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
					if (sim_veh_ptr != 0) sim_veh_ptr->Follower (-1);
				}
			}
			
			//---- move the vehicle to the destination parking lot ----

			reload_flag = false;

			if (step.Parking () >= 0) {
				step.sim_traveler_ptr->Status (2);

				stats.num_veh_end++;
				stats.tot_hours += exe->step - plan_ptr->Depart ();

				exe->vehicle_array [step.Vehicle ()].Parking (step.Parking ());
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " PARK vehicle=") << step.Vehicle (); END_LOCK
#endif
				//---- output event record ----

				if (exe->Check_Event (VEH_END_EVENT, plan_ptr->Mode (), subarea)) {
					Event_Data event_data;

					event_data.Household (plan_ptr->Household ());
					event_data.Person (plan_ptr->Person ());
					event_data.Tour (plan_ptr->Tour ());
					event_data.Trip (plan_ptr->Trip ());
					event_data.Mode (plan_ptr->Mode ());
					event_data.Schedule (plan_ptr->Arrive ());
					event_data.Actual (exe->step);
					event_data.Event (VEH_END_EVENT);

					event_data.Dir_Index (front.Index ());
					event_data.Lane (front.Lane ());
					event_data.Offset (UnRound (front.Cell () * param.cell_size + (param.cell_size >> 1)));

					event_data.Route (-1);

					exe->Output_Event (event_data);
				}
				leg_itr = plan_ptr->begin ();
				plan_ptr->Next_Event (exe->step + leg_itr->Time ());
				plan_ptr->Next_Leg ();
				remove_flag = true;

			} else if (step.sim_traveler_ptr->Active ()) {

				//---- check the reload option ----

				if (step.Status () == 1 && param.reload_problems) {
					plan_ptr->Next_Leg ();
					leg_itr = plan_ptr->begin ();

					if (leg_itr != plan_ptr->end () && leg_itr->Type () == DIR_ID) {
						reload_flag = true;
						step.Status (0);
						step.sim_traveler_ptr->Status (-4);

						to_index = leg_itr->Index ();

						for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
							veh_cell_itr->Location (to_index, -1, 0);
						}
						last_index = to_index;
						sim_dir_ptr = &exe->sim_dir_array [last_index];
						if (sim_dir_ptr->From_Part () != partition) {
							remove_flag = true;
							to_part = sim_dir_ptr->From_Part ();
						}
					}
				}

				//---- move the vehicle to the destination parking lot ----

				if (!reload_flag) {
					for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
						if (leg_itr->Type () == PARKING_ID) {
							if (plan_ptr->Next_Event () < 0) {
								plan_ptr->Next_Event (0);
							} else {
								exe->vehicle_array [step.Vehicle ()].Parking (leg_itr->Index ());
								break;
							}
						}
					}
					step.sim_traveler_ptr->Delete ();
				}
			}

			//---- delete the vehicle ----

			if (!reload_flag) {
				delete step.sim_veh_ptr;
				exe->sim_veh_array [step.Vehicle ()] = step.sim_veh_ptr = 0;

				exe->veh_status [step.Vehicle ()] = 3;
			} else {

				step.sim_veh_ptr->Leader (-1);
				step.sim_veh_ptr->Follower (-1);
				step.sim_veh_ptr->Wait (0);

				exe->veh_status [step.Vehicle ()] = (remove_flag) ? 3 : ((step.size () > 0) ? 2 : 1);
			}
		} else {
			exe->veh_status [step.Vehicle ()] = (transfer_flag) ? 3 : ((step.size () > 0) ? 2 : 1);
		}
		if (remove_flag || transfer_flag) {
			sim_dir_ptr = &exe->sim_dir_array [last_index];

#ifdef DEBUG_KEYS
			if (exe->debug) {
				MAIN_LOCK 
				if (remove_flag) {
					exe->Write (1, "\tRemove=") << step.Vehicle ();
				} else {
					exe->Write (1, "\tTransfer=") << step.Vehicle ();
				}
				exe->Write (0, " part=") << partition << " to " << to_part << " status=" << (int) exe->veh_status [step.Vehicle ()] << " push_traveler=" << step.Traveler ();
				END_LOCK
			}
#endif
			plan_ptr->Partition (to_part);
			plan_ptr->Next_Event (0);

			transfers->push_back (Int2_Key (step.Traveler (), plan_ptr->Partition ()));

			//---- remove links between subareas ----

			if (step.sim_veh_ptr != 0 && sim_dir_ptr->Method () == MESOSCOPIC) {
#ifdef DEBUG_KEYS
				if (exe->debug) {
					MAIN_LOCK 
					cell_itr = step.sim_veh_ptr->begin ();
					exe->Write (0, " to cell=") << exe->link_array [exe->dir_array [cell_itr->Index ()].Link ()].Link () << "-" << cell_itr->Lane () << "-" << cell_itr->Cell () << " veh=" << sim_dir_ptr->Cell (*(cell_itr));
					END_LOCK
				}
#endif
				if (step.sim_veh_ptr->Follower () >= 0) {
					sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Follower ()];
					if (sim_veh_ptr != 0) sim_veh_ptr->Leader (-1);
					step.sim_veh_ptr->Follower (-1);
				}
				if (step.sim_veh_ptr->Leader () >= 0) {
					sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
					if (sim_veh_ptr != 0) sim_veh_ptr->Follower (-1);
					step.sim_veh_ptr->Leader (-1);
				}
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " set leader=") << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower () << " trv_stat=" << step.sim_traveler_ptr->Status () ; END_LOCK
#endif
			}
		}
	}
	if (step.Status () > 0) return (false);
	return (!remove_flag);
}
