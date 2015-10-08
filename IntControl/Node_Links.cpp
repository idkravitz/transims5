//*********************************************************
//	Node_Links.cpp - set the node and link data
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Node_Links
//---------------------------------------------------------

void IntControl::Node_Links (void)
{
	int index;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	List_Data list_rec, *list_a, *list_b;
	List_Itr list_itr;


	Show_Message ("Building Node Links -- Record");
	Set_Progress ();

	//---- allocate node list ----

	if (node_list.size () == 0) {
		node_list.assign (node_array.size (), list_rec);
	} else {
		for (list_itr = node_list.begin (); list_itr != node_list.end (); list_itr++) {
			list_itr->Clear ();
		}
	}

	//---- add link directions to the node list ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Length () == 0) continue;

		list_a = &node_list [link_itr->Anode ()];
		list_b = &node_list [link_itr->Bnode ()];

		if (link_itr->AB_Dir () >= 0) {
			index = link_itr->AB_Dir ();
			dir_ptr = &dir_array [index];

			dir_ptr->First_Turn_From (list_a->From_List ());
			dir_ptr->First_Turn_To (list_b->To_List ());

			list_a->From_List (index);
			list_b->To_List (index);
		}
		if (link_itr->BA_Dir () >= 0) {
			index = link_itr->BA_Dir ();
			dir_ptr = &dir_array [index];

			dir_ptr->First_Turn_From (list_b->From_List ());
			dir_ptr->First_Turn_To (list_a->To_List ());

			list_b->From_List (index);
			list_a->To_List (index);
		}
	}
	End_Progress ();
}
