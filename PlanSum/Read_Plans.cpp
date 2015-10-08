//*********************************************************
//	Read_Plans.cpp - read the travel plan file
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanSum::Plan_Processing::Read_Plans (int part)
{
	int mode, period, index, previous_index, flow_index, high_mode;
	Dtime time, time2;
	double flow;
	bool drive_flag, transit_flag;

	Plan_Data plan;
	Plan_Leg_Itr leg_itr;
	Flow_Time_Array *flow_time_ptr;
	Flow_Time_Data *flow_ptr;
	Link_Data *link_ptr;
	Connect_Data *connect_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Select_Map_Itr sel_itr;
	Flow_Time_Period_Array  *link_flow_ptr, *turn_flow_ptr;
	Trip_Sum_Data *trip_sum_ptr;
	Trip_Sum_Data *pass_sum_ptr;
	Transfer_Array *xfer_ptr;
	Transfer_Data *transfer_ptr;
	Line_Data *line_ptr;
	Ints_Array *total_on_ptr;
	Board_Array *walk_on_ptr;
	Board_Array *drive_on_ptr;

	//---- open the file partition ----

	if (!plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % plan_file.Filename ()); 
		END_LOCK
	}
	if (thread_flag) {
		MAIN_LOCK
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s") % plan_file.File_Type ());
		}
		END_LOCK

		link_flow_ptr = &link_delay_array;
		turn_flow_ptr = &turn_delay_array;
		trip_sum_ptr = &trip_sum_data;
		pass_sum_ptr = &pass_sum_data;
		xfer_ptr = &transfer_array;
		total_on_ptr = &total_on_array;
		walk_on_ptr = &walk_on_array;
		drive_on_ptr = &drive_on_array;
	} else {
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d -- Record") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s -- Record") % plan_file.File_Type ());
		}
		exe->Set_Progress ();

		link_flow_ptr = &exe->link_delay_array;
		turn_flow_ptr = &exe->turn_delay_array;
		trip_sum_ptr = &exe->trip_sum_data;
		pass_sum_ptr = &exe->pass_sum_data;
		xfer_ptr = &exe->transfer_array;
		total_on_ptr = &exe->total_on_array;
		walk_on_ptr = &exe->walk_on_array;
		drive_on_ptr = &exe->drive_on_array;
	}

	//---- read the plan file ----

	while (plan_file.Read_Plan (plan)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}

		//---- check the selection criteria ----

		if (exe->select_households && !exe->hhold_range.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && !exe->select_mode [plan.Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan.Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan.Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (plan.Depart ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (plan.Arrive ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (plan.Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (plan.Destination ())) continue;
		if (exe->select_stops && !exe->Select_Plan_Stops (plan)) continue;
		if (exe->select_routes && !exe->Select_Plan_Routes (plan)) continue;
		if (exe->select_links && !exe->Select_Plan_Links (plan)) continue;
		if (exe->select_nodes && !exe->Select_Plan_Nodes (plan)) continue;
		if (exe->select_subarea && !exe->Select_Plan_Subarea (plan)) continue;
		
		//---- check the selection records ----

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (plan.Household (), plan.Person (), 
				plan.Tour (), plan.Trip ());
			if (sel_itr == exe->select_map.end ()) continue;
		}
		if (exe->percent_flag && exe->random.Probability () > exe->select_percent) continue;

		//---- save the trip times ----

		if (exe->time_flag) {
			period = exe->sum_periods.Period (plan.Depart ());
			if (period >= 0) {
				if (thread_flag) {
					start_time [period]++;
				} else {
					exe->start_time [period]++;
				}
			}
			period = exe->sum_periods.Period (plan.Arrive ());
			if (period >= 0) {
				if (thread_flag) {
					end_time [period]++;
				} else {
					exe->end_time [period]++;
				}
			}
			period = exe->sum_periods.Period ((plan.Depart () + plan.Arrive ()) / 2);
			if (period >= 0) {
				if (thread_flag) {
					mid_time [period]++;
				} else {
					exe->mid_time [period]++;
				}
			}
		}

		//---- check the trip mode ----

		mode = plan.Mode ();

		drive_flag = (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
					mode != TRANSIT_MODE && mode != OTHER_MODE);

		transit_flag = (mode == TRANSIT_MODE || mode == PNR_OUT_MODE || mode == PNR_IN_MODE ||
			mode == KNR_OUT_MODE || mode == KNR_IN_MODE);

		//---- save the trip lengths ----

		if (exe->travel_flag && drive_flag) {
			if (exe->sum_periods.Period_Control_Point () == MID_TRIP) {
				time = (plan.Depart () + plan.Arrive ()) >> 1;
			} else if (exe->sum_periods.Period_Control_Point () == TRIP_START) {
				time = plan.Depart ();
			} else {
				time = plan.Arrive ();
			}
			period = exe->sum_periods.Period (time);

			if (period >= 0) {
				int turns = 0;
				previous_index = -1;

				for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
					if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Link_Type ()) {

						index = leg_itr->Link_ID ();
						map_itr = exe->link_map.find (index);

						if (map_itr != exe->link_map.end ()) {
							link_ptr = &exe->link_array [map_itr->second];

							if (leg_itr->Link_Dir () == 1) {
								index = link_ptr->BA_Dir ();
							} else {
								index = link_ptr->AB_Dir ();
							}
							if (previous_index >= 0) {
								map2_itr = exe->connect_map.find (Int2_Key (previous_index, index));

								if (map2_itr != exe->connect_map.end ()) {
									connect_ptr = &exe->connect_array [map2_itr->second];
									if (connect_ptr->Type () == LEFT || connect_ptr->Type () == RIGHT || connect_ptr->Type () == UTURN) {
										turns++;
									}
								}
							}
							previous_index = index;
						}
					}
				}

				//---- save the length data ----

				trip_sum_ptr->Add_Trip (period, plan.Length (), plan.Total_Time (), turns);
			}
		}

		//---- save the transit passenger data ----

		if (exe->passenger_flag && transit_flag) {
			if (exe->sum_periods.Period_Control_Point () == MID_TRIP) {
				time = (plan.Depart () + plan.Arrive ()) >> 1;
			} else if (exe->sum_periods.Period_Control_Point () == TRIP_START) {
				time = plan.Depart ();
			} else {
				time = plan.Arrive ();
			}
			period = exe->sum_periods.Period (time);

			if (period >= 0) {
				double sum_len, sum_time;
				int transfers = -1;
				sum_len = sum_time = 0;

				for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
					if (leg_itr->Mode () == TRANSIT_MODE) {
						transfers++;
						sum_len += UnRound (leg_itr->Length ());
						sum_time += leg_itr->Time ().Seconds ();
					}
				}
				if (transfers >= 0) {
					pass_sum_ptr->Add_Trip (period, sum_len, sum_time, transfers);
				}
			}
		}

		//---- trace the path ----

		if (exe->new_delay_flag && drive_flag) {

			time = plan.Depart ();
			previous_index = -1;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
				if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Link_Type ()) {

					period = exe->time_periods.Period (time);

					if (period >= 0) {
						index = leg_itr->Link_ID ();
						map_itr = exe->link_map.find (index);

						if (map_itr != exe->link_map.end ()) {
							link_ptr = &exe->link_array [map_itr->second];

							//---- get the flow and time data ----

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
							if (index < 0) {
								exe->Warning (String ("Traveler=%d-%d-%d-%d Link=%d was Not Found") % plan.Household () % plan.Person () % plan.Tour () % plan.Trip () % leg_itr->ID ());
								break;
							}
							flow_time_ptr = &link_flow_ptr->at (period);
							flow_ptr = &flow_time_ptr->at (flow_index);

							if (leg_itr->Length () >= link_ptr->Length ()) {
								flow = 1.0;
							} else {
								flow = (double) leg_itr->Length () / link_ptr->Length ();
								if (flow < 0.01) flow = 0.01;
							}
							flow_ptr->Add_Flow (flow);

							if (turn_flag) {
								if (previous_index >= 0) {
									map2_itr = exe->connect_map.find (Int2_Key (previous_index, index));

									if (map2_itr != exe->connect_map.end ()) {
										flow_time_ptr = &turn_flow_ptr->at (period);
										flow_ptr = &flow_time_ptr->at (map2_itr->second);
										flow_ptr->Add_Flow (1.0);
									}
								}
								previous_index = index;
							}
						}
					}
				}
				time += leg_itr->Time ();
			}
		}

		//---- transit transfers ----
		
		if (exe->transfer_flag && transit_flag) {
			previous_index = -1;
			high_mode = 0;

			for (leg_itr = plan.begin (); ; leg_itr++) {
				if (leg_itr == plan.end ()) {
					if (previous_index >= 0) {
						transfer_ptr = &xfer_ptr->at (previous_index);

						if (high_mode == DRIVE_MODE) {
							transfer_ptr->Add_Drive_Alight ();
						} else {
							transfer_ptr->Add_Walk_Alight ();
						}
					}
					break;
				} else if (leg_itr->Type () == STOP_ID) {
					if (previous_index >= 0 && leg_itr->Mode () != TRANSIT_MODE) {
						transfer_ptr = &xfer_ptr->at (previous_index);

						switch (high_mode) {
							case WALK_MODE:
								transfer_ptr->Add_Walk_Alight ();
								break;
							case DRIVE_MODE:
								transfer_ptr->Add_Drive_Alight ();
								break;
							case TRANSIT_MODE:
								transfer_ptr->Add_Bus_Alight ();
								break;
							case OTHER_MODE:
							default:
								transfer_ptr->Add_Rail_Alight ();
								break;
						}
					}
					map_itr = exe->stop_map.find (leg_itr->ID ());
					if (map_itr != exe->stop_map.end ()) {

						if (leg_itr->Mode () == TRANSIT_MODE) {
							previous_index = map_itr->second;
						} else {
							transfer_ptr = &xfer_ptr->at (map_itr->second);

							switch (high_mode) {
								case WALK_MODE:
									transfer_ptr->Add_Walk_Board ();
									break;
								case DRIVE_MODE:
									transfer_ptr->Add_Drive_Board ();
									break;
								case TRANSIT_MODE:
									transfer_ptr->Add_Bus_Board ();
									break;
								case OTHER_MODE:
								default:
									transfer_ptr->Add_Rail_Board ();
									break;
							}
						}
					}
				} else if (leg_itr->Type () == ROUTE_ID) {
					map_itr = exe->line_map.find (leg_itr->ID ());
					if (map_itr != exe->line_map.end ()) {
						line_ptr = &exe->line_array [map_itr->second];

						if (line_ptr->Type () == LOCAL_BUS || line_ptr->Type () == EXPRESS_BUS) {
							high_mode = TRANSIT_MODE;
						} else {
							high_mode = OTHER_MODE;
						}
					}
				} else if (leg_itr->Mode () == WALK_MODE || leg_itr->Mode () == BIKE_MODE) {
					if (high_mode == 0) high_mode = WALK_MODE;
				} else {
					high_mode = DRIVE_MODE;
				}
			}
		}

		//---- transit boardings ----
		
		if ((exe->xfer_flag || exe->xfer_detail) && transit_flag) {
			int bus_on, rail_on;
			bus_on = rail_on = 0;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
				if (leg_itr->Type () == ROUTE_ID) {
					map_itr = exe->line_map.find (leg_itr->ID ());
					if (map_itr != exe->line_map.end ()) {
						line_ptr = &exe->line_array [map_itr->second];

						if (line_ptr->Type () == LOCAL_BUS || line_ptr->Type () == EXPRESS_BUS) {
							bus_on++;
						} else {
							rail_on++;
						}
					}
				}
			}
			if (exe->xfer_flag) {
				(*total_on_ptr) [bus_on] [rail_on]++;
			}
			if (exe->xfer_detail) {
				int period;
				
				if (exe->sum_periods.Num_Periods () > 0) {
					period = exe->sum_periods.Period (plan.Depart ());
				} else {
					period = 0;
				}
				if (period >= 0) {
					if (mode == TRANSIT_MODE) {
						(*walk_on_ptr) [period] [bus_on] [rail_on]++;
					} else {
						(*drive_on_ptr) [period] [bus_on] [rail_on]++;
					}
				}
			}
		}

		//---- transit ridership data ----

		if (exe->rider_flag && transit_flag) {
			int board, alight, route, run;
			Line_Stop_Itr stop_itr, board_itr, alight_itr;
			Line_Run_Itr run_itr;
			Line_Run *run_ptr;

			board = alight = route = -1;

			time = plan.Depart ();
			time2 = 0;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++, time += time2) {
				time2 = leg_itr->Time ();

				if (leg_itr->Type () == STOP_ID) {
					if (leg_itr->Mode () == TRANSIT_MODE) {
						alight = leg_itr->ID ();

						map_itr = exe->stop_map.find (alight);
						if (map_itr == exe->stop_map.end ()) continue;

						alight = map_itr->second;

						map_itr = exe->stop_map.find (board);
						if (map_itr == exe->stop_map.end ()) continue;

						board = map_itr->second;

						map_itr = exe->line_map.find (route);
						if (map_itr == exe->line_map.end ()) continue;

						line_ptr = &exe->line_array [map_itr->second];

						//---- find the boarding and alighting locations ----

						for (board_itr = line_ptr->begin (); board_itr != line_ptr->end (); board_itr++) {
							if (board_itr->Stop () == board) break;
						}
						if (board_itr == line_ptr->end ()) continue;

						for (alight_itr = board_itr; alight_itr != line_ptr->end (); alight_itr++) {
							if (alight_itr->Stop () == alight) break;
							if (alight_itr->Stop () == board) {
								board_itr = alight_itr;
							}
						}
						if (alight_itr == line_ptr->end ()) continue;

						//---- find the run number ----

						for (run=0, run_itr = board_itr->begin (); run_itr != board_itr->end (); run_itr++, run++) {
							if (time <= run_itr->Schedule ()) break;
						}
						if (run_itr == board_itr->end ()) continue;

						//---- load the trip ----

						for (stop_itr = board_itr; stop_itr <= alight_itr; stop_itr++) {
							run_ptr = &stop_itr->at (run);
							if (stop_itr == board_itr) {
								run_ptr->Add_Board ();
							}
							if (stop_itr == alight_itr) {
								run_ptr->Add_Alight ();
							} else {
								run_ptr->Add_Load ();
							}
						}
						board = alight = route = -1;
					} else {
						board = leg_itr->ID ();
					}
				} else if (leg_itr->Type () == ROUTE_ID) {
					route = leg_itr->ID ();
				}
			}
		}
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file.Close ();
}
