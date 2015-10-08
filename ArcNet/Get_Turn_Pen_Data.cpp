//*********************************************************
//	Get_Turn_Pen_Data.cpp - read the turn penalty file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Turn_Pen_Data
//---------------------------------------------------------

bool ArcNet::Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &turn_rec)
{
	int in, out;
	double side_in, side_out;

	if (Data_Service::Get_Turn_Pen_Data (file, turn_rec)) {
		if (time_flag) {
			if (turn_rec.Start () > time || time > turn_rec.End ()) return (false);
		}
		if (arcview_turn.Is_Open ()) {
			arcview_turn.Copy_Fields (file);

			arcview_turn.parts.clear ();
			arcview_turn.clear ();

			in = turn_rec.Dir_Index ();
			out = turn_rec.To_Index ();

			//---- process lane connections ----

			if (lanes_flag && System_Data_Flag (CONNECTION)) {
				int center_in, center_out;
				double width;

				Connect_Data *connect_ptr;
				Int2_Map_Itr map2_itr;
				Points_Itr pt_itr;
				Lane_Map_Array lane_map;
				Lane_Map_Itr map_itr;

				map2_itr = connect_map.find (Int2_Key (in, out));

				if (map2_itr == connect_map.end ()) {
					Dir_Data *dir_ptr;
					Link_Data *link_ptr;

					dir_ptr = &dir_array [in];
				
					link_ptr = &link_array [dir_ptr->Link ()];
					in = link_ptr->Link ();

					dir_ptr = &dir_array [out];

					link_ptr = &link_array [dir_ptr->Link ()];
					out = link_ptr->Link ();

					Warning (String ("Turn Penalty Lane Connections from %d to %d were Not Found") % in % out);
					return (false);
				}

				//---- draw the lane connections ----

				connect_ptr = &connect_array [map2_itr->second];

				width = lane_width / 2.0;
				center_in = center_out = 1;

				if (center_flag) {
					Link_Data *link_ptr;
					Dir_Data *dir_ptr;

					//---- gather inbound data ----

					dir_ptr = &dir_array [in];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
						center_in = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
					}

					//---- gather outbound data ----

					dir_ptr = &dir_array [out];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
						center_out = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
					}
				}
				
				//---- get the lane map ----

				Lane_Map (connect_ptr, lane_map);

				//---- draw each connection ----

				for (map_itr = lane_map.begin (); map_itr != lane_map.end (); map_itr++) {
					side_in = (2 + 2 * map_itr->In_Lane () - center_in) * width;
					side_out = (2 + 2 * map_itr->Out_Lane () - center_out) * width;

					Turn_Shape (in, out, points, turn_shape_setback, curve_flag, side_in, side_out);

					arcview_turn.parts.push_back ((int) arcview_turn.size ());

					for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
						arcview_turn.push_back (*pt_itr);
					}
				}
			} else {
				if (lanes_flag) {
					side_in = side_out = lane_width;
				} else {
					side_in = side_out = link_offset;
				}
				arcview_turn.parts.push_back ((int) arcview_turn.size ());

				Turn_Shape (in, out, arcview_turn, turn_shape_setback, curve_flag, side_in, side_out);
			}
			if (!arcview_turn.Write_Record ()) {
				Error (String ("Writing %s") % arcview_turn.File_Type ());
			}
		}
	}
	return (false);
}
