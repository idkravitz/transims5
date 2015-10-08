//*********************************************************
//	Read_Subzone.cpp - Read the Subzone Data File
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Subzone
//---------------------------------------------------------

void LocationData::Read_Subzone (void)
{
	int id, zone, count;
	double factor, field;

	Sub_Group_Itr sub_itr;
	Int2_Key key;
	Int2_Map_Stat map_stat;
	I2_Dbl_Map_Itr map_itr;
	Subzone_Data data;

	for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {

		if (sub_itr->loc_field < 0) continue;

		sub_itr->data.reserve (sub_itr->file->Num_Records ());

		//---- read the subzone file ----

		Show_Message (String ("Reading %s -- Record") % sub_itr->file->File_Type ());
		Set_Progress ();

		count = 0;

		while (sub_itr->file->Read ()) {
			Show_Progress ();

			id = sub_itr->file->ID ();
			if (id == 0) continue;

			field = sub_itr->file->Get_Double (sub_itr->data_field);
			if (field == 0.0 && sub_itr->max_distance > 0) continue;

			//---- save the subzone data ----

			zone = sub_itr->file->Zone ();

			if (subzone_map_flag) {
				key.first = zone;
				key.second = id;

				map_itr = subzone_map.find (key);

				if (map_itr == subzone_map.end ()) {
					factor = 1.0;
					subzone_map.insert (I2_Dbl_Map_Data (key, factor));
				}
				data.Zone (0);
			} else {
				data.Zone (zone);
			}
			data.ID (id);
			data.X (sub_itr->file->X ());
			data.Y (sub_itr->file->Y ());
			data.Data (field);

			key.first = data.Zone ();
			key.second = data.ID ();

			map_stat = sub_itr->data_map.insert (Int2_Map_Data (key, (int) sub_itr->data.size ()));

			if (!map_stat.second) {
				Warning (String ("Duplicate Subzone Data for Zone %d Subzone %d") % key.first % key.second);
			} else {
				sub_itr->data.push_back (data);
				count++;
			}
		}
		End_Progress ();

		Print (2, String ("Number of Subzone #%d Data Records = %d") % sub_itr->group % count);

		sub_itr->file->Close ();
	}
}
