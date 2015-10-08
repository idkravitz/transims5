//*********************************************************
//	Trip_Combine.cpp - combine trip sorted files
//*********************************************************

#include "PlanPrep.hpp"

#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Trip_Combine
//---------------------------------------------------------

void PlanPrep::Trip_Combine (Plan_File *temp_file, int num_temp)
{
	int num, first_num;

	Plan_Data *plan_ptr;

	Trip_Index trip_index, last_trip, first_trip, *trip_ptr;
	Trip_Map_Itr trip_itr;

	//---- open the temp files ----

	Partition_Files <Plan_File> temp_file_set;
	Partition_Data <Plan_Data> temp_plan_set;
	Partition_Data <Trip_Index> trip_rec_set;

	temp_file_set.Initialize (temp_file, num_temp);
	temp_plan_set.Initialize (num_temp);
	trip_rec_set.Initialize (num_temp);

	//---- initialize the first index for each partition -----

	for (num = 0; num < num_temp; num++) {
		plan_ptr = temp_plan_set [num];
		trip_ptr = trip_rec_set [num];

		if (temp_file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Trip_Index (*trip_ptr);
		} else {
			trip_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}
	if (Num_Threads () > 1) Show_Message (1);
	Show_Message (String ("Combining %s -- Record") % new_plan_file->File_Type ());
	Set_Progress ();

	//---- read records in sorted order ----

	for (;;) {

		//---- find the first partition ----

		first_num = -1;
		first_trip.Set (MAX_INTEGER, 0, 0);

		for (num = 0; num < num_temp; num++) {
			trip_ptr = trip_rec_set [num];

			if (*trip_ptr < first_trip) {
				first_num = num;
				first_trip = *trip_ptr;
			}
		}
		if (first_num < 0) break;

		Show_Progress ();

		plan_ptr = temp_plan_set [first_num];
		new_plan_file->Write_Plan (*plan_ptr);

		//---- get the next record for the first partition ----

		num = first_num;
		plan_ptr = temp_plan_set [num];
		trip_ptr = trip_rec_set [num];

		if (temp_file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Trip_Index (*trip_ptr);
		} else {
			trip_ptr->Set (MAX_INTEGER, 0, 0);
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
