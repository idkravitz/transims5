//*********************************************************
//	Selection_Data.cpp - selection data map
//*********************************************************

#include "Selection_Data.hpp"

//---------------------------------------------------------
//	Best
//---------------------------------------------------------

Select_Map_Itr Select_Map::Best (int hhold, int person, int tour, int trip)
{
	Trip_Index trip_index, index;
	Select_Map_Itr map_itr;

	trip_index.Set (hhold, person, tour, trip);

	map_itr = find (trip_index);
	if (map_itr == end ()) {
		trip_index.Person (0);
		trip_index.Tour (0);
		trip_index.Trip (0);

		map_itr = find (trip_index);
		if (map_itr == end ()) {
			trip_index.Person (person);

			map_itr = find (trip_index);
			if (map_itr == end ()) {
				trip_index.Tour (tour);

				return (find (trip_index));
			}
		}
	}
	return (map_itr);
}
#ifdef MAKE_MPI
bool Select_Map::Pack (Data_Buffer &data)
{
	size_t num = size ();
	if (data.Add_Data (&num, sizeof (num))) {
		if (num == 0) return (true);
		Select_Map_Itr itr;
		Selection_Data rec;
		for (itr = begin (); itr != end (); itr++) {
			Trip_Index index = itr->first;
			rec.Household (index.Household ());
			rec.Person (index.Person ());
			rec.Tour (index.Tour ());
			rec.Trip (index.Trip ());
			rec.Type (itr->second.Type ());
			rec.Partition (itr->second.Partition ());
			if (!data.Add_Data (&rec, sizeof (rec))) return (false);
		}
	}
	return (false);
}
bool Select_Map::UnPack (Data_Buffer &data)
{
	size_t num;
	if (data.Get_Data (&num, sizeof (num))) {
		Selection_Data record;
		Trip_Index index;
		Select_Data rec;

		while (num-- > 0) {
			if (!data.Get_Data (&record, sizeof (record))) return (false);

			record.Get_Trip_Index (index);
			rec.Type (record.Type ());
			rec.Partition (record.Partition ());

			insert (Select_Map_Data (index, rec));
		}
		return (true);
	}
	return (false);
}
#endif
