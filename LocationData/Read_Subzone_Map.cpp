//*********************************************************
//	Read_Subzone_Map.cpp - read the subzone zone map file
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Subzone_Map
//---------------------------------------------------------

void LocationData::Read_Subzone_Map (void)
{
	int i, id, zone, num_field, field;
	double factor;
	int id_field, zone_field [20], fac_field [20];
	String name1, name2, name3, name4;
	I2_Dbl_Map_Stat map_stat;

	//---- get the field names ----

	id_field = subzone_zone_file.Required_Field ("ID", "SUBZONE", "RECORD", "REC_ID", "SUB_ID");

	num_field = 0;

	for (i=1; i <= 20; i++) {
		name1 = String ("ZONE_%d") % i;
		name2 = String ("TAZ_%d") % i;
		name3 = String ("ZONE%d") % i;
		name4 = String ("TAZ%d") % i;

		field = subzone_zone_file.Optional_Field (name1.c_str (), name2.c_str (), name3.c_str (), name4.c_str ());

		if (field < 0) break;
		num_field = i;
		zone_field [i-1] = field;
		
		name1 = String ("ZONE_%d_PER") % i;
		name2 = String ("TAZ_%d_PER") % i;
		name3 = String ("ZONE%d_PER") % i;
		name4 = String ("TAZ%d_PER") % i;

		field = subzone_zone_file.Optional_Field (name1.c_str (), name2.c_str (), name3.c_str (), name4.c_str ());

		if (field < 0) {
			name1 = String ("ZONE_%d_FAC") % i;
			name2 = String ("TAZ_%d_FAC") % i;
			name3 = String ("ZONE%d_FAC") % i;
			name4 = String ("TAZ%d_FAC") % i;

			field = subzone_zone_file.Optional_Field (name1.c_str (), name2.c_str (), name3.c_str (), name4.c_str ());
		}
		if (field < 0) {
			Error (String ("A Subzone Factor Field Name for %d was Not Found") % 
				subzone_zone_file.Field (zone_field [i-1])->Name ());
		}
		fac_field [i-1] = field;
	}
	if (num_field == 0) {
		Error ("No Subzone Zone Field Names were Found");
	}

	//---- read the subzone zone file ----

	Show_Message (String ("Reading %s -- Record") % subzone_zone_file.File_Type ());
	Set_Progress ();

	while (subzone_zone_file.Read ()) {
		Show_Progress ();

		id = subzone_zone_file.Get_Integer (id_field);
		if (id == 0) continue;

		for (i=0; i < num_field; i++) {
			zone = subzone_zone_file.Get_Integer (zone_field [i]);
			if (zone == 0) break;

			factor = subzone_zone_file.Get_Double (fac_field [i]);

			if (factor == 0.0) continue;
			if (factor > 1.0) factor /= 100.0;

			map_stat = subzone_map.insert (I2_Dbl_Map_Data (Int2_Key (zone, id), factor));

			if (!map_stat.second) {
				Warning (String ("Duplicate Subzone Map Record for Subzone %d Zone %d") % id % zone);
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of Subzone Zone Data Records = %d") % Progress_Count ());

	subzone_zone_file.Close ();
}
