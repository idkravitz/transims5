//*********************************************************
//	Draw_Vehicle.cpp - draw the vehicle shape
//*********************************************************

#include "ArcSnapshot.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Draw_Vehicle
//---------------------------------------------------------

void ArcSnapshot::Draw_Vehicle (void)
{
	int i, center, link, dir, lane, type, num_pts, num, cell;
	double offset, side, veh_width, veh_len;
	bool front_flag;

	XYZ_Point pt1, pt2;
	Points pts;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr map_itr;

	arcview_snapshot.Get_Field (link_field, link);
	arcview_snapshot.Get_Field (dir_field, dir);

	if (link < 0) {
		link = -link;
		dir = 1;
	}
	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("Snapshot Link %d was Not Found") % link);
		return;
	}
	link_ptr = &link_array [map_itr->second];

	//---- check the subarea flag ----

	if (link_ptr->Use () == 0) return;

	arcview_snapshot.Get_Field (type_field, type);
	if (type == 0) type = 1;

	arcview_snapshot.Get_Field (offset_field, offset);
	arcview_snapshot.Get_Field (lane_field, lane);

	if (cell_field >= 0) {
		arcview_snapshot.Get_Field (cell_field, cell);
		front_flag = (cell == 0);
	} else if (occupancy_flag) {
		front_flag = false;
	} else {
		front_flag = true;
	}
	if (shape_flag) {
		if (cell_field >= 0 || occupancy_flag) {
			veh_len = cell_size;
		} else {
			if (type >= max_type) type = 1;
			veh_len = veh_length [type];
		}
		if (veh_len > offset) {
			veh_len = offset;
		}
		offset -= veh_len;
		if (offset < 0) offset = 0;

		if (front_flag) {
			veh_len -= Internal_Units (1.0, METERS);
		}
	} else {
		offset -= veh_len = Internal_Units (2.0, METERS);
	}
	if (arcview_snapshot.LinkDir_Type () == LINK_NODE) {		//***** from node *****
		if (dir == link_ptr->Anode ()) {
			dir = 0;
		} else if (dir == link_ptr->Bnode ()) {
			dir = 1;
		} else {
			Warning (String ("Snapshot Node %d not on Link %d") % dir % link);
			return;
		}
	}	
	if (dir == 1) {
		offset = UnRound (link_ptr->Length ()) - offset;
		dir_ptr = &dir_array [link_ptr->BA_Dir ()];
	} else {
		dir_ptr = &dir_array [link_ptr->AB_Dir ()];
	}
	lane = Convert_Lane_ID (dir_ptr, lane) + 1;

	if (center_flag) {
		if (link_ptr->BA_Dir () < 0) {
			dir_ptr = &dir_array [link_ptr->AB_Dir ()];
			center = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
		} else if (link_ptr->AB_Dir () < 0) {
			dir_ptr = &dir_array [link_ptr->BA_Dir ()];
			center = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
		} else {
			center = 1;
		}
	} else {
		center = 1;
	}
	side = (2 * lane - center) * lane_width / 2.0;
	num_pts = 0;

	if (shape_flag) {
		Link_Shape (link_ptr, dir, points, offset, veh_len, side);

		veh_width = lane_width * 0.80;

		pt1 = points [points.size ()-1];
		pt2 = points [0];

		if (snapshot_flag) {
			if (num_pts > 2) {
				num = num_pts - 1;

				pts [0] = pt1;
				pts [num] = pt2;

				for (i=1; i < num; i++) {
					Link_Shape (link_ptr, dir, points, offset, veh_len * (num - i) / num, side);

					pts [i] = points [points.size () - 1];
				}
				Vehicle_Shape (pts, veh_width, arcview_snapshot, front_flag);
			} else {
				Vehicle_Shape (pt1, pt2, veh_width, arcview_snapshot, front_flag);
			}
		}
	} else {
		if (snapshot_flag) {
			Link_Shape (link_ptr, dir, points, offset, 0.0, side);

			arcview_snapshot.assign (1, points [0]);
		}
		if (output_flag) {
			Link_Shape (link_ptr, dir, points, offset, veh_len, side);

			pt1 = points [points.size () - 1];
			pt2 = points [0];
		}
	}
	if (snapshot_flag && !arcview_snapshot.Write_Record ()) {
		Error ("Writing ArcView Snapshot File");
	}
	nsaved++;

	if (output_flag) {
		double dx, dy, bear;

		new_snapshot_file.Copy_Fields (arcview_snapshot);

		dx = pt1.x - pt2.x;
		dy = pt1.y - pt2.y;
		bear = compass.Direction (dx, dy);

		if (project_flag) {
			project_coord.Convert (&(pt1.x), &(pt1.y));
		}
		new_snapshot_file.X (pt1.x);
		new_snapshot_file.Y (pt1.y);
		new_snapshot_file.Z (0.0);
		new_snapshot_file.Bearing (bear);

		if (!new_snapshot_file.Write ()) {
			Error (String ("Writing %s") % new_snapshot_file.File_Type ());
		}
	}
}

