//*********************************************************
//	Get_Sign_Data.cpp - additional sign processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Sign_Data
//---------------------------------------------------------

bool ArcNet::Get_Sign_Data (Sign_File &file, Sign_Data &sign_rec)
{
	if (Data_Service::Get_Sign_Data (file, sign_rec)) {
		if (arcview_sign.Is_Open ()) {
			arcview_sign.Copy_Fields (file);

			Dir_Data *dir_ptr = &dir_array [sign_rec.Dir_Index ()];
			int dir = dir_ptr->Dir ();

			Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

			double offset = UnRound (link_ptr->Length ()) - sign_setback;
			double side = sign_side;

			bool dir_flag;

			if (dir == 0) {
				offset -= UnRound (link_ptr->Boffset ());
				offset = MAX (offset, UnRound (link_ptr->Aoffset ()));
				dir_flag = (link_ptr->BA_Dir () >= 0);
			} else {
				offset -= UnRound (link_ptr->Aoffset ());
				offset = MAX (offset, UnRound (link_ptr->Boffset ()));
				dir_flag = (link_ptr->AB_Dir () >= 0);
			}
			if (lanes_flag) {
				int center, num_lanes;

				num_lanes = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ();

				if (center_flag && !dir_flag) {
					center = num_lanes + 1;
				} else {
					center = 1;
				}
				side += (2 * num_lanes - center) * lane_width / 2.0;
			}
			Link_Shape (link_ptr, dir, arcview_sign, offset, 0.0, side);

			if (!arcview_sign.Write_Record ()) {
				Error (String ("Writing %s") % arcview_sign.File_Type ());
			}
		}
	}
	return (false);
}
