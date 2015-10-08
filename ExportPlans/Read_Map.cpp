//*********************************************************
//	Read_Map.cpp - read the location zone map file
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	Read_Map
//---------------------------------------------------------

void ExportPlans::Read_Map (void)
{
	int location, zone;
	String result, text;

	Int_Map_Stat map_stat;

	//---- read the map file ----

	while (map_file.Read ()) {

		text = map_file.Record_String ();
		if (text.empty ()) continue;

		if (!text.Split (result)) continue;

		location = result.Integer ();
		if (location == 0) continue;

		text.Split (result);

		zone = result.Integer ();
		if (zone == 0) continue;

		map_stat = loc_zone_map.insert (Int_Map_Data (location, zone));

		if (!map_stat.second) {
			Warning ("Location ") << location << " is mapped to " << map_stat.first->second << " and " << zone;
		} 
	}
	Write (1, "Number of Location Maps = ") << loc_zone_map.size ();
	Show_Message (1);
	map_file.Close ();
}
