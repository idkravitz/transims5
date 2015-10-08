//*********************************************************
//	Draw_Links.cpp - write the link shapes
//*********************************************************

#include "ArcNet.hpp"

//#include <math.h>

//---------------------------------------------------------
//	Draw_Links
//---------------------------------------------------------

void ArcNet::Draw_Links (void)
{
	Show_Message (String ("Writing %s -- Record") % arcview_link.File_Type ());
	Set_Progress ();

	bool offset_flag, dir_flag, link_flag, centerline_flag;	
	int i, link_fld, index, lane, center, num_lanes;
	double side, start, length, width;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Points_Itr pt_itr;

	link_fld = link_db.Optional_Field ("LINK", "ID");
	offset_flag = (link_offset != 0.0);

	link_flag = arcview_link.Is_Open ();
	centerline_flag = arcview_center.Is_Open ();

	link_db.Rewind ();

	while (link_db.Read_Record ()) {
		Show_Progress ();

		map_itr = link_map.find (link_db.Get_Integer (link_fld));
		if (map_itr == link_map.end ()) continue;

		link_ptr = &link_array [map_itr->second];

		//---- link file ----

		if (link_flag) {
			arcview_link.parts.clear ();
			arcview_link.clear ();
			arcview_link.Copy_Fields (link_db);

			if (lanes_flag) {

				length = UnRound (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ());
				width = lane_width / 2.0;

				//---- draw each direction ----

				for (i=0; i < 2; i++) {

					if (i) {
						index = link_ptr->BA_Dir ();
						start = UnRound (link_ptr->Boffset ());
						dir_flag = (link_ptr->AB_Dir () >= 0);
					} else {
						index = link_ptr->AB_Dir ();
						start = UnRound (link_ptr->Aoffset ());
						dir_flag = (link_ptr->BA_Dir () >= 0);
					}
					if (index < 0) continue;

					dir_ptr = &dir_array [index];

					num_lanes = dir_ptr->Lanes () + dir_ptr->Left ();

					if (center_flag && !dir_flag) {
						center = num_lanes + dir_ptr->Right () + 1;
					} else {
						center = 1;
					}
					for (lane = dir_ptr->Left (); lane < num_lanes; lane++) {
						side = (2 + 2 * lane - center) * width;

						Link_Shape (link_ptr, i, points, start, length, side);

						arcview_link.parts.push_back ((int) arcview_link.size ());

						if (arrow_flag) Add_Arrow (points);

						for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
							arcview_link.push_back (*pt_itr);
						}
					}
				} 

			} else {

				if (offset_flag) {

					//---- draw each direction ----

					for (i=0; i < 2; i++) {
						side = link_offset;

						if (i) {
							if (center_flag && link_ptr->AB_Dir () < 0) side = 0.0;
							start = -1.0;
						} else {
							if (center_flag && link_ptr->BA_Dir () < 0) side = 0.0;
							start = 0.0;
						}
						Link_Shape (link_ptr, i, points, start, -1.0, side);

						arcview_link.parts.push_back ((int) arcview_link.size ());

						if (arrow_flag) Add_Arrow (points);

						for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
							arcview_link.push_back (*pt_itr);
						}
					}

				} else {
					
					//---- draw centerline -----

					arcview_link.parts.push_back ((int) arcview_link.size ());

					if (direction_flag) {
						Link_Shape (link_ptr, 0, arcview_link);

						if (arrow_flag) Add_Arrow (arcview_link);

					} else if (link_ptr->AB_Dir () < 0) {
						Link_Shape (link_ptr, 1, arcview_link);

						if (arrow_flag) Add_Arrow (arcview_link);

					} else {
						Link_Shape (link_ptr, 0, arcview_link);

						if (arrow_flag && link_ptr->BA_Dir () < 0) {
							Add_Arrow (arcview_link);
						}
					}
				}
			}
			if (!arcview_link.Write_Record ()) {
				Error (String ("Writing %s") % arcview_link.File_Type ());
			}
		}

		//---- centerline file ----

		if (centerline_flag) {
			arcview_center.clear ();
			arcview_center.Copy_Fields (link_db);

			//---- draw centerline -----

			Link_Shape (link_ptr, 0, arcview_center);

			if (!arcview_center.Write_Record ()) {
				Error (String ("Writing %s") % arcview_center.File_Type ());
			}
		}
	}
	End_Progress ();

	if (link_flag) {
		arcview_link.Close ();
	}
	if (centerline_flag) {
		arcview_center.Close ();
	}
}

