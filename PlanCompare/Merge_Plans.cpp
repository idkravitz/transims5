//*********************************************************
//	Merge_Plans.cpp - merge travel plan files
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Merge_Plans
//---------------------------------------------------------

void PlanCompare::Plan_Output::Merge_Plans (int part)
{
	Plan_Data plan, compare_plan;
	Trip_Index trip_index, trip_rec;
	Time_Index time_index, time_rec;
	Select_Map_Itr sel_itr;

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
	if (!new_plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % new_plan_file.Filename ()); 
		END_LOCK
	}

	//---- write the process message ----

	if (thread_flag) {
		MAIN_LOCK
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Merging %s %d") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Merging %s") % plan_file.File_Type ());
		}
		END_LOCK
	} else {
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("Merging %s %d -- Record") % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("Merging %s -- Record") % plan_file.File_Type ());
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
	} else if (time_sort) {
		time_index.Start (MAX_INTEGER);
	} else {
		trip_index.Household (MAX_INTEGER);
	}
	time_rec.Clear ();
	trip_rec.Clear ();

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
		
		//---- check the selection records ----

		plan.Get_Trip_Index (trip_rec);

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (trip_rec);
			if (sel_itr == exe->select_map.end ()) continue;
		}
		
		//---- check the deletion records ----

		if (exe->delete_flag) {
			sel_itr = exe->delete_map.Best (trip_rec);
			if (sel_itr != exe->delete_map.end ()) continue;
		}

		sel_itr = exe->selected.find (trip_rec);
		if (sel_itr == exe->selected.end () || sel_itr->second.Partition () < 0) continue;

		if (time_sort) {
			if (time_rec < time_index) {
				new_plan_file.Write_Plan (plan);
				continue;
			} 
		} else {
			if (trip_rec < trip_index) {
				new_plan_file.Write_Plan (plan);
				continue;
			}
		}
		while ((time_sort && time_index <= time_rec) || (!time_sort && trip_index <= trip_rec)) {
			if (time_sort && time_index == time_rec) {
				new_plan_file.Write_Plan (plan);
			} else if (!time_sort && trip_index == trip_rec) {
				new_plan_file.Write_Plan (plan);
			} else if (exe->delete_flag) {
				sel_itr = exe->delete_map.Best (trip_rec);
				if (sel_itr == exe->delete_map.end ()) {
					new_plan_file.Write_Plan (compare_plan);
				}
			} else {
				new_plan_file.Write_Plan (compare_plan);
			}
			if (compare_file.Read_Plan (compare_plan)) {
				if (time_sort) {
					compare_plan.Get_Time_Index (time_index);
				} else {
					compare_plan.Get_Trip_Index (trip_index);
				}
			} else if (time_sort) {
				time_index.Start (MAX_INTEGER);
			} else {
				trip_index.Household (MAX_INTEGER);
				break;
			}
		}
	}

	//---- copy the remaining records ----

	if ((time_sort && time_index.Start () != MAX_INTEGER) || (!time_sort && trip_index.Household () != MAX_INTEGER)) {
		while (compare_file.Read_Plan (compare_plan)) {
			if (thread_flag) {
				exe->Show_Dot ();
			} else {
				exe->Show_Progress ();
			}
			new_plan_file.Write_Plan (compare_plan);
		}
	}
	if (!thread_flag) exe->End_Progress ();

	plan_file.Close ();
	compare_file.Close ();
	new_plan_file.Close ();
}
