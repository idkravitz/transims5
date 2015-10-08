//*********************************************************
//	Get_Location_Data.cpp - Read the Location File
//*********************************************************

#include "LocationData.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool LocationData::Get_Location_Data (Location_File &file, Location_Data &data)
{
	if (Data_Service::Get_Location_Data (file, data)) {

		int zone = file.Zone ();

		if (boundary_flag && (!range_flag || zone_range.In_Range (zone))) {
			double x, y;
			Points_Map_Itr boundary_itr;

			x = UnRound (data.X ());
			y = UnRound (data.Y ());

			//---- check current zone number ----

			boundary_itr = boundary_map.find (zone);

			if ((boundary_itr != boundary_map.end () && !In_Polygon (boundary_itr->second, x, y)) ||
				boundary_itr == boundary_map.end ()) {
				zone = 0;

				for (boundary_itr = boundary_map.begin (); boundary_itr != boundary_map.end (); boundary_itr++) {
					if (In_Polygon (boundary_itr->second, x, y)) {
						zone = boundary_itr->first;
						break;
					}
				}
				if (zone <= 0) {
					zone = file.Zone ();
					Warning (String ("Location %d was not within a Zone Polygon") % data.Location ());
				} else {
					if (System_Data_Flag (ZONE)) {
						Int_Map_Itr map_itr = zone_map.find (zone);
						if (map_itr != zone_map.end ()) {
							data.Zone (map_itr->second);
						}
					} else {
						data.Zone (zone);
					}
				}
			}
		}

		//---- update the zone number ----

		if (coverage_flag || zone_loc_flag) {
			Int_Map_Stat int_stat;

			int_stat = loc_zone_map.insert (Int_Map_Data (data.Location (), zone));
			if (!int_stat.second) {
				Warning ("Duplicate Location Number ") << data.Location ();
			}
		}

		//---- count the records for the data file -----
	
		for (Data_Itr data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
			Db_Sort_Array *data = data_itr->data_db;

			int field = data_itr->loc_field;

			if (field != file.Zone_Field ()) {
				zone = file.Get_Integer (field);
			}
			if (zone < 0) continue;

			if (data->Read_Record (zone)) {
				int count = data->Get_Integer (1);
				data->Put_Field (1, ++count);
				data->Write_Record (zone);
			}
		}
		return (true);
	}
	return (false);
}
