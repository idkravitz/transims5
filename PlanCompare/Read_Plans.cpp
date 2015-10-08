//*********************************************************
//	Read_Plans.cpp - read the travel plan file
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanCompare::Plan_Processing::Read_Plans (int part)
{
	double percent;
	int time1, time2, cost1, cost2, diff;
	Dtime tod;

	Select_Map_Stat map_stat;
	Select_Data select_rec;
	Plan_Data plan, compare_plan;
	Trip_Index trip_index, last_trip_index, trip_rec, last_trip_rec;
	Time_Index time_index, last_time_index, time_rec, last_time_rec;

	//---- open the file partition ----

	if (!plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % plan_file.Filename ()); 
		END_LOCK
	}
	if (!compare_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % compare_file.Filename ()); 
		END_LOCK
	}

	//---- write the process message ----

	if (thread_flag) {
		MAIN_LOCK
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Comparing %s %d") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Comparing %s") % plan_file.File_Type ());
		}
		END_LOCK
	} else {
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Comparing %s %d -- Record") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Comparing %s -- Record") % plan_file.File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- initialize the compare index ----

	if (compare_file.Read_Plan (compare_plan)) {
		if (time_sort) {
			compare_plan.Get_Time_Index (time_index);
		} else {
			compare_plan.Get_Trip_Index (trip_index);
		}
		num_compare++;
	} else if (time_sort) {
		time_index.Start (MAX_INTEGER);
	} else {
		trip_index.Household (MAX_INTEGER);
	}
	if (time_sort) {
		last_time_index = time_index;
		time_rec.Clear ();
	} else {
		last_trip_index = trip_index;
		trip_rec.Clear ();
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

		if (exe->select_households && !exe->hhold_range.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && !exe->select_mode [plan.Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan.Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan.Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (plan.Depart ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (plan.Arrive ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (plan.Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (plan.Destination ())) continue;
		if (exe->select_links && !exe->Select_Plan_Links (plan)) continue;
		if (exe->select_nodes && !exe->Select_Plan_Nodes (plan)) continue;
		if (exe->select_subarea && !exe->Select_Plan_Subarea (plan)) continue;

		//---- check the selection records ----

		plan.Get_Trip_Index (trip_rec);

		if (exe->select_flag && exe->select_map.Best (trip_rec) == exe->select_map.end ()) continue;

		//---- check the deletion records ----

		if (exe->delete_flag && exe->delete_map.Best (trip_rec) != exe->delete_map.end ()) continue;
		if (exe->delete_households && exe->hhold_delete.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && exe->delete_mode [plan.Mode ()]) continue;
		if (exe->delete_travelers && exe->traveler_delete.In_Range (plan.Type ())) continue;

		//---- check the plan sort ----

		if (time_sort) {
			plan.Get_Time_Index (time_rec);

			if (time_rec < last_time_rec) {
				MAIN_LOCK
				exe->Error ("Plan File is Not Time Sorted");
				END_LOCK
			}
			last_time_rec = time_rec;
		} else {
			if (trip_rec < last_trip_rec) {
				MAIN_LOCK
				exe->Error ("Plan File is Not Traveler Sorted");
				END_LOCK
			}
			last_trip_rec = trip_rec;
		}

		//---- compare the sort indices ----

		while ((time_sort && time_index < time_rec) || (!time_sort && trip_index < trip_rec)) {
			if (compare_file.Read_Plan (compare_plan)) {
				if (time_sort) {
					compare_plan.Get_Time_Index (time_index);

					if (time_index < last_time_index) {
						MAIN_LOCK
						exe->Error ("Compare Plan File is Not Time Sorted");
						END_LOCK
					}
					last_time_index = time_index;
				} else {
					compare_plan.Get_Trip_Index (trip_index);

					if (trip_index < last_trip_index) {
						MAIN_LOCK
						exe->Error ("Compare Plan File is Not Traveler Sorted");
						END_LOCK
					}
					last_trip_index = trip_index;
				}
				num_compare++;
			} else if (time_sort) {
				time_index.Start (MAX_INTEGER);
				break;
			} else {
				trip_index.Household (MAX_INTEGER);
				break;
			}
		}
		if (time_sort) {
			if (time_index != time_rec) continue;
		} else {
			if (trip_index != trip_rec) continue;
		}
		if (exe->match_flag) {
			select_rec.Type (plan_file.Type ());
			select_rec.Partition (plan_file.Part_Number ());

			if (thread_flag) {
				matched.insert (Select_Map_Data (trip_rec, select_rec));
			} else {
				exe->matched.insert (Select_Map_Data (trip_rec, select_rec));
			}
		}
		time1 = plan.Arrive () - plan.Depart ();
		time2 = compare_plan.Arrive () - compare_plan.Depart ();

		if (exe->sum_periods.Period_Control_Point () == MID_TRIP) {
			tod = (compare_plan.Arrive () + compare_plan.Depart ()) >> 1;
		} else if (exe->sum_periods.Period_Control_Point () == TRIP_START) {
			tod = compare_plan.Depart ();
		} else {
			tod = compare_plan.Arrive ();
		}
		cost1 = plan.Impedance ();
		cost2 = compare_plan.Impedance ();

		//---- save report data ----

		if (exe->time_sum_flag) {
			if (thread_flag) {
				time_diff.Add_Trip (tod, time1, time2);
			} else {
				exe->time_diff.Add_Trip (tod, time1, time2);
			}
		}
		if (exe->cost_sum_flag) {
			if (thread_flag) {
				cost_diff.Add_Trip (tod, cost1, cost2);
			} else {
				exe->cost_diff.Add_Trip (tod, cost1, cost2);
			}
		}

		//---- save gap data ----

		if (exe->time_gap_flag) {
			if (thread_flag) {
				time_gap.Add_Trip_Gap_Data (tod, time1, time2);
			} else {
				exe->time_gap.Add_Trip_Gap_Data (tod, time1, time2);
			}
		}
		if (exe->cost_gap_flag) {
			if (thread_flag) {
				cost_gap.Add_Trip_Gap_Data (tod, cost1, cost2);
			} else {
				exe->cost_gap.Add_Trip_Gap_Data (tod, cost1, cost2);
			}
		}

		//---- compare plan times ----

		if (exe->time_diff_flag) {
			diff = abs (time1 - time2);
			if (diff < exe->min_time_diff) continue;
			percent = 1.0;

			if (diff < exe->max_time_diff && time2 > 0) {
				percent = (double) diff / time2;
				if (percent < exe->percent_time_diff) continue;
			}
		}

		//---- compare plan costs ----

		if (exe->cost_diff_flag) {
			diff = abs (cost1 - cost2);
			if (diff < exe->min_cost_diff) continue;
			percent = 1.0;

			if (diff < exe->max_cost_diff && cost2 > 0) {
				percent = (double) diff / cost2;
				if (percent < exe->percent_cost_diff) continue;
			}
		}

		//---- selection difference ----

		if (exe->cost_flag) {

			//---- compare generalized costs ----
			
			diff = abs (cost1 - cost2);
			if (cost2 > 0) {
				percent = (double) diff / cost2;
			} else {
				percent = 1.0;
			}
		} else {

			//---- compare travel times ----

			diff = abs (time1 - time2);
			if (time2 > 0) {
				percent = (double) diff / time2;
			} else {
				percent = 1.0;
			}
		}

		//---- limit to unsigned short ----

		if (percent > 6.55) percent = 6.55;

		select_rec.Type (DTOI (percent * 10000.0));

		//---- mark the selected plan ----

		select_rec.Partition (plan_file.Part_Number ());

		if (thread_flag) {
			map_stat = selected.insert (Select_Map_Data (trip_rec, select_rec));
		} else {
			map_stat = exe->selected.insert (Select_Map_Data (trip_rec, select_rec));
		}
		if (!map_stat.second) {
			MAIN_LOCK
			exe->Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
				trip_rec.Household () % trip_rec.Person () % trip_rec.Tour () % trip_rec.Trip ());
			END_LOCK
		}
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file.Close ();
	compare_file.Close ();
}
