//*********************************************************
//	Temp_Time_Write.cpp - write temporary times to output trips
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Temp_Time_Write
//---------------------------------------------------------

void TripPrep::Trip_Processing::Temp_Time_Write (int part)
{
	int num, first_num, partition;
	bool stat;
	string process_type;

	Trip_Data *trip_ptr, *merge_ptr;

	Time_Index time_index, last_time, first_time, *time_ptr;
	Time_Map_Itr time_itr;
	Select_Map_Itr sel_itr;

	//---- open the temp files ----

	Partition_Files <Trip_File> temp_file_set;
	Partition_Data <Trip_Data> temp_trip_set;
	Partition_Data <Time_Index> time_rec_set;

	Trip_File temp_file (READ, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (trip_file.Filename () + ".temp");

	temp_file_set.Initialize (&temp_file, num_temp);

	trip_ptr = 0;
	temp_trip_set.Initialize (num_temp);
	temp_trip_set.push_back (trip_ptr);

	time_rec_set.Initialize (num_temp + 1);

	//---- initialize the first index for each partition -----

	for (num = 0; num <= num_temp; num++) {
		if (num == num_temp) {
			time_itr = time_sort.begin ();
			stat = (time_itr != time_sort.end ());

			if (stat) {
				trip_ptr = trip_ptr_array [time_itr->second];
			} else {
				trip_ptr = 0;
			}
			temp_trip_set [num] = trip_ptr;
		} else {
			trip_ptr = temp_trip_set [num];
			stat = temp_file_set [num]->Read_Trip (*trip_ptr);
		}
		time_ptr = time_rec_set [num];

		if (stat) {
			trip_ptr->Get_Time_Index (*time_ptr);
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
		merge_ptr = new Trip_Data ();

		if (merge_file.Read_Trip (*merge_ptr)) {
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
		if (new_trip_file.Part_Flag () && !exe->update_flag) {
			exe->Show_Message (String ("%s %s %d") % process_type % new_trip_file.File_Type () % new_trip_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s") % process_type % new_trip_file.File_Type ());
		}
		END_LOCK
	} else {
		if (new_trip_file.Part_Flag () && !exe->update_flag) {
			exe->Show_Message (String ("%s %s %d -- Record") % process_type % new_trip_file.File_Type () % new_trip_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s -- Record") % process_type % new_trip_file.File_Type ());
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
					trip_ptr = trip_ptr_array [time_itr->second];
				} else {
					trip_ptr = 0;
				}
				temp_trip_set [num] = trip_ptr;
			} else {
				trip_ptr = temp_trip_set [num];
				stat = temp_file_set [num]->Read_Trip (*trip_ptr);
			}
			time_ptr = time_rec_set [num];

			if (stat) {
				trip_ptr->Get_Time_Index (*time_ptr);
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
				if (exe->new_trip_flag && time_index < first_time) {
					if (exe->update_flag) {
						sel_itr = exe->select_map.Best (merge_ptr->Household (), merge_ptr->Person (), 
							merge_ptr->Tour (), merge_ptr->Trip ());
						if (sel_itr == exe->select_map.end ()) continue;

						partition = sel_itr->second.Partition ();
						merge_ptr->Partition (partition);

						exe->new_file_set [partition]->Write_Trip (*merge_ptr);
					} else {
						new_trip_file.Write_Trip (*merge_ptr);
					}
				}
				if (merge_file.Read_Trip (*merge_ptr)) {
					merge_ptr->Get_Time_Index (time_index);

					if (time_index < last_time) {
						MAIN_LOCK
						exe->Error ("Merge Trip File is Not Time Sorted");
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
		if (exe->new_trip_flag && first_num >= 0) {
			trip_ptr = temp_trip_set [first_num];
			if (exe->update_flag) {
				partition = trip_ptr->Partition ();
				exe->new_file_set [partition]->Write_Trip (*trip_ptr);
			} else {
				new_trip_file.Write_Trip (*trip_ptr);
			}
			if (first_num == num_temp) delete trip_ptr;
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
