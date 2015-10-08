//*********************************************************
//	Read_Plans.cpp - read the travel plan file
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Read_Plans (int part)
{
	string process_type;

	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Select_Map_Itr sel_itr;

	Plan_Data *plan_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, trip_rec, last_sort;
	Time_Index time_index, last_time, time_rec;
	Trip_Map_Stat trip_stat;
	Time_Map_Stat time_stat;

	//---- open the file partition ----

	if (!plan_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % plan_file.Filename ()); 
		END_LOCK
	}
	if (exe->merge_flag) {
		if (!merge_file.Open (part)) {
			MAIN_LOCK 
			exe->Error (String ("Opening %s") % merge_file.Filename ()); 
			END_LOCK
		}
		merge_ptr = new Plan_Data ();
	} else {
		merge_ptr = 0;
	}
	if (exe->new_plan_flag && (!exe->output_flag || part == 0)) {
		new_plan_file.Open (part);
	}

	//---- write the process message ----

	if (exe->Trip_Sort () == TRAVELER_SORT) {
		process_type = "Traveler Sorting";
	} else if (exe->Trip_Sort () == TIME_SORT) {
		process_type = "Time Sorting";
	} else if (exe->merge_flag) {
		process_type = "Merging";

		if (merge_file.Read_Plan (*merge_ptr)) {
			merge_ptr->Get_Trip_Index (trip_index);
		} else {
			trip_index.Set (MAX_INTEGER);
		}
		last_trip = trip_index;
		trip_rec.Clear ();
	} else {
		process_type = "Copying";
	}
	if (thread_flag) {
		MAIN_LOCK
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d") % process_type % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s") % process_type % plan_file.File_Type ());
		}
		END_LOCK
	} else {
		if (plan_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d -- Record") % process_type % plan_file.File_Type () % plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s -- Record") % process_type % plan_file.File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- read the plan file ----

	plan_ptr = new Plan_Data ();

	while (plan_file.Read_Plan (*plan_ptr)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}

		//---- check the selection criteria ----

		if (exe->select_households && !exe->hhold_range.In_Range (plan_ptr->Household ())) continue;
		if (plan_ptr->Mode () < MAX_MODE && !exe->select_mode [plan_ptr->Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan_ptr->Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan_ptr->Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (plan_ptr->Depart ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (plan_ptr->Arrive ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (plan_ptr->Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (plan_ptr->Destination ())) continue;
		if (exe->select_links && !exe->Select_Plan_Links (*plan_ptr)) continue;
		if (exe->select_nodes && !exe->Select_Plan_Nodes (*plan_ptr)) continue;
		if (exe->select_subarea && !exe->Select_Plan_Subarea (*plan_ptr)) continue;
		
		//---- check the deletion records ----

		if (exe->delete_flag) {
			sel_itr = exe->delete_map.Best (plan_ptr->Household (), plan_ptr->Person (), 
				plan_ptr->Tour (), plan_ptr->Trip ());
			if (sel_itr != exe->delete_map.end ()) continue;
		}
		if (exe->delete_households && exe->hhold_delete.In_Range (plan_ptr->Household ())) continue;
		if (plan_ptr->Mode () < MAX_MODE && exe->delete_mode [plan_ptr->Mode ()]) continue;
		if (exe->delete_travelers && exe->traveler_delete.In_Range (plan_ptr->Type ())) continue;

		//---- check the selection records ----

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (plan_ptr->Household (), plan_ptr->Person (), 
				plan_ptr->Tour (), plan_ptr->Trip ());
			if (sel_itr == exe->select_map.end ()) continue;
		}

		if (exe->percent_flag && exe->random.Probability () > exe->select_percent) continue;

		//---- save the sort key ----

		if (exe->Trip_Sort () == TRAVELER_SORT) {
			plan_ptr->Get_Trip_Index (trip_index);

			trip_stat = traveler_sort.insert (Trip_Map_Data (trip_index, (int) plan_ptr_array.size ()));

			if (!trip_stat.second) {
				MAIN_LOCK
				exe->Warning (String ("Duplicate Plan Index = %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				END_LOCK
			} else {
				plan_ptr_array.push_back (plan_ptr);
				plan_ptr = new Plan_Data ();

				if (exe->sort_size > 0 && (int) plan_ptr_array.size () > exe->sort_size) {
					Write_Temp ();
				}
			}
		} else if (exe->Trip_Sort () == TIME_SORT) {
			plan_ptr->Get_Time_Index (time_index);

			time_stat = time_sort.insert (Time_Map_Data (time_index, (int) plan_ptr_array.size ()));

			if (!time_stat.second) {
				MAIN_LOCK
				exe->Warning (String ("Duplicate Plan Index = %s-%d-%d") % 
					time_index.Start ().Time_String () % 
					time_index.Household () % time_index.Person ());
				END_LOCK
			} else {
				plan_ptr_array.push_back (plan_ptr);
				plan_ptr = new Plan_Data ();

				if (exe->sort_size > 0 && (int) plan_ptr_array.size () > exe->sort_size) {
					Write_Temp ();
				}
			}
		} else if (exe->merge_flag) {
			plan_ptr->Get_Trip_Index (trip_rec);

			if (trip_rec < last_sort) {
				MAIN_LOCK
				exe->Error ("Plan File is Not Traveler Sorted");
				END_LOCK
			}
			last_sort = trip_rec;

			while (trip_index <= trip_rec) {
				if (exe->new_plan_flag && trip_index < trip_rec) {
					if (exe->delete_flag && exe->delete_map.Best (trip_index) != exe->delete_map.end ()) goto next;
					if (exe->delete_households && exe->hhold_delete.In_Range (merge_ptr->Household ())) goto next;
					if (merge_ptr->Mode () < MAX_MODE && exe->delete_mode [merge_ptr->Mode ()]) goto next;
					if (exe->delete_travelers && exe->traveler_delete.In_Range (merge_ptr->Type ())) goto next;

					new_plan_file.Write_Plan (*merge_ptr);
				}
next:
				if (merge_file.Read_Plan (*merge_ptr)) {
					merge_ptr->Get_Trip_Index (trip_index);

					if (trip_index < last_trip) {
						MAIN_LOCK
						exe->Error ("Merge Plan File is Not Traveler Sorted");
						END_LOCK
					}
					last_trip = trip_index;
				} else {
					trip_index.Set (MAX_INTEGER);
					break;
				}
			}
			if (exe->new_plan_flag) {
				new_plan_file.Write_Plan (*plan_ptr);
			}
		} else {
			if (exe->new_plan_flag) {
				new_plan_file.Write_Plan (*plan_ptr);
			}
		}
	}
	if (!thread_flag) exe->End_Progress ();

	//---- process the sorted records ----

	if ((exe->new_plan_flag || exe->merge_flag) && (exe->Trip_Sort () == TRAVELER_SORT || exe->Trip_Sort () == TIME_SORT)) {
		if (num_temp > 0) {
			if (exe->Trip_Sort () == TRAVELER_SORT) {
				Temp_Trip_Write (part);
			} else {
				Temp_Time_Write (part);
			}
		} else {
			if (exe->Trip_Sort () == TRAVELER_SORT) {
				Trip_Write ();
			} else {
				Time_Write ();
			}
		}
	} else if (exe->new_plan_flag && exe->merge_flag && trip_index.Household () < MAX_INTEGER) {

		//---- copy the remaining merge records ----

		while (merge_file.Read_Plan (*merge_ptr)) {
			new_plan_file.Write_Plan (*merge_ptr);
		}
	}
	plan_file.Close ();
	if (exe->new_plan_flag && !exe->output_flag) {
		new_plan_file.Close ();
	}
	if (exe->merge_flag) {
		merge_file.Close ();

		if (merge_ptr != 0) {
			delete merge_ptr;
		}
	}
	if (plan_ptr != 0) {
		delete plan_ptr;
	}
}
