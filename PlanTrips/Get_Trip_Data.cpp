//*********************************************************
//	Get_Trip_Data.cpp - filter the trip data
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Initialize_Trips
//---------------------------------------------------------

void PlanTrips::Initialize_Trips (Trip_File &file)
{
	trip_array.reserve (file.Estimate_Records ());
}

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool PlanTrips::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	trip_rec.Household (file.Household ());
	trip_rec.Person (file.Person ());
	trip_rec.Tour (MAX (file.Tour (), 1));
	trip_rec.Trip (file.Trip ());

	trip_rec.Start (file.Start ());
	trip_rec.End (file.End ());
	trip_rec.Origin (file.Origin ());
	trip_rec.Destination (file.Destination ());
	trip_rec.Vehicle (file.Vehicle ());

	trip_rec.Mode (file.Mode ());
	trip_rec.Purpose (file.Purpose ());
	trip_rec.Constraint (file.Constraint ());
	trip_rec.Passengers (file.Passengers ());
	trip_rec.Type (file.Type ());

	if (file.Partition_Flag ()) {
		trip_rec.Partition (file.Partition ());
	} else {
		trip_rec.Partition (partition);
	}
		
	//---- check the selection criteria ----

	if (select_households && !hhold_range.In_Range (trip_rec.Household ())) return (false);
	if (trip_rec.Mode () < MAX_MODE && !select_mode [trip_rec.Mode ()]) return (false);
	if (select_purposes && !purpose_range.In_Range (trip_rec.Purpose ())) return (false);
	if (select_travelers && !traveler_range.In_Range (trip_rec.Type ())) return (false);
	if (select_start_times && !start_range.In_Range (trip_rec.Start ())) return (false);
	if (select_end_times && !end_range.In_Range (trip_rec.End ())) return (false);
	if (select_origins && !org_range.In_Range (trip_rec.Origin ())) return (false);
	if (select_destinations && !des_range.In_Range (trip_rec.Destination ())) return (false);
		
	//---- check the selection records ----

	if (select_flag) {
		Select_Map_Itr sel_itr;

		sel_itr = select_map.Best (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour (), trip_rec.Trip ());
		if (sel_itr == select_map.end ()) return (false);
	}
	return (true);
}
