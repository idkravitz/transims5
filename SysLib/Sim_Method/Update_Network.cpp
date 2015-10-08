//*********************************************************
//	Update_Network.cpp - update lane use restrictions
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Update_Network
//---------------------------------------------------------

void Sim_Method::Update_Network (void)
{
	int i, dir, index, lanes, min_lane, max_lane, capacity, first [20], num;
	bool use_update_flag, turn_update_flag;
	
	Sim_Dir_Itr sim_dir_itr;
	Sim_Lane_Ptr sim_lane_ptr;
	Sim_Cap_Ptr sim_cap_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;
	Turn_Pen_Data *turn_ptr;

	use_update_flag = (use_update_time <= exe->step);
	if (use_update_flag) use_update_time = MAX_INTEGER;

	turn_update_flag = (turn_update_time <= exe->step);
	if (turn_update_flag) turn_update_time = MAX_INTEGER;

	//---- initialize link dir data ----

	for (dir=0, sim_dir_itr = exe->sim_dir_array.begin (); sim_dir_itr != exe->sim_dir_array.end (); sim_dir_itr++, dir++) {
		if (sim_dir_itr->To_Part () != partition) continue;

		dir_ptr = &exe->dir_array [dir];

		if (use_update_flag && dir_ptr->First_Lane_Use () >= 0) {

			min_lane = dir_ptr->Left ();
			max_lane = dir_ptr->Lanes () + min_lane - 1;

			//---- link data by subarea method ----

			if (sim_dir_itr->Method () == MACROSCOPIC) {
				sim_cap_ptr = sim_dir_itr->Sim_Cap ();

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

						if (use_ptr->Low_Lane () <= min_lane &&	use_ptr->High_Lane () >= max_lane) {

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

				//---- reset the lane use codes ----

				lanes = sim_dir_itr->Lanes ();

				for (i=0; i < lanes; i++) {
					sim_lane_ptr = sim_dir_itr->Lane (i);
					sim_lane_ptr->Type (LIMIT);
					sim_lane_ptr->Use (link_ptr->Use ());
					sim_lane_ptr->Min_Veh_Type (-1);
					sim_lane_ptr->Max_Veh_Type (0);
					sim_lane_ptr->Min_Traveler (0);
					sim_lane_ptr->Max_Traveler (0);
					sim_lane_ptr->First_Use (-1);
				}

				//---- set the lane use restrictions ----

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
		}
exit_use:
		//---- set the turn prohibition flag ----

		if (turn_update_flag && dir_ptr->First_Turn () >= 0) {
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
	}
}
