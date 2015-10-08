//*********************************************************
//	Write_Temp.cpp - write plans to a temporary file
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Write_Temp
//---------------------------------------------------------

void PlanPrep::Plan_Processing::Write_Temp (void)
{
	Plan_Data *plan_ptr;
	Trip_Map_Itr trip_itr;
	Time_Map_Itr time_itr;

	//---- open the next temp file ----

	Plan_File temp_file (CREATE, BINARY);

	temp_file.Part_Flag (true);
	temp_file.Pathname (plan_file.Filename () + ".temp");
	temp_file.First_Open (false);
	temp_file.Open (num_temp++);

	//---- read records in sorted order ----

	if (exe->Trip_Sort () == TRAVELER_SORT) {
		for (trip_itr = traveler_sort.begin (); trip_itr != traveler_sort.end (); trip_itr++) {
			plan_ptr = plan_ptr_array [trip_itr->second];
			temp_file.Write_Plan (*plan_ptr);
			delete plan_ptr;
		}
		traveler_sort.clear ();
	} else {
		for (time_itr = time_sort.begin (); time_itr != time_sort.end (); time_itr++) {
			plan_ptr = plan_ptr_array [time_itr->second];
			temp_file.Write_Plan (*plan_ptr);
			delete plan_ptr;
		}
		time_sort.clear ();
	}
	temp_file.Close ();
	plan_ptr_array.clear ();
}
