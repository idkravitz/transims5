//*********************************************************
//	Draw_Pockets.cpp - draw pocket lanes
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Draw_Pockets
//---------------------------------------------------------

void ArcNet::Draw_Pockets (void)
{
	Show_Message (String ("Writing %s -- Record") % arcview_pocket.File_Type ());
	Set_Progress ();

	int i, index_fld, center, dir, index, type, lane, num_lanes;
	double length, offset, side, width, setbacka, setbackb;
	bool dir_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Pocket_Data *pocket_ptr;
	Points_Itr pt_itr;

	index_fld = pocket_db.Required_Field ("POCKET_INDEX");
	width = lane_width / 2.0;

	pocket_db.Rewind ();

	while (pocket_db.Read_Record ()) {
		Show_Progress ();

		arcview_pocket.Copy_Fields (pocket_db);

		index = pocket_db.Get_Integer (index_fld);

		pocket_ptr = &pocket_array [index];

		type = pocket_ptr->Type ();
		num_lanes = pocket_ptr->Lanes ();
		offset = UnRound (pocket_ptr->Offset ());
		length = UnRound (pocket_ptr->Length ());

		dir_ptr = &dir_array [pocket_ptr->Dir_Index ()];
		dir = dir_ptr->Dir ();

		link_ptr = &link_array [dir_ptr->Link ()];

		if (dir) {
			dir_flag = (link_ptr->AB_Dir () >= 0);
		} else {
			dir_flag = (link_ptr->BA_Dir () >= 0);
		}

		//---- draw the pocket lanes ----

		if (lanes_flag) {
			if (type == LEFT_TURN || type == LEFT_MERGE) {
				lane = dir_ptr->Left () - num_lanes;
			} else {
				lane = dir_ptr->Lanes () + dir_ptr->Left ();
			}
			setbacka = UnRound ((dir) ? link_ptr->Boffset () : link_ptr->Aoffset ());
			setbackb = UnRound ((dir) ? link_ptr->Aoffset () : link_ptr->Boffset ());
			side = UnRound (link_ptr->Length ()) - setbacka - setbackb;

			if (type == LEFT_TURN || type == RIGHT_TURN) {
				length -= setbackb;
				if (offset < setbacka) offset = setbacka;
			} else {
				length -= setbacka;
				offset = setbacka;
			}
			if (length > side) length = side;

			if (center_flag) {
				if (!dir_flag) {
					center = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
			} else {
				center = 1;
			}
			arcview_pocket.parts.clear ();
			arcview_pocket.clear ();

			for (i=0; i < num_lanes; i++, lane++) {
				side = (2 + 2 * lane - center) * width;

				Link_Shape (link_ptr, dir, points, offset, length, side);

				arcview_pocket.parts.push_back ((int) arcview_pocket.size ());
					
				if (arrow_flag) {
					if (type == LEFT_TURN || type == RIGHT_TURN || type == AUX_LANE) {
						Add_Arrow (points);
					}
				}
				for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
					arcview_pocket.push_back (*pt_itr);
				}
			}
		} else {
			if (type == LEFT_TURN || type == LEFT_MERGE) {
				side = 0.0;
			} else {
				side = pocket_side;
			}
			Link_Shape (link_ptr, dir, arcview_pocket, offset, length, side);
		}
		if (!arcview_pocket.Write_Record ()) {
			Error (String ("Writing %s") % arcview_pocket.File_Type ());
		}
	}
	End_Progress ();
	arcview_pocket.Close ();
}

