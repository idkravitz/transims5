//*********************************************************
//	Trip_Write.cpp - write trip sorted plans
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Trip_Write
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Trip_Write (void)
{
	bool first;
	string process_type;

	Plan_Data *plan_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, trip_rec;
	Trip_Map_Stat trip_stat;
	Trip_Map_Itr trip_itr;

	plan_ptr = merge_ptr = 0;

	//---- write the process message ----

	if (exe->merge_flag) {
		process_type = "Merging";
		merge_ptr = new Plan_Data ();

		if (merge_file.Read_Plan (*merge_ptr)) {
			merge_ptr->Get_Trip_Index (trip_index);
		} else {
			trip_index.Set (MAX_INTEGER);
		}
		last_trip = trip_index;
	} else {
		process_type = "Writing";
	}
	if (thread_flag) {
		MAIN_LOCK
		if (new_plan_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d") % process_type % new_plan_file.File_Type () % new_plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s") % process_type % new_plan_file.File_Type ());
		}
		END_LOCK
	} else {
		if (new_plan_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d -- Record") % process_type % new_plan_file.File_Type () % new_plan_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s -- Record") % process_type % new_plan_file.File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- read records in sorted order ----

	for (first = true; ; first = false) {
		if (trip_rec.Household () < MAX_INTEGER) {
			if (first) {
				trip_itr = traveler_sort.begin ();
			} else {
				trip_itr++;
			}
			if (trip_itr == traveler_sort.end ()) {
				if (!exe->merge_flag || trip_index.Household () == MAX_INTEGER) break;
				trip_rec.Set (MAX_INTEGER);
				plan_ptr = 0;
			} else {
				trip_rec = trip_itr->first;
				plan_ptr = plan_ptr_array [trip_itr->second];
			}
		}
		if (exe->merge_flag) {
			while (trip_index <= trip_rec) {
				if (exe->new_plan_flag && trip_index < trip_rec) {
					new_plan_file.Write_Plan (*merge_ptr);
				}
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
			if (trip_rec.Household () == MAX_INTEGER && trip_index.Household () == MAX_INTEGER) break;
		}
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		if (exe->new_plan_flag && plan_ptr != 0) {
			new_plan_file.Write_Plan (*plan_ptr);
			delete plan_ptr;
			plan_ptr = 0;
		}
	}
	if (!thread_flag) exe->End_Progress ();

	if (merge_ptr != 0) {
		delete merge_ptr;
	}
}
