//*********************************************************
//	Read_Signs.cpp - read the sign file
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Signs
//---------------------------------------------------------

void IntControl::Read_Sign (void)
{
	int link, dir, dir_index, node, sign;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Node_Data *node_ptr;

	//---- store the sign data ----

	Show_Message (String ("Reading %s -- Record") % sign_file.File_Type ());
	Set_Progress ();

	while (sign_file.Read ()) {
		Show_Progress ();

		//---- sign code ----

		sign = sign_file.Sign ();
		if (sign == NO_CONTROL || sign == SIGNAL_GROUP) continue;

		//---- check/convert the link number ----

		link = sign_file.Link ();
		dir = sign_file.Dir ();

		link_ptr = Set_Link_Direction (sign_file, link, dir);

		if (link_ptr == 0) continue;

		if (dir == 1) {
			dir_index = link_ptr->BA_Dir ();
			node = link_ptr->Anode ();
		} else {
			dir_index = link_ptr->AB_Dir ();
			node = link_ptr->Bnode ();
		}
		node_ptr = &node_array [node];

		if (node_ptr->Control () >= 0) {
			Warning (String ("Node %d has a Sign/Signal control conflict") % node_ptr->Node ());
			continue;
		}
		if (dir_index < 0) {
			Warning (String ("Sign %d Link %d Direction %s was Not Found") % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
			continue;
		}
		dir_ptr = &dir_array [dir_index];

		if (dir_ptr->Sign () > 0) {
			Warning ("Duplicate Sign Record on Link ") << link_ptr->Link ();
			continue;
		}
		node_ptr->Control (-2);
		dir_ptr->Sign (sign);
		nsign++;
	}
	End_Progress ();

	sign_file.Close ();

	Print (2, String ("Number of %s Records = %d") % sign_file.File_Type () % Progress_Count ());
	if (nsign != Progress_Count ()) {
		Print (1, "Number of New Sign Records = ") << nsign;
	}
}
