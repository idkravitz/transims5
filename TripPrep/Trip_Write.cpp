//*********************************************************
//	Trip_Write.cpp - write trip sorted trips
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Trip_Write
//---------------------------------------------------------

void TripPrep::Trip_Processing::Trip_Write (void)
{
	bool first;
	int partition;
	string process_type;

	Trip_Data *trip_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, trip_rec;
	Trip_Map_Stat trip_stat;
	Trip_Map_Itr trip_itr;
	Select_Map_Itr sel_itr;

	trip_ptr = merge_ptr = 0;

	//---- write the process message ----

	if (exe->merge_flag) {
		process_type = "Merging";
		merge_ptr = new Trip_Data ();

		if (merge_file.Read_Trip (*merge_ptr)) {
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
		if (trip_rec.Household () < MAX_INTEGER) {
			if (first) {
				trip_itr = traveler_sort.begin ();
			} else {
				trip_itr++;
			}
			if (trip_itr == traveler_sort.end ()) {
				if (!exe->merge_flag || trip_index.Household () == MAX_INTEGER) break;
				trip_rec.Set (MAX_INTEGER);
				trip_ptr = 0;
			} else {
				trip_rec = trip_itr->first;
				trip_ptr = trip_ptr_array [trip_itr->second];
			}
		}
		if (exe->merge_flag) {
			while (trip_index <= trip_rec) {
				if (exe->new_trip_flag && trip_index < trip_rec) {
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
					merge_ptr->Get_Trip_Index (trip_index);

					if (trip_index < last_trip) {
						MAIN_LOCK
						exe->Error ("Merge Trip File is Not Traveler Sorted");
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
