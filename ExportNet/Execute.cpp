//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ExportNet::Execute (void)
{
	Link_Itr link_itr;
	Node_Itr node_itr;
	Node_Data *anode_ptr, *bnode_ptr;
	Data_Range_Itr range_itr;

	//---- read the network data ----

	Data_Service::Execute ();

	//---- set node selection flag ----
	
	Show_Message ("Select Network Records");
	Set_Progress ();

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		Show_Progress ();

		if (select_nodes || select_subarea) {
			node_itr->Subarea (0);

			if (select_subarea) {
				if (In_Polygon (subarea_file, UnRound (node_itr->X ()), UnRound (node_itr->Y ()))) node_itr->Subarea (1);
			}
			if (node_itr->Subarea () == 0 && select_nodes) {
				for (range_itr = node_ranges.begin (); range_itr != node_ranges.end (); range_itr++) {
					if (range_itr->In_Range (node_itr->Node ())) {
						node_itr->Subarea (1);
						break;
					}
				}
			}
		} else {
			node_itr->Subarea (1);
		}
	}
	Set_Progress ();

	//---- set link selection flag ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_nodes || select_subarea) {
			link_itr->Divided (0);

			anode_ptr = &node_array [link_itr->Anode ()];
			bnode_ptr = &node_array [link_itr->Bnode ()];

			if (anode_ptr->Subarea () == 1 || bnode_ptr->Subarea () == 1) {
				if (anode_ptr->Subarea () == 1) link_itr->Divided (1);
				if (bnode_ptr->Subarea () == 1) link_itr->Divided (link_itr->Divided () + 2);
				if (anode_ptr->Subarea () == 0) anode_ptr->Subarea (2);
				if (bnode_ptr->Subarea () == 0) bnode_ptr->Subarea (2);
			}
		} else {
			link_itr->Divided (3);
		}
	}
	End_Progress ();

	//---- write the utdf network ----

	if (utdf_flag) {
		Write_UTDF ();
	}

	//---- write the vissim network ----

	if (vissim_flag) {
		Write_VISSIM ();

		if (System_File_Flag (NEW_NODE)) {
			Write_Nodes ();
		}
		if (System_File_Flag (NEW_LINK)) {
			Write_Links ();
		}
		if (System_File_Flag (NEW_SHAPE)) {
			Write_Shapes ();
		}
		if (System_File_Flag (NEW_CONNECTION)) {
			Write_Connections ();
		}
	}

	//---- write the transit routes ----

	if (route_flag) {
		Write_Route ();

		//---- print the results ----

		Break_Check (3);
		Write (2, "Number of Route Header Records = ") << nroute;
		Write (1, "Number of Route Nodes Records = ") << nnodes;
	}
	Exit_Stat (DONE);
}


