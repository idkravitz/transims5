//*********************************************************
//	Read_Times.cpp - read the comparison travel times
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_Times
//---------------------------------------------------------

void EventSum::Read_Times (void)
{
	int hhold, num_rec, mode, purpose;

	Travel_Time_Data time_data;
	Trip_Map_Stat map_stat;
	Trip_Index trip_index;

	num_rec = time_in_file.Estimate_Records ();
	travel_time_array.reserve (num_rec);

	Show_Message (String ("Reading %s -- Record") % time_in_file.File_Type ());
	Set_Progress ();

	num_rec = 0;

	//---- store the travel time data ----

	while (time_in_file.Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = time_in_file.Household ();
		if (hhold <= 0) continue;

		if (select_households && !hhold_range.In_Range (hhold)) continue;

		mode = time_in_file.Mode ();
		if (mode > 0 && mode < MAX_MODE && !select_mode [mode]) continue;

		purpose = time_in_file.Purpose ();
		if (select_purposes && !purpose_range.In_Range (purpose)) continue;

		//---- check the selection records ----
		
		time_data.Household (hhold);
		time_data.Person (time_in_file.Person ());
		time_data.Tour (time_in_file.Tour ());
		time_data.Trip (time_in_file.Trip ());

		time_data.Get_Trip_Index (trip_index);

		if (select_flag && select_map.Best (trip_index) == select_map.end ()) continue;

		//---- save a new travel time ----

		time_data.Mode (mode);
		time_data.Purpose (purpose);
		time_data.Constraint (time_in_file.Constraint ());

		time_data.Start_Link (time_in_file.Start_Link ());
		time_data.End_Link (time_in_file.End_Link ());
		time_data.Trip_Start (time_in_file.Trip_Start ());
		time_data.Trip_End (time_in_file.Trip_End ());
		time_data.Base_Start (time_in_file.Started ());
		time_data.Base_End (time_in_file.Ended ());

		num_rec = (int) travel_time_array.size ();

		map_stat = travel_time_map.insert (Trip_Map_Data (trip_index, num_rec));
		if (!map_stat.second) {
			Warning (String ("Duplicate Travel Time Index %d-%d-%d-%d") % 
				trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
		} else {
			travel_time_array.push_back (time_data);
		}
	}
	End_Progress ();

	time_in_file.Close ();
	num_rec = (int) travel_time_array.size ();
	
	Print (2, String ("Number of %s Records = %d") % time_in_file.File_Type () % Progress_Count ());
	if (num_rec != Progress_Count ()) Print (1, "Number of Records Saved = ") << num_rec;
}
