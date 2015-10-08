//*********************************************************
//	Load_Vehicle.cpp - attempt to load a vehicle to the link
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Load_Vehicle
//---------------------------------------------------------

bool Sim_Method::Load_Vehicle (Travel_Step &step)
{
	int lane, cell, pce, part;
	bool loaded, type_flag, bound_flag, side_flag;

	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Vehicle_Data *vehicle_ptr;
	Sim_Veh_Ptr sim_veh_ptr, lead_veh_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;
	Sim_Cap_Ptr sim_cap_ptr;
	Cell_Data cell_rec, veh_cell;

	//---- check the pointers ----

	plan_ptr = step.sim_traveler_ptr->plan_ptr;
	if (plan_ptr == 0) return (false);
	
	leg_itr = plan_ptr->begin ();
	if (leg_itr == plan_ptr->end ()) return (false);

	if (leg_itr->Type () != DIR_ID) {
		step.Problem (ACCESS_PROBLEM);
		step.Status (2);
		return (false);
	}
	step.Dir_Index (leg_itr->Index ());
	step.sim_dir_ptr = &exe->sim_dir_array [step.Dir_Index ()];

	//---- locate the load point ----

	type_flag = (step.sim_traveler_ptr->Status () != -2);

	if (type_flag) {

		//---- external link load ----

		cell = 0;
		bound_flag = true;
		side_flag = (step.sim_traveler_ptr->random.Probability () >= 0.5);

	} else {

		vehicle_ptr = &exe->vehicle_array [step.Vehicle ()];

		//---- transit loading ----

		if (plan_ptr->Person () == 0) {
			sim_stop_ptr = &exe->sim_stop_array [vehicle_ptr->Parking ()];

			bound_flag = false;
			side_flag = true;
			cell = sim_stop_ptr->Cell ();
			part = sim_stop_ptr->Partition ();

		} else {	//---- parking lot load ----

			sim_park_ptr = &exe->sim_park_array [vehicle_ptr->Parking ()];

			bound_flag = (sim_park_ptr->Type () == BOUNDARY);

			if (bound_flag) {
				side_flag = (step.sim_traveler_ptr->random.Probability () >= 0.5);
			} else {
				side_flag = (sim_park_ptr->Dir () == step.sim_dir_ptr->Dir ());
			}
			if (step.sim_dir_ptr->Dir () == 0) {
				cell = sim_park_ptr->Cell_AB ();
				part = sim_park_ptr->Part_AB ();
			} else {
				cell = sim_park_ptr->Cell_BA ();
				part = sim_park_ptr->Part_BA ();
			}
		}
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (1, "   Load=") << step.Vehicle () << " park=" << vehicle_ptr->Parking () << " dir=" << step.sim_dir_ptr->Dir () << " cell=" << cell << " part=" << part << " vs " << step.sim_dir_ptr->Partition (cell) << " traveler=" << step.Traveler (); END_LOCK
#endif
		if (part != partition) {
			exe->veh_status [step.Vehicle ()] = 3;
			transfers->push_back (Int2_Key (step.Traveler (), part));
			return (false);
		}
	}

	//---- set vehicle type attributes ----

	if (bound_flag) {
		step.sim_veh_ptr->Speed (step.veh_type_ptr->Max_Speed ());
	} else {
		step.sim_veh_ptr->Speed (step.veh_type_ptr->Max_Accel ());
	}
	if (step.sim_veh_ptr->Speed () > step.sim_dir_ptr->Speed ()) {
		step.sim_veh_ptr->Speed (step.sim_dir_ptr->Speed ());
	}
	pce = (int) step.sim_veh_ptr->size ();

	//---- set the access point ----

	cell_rec.Location (step.Dir_Index (), -1, cell);
	step.sim_veh_ptr->front () = cell_rec;
	loaded = false;
	lane = 0;

	//---- macroscopic loading ----

	if (step.sim_dir_ptr->Method () == MACROSCOPIC) {

		//---- load from the right ----

		sim_cap_ptr = step.sim_dir_ptr->Sim_Cap ();

		//---- try the restricted lanes first ----

		if (Use_Permission (sim_cap_ptr->High_Use (), (Use_Type) step.veh_type_ptr->Use ())) {
			if ((sim_cap_ptr->High_Volume () + pce) <= sim_cap_ptr->High_Capacity ()) {
				if (side_flag) {
					lane = sim_cap_ptr->High_Max_Lane ();
				} else {
					lane = sim_cap_ptr->High_Min_Lane ();
				}
				loaded = true;
				step.sim_veh_ptr->Restricted (true);
			}
		}

		//---- try the general purpose lanes ----

		if (!loaded && (sim_cap_ptr->Low_Volume () + pce) <= sim_cap_ptr->Low_Capacity ()) {
			if (side_flag) {
				lane = sim_cap_ptr->Low_Max_Lane ();
			} else {
				lane = sim_cap_ptr->Low_Min_Lane ();
			}
			loaded = true;
			step.sim_veh_ptr->Restricted (false);
		}
		if (loaded) {

			//---- find the insert position ----

			if (sim_cap_ptr->First_Veh () >= 0) {

				sim_veh_ptr = exe->sim_veh_array [sim_cap_ptr->First_Veh ()];
				if (sim_veh_ptr != 0) {
					veh_cell = sim_veh_ptr->front ();

					if (veh_cell.Cell () < cell) {

						//---- insert in front ----

						step.sim_veh_ptr->Leader (-1);
						step.sim_veh_ptr->Follower (sim_cap_ptr->First_Veh ());
						sim_veh_ptr->Leader (step.Vehicle ());
						sim_cap_ptr->First_Veh (step.Vehicle ());

					} else if (sim_veh_ptr->Follower () < 0) {

						if (veh_cell.Cell () > cell || (veh_cell.Cell () == cell && veh_cell.Lane () != lane)) {

							//---- insert behind ----

							sim_veh_ptr->Follower (step.Vehicle ());
							step.sim_veh_ptr->Leader (sim_cap_ptr->Last_Veh ());
							step.sim_veh_ptr->Follower (-1);
							sim_cap_ptr->Last_Veh (step.Vehicle ());
						} else {
							loaded = false;
						}

					} else {

						lead_veh_ptr = sim_veh_ptr;
						sim_veh_ptr = exe->sim_veh_array [sim_cap_ptr->Last_Veh ()];
						if (sim_veh_ptr != 0) {
							veh_cell = sim_veh_ptr->front ();

							if (veh_cell.Cell () > cell || (veh_cell.Cell () == cell && veh_cell.Lane () != lane)) {

								//---- insert behind ----

								sim_veh_ptr->Follower (step.Vehicle ());
								step.sim_veh_ptr->Leader (sim_cap_ptr->Last_Veh ());
								step.sim_veh_ptr->Follower (-1);
								sim_cap_ptr->Last_Veh (step.Vehicle ());

							} else {

								sim_veh_ptr = lead_veh_ptr;
								veh_cell = sim_veh_ptr->front ();

								while (veh_cell.Cell () >= cell && sim_veh_ptr->Follower () >= 0) {
									lead_veh_ptr = sim_veh_ptr;
									sim_veh_ptr = exe->sim_veh_array [lead_veh_ptr->Follower ()];
									if (sim_veh_ptr == 0) break;
									veh_cell = sim_veh_ptr->front ();
								}
								if (veh_cell.Cell () < cell || (veh_cell.Cell () == cell && veh_cell.Lane () != lane)) {

									//---- insert in between ----

									step.sim_veh_ptr->Follower (lead_veh_ptr->Follower ());
									step.sim_veh_ptr->Leader (sim_veh_ptr->Leader ());
									sim_veh_ptr->Leader (step.Vehicle ());
									lead_veh_ptr->Follower (step.Vehicle ());

								} else {
									loaded = false;
								}
							}
						}
					}
				}

			} else {

				//---- place on the link ----

				sim_cap_ptr->First_Veh (step.Vehicle ());
				sim_cap_ptr->Last_Veh (step.Vehicle ());
			}
			if (loaded) {
				if (step.sim_veh_ptr->Restricted ()) {
					sim_cap_ptr->Add_High (pce);
				} else {
					sim_cap_ptr->Add_Low (pce);
				}
				cell_rec.Lane (lane);

				if (step.Next_Vehicle () < 0 && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Follower ()) {
					if (exe->veh_status [step.sim_veh_ptr->Follower ()] == 0) {
						step.Next_Vehicle (step.sim_veh_ptr->Follower ());
					}
				}
			}
		}

		//---- add to the priority list ----

		if (!loaded && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 0) {
			priority_list.push_back (step.Vehicle ());
		}

	} else {		//---- mescoscopic loading ----

		//---- load from the right ----

		if (side_flag) {

			//---- try to load onto the best lanes ----

			for (lane = leg_itr->In_Best_High (); lane >= leg_itr->In_Best_Low (); lane--) {
				cell_rec.Lane (lane);

				loaded = Check_Behind (cell_rec, step);
				if (loaded) break;
			}
			if (!loaded) {

				//---- try to load on the permitted lanes ----

				for (lane = leg_itr->In_Lane_High (); lane >= leg_itr->In_Lane_Low (); lane--) {
					if (lane < leg_itr->In_Best_Low () || lane > leg_itr->In_Best_High ()) {
						cell_rec.Lane (lane);
						loaded = Check_Behind (cell_rec, step);
						if (loaded) break;
					}
				}
			}

			//---- start a priority load ----

			if (!loaded && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 0) {

				//---- try to reserve a cell onto the best lanes ----

				for (lane = leg_itr->In_Best_High (); lane >= leg_itr->In_Best_Low (); lane--) {
					cell_rec.Lane (lane);
					if (Reserve_Cell (cell_rec, step)) break;
				}
				if (step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 1) {

					//---- try to reserve a cell on the permitted lanes ----

					for (lane = leg_itr->In_Lane_High (); lane >= leg_itr->In_Lane_Low (); lane--) {
						if (lane < leg_itr->In_Best_Low () || lane > leg_itr->In_Best_High ()) {
							cell_rec.Lane (lane);
							if (Reserve_Cell (cell_rec, step)) break;
						}
					}
				}
				priority_list.push_back (step.Vehicle ());
			}

		} else {	//---- load from the left ----

			//---- try to load onto the best lanes ----

			for (lane = leg_itr->In_Best_Low (); lane <= leg_itr->In_Best_High (); lane++) {
				cell_rec.Lane (lane);
				loaded = Check_Behind (cell_rec, step);
				if (loaded) break;
			}
			if (!loaded) {

				//---- try to load on the permitted lanes ----

				for (lane = leg_itr->In_Lane_Low (); lane <= leg_itr->In_Lane_High (); lane++) {
					if (lane < leg_itr->In_Best_Low () || lane > leg_itr->In_Best_High ()) {
						cell_rec.Lane (lane);
						loaded = Check_Behind (cell_rec, step);
						if (loaded) break;
					}
				}
			}

			//---- start a priority load ----

			if (!loaded && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 0) {

				//---- try to reserve a cell onto the best lanes ----

				for (lane = leg_itr->In_Best_Low (); lane <= leg_itr->In_Best_High (); lane++) {
					cell_rec.Lane (lane);
					if (Reserve_Cell (cell_rec, step)) break;
				}
				if (step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 1) {

					//---- try to reserve a cell on the permitted lanes ----

					for (lane = leg_itr->In_Lane_Low (); lane <= leg_itr->In_Lane_High (); lane++) {
						if (lane < leg_itr->In_Best_Low () || lane > leg_itr->In_Best_High ()) {
							cell_rec.Lane (lane);
							if (Reserve_Cell (cell_rec, step)) break;
						}
					}
				}
				priority_list.push_back (step.Vehicle ());
			}
		}
	}

	//---- process the result ----

	if (!loaded) return (false);

	step.push_back (cell_rec);
	step.sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
	step.sim_veh_ptr->Next_Event (0);

	//---- put the vehicle on the network ----

	if (step.sim_traveler_ptr->Status () != -4) {
		if (plan_ptr->Person () == 0) {
			stats.num_run_start++;
		} else {
			stats.num_veh_start++;

			//---- output event record ----

			if (exe->Check_Event (VEH_START_EVENT, plan_ptr->Mode (), subarea)) {
				Event_Data event_data;

				event_data.Household (plan_ptr->Household ());
				event_data.Person (plan_ptr->Person ());
				event_data.Tour (plan_ptr->Tour ());
				event_data.Trip (plan_ptr->Trip ());
				event_data.Mode (plan_ptr->Mode ());
				event_data.Schedule (plan_ptr->Depart ());
				event_data.Actual (exe->step);
				event_data.Event (VEH_START_EVENT);
				event_data.Dir_Index (step.Dir_Index ());
				event_data.Offset (UnRound (cell * param.cell_size + (param.cell_size >> 1)));
				event_data.Lane (lane);
				event_data.Route (-1);

				exe->Output_Event (event_data);
			}
		}
	}
	step.sim_traveler_ptr->Status (0);
	return (true);
}
