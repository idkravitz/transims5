//*********************************************************
//	Read_Trips.cpp - read the trip file
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void EventSum::Read_Trips (void)
{
	int hhold, num_rec, mode, purpose;

	Location_Data *location_ptr;
	Trip_File *trip_file;
	Travel_Time_Data *time_ptr, time_data;
	Travel_Time_Itr time_itr;
	Trip_Index trip_index;
	Trip_Map_Stat map_stat;
	Trip_Map_Itr trip_itr;
	Int_Map_Itr map_itr;

	trip_file = (Trip_File *) System_File_Handle (TRIP);
	
	//----- initialize the summary memory ----
	
	if (travel_time_array.size () == 0) {
		num_rec = trip_file->Estimate_Records ();
		travel_time_array.reserve (num_rec);
	}
	Show_Message (String ("Reading %s -- Record") % trip_file->File_Type ());
	Set_Progress ();

	num_rec = 0;

	//---- store the trip data ----

	while (trip_file->Read ()) {
		Show_Progress ();

		//---- check the household id ----

		hhold = trip_file->Household ();
		if (hhold <= 0) continue;

		if (select_households && !hhold_range.In_Range (hhold)) continue;

		mode = trip_file->Mode ();
		if (mode > 0 && mode < MAX_MODE && !select_mode [mode]) continue;

		purpose = trip_file->Purpose ();
		if (select_purposes && !purpose_range.In_Range (purpose)) continue;

		//---- check the selection records ----
		
		time_data.Household (hhold);
		time_data.Person (trip_file->Person ());
		time_data.Tour (trip_file->Tour ());
		time_data.Trip (trip_file->Trip ());

		time_data.Get_Trip_Index (trip_index);

		if (select_flag && select_map.Best (trip_index) == select_map.end ()) continue;

		//---- save a new travel time ----

		if (time_in_flag) {
			trip_itr = travel_time_map.find (trip_index);

			if (trip_itr == travel_time_map.end ()) continue;

			time_ptr = &travel_time_array [trip_itr->second];
		} else {
			num_rec = (int) travel_time_array.size ();

			map_stat = travel_time_map.insert (Trip_Map_Data (trip_index, num_rec));
			if (!map_stat.second) {
				Warning (String ("Duplicate Travel Time Index %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
			} else {
				travel_time_array.push_back (time_data);
			}
			time_ptr = &travel_time_array [map_stat.first->second];
		}

		//---- save a new travel time ----

		time_ptr->Mode (mode);
		time_ptr->Purpose (purpose);
		time_ptr->Constraint (trip_file->Constraint ());

		time_ptr->Trip_Start (trip_file->Start ());
		time_ptr->Trip_End (trip_file->End ());

		//---- find the origin and destination links ----

		map_itr = location_map.find (trip_file->Origin ());

		if (map_itr == location_map.end ()) {
			Warning (String ("Trip Origin %d was Not Found in the Location File") % trip_file->Origin ());
			time_ptr->Start_Link (0);
		} else {
			location_ptr = &location_array [map_itr->second];
			time_ptr->Start_Link (link_array [location_ptr->Link ()].Link ());
		}

		map_itr = location_map.find (trip_file->Destination ());

		if (map_itr == location_map.end ()) {
			Warning (String ("Trip Destination %d was Not Found in the Location File") % trip_file->Destination ());
			time_ptr->End_Link (0);
		} else {
			location_ptr = &location_array [map_itr->second];
			time_ptr->End_Link (link_array [location_ptr->Link ()].Link ());
		}
		num_rec++;
	}
	End_Progress ();

	trip_file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % trip_file->File_Type () % Progress_Count ());
	if (num_rec != Progress_Count ()) Print (1, "Number of Records Saved = ") << num_rec;
}
