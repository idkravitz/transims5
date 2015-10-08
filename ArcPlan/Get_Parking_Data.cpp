//*********************************************************
//	Get_Parking_Data.cpp - additional parking processing
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool ArcPlan::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	int link, dir, index;
	double offset, side;

	Point_Map_Stat map_stat;
	Link_Data *link_ptr;

	if (Data_Service::Get_Parking_Data (file, parking_rec)) {
		if (!file.Nested ()) {
			link = parking_rec.Link ();
			dir = parking_rec.Dir ();
			offset = UnRound (parking_rec.Offset ());
			side = parking_side;

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
			Link_Shape (link_ptr, dir, points, offset, 0.0, side);

			//---- save the parking point data ----

			map_stat = parking_pt.insert (Point_Map_Data (parking_rec.Parking (), points [0]));

			if (!map_stat.second && file.Version () > 40) {
				Warning ("Duplicate Parking Record = ") << parking_rec.Parking ();
			}
		}
		return (true);
	}
	return (false);
}

