//*********************************************************
//	Get_Connect_Data.cpp - read the lane connectivity file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Connect_Data
//---------------------------------------------------------

bool ArcNet::Get_Connect_Data (Connect_File &file, Connect_Data &connect_rec)
{
	int dir_in, dir_out;
	double side_in, side_out;

	if (Data_Service::Get_Connect_Data (file, connect_rec)) {
		if (arcview_connect.Is_Open ()) {
			arcview_connect.Copy_Fields (file);
			
			arcview_connect.parts.clear ();
			arcview_connect.clear ();

			dir_in = connect_rec.Dir_Index ();
			dir_out = connect_rec.To_Index ();

			if (lanes_flag) {
				int center_in, center_out;
				double width;

				Points_Itr pt_itr;
				Lane_Map_Array lane_map;
				Lane_Map_Itr map_itr;

				width = lane_width / 2.0;
				center_in = center_out = 1;

				if (center_flag) {
					Link_Data *link_ptr;
					Dir_Data *dir_ptr;

					//---- gather inbound data ----

					dir_ptr = &dir_array [dir_in];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
						center_in = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
					}

					//---- gather outbound data ----

					dir_ptr = &dir_array [dir_out];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
						center_out = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
					}
				}
				
				//---- get the lane map ----

				Lane_Map (&connect_rec, lane_map);

				//---- draw each connection ----

				for (map_itr = lane_map.begin (); map_itr != lane_map.end (); map_itr++) {
					side_in = (2 + 2 * map_itr->In_Lane () - center_in) * width;
					side_out = (2 + 2 * map_itr->Out_Lane () - center_out) * width;

					Turn_Shape (dir_in, dir_out, points, turn_shape_setback, curve_flag, side_in, side_out);

					arcview_connect.parts.push_back ((int) arcview_connect.size ());

					for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
						arcview_connect.push_back (*pt_itr);
					}
				}

			} else {
				side_in = side_out = link_offset;

				arcview_connect.parts.push_back ((int) arcview_connect.size ());

				Turn_Shape (dir_in, dir_out, arcview_connect, turn_shape_setback, curve_flag, side_in, side_out);
			}
			if (!arcview_connect.Write_Record ()) {
				Error (String ("Writing %s") % arcview_connect.File_Type ());
			}
		}
		return (true);
	}
	return (false);
}
