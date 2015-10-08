//*********************************************************
//	Get_Household_Data.cpp - read the household file
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	Get_Household_Data
//---------------------------------------------------------

bool RandomSelect::Get_Household_Data (Household_File &file, Household_Data &data, int partition)
{
	if (!file.Nested ()) {
		int hhold, person, persons;
		Trip_Index trip_index;
		Select_Data select_data;
		Select_Map_Stat map_stat;

		hhold = file.Household ();
		if (hhold <= 0) return (false);

		if (select_households && !hhold_range.In_Range (hhold)) return (false);
		if (percent_flag && random.Probability () > select_percent) return (false);

		persons = file.Persons ();
		if (persons < 1) persons = 1;

		data.Household (hhold);
		data.Persons (persons);
			
		trip_index.Household (hhold);
		trip_index.Tour (0);
		trip_index.Trip (0);
		select_data.Type (0);
		select_data.Partition (partition);

		//---- save each person ----

		for (person = 1; person <= persons; person++) {
			trip_index.Person (person);

			map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));

			if (!map_stat.second) {
				Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
			}
		}
	}
	return (false);
}
