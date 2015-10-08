//*********************************************************
//	Get_Trip_Data.cpp - read the trip file
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	Initialize_Trips
//---------------------------------------------------------

void RandomSelect::Initialize_Trips (Trip_File &file)
{
	if (file.Household () > 0) return;
}

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool RandomSelect::Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition)
{
	int hhold;
	Trip_Index trip_index;
	Select_Data select_data;
	Select_Map_Stat map_stat;

	hhold = file.Household ();
	if (hhold <= 0) return (false);

	if (select_households && !hhold_range.In_Range (hhold)) return (false);
	if (percent_flag && random.Probability () > select_percent) return (false);

	data.Household (hhold);
		
	trip_index.Household (hhold);
	trip_index.Person (file.Person ());
	trip_index.Tour (file.Tour ());
	trip_index.Trip (file.Trip ());
	select_data.Type (0);
	select_data.Partition (partition);

	map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));

	if (!map_stat.second) {
		Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
			trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
	}
	return (false);
}
