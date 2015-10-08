//*********************************************************
//	Get_Phasing_Data.cpp - draw signal phasing plan records
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Phasing_Data
//---------------------------------------------------------

int ArcNet::Get_Phasing_Data (Phasing_File &file, Phasing_Data &phasing_rec)
{
	int signal = Data_Service::Get_Phasing_Data (file, phasing_rec);

	if (signal >= 0) {
		if (arcview_phasing.Is_Open ()) {
			arcview_phasing.Copy_Fields (file);
		}
		if (!file.Nested ()) {
			if (time_flag) {
				Signal_Data *signal_ptr = &signal_array [signal];

				Signal_Time_Itr itr;
				bool keep = false;

				for (itr = signal_ptr->begin (); itr != signal_ptr->end (); itr++) {
					if (itr->Phasing () == phasing_rec.Phasing () && 
						itr->Start () <= time && time <= itr->End ()) {
						keep = true;
						break;
					}
				}
				if (!keep) {
					phasing_rec.Phasing (0);
					return (-1);
				}
			}
			return (signal);
		}
		if (arcview_phasing.Is_Open ()) {
			arcview_phasing.parts.clear ();
			arcview_phasing.clear ();

			if (phasing_rec.size () == 0) return (-1);

			Movement_Itr move_itr = --phasing_rec.end ();

			double side_in, side_out;

			Connect_Data *connect_ptr = &connect_array [move_itr->Connection ()];

			int dir_in = connect_ptr->Dir_Index ();
			int dir_out = connect_ptr->To_Index ();

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

				Lane_Map (connect_ptr, lane_map);

				//---- draw each connection ----

				for (map_itr = lane_map.begin (); map_itr != lane_map.end (); map_itr++) {
					side_in = (2 + 2 * map_itr->In_Lane () - center_in) * width;
					side_out = (2 + 2 * map_itr->Out_Lane () - center_out) * width;

					Turn_Shape (dir_in, dir_out, points, turn_shape_setback, curve_flag, side_in, side_out);

					arcview_phasing.parts.push_back ((int) arcview_phasing.size ());

					for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
						arcview_phasing.push_back (*pt_itr);
					}
				}
			} else {
				side_in = side_out = link_offset;

				arcview_phasing.parts.push_back ((int) arcview_phasing.size ());

				Turn_Shape (dir_in, dir_out, arcview_phasing, turn_shape_setback, curve_flag, side_in, side_out);
			}
			if (!arcview_phasing.Write_Record ()) {
				Error (String ("Writing %s") % arcview_phasing.File_Type ());
			}
		}
		return (signal);
	}
	return (-1);
}
