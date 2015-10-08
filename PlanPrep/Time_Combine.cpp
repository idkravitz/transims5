//*********************************************************
//	Time_Combine.cpp - combine time sorted files
//*********************************************************

#include "PlanPrep.hpp"

#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Time_Combine
//---------------------------------------------------------

void PlanPrep::Time_Combine (Plan_File *temp_file, int num_temp)
{
	int i, j, num, first_num;

	Plan_Data *plan_ptr;

	Time_Index time_index, last_time, first_time, *time_ptr;
	Time_Map_Itr time_itr;
	Integers next;

	//---- open the temp files ----

	Partition_Files <Plan_File> temp_file_set;
	Partition_Data <Plan_Data> temp_plan_set;
	Partition_Data <Time_Index> time_rec_set;

	temp_file_set.Initialize (temp_file, num_temp);
	temp_plan_set.Initialize (num_temp);
	time_rec_set.Initialize (num_temp);
	next.assign (num_temp, -1);

	//---- initialize the first index for each partition -----

	for (num = 0; num < num_temp; num++) {
		plan_ptr = temp_plan_set [num];
		time_ptr = time_rec_set [num];

		if (temp_file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Time_Index (*time_ptr);
		} else {
			time_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}

	//---- sort the current records ----

	first_num = 0;

	for (num = 1; num < num_temp; num++) {
		time_ptr = time_rec_set [num];

		for (i=j=first_num; i >= 0; i = next [j = i]) {
			if (*time_ptr < *time_rec_set [i]) {
				if (i == first_num) {
					next [num] = first_num;
					first_num = num;
				} else {
					next [j] = num;
					next [num] = i;
				}
				break;
			}
			if (next [i] < 0) {
				next [i] = num;
				next [num] = -1;
				break;
			}
		}
	}
	if (Num_Threads () > 1) Show_Message (1);
	Show_Message (String ("Combining %s -- Record") % new_plan_file->File_Type ());
	Set_Progress ();

	//---- read records in sorted order ----

	for (; first_num >= 0; ) {

		//---- process the first record ----

		num = first_num;
		first_num = next [num];

		time_ptr = time_rec_set [num];
		if (time_ptr->Household () == MAX_INTEGER) break;

		Show_Progress ();

		plan_ptr = temp_plan_set [num];
		new_plan_file->Write_Plan (*plan_ptr);

		//---- get the next record for the first partition ----

		if (temp_file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Time_Index (*time_ptr);

			//---- update the record order ---

			if (first_num < 0) {
				first_num = num;
			} else {
				for (i=j=first_num; i >= 0; i = next [j = i]) {
					if (*time_ptr < *time_rec_set [i]) {
						if (i == first_num) {
							next [num] = first_num;
							first_num = num;
						} else {
							next [j] = num;
							next [num] = i;
						}
						break;
					}
					if (next [i] < 0) {
						next [i] = num;
						next [num] = -1;
						break;
					}
				}
			}
		} else {
			time_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}
	End_Progress ();

	//---- close and remove each temp file -----

	for (num = 0; num < num_temp; num++) {
		temp_file_set [num]->Close ();

		string name = temp_file_set [num]->Filename ();
		remove (name.c_str ());

		name += ".def";
		remove (name.c_str ());
	}
}
