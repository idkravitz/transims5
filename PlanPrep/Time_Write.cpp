//*********************************************************
//	Time_Write.cpp - write time sorted plans
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Time_Write
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Time_Write (void)
{
	bool first;
	string process_type;

	Plan_Data *plan_ptr, *merge_ptr;

	Time_Index time_index, last_time, time_rec;
	Time_Map_Itr time_itr;

	plan_ptr = merge_ptr = 0;

	//---- write the process message ----

	if (exe->merge_flag) {
		process_type = "Merging";
		merge_ptr = new Plan_Data ();

		if (merge_file.Read_Plan (*merge_ptr)) {
			merge_ptr->Get_Time_Index (time_index);
		} else {
			time_index.Set (MAX_INTEGER, 0);
		}
		last_time = time_index;
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
		if (time_rec.Start () < MAX_INTEGER) {
			if (first) {
				time_itr = time_sort.begin ();
			} else {
				time_itr++;
			}
			if (time_itr == time_sort.end ()) {
				if (!exe->merge_flag || time_index.Start () == MAX_INTEGER) break;
				time_rec.Set (MAX_INTEGER, 0);
				plan_ptr = 0;
			} else {
				time_rec = time_itr->first;
				plan_ptr = plan_ptr_array [time_itr->second];
			}
		}
		if (exe->merge_flag) {
			while (time_index <= time_rec) {
				if (exe->new_plan_flag && time_index < time_rec) {
					new_plan_file.Write_Plan (*merge_ptr);
				}
				if (merge_file.Read_Plan (*merge_ptr)) {
					merge_ptr->Get_Time_Index (time_index);

					if (time_index < last_time) {
						MAIN_LOCK
						exe->Error ("Merge Plan File is Not Time Sorted");
						END_LOCK
					}
					last_time = time_index;
				} else {
					time_index.Set (MAX_INTEGER, 0);
					break;
				}
			}
			if (time_rec.Start () == MAX_INTEGER && time_index.Start () == MAX_INTEGER) break;
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
