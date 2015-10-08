//*********************************************************
//	Temp_Time_Write.cpp - write temporary times to output plans
//*********************************************************

#include "PlanPrep.hpp"

#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Temp_Time_Write
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Temp_Time_Write (int part)
{
	int num, first_num;
	bool stat;
	string process_type;

	Plan_Data *plan_ptr, *merge_ptr;

	Time_Index time_index, last_time, first_time, *time_ptr;
	Time_Map_Itr time_itr;

	//---- open the temp files ----

	Partition_Files <Plan_File> temp_file_set;
	Partition_Data <Plan_Data> temp_plan_set;
	Partition_Data <Time_Index> time_rec_set;

	Plan_File temp_file (READ, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (plan_file.Filename () + ".temp");

	temp_file_set.Initialize (&temp_file, num_temp);

	plan_ptr = 0;
	temp_plan_set.Initialize (num_temp);
	temp_plan_set.push_back (plan_ptr);

	time_rec_set.Initialize (num_temp + 1);

	//---- initialize the first index for each partition -----

	for (num = 0; num <= num_temp; num++) {
		if (num == num_temp) {
			time_itr = time_sort.begin ();
			stat = (time_itr != time_sort.end ());

			if (stat) {
				plan_ptr = plan_ptr_array [time_itr->second];
			} else {
				plan_ptr = 0;
			}
			temp_plan_set [num] = plan_ptr;
		} else {
			plan_ptr = temp_plan_set [num];
			stat = temp_file_set [num]->Read_Plan (*plan_ptr);
		}
		time_ptr = time_rec_set [num];

		if (stat) {
			plan_ptr->Get_Time_Index (*time_ptr);
		} else {
			time_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}

	//---- write the process message ----

	if (exe->merge_flag) {
		process_type = "Merging";
		if (!merge_file.Open (part)) {
			MAIN_LOCK 
			exe->Error (String ("Opening %s") % merge_file.Filename ()); 
			END_LOCK
		}
		merge_ptr = new Plan_Data ();

		if (merge_file.Read_Plan (*merge_ptr)) {
			merge_ptr->Get_Time_Index (time_index);
		} else {
			time_index.Set (MAX_INTEGER, 0, 0);
		}
		last_time = time_index;
	} else {
		process_type = "Writing";
		merge_ptr = 0;
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

	for (first_num = -1;;) {

		//---- get the next record for the first partition ----

		if (first_num >= 0) {
			num = first_num;

			if (num == num_temp) {
				time_itr++;
				stat = (time_itr != time_sort.end ());

				if (stat) {
					plan_ptr = plan_ptr_array [time_itr->second];
				} else {
					plan_ptr = 0;
				}
				temp_plan_set [num] = plan_ptr;
			} else {
				plan_ptr = temp_plan_set [num];
				stat = temp_file_set [num]->Read_Plan (*plan_ptr);
			}
			time_ptr = time_rec_set [num];

			if (stat) {
				plan_ptr->Get_Time_Index (*time_ptr);
			} else {
				time_ptr->Set (MAX_INTEGER, 0, 0);
			}
		}

		//---- find the first partition ----

		first_num = -1;
		first_time.Set (MAX_INTEGER, 0, 0);

		for (num = 0; num <= num_temp; num++) {
			time_ptr = time_rec_set [num];

			if (*time_ptr < first_time) {
				first_num = num;
				first_time = *time_ptr;
			}
		}
		if (exe->merge_flag) {
			while (time_index <= first_time) {
				if (exe->new_plan_flag && time_index < first_time) {
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
					time_index.Set (MAX_INTEGER, 0, 0);
					break;
				}
			}
			if (first_num < 0 && time_index.Start () == MAX_INTEGER) break;
		} else {
			if (first_num < 0) break;
		}
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		if (exe->new_plan_flag && first_num >= 0) {
			plan_ptr = temp_plan_set [first_num];
			new_plan_file.Write_Plan (*plan_ptr);
			if (first_num == num_temp) delete plan_ptr;
		}
	}
	if (!thread_flag) exe->End_Progress ();

	if (merge_ptr != 0) {
		delete merge_ptr;
	}

	//---- close and remove each temp file -----

	for (num = 0; num < num_temp; num++) {
		temp_file_set [num]->Close ();

		process_type = temp_file_set [num]->Filename ();
		remove (process_type.c_str ());

		process_type += ".def";
		remove (process_type.c_str ());
	}
}
