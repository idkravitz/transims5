//*********************************************************
//	Input_Nodes.cpp - convert the input node data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Input_Nodes
//---------------------------------------------------------

void NetPrep::Input_Nodes (void)
{
	int node, node_field, x_field, y_field, z_field;
	double x, y, z;

	Db_Header *file;
	Points_Itr pt_itr;
	Node_Data node_rec;
	Zone_Data zone_rec;
	Int_Map_Stat map_stat;

	if (node_shape_flag) {
		file = &node_shape_file;

		x_field = y_field = z_field = -1;
	} else {
		file = &node_file;

		x_field = file->Required_Field (X_FIELD_NAMES);
		y_field = file->Required_Field (Y_FIELD_NAMES);
		z_field = file->Optional_Field (Z_FIELD_NAMES);
	}
	node_field = file->Required_Field (NODE_FIELD_NAMES);

	//---- read each node record----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	while (((node_shape_flag) ? node_shape_file.Read_Record () : node_file.Read_Record ())) {
		Show_Progress ();

		//---- read the node data ----

		node = file->Get_Integer (node_field);
		if (node == 0) continue;

		if (int_zone_flag && int_zone_range.In_Range (node)) {
			if (centroid_flag) {
				if (node_shape_flag) {
					pt_itr = node_shape_file.begin ();

					x = pt_itr->x;
					y = pt_itr->y;
					z = pt_itr->z;
				} else {
					x = file->Get_Double (x_field);
					y = file->Get_Double (y_field);
					z = file->Get_Double (z_field);

					if (!projection.Convert (&x, &y)) {
						Error (String ("Converting Coordinate %lf, %lf") % x % y);
					}
				}

				//---- save the zone centroid -----

				zone_rec.Zone (node);
				zone_rec.X (Round (x));
				zone_rec.Y (Round (y));
				zone_rec.Z (Round (z));
				zone_rec.Area_Type (0);

				map_stat = zone_map.insert (Int_Map_Data (node, (int) zone_array.size ()));
				if (!map_stat.second) {
					Warning ("Duplicate Zone Number = ") << node;
				} else {
					zone_array.push_back (zone_rec);
				}
			}
			if (!connector_flag) continue;
		}
		if (node >= new_node) new_node = node + 1;

		if (drop_node_flag && drop_node_range.In_Range (node)) continue;

		if (node_shape_flag) {
			pt_itr = node_shape_file.begin ();

			x = pt_itr->x;
			y = pt_itr->y;
			z = pt_itr->z;
		} else {
			x = file->Get_Double (x_field);
			y = file->Get_Double (y_field);
			z = file->Get_Double (z_field);

			if (!projection.Convert (&x, &y)) {
				Error (String ("Converting Coordinate %lf, %lf") % x % y);
			}
		}

		//---- save to node data -----

		node_rec.Node (node);
		node_rec.X (Round (x));
		node_rec.Y (Round (y));
		node_rec.Z (Round (z));
		node_rec.Count (0);

		map_stat = node_map.insert (Int_Map_Data (node, (int) node_array.size ()));
		if (!map_stat.second) {
			Warning ("Duplicate Node Number = ") << node;
		} else {
			node_array.push_back (node_rec);
		}
	}
	End_Progress ();

	if (node_shape_flag) {
		node_shape_file.Close ();

		Print (2, String ("Number of %s Records = %d") % node_shape_file.File_Type () % Progress_Count ());
	} else {
		node_file.Close ();

		Print (2, String ("Number of %s Records = %d") % node_file.File_Type () % Progress_Count ());
	}
}
