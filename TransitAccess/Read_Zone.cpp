//*********************************************************
//	Read_Zone.cpp - read the zone file
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Zone
//---------------------------------------------------------

void TransitAccess::Read_Zone (void)
{
	int zone, barrier, pef;
	double area;

	Int_Itr value_itr;
	Dbl_Itr factor_itr;
	Zone_Data zone_data;
	Zone_Map_Stat map_stat;
	Zone_Map_Itr zone_itr;
	Data_Range_Itr range_itr;

	bool barrier_flag = (location_map.size () > 0);

	Show_Message (String ("Reading %s -- Record") % zone_file.File_Type ());
	Set_Progress ();

	nzones = 0;
	memset (&zone_data, '\0', sizeof (zone_data));

	while (zone_file.Read ()) {
		Show_Progress ();

		zone = zone_file.Get_Integer (zone_fld);
		if (zone == 0) continue;

		zone_data.x_coord = zone_file.Get_Integer (zone_x_fld);
		zone_data.y_coord = zone_file.Get_Integer (zone_y_fld);

		if (zone_data.x_coord == 0 && zone_data.y_coord == 0) continue;

		zone_data.location = zone_file.Get_Integer (zone_loc_fld);
		zone_data.barrier = 0;

		if (barrier_flag) {
			for (barrier=1, range_itr = location_map.begin (); range_itr != location_map.end (); range_itr++, barrier++) {
				if (range_itr->In_Range (zone_data.location)) {
					zone_data.barrier = barrier;
					break;
				}
			}
			if (zone_data.barrier == 0) {
				Warning (String ("Location %d was Not Assigned to a Barrier Group") % zone_data.location);
				continue;
			}
		}
		area = zone_file.Get_Double (zone_area_fld);

		zone_data.distance = MIN (1.0, (0.75 * sqrt (area)));

		zone_data.walk = zone_file.Get_Integer (zone_long_fld);

		pef = zone_file.Get_Integer (zone_pef_fld);
		zone_data.weight = 1.0;

		for (value_itr = pef_values.begin (), factor_itr = pef_factors.begin (); value_itr != pef_values.end (); value_itr++, factor_itr++) {
			if (pef < *value_itr) {
				zone_data.weight = *factor_itr;
				break;
			}
		}
		map_stat = zone_map.insert (Zone_Map_Data (zone, zone_data));

		if (!map_stat.second) {
			Warning (String ("Duplicate Zone Number = %d") % zone);
		}
		if (zone > nzones) nzones = zone;
	}
	End_Progress ();
	zone_file.Close ();

	Print (2, "Number of Zone Records = ") << Progress_Count ();

	zone = (int) zone_map.size ();

	if (zone != Progress_Count ()) {
		Print (1, "Number of Zones Kept = ") << zone;
	}
	Print (1, "Highest Zone Number = ") << nzones;

	//---- get the CBD coordinates ----

	zone_itr = zone_map.find (cbd_zone);

	if (zone_itr == zone_map.end ()) {
		Error (String ("CBD Zone %d was Not Found") % cbd_zone);
	}
	cbd_x = zone_itr->second.x_coord;
	cbd_y = zone_itr->second.y_coord;
}
