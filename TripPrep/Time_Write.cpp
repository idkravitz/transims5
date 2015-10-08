//*********************************************************
//	Time_Write.cpp - write time sorted trips
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Time_Write
//---------------------------------------------------------

void TripPrep::Trip_Processing::Time_Write (void)
{
	bool first;
	int partition;
	string process_type;

	Trip_Data *trip_ptr, *merge_ptr;

	Time_Index time_index, last_time, time_rec;
	Time_Map_Itr time_itr;
	Select_Map_Itr sel_itr;

	trip_ptr = merge_ptr = 0;

	//---- write the process message ----

	if (exe->merge_flag) {
		process_type = "Merging";
		merge_ptr = new Trip_Data ();

		if (merge_file.Read_Trip (*merge_ptr)) {
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
				trip_ptr = 0;
			} else {
				time_rec = time_itr->first;
				trip_ptr = trip_ptr_array [time_itr->second];
			}
		}
		if (exe->merge_flag) {
			while (time_index <= time_rec) {
				if (exe->new_trip_flag && time_index < time_rec) {
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
		if (exe->new_trip_flag && trip_ptr != 0) {
			if (exe->update_flag) {
				partition = trip_ptr->Partition ();
				exe->new_file_set [partition]->Write_Trip (*trip_ptr);
			} else {
				new_trip_file.Write_Trip (*trip_ptr);
			}
			delete trip_ptr;
			trip_ptr = 0;
		}
	}
	if (!thread_flag) exe->End_Progress ();

	if (merge_ptr != 0) {
		delete merge_ptr;
	}
}
