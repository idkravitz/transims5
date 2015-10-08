//*********************************************************
//	Read_Plans.cpp - read the travel plan file
//*********************************************************

#include "PlanSelect.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanSelect::Plan_Processing::Read_Plans (int part)
{
	int mode, index, dir_index, flow_index, rec, cap, lanes, lane, group;
	double percent, ratio, flow_factor;
	Dtime time, skim, ttime, diff;
	bool vc_flag, ratio_flag, fac_flag, park_flag, subarea_flag;

	Plan_Data plan;
	Plan_Leg_Itr leg_itr;
	Flow_Time_Array *link_delay_ptr, *turn_delay_ptr;
	Flow_Time_Data *flow_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Trip_Index trip_index;
	Select_Data select_rec;
	Select_Map_Stat map_stat;
	Location_Data *loc_ptr;
	Dir_Data *dir_ptr;
	Lane_Use_Period *period_ptr;
	Node_Data *node_ptr;

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
	} else {
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d -- Record") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s -- Record") % plan_file.File_Type ());
		}
		exe->Set_Progress ();
	}

	if (exe->select_vc) {
		time = exe->time_periods.Increment ();
		if (time < 1) time = 1;
		flow_factor = (double) Dtime (60, MINUTES) / time;
	} else {
		flow_factor = 1.0;
	}

	//---- read the plan file ----

	while (plan_file.Read_Plan (plan)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		num_trips++;

		//---- check the selection criteria ----

		mode = plan.Mode ();

		if (exe->select_households && !exe->hhold_range.In_Range (plan.Household ())) continue;
		if (mode >= 0 && mode < MAX_MODE && !exe->select_mode [mode]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan.Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan.Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (plan.Depart ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (plan.Arrive ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (plan.Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (plan.Destination ())) continue;

		if (exe->select_org_zones) {
			map_itr = exe->location_map.find (plan.Origin ());
			if (map_itr != exe->location_map.end ()) {
				loc_ptr = &exe->location_array [map_itr->second];
				if (!exe->org_zone_range.In_Range (loc_ptr->Zone ())) continue;
			}
		}
		if (exe->select_des_zones) {
			map_itr = exe->location_map.find (plan.Destination ());
			if (map_itr != exe->location_map.end ()) {
				loc_ptr = &exe->location_array [map_itr->second];
				if (!exe->des_zone_range.In_Range (loc_ptr->Zone ())) continue;
			}
		}

		if (exe->select_links) {
			if (!exe->Select_Plan_Links (plan)) continue;
		}
		if (exe->select_nodes) {
			if (!exe->Select_Plan_Nodes (plan)) continue;
		}
		
		//---- check the deletion records ----
		
		plan.Get_Trip_Index (trip_index);

		if (exe->delete_flag && exe->delete_map.Best (trip_index) != exe->delete_map.end ()) continue;
		if (exe->delete_households && exe->hhold_delete.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && exe->delete_mode [plan.Mode ()]) continue;
		if (exe->delete_travelers && exe->traveler_delete.In_Range (plan.Type ())) continue;

		//---- path-based criteria ----

		if (exe->time_diff_flag || exe->select_vc || exe->select_facilities || exe->select_parking || exe->select_ratio || exe->select_subarea) {
			if (mode == WAIT_MODE || mode == WALK_MODE || mode == BIKE_MODE || 
				mode == TRANSIT_MODE || mode == OTHER_MODE) continue;

			time = skim = plan.Start ();
			dir_index = -1;
			vc_flag = ratio_flag = fac_flag = park_flag = subarea_flag = false;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
				ttime = leg_itr->Time ();

				//---- check the parking lot ----

				if (exe->select_parking && park_flag == false) {
					if (leg_itr->Type () == PARKING_ID) {
						if (exe->parking_range.In_Range (leg_itr->ID ())) {
							park_flag = true;
						}
					}
				}

				//---- check the drive link ----

				if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Link_Type ()) {

					link_delay_ptr = exe->link_delay_array.Period_Ptr (skim);

					if (link_delay_ptr > 0) {
						index = leg_itr->Link_ID ();
						map_itr = exe->link_map.find (index);

						if (map_itr != exe->link_map.end ()) {
							link_ptr = &exe->link_array [map_itr->second];

							//---- check the facility type ----

							if (exe->select_facilities && !fac_flag) {
								if (exe->select_facility [link_ptr->Type ()]) {
									fac_flag = true;
								}
							}

							//---- get the flow and time data ----

							group = 0;

							if (leg_itr->ID () < 0 || leg_itr->Type () == LINK_BA || leg_itr->Type () == USE_BA) {
								flow_index = index = link_ptr->BA_Dir ();
								if (leg_itr->Type () == USE_BA && exe->Lane_Use_Flows ()) {
									flow_index = exe->dir_array [index].Flow_Index ();
									if (flow_index >= 0) {
										group = 1;
									} else {
										flow_index = index;
									}
								}
							} else {
								flow_index = index = link_ptr->AB_Dir ();
								if (leg_itr->Type () == USE_AB && exe->Lane_Use_Flows ()) {
									flow_index = exe->dir_array [index].Flow_Index ();
									if (flow_index >= 0) {
										group = 1;
									} else {
										flow_index = index;
									}
								}
							}
							flow_ptr = link_delay_ptr->Data_Ptr (flow_index);
							ttime = flow_ptr->Time ();

							//---- check the vc ratio ----

							if (exe->select_vc) {
								dir_ptr = &exe->dir_array [index];
								cap = dir_ptr->Capacity ();
								lanes = dir_ptr->Lanes ();

								rec = dir_ptr->First_Lane_Use ();
								if (rec >= 0) {
									for (period_ptr = &exe->use_period_array [rec]; ; period_ptr = &exe->use_period_array [++rec]) {
										if (period_ptr->Start () <= skim && skim < period_ptr->End ()) {
											lane = period_ptr->Lanes (group);
											cap = (cap * lane + lanes / 2) / lanes;
											break;
										}
										if (period_ptr->Periods () == 0) break;
									}
								}
								ratio = flow_factor * flow_ptr->Flow () / cap;
								if (ratio >= exe->vc_ratio) {
									vc_flag = true;
								}
							}

							//---- check the time ratio ----

							if (exe->select_ratio) {
								dir_ptr = &exe->dir_array [index];

								ratio = (double) ttime / dir_ptr->Time0 ();

								if (ratio >= exe->time_ratio) {
									ratio_flag = true;
								}
							}

							//---- check the subarea polygon ----

							if (exe->select_subarea) {
								node_ptr = &exe->node_array [link_ptr->Anode ()];

								if (In_Polygon (exe->subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
									subarea_flag = true;
								} else {
									node_ptr = &exe->node_array [link_ptr->Bnode ()];

									if (In_Polygon (exe->subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
										subarea_flag = true;
									}
								}
							}

							//---- add the turning movement delay ----

							if (exe->turn_flag && dir_index >= 0) {
								map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));

								if (map2_itr != exe->connect_map.end ()) {
									turn_delay_ptr = exe->turn_delay_array.Period_Ptr (skim);
									ttime += turn_delay_ptr->Time (map2_itr->second);
									if (ttime < 1) ttime = 1;
								}
							}
							dir_index = index;
						}
					}
				}
				time += leg_itr->Time ();
				skim += ttime;
			}

			//---- check the selection criteria ----

			if (exe->select_facilities && !fac_flag) continue;
			if (exe->select_parking && !park_flag) continue;
			if (exe->select_vc && !vc_flag) continue;
			if (exe->select_ratio && !ratio_flag) continue;
			if (exe->select_subarea && !subarea_flag) continue;

			if (exe->time_diff_flag) {
				diff = abs (time - skim);
				if (diff < exe->min_time_diff) continue;

				if (diff < exe->max_time_diff) {
					if (time > 0) {
						percent = diff / time;
					} else {
						percent = 1.0;
					}
					if (percent < exe->percent_time_diff) continue;
				}
			}
		}

		//---- save the selection data ----

		select_rec.Type (0);
		select_rec.Partition (plan_file.Part_Number ());

		if (thread_flag) {
			map_stat = select_map.insert (Select_Map_Data (trip_index, select_rec));
		} else {
			map_stat = exe->select_map.insert (Select_Map_Data (trip_index, select_rec));
		}
		if (!map_stat.second) {
			MAIN_LOCK
			exe->Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
				trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
			END_LOCK
		}
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file.Close ();
}
