//*********************************************************
//	Time_Combine.cpp - combine time sorted files
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Time_Combine
//---------------------------------------------------------

void TripPrep::Time_Combine (Trip_File *temp_file, int num_temp)
{
	int num, first_num;

	Trip_Data *trip_ptr;

	Time_Index time_index, last_time, first_time, *time_ptr;
	Time_Map_Itr time_itr;

	//---- open the temp files ----

	Partition_Files <Trip_File> temp_file_set;
	Partition_Data <Trip_Data> temp_trip_set;
	Partition_Data <Time_Index> time_rec_set;

	temp_file_set.Initialize (temp_file, num_temp);
	temp_trip_set.Initialize (num_temp);
	time_rec_set.Initialize (num_temp);

	//---- initialize the first index for each partition -----

	for (num = 0; num < num_temp; num++) {
		trip_ptr = temp_trip_set [num];
		time_ptr = time_rec_set [num];

		if (temp_file_set [num]->Read_Trip (*trip_ptr)) {
			trip_ptr->Get_Time_Index (*time_ptr);
		} else {
			time_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}
	if (Num_Threads () > 1) Show_Message (1);
	Show_Message (String ("Combining %s -- Record") % new_trip_file->File_Type ());
	Set_Progress ();

	//---- read records in sorted order ----

	for (;;) {

		//---- find the first partition ----

		first_num = -1;
		first_time.Set (MAX_INTEGER, 0, 0);

		for (num = 0; num < num_temp; num++) {
			time_ptr = time_rec_set [num];

			if (*time_ptr < first_time) {
				first_num = num;
				first_time = *time_ptr;
			}
		}
		if (first_num < 0) break;

		Show_Progress ();

		trip_ptr = temp_trip_set [first_num];
		new_trip_file->Write_Trip (*trip_ptr);

		//---- get the next record for the first partition ----

		num = first_num;
		trip_ptr = temp_trip_set [num];
		time_ptr = time_rec_set [num];

		if (temp_file_set [num]->Read_Trip (*trip_ptr)) {
			trip_ptr->Get_Time_Index (*time_ptr);
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
