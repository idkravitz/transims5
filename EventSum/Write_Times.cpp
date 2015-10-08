//*********************************************************
//	Write_Times.cpp - write travel time file
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Times
//---------------------------------------------------------

void EventSum::Write_Times (void)
{
	int num_out;

	Trip_Map_Itr map_itr;
	Travel_Time_Data *time_ptr;

	Show_Message (String ("Writing %s -- Record") % travel_time_file.File_Type ());
	Set_Progress ();

	num_out = 0;

	//---- write travel time file ----

	for (map_itr = travel_time_map.begin (); map_itr != travel_time_map.end (); map_itr++) {
		Show_Progress ();

		time_ptr = &travel_time_array [map_itr->second];

		//---- check the time range ----

		if (!sum_periods.Span_Range (time_ptr->Base_Start (), time_ptr->Base_End ()) &&
			!sum_periods.Span_Range (time_ptr->Started (), time_ptr->Ended ())) continue;

		if (time_ptr->Ended () == 0 || time_ptr->Ended () < time_ptr->Started ()) continue;

		//---- set the data fields ----

		travel_time_file.Household (time_ptr->Household ());
		travel_time_file.Person (time_ptr->Person ());
		travel_time_file.Tour (time_ptr->Tour ());
		travel_time_file.Trip (time_ptr->Trip ());
		travel_time_file.Mode (time_ptr->Mode ());
		travel_time_file.Purpose (time_ptr->Purpose ());
		travel_time_file.Constraint (time_ptr->Constraint ());
		travel_time_file.Start_Link (time_ptr->Start_Link ());
		travel_time_file.End_Link (time_ptr->End_Link ());

		travel_time_file.Trip_Start (time_ptr->Trip_Start ());
		travel_time_file.Base_Start (time_ptr->Base_Start ());
		travel_time_file.Started (time_ptr->Started ());
		travel_time_file.Start_Diff (time_ptr->Base_Start_Diff ());

		travel_time_file.Trip_End (time_ptr->Trip_End ());
		travel_time_file.Base_End (time_ptr->Base_End ());
		travel_time_file.Ended (time_ptr->Ended ());
		travel_time_file.End_Diff (time_ptr->Base_End_Diff ());

		travel_time_file.Mid_Trip (time_ptr->Mid_Trip ().Round_Seconds ());
		travel_time_file.Travel_Time (time_ptr->Travel_Time ());

		if (!travel_time_file.Write ()) {
			Error (String ("Writing %s") % travel_time_file.File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	travel_time_file.Close ();

	Print (2, String ("Number of %s Records = %d") % travel_time_file.File_Type () % num_out);
}
