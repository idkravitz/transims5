//*********************************************************
//	Get_Location_Data.cpp - additional location processing
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool ArcPlan::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	int link, dir, index;
	double offset, setback, side;

	Point_Map_Stat map_stat;
	Link_Data *link_ptr;

	if (Data_Service::Get_Location_Data (file, location_rec)) {
		link = location_rec.Link ();
		dir = location_rec.Dir ();
		offset = UnRound (location_rec.Offset ());

		setback = UnRound (location_rec.Setback ());
		side = location_side;

		if (setback > side) side = setback;

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

		//---- save the location point data ----

		map_stat = location_pt.insert (Point_Map_Data (location_rec.Location (), points [0]));

		if (!map_stat.second) {
			Warning ("Duplicate Location Record = ") << location_rec.Location ();
		}
		return (true);
	}
	return (false);
}
