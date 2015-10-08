//*********************************************************
//	Delete_Link.cpp - remove a link from the connection lists
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Delete_Link
//---------------------------------------------------------

void NetPrep::Delete_Link (int link)
{
	int dir, node;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *dir2_ptr;
	Node_Data *node_ptr;
	List_Data *list_ptr;

	if (link < 0) return;

	dir2_ptr = 0;

	link_ptr = &link_array [link];
	link_ptr->Length (0);

	node = link_ptr->Anode ();
	list_ptr = &node_list [node];

	node_ptr = &node_array [node];
	node_ptr->Drop_Count ();

	for (dir = list_ptr->To_List (); dir >= 0; dir = dir_ptr->First_Connect_To ()) {
		dir_ptr = &dir_array [dir];

		if (link == dir_ptr->Link ()) {
			if (dir == list_ptr->To_List ()) {
				list_ptr->To_List (dir_ptr->First_Connect_To ());
			} else {
				dir2_ptr->First_Connect_To (dir_ptr->First_Connect_To ());
			}
		}
		dir2_ptr = dir_ptr;
	}
	for (dir = list_ptr->From_List (); dir >= 0; dir = dir_ptr->First_Connect_From ()) {
		dir_ptr = &dir_array [dir];

		if (link == dir_ptr->Link ()) {
			if (dir == list_ptr->From_List ()) {
				list_ptr->From_List (dir_ptr->First_Connect_From ());
			} else {
				dir2_ptr->First_Connect_From (dir_ptr->First_Connect_From ());
			}
		}
		dir2_ptr = dir_ptr;
	}
	node = link_ptr->Bnode ();
	list_ptr = &node_list [node];

	node_ptr = &node_array [node];
	node_ptr->Drop_Count ();

	for (dir = list_ptr->To_List (); dir >= 0; dir = dir_ptr->First_Connect_To ()) {
		dir_ptr = &dir_array [dir];

		if (link == dir_ptr->Link ()) {
			if (dir == list_ptr->To_List ()) {
				list_ptr->To_List (dir_ptr->First_Connect_To ());
			} else {
				dir2_ptr->First_Connect_To (dir_ptr->First_Connect_To ());
			}
		}
		dir2_ptr = dir_ptr;
	}
	for (dir = list_ptr->From_List (); dir >= 0; dir = dir_ptr->First_Connect_From ()) {
		dir_ptr = &dir_array [dir];

		if (link == dir_ptr->Link ()) {
			if (dir == list_ptr->From_List ()) {
				list_ptr->From_List (dir_ptr->First_Connect_From ());
			} else {
				dir2_ptr->First_Connect_From (dir_ptr->First_Connect_From ());
			}
		}
		dir2_ptr = dir_ptr;
	}
}
