//*********************************************************
//	Get_Stop_Data.cpp - additional transit stop processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool ArcNet::Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec)
{
	int link, dir, index;
	double offset, side;
	
	Point_Map_Stat map_stat;
	Link_Data *link_ptr;

	if (Data_Service::Get_Stop_Data (file, stop_rec)) {
		if (arcview_stop.Is_Open ()) {
			arcview_stop.Copy_Fields (file);

			link = stop_rec.Link ();
			dir = stop_rec.Dir ();
			offset = UnRound (stop_rec.Offset ());
			side = stop_side;

			link_ptr = &link_array [link];

			if (lanes_flag) {
				bool dir_flag;
				int center, num_lanes;
				Dir_Data *dir_ptr;

				if (dir == 0) {
					index = link_ptr->AB_Dir ();
					dir_flag = (link_ptr->BA_Dir () >= 0);
				} else {
					index = link_ptr->BA_Dir ();
					dir_flag = (link_ptr->AB_Dir () >= 0);
				}
				if (index >= 0) {
					dir_ptr = &dir_array [index];
					num_lanes = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ();
				} else {
					num_lanes = 0;
				}

				if (center_flag && !dir_flag) {
					center = num_lanes + 1;
				} else {
					center = 1;
				}
				side += (2 * num_lanes - center) * lane_width / 2.0;
			}
			Link_Shape (link_ptr, dir, arcview_stop, offset, 0.0, side);

			if (!arcview_stop.Write_Record ()) {
				Error (String ("Writing %s") % arcview_stop.File_Type ());
			}

			//---- save the location point data ----

			map_stat = stop_pt.insert (Point_Map_Data (stop_rec.Stop (), arcview_stop [0]));

			if (!map_stat.second) {
				Warning ("Duplicate Stop Record = ") << stop_rec.Stop ();
			}
		}
		return (true);
	}
	return (false);
}
