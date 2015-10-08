//*********************************************************
//	Bearing_Update.cpp - calculate the entry/exit bearings
//*********************************************************

#include "NetPrep.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Bearing_Update
//---------------------------------------------------------

void NetPrep::Bearing_Update (void)
{
	int index, bearing_in, bearing_out, link;

	Dir_Data *dir_ptr;
	Link_Itr link_itr;
	Int_Map_Itr map_itr;

	Show_Message ("Bearing Updates -- Record");
	Set_Progress ();

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();
		if (link_itr->Length () == 0) continue;

		//---- calculate link end bearings ----

		if (shape_flag) {
			map_itr = shape_map.find (link_itr->Link ());

			if (map_itr != shape_map.end ()) {
				link_itr->Shape (map_itr->second);
			} else {
				link_itr->Shape (-1);
			}
		} else {
			link_itr->Shape (-1);
		}
		Link_Bearings (*link_itr, bearing_in, bearing_out);

		index = link_itr->AB_Dir ();

		if (index >= 0) { 
			dir_ptr = &dir_array [index];
			dir_ptr->In_Bearing (bearing_in);
			dir_ptr->Out_Bearing (bearing_out);
		}
		index = link_itr->BA_Dir ();

		if (index >= 0) { 
			dir_ptr = &dir_array [index];
			dir_ptr->In_Bearing (compass.Flip (bearing_out));
			dir_ptr->Out_Bearing (compass.Flip (bearing_in));
		}
	}
	End_Progress ();
}
