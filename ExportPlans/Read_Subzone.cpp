//*********************************************************
//	Read_Subzone.cpp - read the subzone weight file
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	Read_Subzone
//---------------------------------------------------------

void ExportPlans::Read_Subzone (void)
{
	int location, count;
	String result, text;
	double total;

	Subzone_Weight subzone;
	Subzone_Array subzone_array, *subzone_ptr;
	Subzone_Itr subzone_itr;
	Subzone_Map_Stat map_stat;
	Subzone_Map_Itr map_itr;

	//---- read the subzone weights file ----

	count = 0;

	while (subzone_file.Read ()) {

		text = subzone_file.Record_String ();
		if (text.empty ()) continue;

		if (!text.Split (result)) continue;

		location = result.Integer ();
		if (location == 0) continue;

		text.Split (result);

		subzone.zone = result.Integer ();
		if (subzone.zone == 0) continue;

		if (!text.Split (result)) continue;

		subzone.weight = result.Double ();
		if (subzone.weight == 0) continue;

		map_stat = subzone_map.insert (Subzone_Map_Data (location, subzone_array));

		subzone_ptr = &map_stat.first->second;

		subzone_ptr->push_back (subzone);
		count++;
	}
	Write (1, "Number of Subzone Weights = ") << count;
	Show_Message (1);
	subzone_file.Close ();

	//---- normalize the weights ----

	for (map_itr = subzone_map.begin (); map_itr != subzone_map.end (); map_itr++) {
		total = 0.0;

		for (subzone_itr = map_itr->second.begin (); subzone_itr != map_itr->second.end (); subzone_itr++) {
			total += subzone_itr->weight;
		}
		if (total == 1.0 || total == 0.0) continue;
		total = 1.0 / total;

		for (subzone_itr = map_itr->second.begin (); subzone_itr != map_itr->second.end (); subzone_itr++) {
			subzone_itr->weight *= total;
		}
	}
}
