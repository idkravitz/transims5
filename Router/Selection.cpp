//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

bool Router::Selection (Trip_Data *trip_ptr)
{
	int hhold, person, tour, trip, mode, origin, destination, type;
	
	Int_Map_Itr loc_itr;
	Location_Data *loc_ptr;
	Select_Map_Itr sel_itr;
	Int2_Map_Itr map2_itr;

	//---- check the household id ----

	hhold = trip_ptr->Household ();
	person = trip_ptr->Person ();
	tour = trip_ptr->Tour ();
	trip = trip_ptr->Trip ();

	//---- check the selection criteria ----

	if (select_households && !hhold_range.In_Range (hhold)) return (false);

	mode = trip_ptr->Mode ();
	if (mode < MAX_MODE && !select_mode [mode]) return (false);

	if (select_purposes && !purpose_range.In_Range (trip_ptr->Purpose ())) return (false);

	if (select_start_times && !start_range.In_Range (trip_ptr->Start ())) return (false);
	if (select_end_times && !end_range.In_Range (trip_ptr->End ())) return (false);

	origin = trip_ptr->Origin ();
	destination = trip_ptr->Destination ();

	if (origin == destination && !reroute_flag) return (false);
	if (select_origins && !org_range.In_Range (origin)) return (false);
	if (select_destinations && !des_range.In_Range (destination)) return (false);

	if (select_org_zones) {
		loc_itr = location_map.find (origin);
		if (loc_itr != location_map.end ()) {
			loc_ptr = &location_array [loc_itr->second];
			if (!org_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
	}
	if (select_des_zones) {
		loc_itr = location_map.find (destination);
		if (loc_itr != location_map.end ()) {
			loc_ptr = &location_array [loc_itr->second];
			if (!des_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
	}

	//---- update the traveler type ----

	type = MAX (trip_ptr->Type (), 1);

	if (script_flag || hhfile_flag) {
		map2_itr = hhold_type.find (Int2_Key (hhold, person));
		if (map2_itr == hhold_type.end ()) return (false);
		type = map2_itr->second;
	}
	if (select_travelers && !traveler_range.In_Range (type)) return (false);

	//---- check the selection records ----

	if (select_flag) {
		sel_itr = select_map.Best (hhold, person, tour, trip);
		if (sel_itr == select_map.end ()) return (false);
		if (trip_ptr->Partition () == 0) {
			trip_ptr->Partition (Partition_Index (sel_itr->second.Partition ()));
		}
	}

	//---- check the deletion records ----

	if (delete_flag) {
		sel_itr = delete_map.Best (hhold, person, tour, trip);
		if (sel_itr != delete_map.end ()) return (false);
	}
	if (delete_households && hhold_delete.In_Range (hhold)) return (false);
	if (mode < MAX_MODE && delete_mode [mode]) return (false);
	if (delete_travelers && traveler_delete.In_Range (type)) return (false);

	return (!percent_flag || random_select.Probability () <= select_percent);
}

