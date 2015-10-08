//*********************************************************
//	Temp_Trip_Write.cpp - write temporary trips to output plans
//*********************************************************

#include "PlanPrep.hpp"

#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Temp_Trip_Write
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Temp_Trip_Write (int part)
{
	int num, first_num;
	bool stat;
	string process_type;

	Plan_Data *plan_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, first_trip, *trip_ptr;
	Trip_Map_Itr trip_itr;

	//---- open the temp files ----

	Partition_Files <Plan_File> temp_file_set;
	Partition_Data <Plan_Data> temp_plan_set;
	Partition_Data <Trip_Index> trip_rec_set;

	Plan_File temp_file (READ, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (plan_file.Filename () + ".temp");

	temp_file_set.Initialize (&temp_file, num_temp);

	plan_ptr = 0;
	temp_plan_set.Initialize (num_temp);
	temp_plan_set.push_back (plan_ptr);

	trip_rec_set.Initialize (num_temp + 1);

	//---- initialize the first index for each partition -----

	for (num = 0; num <= num_temp; num++) {
		if (num == num_temp) {
			trip_itr = traveler_sort.begin ();
			stat = (trip_itr != traveler_sort.end ());
			if (stat) {
				plan_ptr = plan_ptr_array [trip_itr->second];
			} else {
				plan_ptr = 0;
			}
			temp_plan_set [num] = plan_ptr;
		} else {
			plan_ptr = temp_plan_set [num];
			stat = temp_file_set [num]->Read_Plan (*plan_ptr);
		}
		trip_ptr = trip_rec_set [num];

		if (stat) {
			plan_ptr->Get_Trip_Index (*trip_ptr);
		} else {
			trip_ptr->Set (MAX_INTEGER);
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
			trip_index = merge_ptr->Get_Trip_Index ();
		} else {
			trip_index.Set (MAX_INTEGER);
		}
		last_trip = trip_index;
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
				trip_itr++;
				stat = (trip_itr != traveler_sort.end ());

				if (stat) {
					plan_ptr = plan_ptr_array [trip_itr->second];
				} else {
					plan_ptr = 0;
				}
				temp_plan_set [num] = plan_ptr;
			} else {
				plan_ptr = temp_plan_set [num];
				stat = temp_file_set [num]->Read_Plan (*plan_ptr);
			}
			trip_ptr = trip_rec_set [num];

			if (stat) {
				plan_ptr->Get_Trip_Index (*trip_ptr);
			} else {
				trip_ptr->Set (MAX_INTEGER);
			}
		}

		//---- find the first partition ----

		first_num = -1;
		first_trip.Set (MAX_INTEGER);

		for (num = 0; num <= num_temp; num++) {
			trip_ptr = trip_rec_set [num];

			if (*trip_ptr < first_trip) {
				first_num = num;
				first_trip = *trip_ptr;
			}
		}
		if (exe->merge_flag) {
			while (trip_index <= first_trip) {
				if (exe->new_plan_flag && trip_index < first_trip) {
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
			if (first_num < 0 && trip_index.Household () == MAX_INTEGER) break;
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
