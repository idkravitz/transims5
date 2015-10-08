//*********************************************************
//	Read_Boundary.cpp - read the zone boundary file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Boundary
//---------------------------------------------------------

void TransimsNet::Read_Boundary (void)
{
	int zone;
	Int_Map_Itr map_itr;
	Points_Map_Stat map_stat;

	//---- read zone polygons----

	Show_Message (String ("Reading %s -- Record") % boundary_file.File_Type ());
	Set_Progress ();

	while (boundary_file.Read_Record ()) {
		Show_Progress ();

		//---- get the zone number ----

		zone = boundary_file.Get_Integer (zone_field);
		if (zone > 0 && ext_zone_flag && ext_zone_range.In_Range (zone)) continue;

		map_itr = zone_map.find (zone);
		if (map_itr == zone_map.end ()) continue;

		zone = map_itr->second;

		map_stat = boundary_map.insert (Points_Map_Data (zone, boundary_file));
		if (!map_stat.second) {
			Warning ("Duplicate Zone Number = ") << map_itr->first;
		}
	}
	End_Progress ();

	boundary_file.Close ();

	Print (2, "Number of ") << boundary_file.File_Type () << " Records = " << boundary_map.size ();
}
