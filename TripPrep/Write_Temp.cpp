//*********************************************************
//	Write_Temp.cpp - write trips to a temporary file
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Write_Temp
//---------------------------------------------------------

void TripPrep::Trip_Processing::Write_Temp (void)
{
	Trip_Data *trip_ptr;
	Trip_Map_Itr trip_itr;
	Time_Map_Itr time_itr;

	//---- open the next temp file ----

	Trip_File temp_file (CREATE, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (trip_file.Filename () + ".temp");
	temp_file.First_Open (false);
	temp_file.Open (num_temp++);

	//---- read records in sorted order ----

	if (exe->Trip_Sort () == TRAVELER_SORT) {
		for (trip_itr = traveler_sort.begin (); trip_itr != traveler_sort.end (); trip_itr++) {
			trip_ptr = trip_ptr_array [trip_itr->second];
			temp_file.Write_Trip (*trip_ptr);
			delete trip_ptr;
		}
		traveler_sort.clear ();
	} else {
		for (time_itr = time_sort.begin (); time_itr != time_sort.end (); time_itr++) {
			trip_ptr = trip_ptr_array [time_itr->second];
			temp_file.Write_Trip (*trip_ptr);
			delete trip_ptr;
		}
		time_sort.clear ();
	}
	temp_file.Close ();
	trip_ptr_array.clear ();
}
