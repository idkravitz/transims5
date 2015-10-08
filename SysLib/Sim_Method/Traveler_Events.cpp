//*********************************************************
//	Traveler_Events.cpp - process the traveler events
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Traveler_Events
//---------------------------------------------------------

void Sim_Method::Traveler_Events (void)
{
	Dtime next_event;
	bool remove_flag, transfer_flag, load_flag, macro_flag, on_link;
	int last_part = 0;

	Int_List_Itr list_itr;
	Travel_Step step;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Leg_Itr leg_itr, leg_ahead;
	Vehicle_Data *vehicle_ptr = 0;
	Sim_Park_Ptr sim_park_ptr;

	//---- process the traveler events ----

	for (list_itr = trip_list.begin (); list_itr != trip_list.end (); ) {
		step.Clear ();
		step.Traveler (*list_itr);
		step.sim_traveler_ptr = exe->sim_traveler_array [step.Traveler ()];

		if (!step.sim_traveler_ptr->Active ()) {
			list_itr = trip_list.erase (list_itr);
			continue;
		}
		plan_ptr = step.sim_traveler_ptr->plan_ptr;

		if (!plan_ptr->In_Partition (partition)) {
			transfers->push_back (Int2_Key (step.Traveler (), plan_ptr->Partition ()));
			list_itr = trip_list.erase (list_itr);
			continue;
		}
		step.Vehicle (plan_ptr->Vehicle ());
		step.Partition (plan_ptr->Partition ());

#ifdef DEBUG_KEYS
		exe->debug = (exe->debug_time_flag && (exe->debug_list.In_Range (step.Vehicle ()) || exe->debug_list.In_Range (plan_ptr->Household () - 1)));
		if (exe->debug) MAIN_LOCK exe->Write (1, "Traveler=") << *list_itr << " trip=" << plan_ptr->Trip () << " vehicle=" << step.Vehicle () << " part=" << partition; END_LOCK
#endif
		//---- check the departure time constraint ----

		if (plan_ptr->Next_Event () < 0 && (plan_ptr->Depart () + param.max_start_variance) < exe->step) {
			step.Problem (DEPARTURE_PROBLEM);
			step.Status (2);
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " DEPARTURE_TIME"); END_LOCK
#endif
			Output_Step (step);
			Next_Travel_Plan (step.Traveler ());
			list_itr = trip_list.erase (list_itr);
			continue;
		}
		remove_flag = false;
		last_part = plan_ptr->Partition ();

		//---- check the next scheduled event ----

		while (plan_ptr->Next_Event () <= exe->step) {

			//---- new trip ----

			if (plan_ptr->Next_Event () < 0) {
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " new trip"); END_LOCK
#endif
				plan_ptr->Next_Event (exe->step);

				if (plan_ptr->Person () > 0) {
					stats.num_start++;
					step.sim_traveler_ptr->Status (-1);

					//---- output event record ----

					if (exe->Check_Event (TRIP_START_EVENT, plan_ptr->Mode (), subarea)) {
						Event_Data event_data;

						event_data.Household (plan_ptr->Household ());
						event_data.Person (plan_ptr->Person ());
						event_data.Tour (plan_ptr->Tour ());
						event_data.Trip (plan_ptr->Trip ());
						event_data.Mode (plan_ptr->Mode ());
						event_data.Schedule (plan_ptr->Depart ());
						event_data.Actual (exe->step);
						event_data.Event (TRIP_START_EVENT);

						exe->Output_Event (event_data);
					}
				}
			}

			//---- end of the trip ----

			if (plan_ptr->size () == 0) {
				remove_flag = true;

				if (plan_ptr->Person () == 0) {
					stats.num_run_end++;
					break;
				}
				stats.num_end++;

				//---- output event record ----

				if (exe->Check_Event (TRIP_END_EVENT, plan_ptr->Mode (), subarea)) {
					Event_Data event_data;

					event_data.Household (plan_ptr->Household ());
					event_data.Person (plan_ptr->Person ());
					event_data.Tour (plan_ptr->Tour ());
					event_data.Trip (plan_ptr->Trip ());
					event_data.Mode (plan_ptr->Mode ());
					event_data.Schedule (plan_ptr->Arrive ());
					event_data.Actual (exe->step);
					event_data.Event (TRIP_END_EVENT);

					exe->Output_Event (event_data);
				}
				if (step.sim_traveler_ptr->Status () == 1) {

					stats.num_veh_end++;

					//---- output event record ----

					if (exe->Check_Event (VEH_END_EVENT, plan_ptr->Mode (), subarea)) {
						Event_Data event_data;

						event_data.Household (plan_ptr->Household ());
						event_data.Person (plan_ptr->Person ());
						event_data.Tour (plan_ptr->Tour ());
						event_data.Trip (plan_ptr->Trip ());
						event_data.Mode (plan_ptr->Mode ());
						event_data.Schedule (plan_ptr->Schedule ());
						event_data.Actual (exe->step);
						event_data.Event (VEH_END_EVENT);

						exe->Output_Event (event_data);
					}
				}
				Next_Travel_Plan (step.Traveler ());
				break;
			}

			//---- get the vehicle data ----
			
			if (step.Vehicle () >= 0 && step.sim_veh_ptr == 0) {
				step.sim_veh_ptr = exe->sim_veh_array [step.Vehicle ()];
			}

			//---- check the arrival time constraint ----

			if (plan_ptr->Arrive () + param.max_end_variance < exe->step) {
				step.Problem (ARRIVAL_PROBLEM);
				step.Status (2);
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " ARRIVAL_TIME"); END_LOCK
#endif
				Output_Step (step);
				Next_Travel_Plan (step.Traveler ());
				remove_flag = true;
				break;
			}

			//---- process the next leg ----

			leg_itr = plan_ptr->begin ();
			next_event = exe->step + leg_itr->Time ();

			//---- no simulation move ----

			if (method == NO_SIMULATION) {
				if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Type () == DIR_ID) {
					sim_dir_ptr = &exe->sim_dir_array [leg_itr->Index ()];
					if (sim_dir_ptr->From_Part () != partition) {
						step.sim_traveler_ptr->plan_ptr->Partition (sim_dir_ptr->From_Part ());
						break;
					} else {

						//---- delete the vehicle -----

						if (step.sim_veh_ptr > 0) {
							int index, lane, cell;
							Cell_Itr cell_itr, end_itr;

							on_link = (step.sim_traveler_ptr->Status () >= 0);
							macro_flag = (sim_dir_ptr->Method () == MACROSCOPIC);

							if (!macro_flag) {
								end_itr = step.sim_veh_ptr->end ();

							for (cell_itr = step.sim_veh_ptr->begin (); cell_itr != end_itr; cell_itr++) {
									cell_itr->Location (&index, &lane, &cell);

									if (index >= 0 && lane >= 0) {
										exe->sim_dir_array.Cell (index, lane, cell, -1);
									}
								}
							}
							if (step.sim_veh_ptr->Follower () >= 0) {
								sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Follower ()];
								if (sim_veh_ptr != 0) {
									if (macro_flag) {
										sim_veh_ptr->Leader (step.sim_veh_ptr->Leader ());
									} else {
										sim_veh_ptr->Leader (-1);
										sim_veh_ptr->Next_Event (0);
									}
								}
							} else if (macro_flag && on_link) {
								sim_dir_ptr->Sim_Cap ()->Last_Veh (step.sim_veh_ptr->Leader ());

							}
							if (step.sim_veh_ptr->Leader () >= 0) {
								sim_veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
								if (sim_veh_ptr != 0) {
									if (macro_flag) {
										sim_veh_ptr->Follower (step.sim_veh_ptr->Follower ());
									} else {
										sim_veh_ptr->Follower (-1);
									}
								}
							} else if (macro_flag && on_link) {
								sim_dir_ptr->Sim_Cap ()->First_Veh (step.sim_veh_ptr->Follower ());
							}
							delete step.sim_veh_ptr;
							exe->sim_veh_array [step.Vehicle ()] = step.sim_veh_ptr = 0;
						}
					}
				}

			} else {

				//---- check for vehicle loading ----

				transfer_flag = (leg_itr->Mode () == DRIVE_MODE);

				if (!transfer_flag && (leg_itr->Type () == PARKING_ID || leg_itr->Type () == STOP_ID)) {
					leg_ahead = leg_itr + 1;
					load_flag = (leg_ahead != plan_ptr->end () && leg_ahead->Mode () == DRIVE_MODE && leg_ahead->Type () == DIR_ID);
				} else {
					load_flag = false;
				}
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " transfer=") << transfer_flag << " load=" << load_flag; END_LOCK
#endif
				if (transfer_flag || load_flag) {

					//---- check the status of the vehicle ----

					if (step.Vehicle () < 0) {
						step.Problem (VEHICLE_PROBLEM);
						step.Status (2);
#ifdef DEBUG_KEYS
						if (exe->debug) MAIN_LOCK exe->Write (0, " VEH_PROBLEM"); END_LOCK
#endif
						Output_Step (step);
						Next_Travel_Plan (step.Traveler ());
						remove_flag = true;
						break;
					}

					//---- check the vehicle availability ----

					if (load_flag) {
						vehicle_ptr = &exe->vehicle_array [step.Vehicle ()];

						if (vehicle_ptr->Parking () != leg_itr->Index ()) {
							if (vehicle_ptr->Parking () >= 0) {
								step.Problem (PARKING_PROBLEM);
								step.Parking (leg_itr->Index ());
								step.Status (2);
#ifdef DEBUG_KEYS
								if (exe->debug) MAIN_LOCK exe->Write (0, " PARK_PROBLEM"); END_LOCK
#endif
								Output_Step (step);
								Next_Travel_Plan (step.Traveler ());
								remove_flag = true;
							}
							break;
						} else if (leg_itr->Type () == PARKING_ID) {
							sim_park_ptr = &exe->sim_park_array [vehicle_ptr->Parking ()];
							sim_dir_ptr = &exe->sim_dir_array [leg_ahead->Index ()];

							last_part = (sim_dir_ptr->Dir () == 0) ? sim_park_ptr->Part_AB () : sim_park_ptr->Part_BA ();

							if (last_part != partition) {
								plan_ptr->Partition (last_part);
								break;
							}
						}
					}

					//---- assign the vehicle to the driver ----

					if (exe->sim_veh_array [step.Vehicle ()] == 0) {
						if (transfer_flag) {
							vehicle_ptr = &exe->vehicle_array [step.Vehicle ()];
						}
						step.veh_type_ptr = &exe->sim_type_array [vehicle_ptr->Type ()];

						step.sim_veh_ptr = new Sim_Veh_Data (step.veh_type_ptr->Length ());
						step.sim_veh_ptr->Type (vehicle_ptr->Type ());
						step.sim_veh_ptr->Driver (step.Traveler ());
#ifdef DEBUG_KEYS
						if (exe->debug) MAIN_LOCK exe->Write (0, " new vehicle"); END_LOCK
#endif
						exe->sim_veh_array [step.Vehicle ()] = step.sim_veh_ptr;

						if (step.sim_traveler_ptr->Status () == -1) {

							//---- set the path following lanes ----

							if (!Best_Lanes (step)) {
								step.Problem (CONNECT_PROBLEM);
								step.Status (2);

								Output_Step (step);
								Next_Travel_Plan (step.Traveler ());
								remove_flag = true;
								break;
							}

							if (transfer_flag) {
								step.sim_veh_ptr->Next_Event (exe->step);
								step.sim_traveler_ptr->Status (-3);
								if (last_part != partition) break;
							} else {
								step.sim_traveler_ptr->Status (-2);

								if (last_part != partition) {
									step.sim_veh_ptr->Next_Event (exe->step);
									break;
								}
							}
						} else if (transfer_flag) {
							step.sim_veh_ptr->Next_Event (exe->step);
							step.sim_traveler_ptr->Status (-4);
						}
					}
					if (load_flag) {
						step.sim_veh_ptr->Next_Event (next_event);
						step.sim_traveler_ptr->Status (-2);
						plan_ptr->Next_Leg ();
					} else {

						//---- check boundary conditions ----

						sim_dir_ptr = &exe->sim_dir_array [leg_itr->Index ()];
						if (sim_dir_ptr->Boundary ()) {
							int split = sim_dir_ptr->Split_Cell ();
							int cell = step.sim_veh_ptr->front ().Cell ();

							if (cell >= split && (cell - split) < step.sim_veh_ptr->Num_Cells ()) {
								first_list.push_back (step.Vehicle ());
							}
						}
					}

					//---- add the traveler to the end of the vehicle queue ----
#ifdef DEBUG_KEYS
					if (exe->debug) MAIN_LOCK exe->Write (0, " veh_list=") << step.Vehicle () << " veh_status=" << (int) exe->veh_status [step.Vehicle ()] << " part=" << partition; END_LOCK
#endif
					remove_flag = true;
					veh_list.push_back (step.Vehicle ());
					break;
				}
			}
			plan_ptr->Next_Event (next_event);
			plan_ptr->Next_Leg ();
			last_part = plan_ptr->Partition ();
		}

		if (remove_flag || !step.sim_traveler_ptr->Active ()) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (1, "Remove Traveler=") << step.Traveler () << " part=" << partition; END_LOCK
#endif
			list_itr = trip_list.erase (list_itr);
		} else {
			if (partition == plan_ptr->Partition ()) {
				list_itr++;
			} else {
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (1, "TRANSFER=") << step.Traveler () << " from=" << partition << " to=" << plan_ptr->Partition (); END_LOCK
#endif
				transfers->push_back (Int2_Key (step.Traveler (), plan_ptr->Partition ()));
				list_itr = trip_list.erase (list_itr);
			}
		}
	}
}
