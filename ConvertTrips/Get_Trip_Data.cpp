//*********************************************************
//	Get_Trip_Data.cpp - copy the trip file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool ConvertTrips::Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition)
{
	int hhold, mode;
	
	Person_Index person_index;
	Int_Map_Itr map_itr;
	Location_Data *loc_ptr;

	hhold = file.Household ();
	if (hhold > max_hh_in) max_hh_in = hhold;

	hhold_map.insert (Int_Map_Data (hhold, 1));

	person_index.Household (hhold);
	person_index.Person (file.Person ());

	person_map.insert (Person_Map_Data (person_index, 1));

	if (select_households && !hhold_range.In_Range (hhold)) return (false);

	mode = file.Mode ();
	if (mode >= 0 && mode < MAX_MODE && !select_mode [mode]) return (false);
	if (select_purposes && !purpose_range.In_Range (file.Purpose ())) return (false);
	if (select_start_times && !start_range.In_Range (file.Start ())) return (false);
	if (select_end_times && !end_range.In_Range (file.End ())) return (false);

	if (select_org_zones) {
		map_itr = location_map.find (file.Origin ());
		if (map_itr != location_map.end ()) {
			loc_ptr = &location_array [map_itr->second];
			if (!org_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
	}
	if (select_des_zones) {
		map_itr = location_map.find (file.Destination ());
		if (map_itr != location_map.end ()) {
			loc_ptr = &location_array [map_itr->second];
			if (!des_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
	}
	trip_file->Copy_Fields (file);

	if (!trip_file->Write ()) {
		Error ("Writing Trip File");
	}
	trip_copy++;

	data.Household (hhold);
	hhold = partition;

	//---- don't save the record ----

	return (false);
}
