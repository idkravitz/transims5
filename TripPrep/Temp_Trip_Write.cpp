//*********************************************************
//	Temp_Trip_Write.cpp - write temporary trips to output trips
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Temp_Trip_Write
//---------------------------------------------------------

void TripPrep::Trip_Processing::Temp_Trip_Write (int part)
{
	int num, first_num, partition;
	bool stat;
	string process_type;

	Trip_Data *trip_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, first_trip, *index_ptr;
	Trip_Map_Itr trip_itr;
	Select_Map_Itr sel_itr;

	//---- open the temp files ----

	Partition_Files <Trip_File> temp_file_set;
	Partition_Data <Trip_Data> temp_trip_set;
	Partition_Data <Trip_Index> trip_rec_set;

	Trip_File temp_file (READ, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (trip_file.Filename () + ".temp");

	temp_file_set.Initialize (&temp_file, num_temp);

	trip_ptr = 0;
	temp_trip_set.Initialize (num_temp);
	temp_trip_set.push_back (trip_ptr);

	trip_rec_set.Initialize (num_temp + 1);

	//---- initialize the first index for each partition -----

	for (num = 0; num <= num_temp; num++) {
		if (num == num_temp) {
			trip_itr = traveler_sort.begin ();
			stat = (trip_itr != traveler_sort.end ());
			if (stat) {
				trip_ptr = trip_ptr_array [trip_itr->second];
			} else {
				trip_ptr = 0;
			}
			temp_trip_set [num] = trip_ptr;
		} else {
			trip_ptr = temp_trip_set [num];
			stat = temp_file_set [num]->Read_Trip (*trip_ptr);
		}
		index_ptr = trip_rec_set [num];

		if (stat) {
			trip_ptr->Get_Trip_Index (*index_ptr);
		} else {
			index_ptr->Set (MAX_INTEGER);
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
				trip_itr++;
				stat = (trip_itr != traveler_sort.end ());

				if (stat) {
					trip_ptr = trip_ptr_array [trip_itr->second];
				} else {
					trip_ptr = 0;
				}
				temp_trip_set [num] = trip_ptr;
			} else {
				trip_ptr = temp_trip_set [num];
				stat = temp_file_set [num]->Read_Trip (*trip_ptr);
			}
			index_ptr = trip_rec_set [num];

			if (stat) {
				trip_ptr->Get_Trip_Index (*index_ptr);
			} else {
				index_ptr->Set (MAX_INTEGER);
			}
		}

		//---- find the first partition ----

		first_num = -1;
		first_trip.Set (MAX_INTEGER);

		for (num = 0; num <= num_temp; num++) {
			index_ptr = trip_rec_set [num];

			if (*index_ptr < first_trip) {
				first_num = num;
				first_trip = *index_ptr;
			}
		}
		if (exe->merge_flag) {
			while (trip_index <= first_trip) {
				if (exe->new_trip_flag && trip_index < first_trip) {
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
			if (first_num < 0 && trip_index.Household () == MAX_INTEGER) break;
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
