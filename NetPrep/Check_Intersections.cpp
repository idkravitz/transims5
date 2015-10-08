//*********************************************************
//	Check_Intersections.cpp - check intersection types
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Check_Intersections
//---------------------------------------------------------

void NetPrep::Check_Intersections (void)
{
	int index, node, link1, link2, count;
	bool freeway_flag, arterial_flag;

	Int_Map_Itr map_itr;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	List_Data *list;

	Show_Message ("Checking Intersection Types -- Record");
	Set_Progress ();

	//---- find intersections between freeways and non-freeway/ramps ----

	Header_Number (CHECK_TYPES);
	New_Page ();
	count = 0;

	for (map_itr = node_map.begin (); map_itr != node_map.end (); map_itr++) {
		Show_Progress ();

		freeway_flag = arterial_flag = false;
		link1 = link2 = 0;

		node = map_itr->second;

		list = &node_list [node];

		for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
			dir_ptr = &dir_array [index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Type () == FREEWAY) {
				freeway_flag = true;
				link1 = link_ptr->Link ();
			} else if (link_ptr->Type () > EXPRESSWAY && link_ptr->Type () < RAMP) {
				arterial_flag = true;
				link2 = link_ptr->Link ();
			}
		}
		for (index = list->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
			dir_ptr = &dir_array [index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Type () == FREEWAY) {
				freeway_flag = true;
				link1 = link_ptr->Link ();
			} else if (link_ptr->Type () > EXPRESSWAY && link_ptr->Type () < RAMP) {
				arterial_flag = true;
				link2 = link_ptr->Link ();
			}
		}
		if (freeway_flag && arterial_flag) {
			node_ptr = &node_array [node];
			Print (1, String ("%10d  %10d  %10d") % node_ptr->Node () % link1 % link2);
			count++;
		}
	}
	End_Progress ();

	if (count > 0) {
		Print (2, "Number of Records in the Report = ") << count;
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Intersection_Header
//---------------------------------------------------------

void NetPrep::Intersection_Header (void)
{
	Print (1, "Check Intersection Types");
	Print (2, "      Node     Freeway    Arterial");
	Print (1);
}

/*********************************************|***********************************************

	Check Intersection Types

	      Node     Freeway    Arterial

	dddddddddd  dddddddddd  dddddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
