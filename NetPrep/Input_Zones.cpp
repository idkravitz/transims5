//*********************************************************
//	Input_Zones.cpp - convert the input zone data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Input_Zones
//---------------------------------------------------------

void NetPrep::Input_Zones (void)
{
	int zone, zone_field, x_field, y_field, z_field, at_field;
	double x, y, z;

	Db_Header *file;
	Points_Itr pt_itr;
	Node_Data *node_ptr;
	Zone_Data zone_rec, *zone_ptr;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;

	if (zone_shape_flag) {
		file = &zone_shape_file;
	} else {
		file = &zone_file;
	}
	zone_field = file->Required_Field (ZONE_FIELD_NAMES);
	x_field = file->Optional_Field (X_FIELD_NAMES);
	y_field = file->Optional_Field (Y_FIELD_NAMES);
	z_field = file->Optional_Field (Z_FIELD_NAMES);
	at_field = file->Optional_Field (AREA_TYPE_FIELD_NAMES);

	//---- read each zone record----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	while (((zone_shape_flag) ? zone_shape_file.Read_Record () : zone_file.Read_Record ())) {
		Show_Progress ();

		//---- read the zone data ----

		zone = file->Get_Integer (zone_field);
		if (zone == 0) continue;

		if (zone > Max_Zone_Number ()) Max_Zone_Number (zone);

		map_stat = zone_map.insert (Int_Map_Data (zone, (int) zone_array.size ()));
		if (!map_stat.second) {
			zone_ptr = &zone_array [map_stat.first->second];
			zone_ptr->Area_Type (file->Get_Integer (at_field));
		} else {
			if (x_field >= 0 && y_field >= 0) {
				x = file->Get_Double (x_field);
				y = file->Get_Double (y_field);
				z = file->Get_Double (z_field);

				if (!projection.Convert (&x, &y)) {
					Error (String ("Converting Coordinate %lf, %lf") % x % y);
				}
			} else {
				map_itr = node_map.find (zone);
				if (map_itr == node_map.end ()) {
					if (zone_shape_flag) {
						pt_itr = zone_shape_file.begin ();

						x = pt_itr->x;
						y = pt_itr->y;
						z = pt_itr->z;
					} else {
						x = y = z = 0.0;
					}
				} else {
					node_ptr = &node_array [map_itr->second];
					x = UnRound (node_ptr->X ());
					y = UnRound (node_ptr->Y ());
					z = UnRound (node_ptr->Z ());
				}
			}
			zone_rec.Zone (zone);
			zone_rec.X (Round (x));
			zone_rec.Y (Round (y));
			zone_rec.Z (Round (z));
			zone_rec.Area_Type (file->Get_Integer (at_field));

			zone_array.push_back (zone_rec);
		}
	}
	End_Progress ();

	if (zone_shape_flag) {
		zone_shape_file.Close ();

		Print (2, String ("Number of %s Records = %d") % zone_shape_file.File_Type () % Progress_Count ());
	} else {
		zone_file.Close ();

		Print (2, String ("Number of %s Records = %d") % zone_file.File_Type () % Progress_Count ());
	}
}
