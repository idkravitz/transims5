//*********************************************************
//	Node_Types.cpp - intersection types report
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Node_Types
//---------------------------------------------------------

void NetPrep::Node_Types (void)
{
	int index, node, count, type1, type2, at, area_type, bear_in, bear_out, change, min_angle, max_angle;

	Int_Map_Itr map_itr;
	Node_Data *node_ptr;
	Link_Data *link_ptr, *to_link_ptr;
	Dir_Data *dir_ptr, *to_dir_ptr;
	List_Data *list;

	Show_Message ("Intersection Types -- Record");
	Set_Progress ();

	//---- find intersection facility types ----

	Header_Number (NODE_TYPES);
	New_Page ();
	count = 0;

	min_angle = compass.Num_Points () * 45 / 360;
	max_angle = compass.Num_Points () * 135 / 360;

	for (map_itr = node_map.begin (); map_itr != node_map.end (); map_itr++) {
		Show_Progress ();

		type1 = type2 = EXTERNAL + 1;

		node = map_itr->second;
		area_type = 100;

		list = &node_list [node];

		//---- process each approach link ----

		for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
			dir_ptr = &dir_array [index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Type () >= type1) continue;

			at = link_ptr->Area_Type ();
			if (at > 0 && at < area_type) area_type = at;

			type1 = link_ptr->Type ();
			type2 = EXTERNAL + 1;

			//---- process all links leaving the node ----

			bear_in = dir_ptr->Out_Bearing ();

			for (index = list->From_List (); index >= 0; index = to_dir_ptr->First_Connect_From ()) {
				to_dir_ptr = &dir_array [index];

				if (to_dir_ptr->Link () == dir_ptr->Link ()) continue;

				bear_out = to_dir_ptr->In_Bearing ();
				
				change = compass.Difference (bear_in, bear_out);
				if (change >= min_angle && change <= max_angle) {
					to_link_ptr = &link_array [to_dir_ptr->Link ()];

					if (to_link_ptr->Type () < type2) {
						type2 = to_link_ptr->Type ();
					}
				}
			}
		}
		if (type1 < EXTERNAL && type2 < EXTERNAL) {
			node_ptr = &node_array [node];

			Print (1, String ("%10d  %20s  %20s     %5d") % node_ptr->Node () % Facility_Code ((Facility_Type) type1) % Facility_Code ((Facility_Type) type2) % area_type);
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
//	Node_Header
//---------------------------------------------------------

void NetPrep::Node_Header (void)
{
	Print (1, "Intersection Types");
	Print (2, "      Node         Facility Type         Crossing Type    Area Type");
	Print (1);
}

/*********************************************|***********************************************

	Intersection Types

	      Node         Facility Type        Crossing Type    Area Type

	dddddddddd  ssssssssssssssssssss  ssssssssssssssssssss     ddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
