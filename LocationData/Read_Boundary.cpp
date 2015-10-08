//*********************************************************
//	Read_Boundary.cpp - read the zone boundary file 
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Boundary
//---------------------------------------------------------

void LocationData::Read_Boundary (void)
{
	int zone;
	Points_Map_Stat map_stat;

	//---- read zone polygons----

	Show_Message (String ("Reading %s -- Record") % boundary_file.File_Type ());
	Set_Progress ();

	while (boundary_file.Read_Record ()) {
		Show_Progress ();

		//---- get the zone number ----

		zone = boundary_file.Get_Integer (zone_field);
		if (zone == 0) continue;

		if (zone_map.find (zone) == zone_map.end ()) continue;
		if (range_flag && !zone_range.In_Range (zone)) continue;

		map_stat = boundary_map.insert (Points_Map_Data (zone, boundary_file));
		if (!map_stat.second) {
			Warning ("Duplicate Zone Number = ") << zone;
		}
	}
	End_Progress ();

	boundary_file.Close ();

	Print (2, "Number of ") << boundary_file.File_Type () << " Records = " << boundary_map.size ();
}
