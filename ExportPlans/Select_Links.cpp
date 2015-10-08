//*********************************************************
//	Select_Links.cpp - select a subarea network
//*********************************************************

#include "ExportPlans.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Select_Links
//---------------------------------------------------------

void ExportPlans::Select_Links (void)
{
	int new_loc, count;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Link_Itr link_itr;
	Node_Itr node_itr;
	Node_Data *anode_ptr, *bnode_ptr, *node_ptr;
	Data_Range_Itr range_itr;
	Location_Data *loc_ptr;

	//---- set node selection flag ----

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
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
	}

	//---- set link selection flag ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		link_itr->Divided (0);

		anode_ptr = &node_array [link_itr->Anode ()];
		bnode_ptr = &node_array [link_itr->Bnode ()];

		if (anode_ptr->Subarea () == 1 || bnode_ptr->Subarea () == 1) {
			if (anode_ptr->Subarea () == 1) link_itr->Divided (1);
			if (bnode_ptr->Subarea () == 1) link_itr->Divided (link_itr->Divided () + 2);
			if (anode_ptr->Subarea () == 0) anode_ptr->Subarea (2);
			if (bnode_ptr->Subarea () == 0) bnode_ptr->Subarea (2);
		}
	}

	//---- flag activity locations within the subarea ----

	for (map_itr = location_map.begin (); map_itr != location_map.end (); map_itr++) {
		loc_ptr = &location_array [map_itr->second];

		link_ptr = &link_array [loc_ptr->Link ()];

		if (link_ptr->Divided () != 0) {
			loc_ptr->Zone (1);

			if (zone_loc_flag) {
				zone_loc_file.File () << (String ("%d\t%.2lf\t%.2lf") % loc_ptr->Location () % UnRound (loc_ptr->X ()) % UnRound (loc_ptr->Y ())) << endl;
			}
		} else {
			loc_ptr->Zone (0);
		}
	}

	//---- set the external stations location number ----
	
	map_itr = --location_map.end ();
	new_loc = map_itr->first;
	new_loc = ((new_loc + 1) / 100 + 1) * 100;
	count = 0;

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		link_ptr = &link_array [map_itr->second];

		if (link_ptr->Divided () != 1 && link_ptr->Divided () != 2) continue;

		if (link_ptr->Divided () == 1) {
			node_ptr = &node_array [link_ptr->Bnode ()];
		} else {
			node_ptr = &node_array [link_ptr->Anode ()];
		}
		if (zone_loc_flag) {
			zone_loc_file.File () << (String ("%d\t%.2lf\t%.2lf") % new_loc % UnRound (node_ptr->X ()) % UnRound (node_ptr->Y ())) << endl;
		}
		node_ptr->Control (new_loc++);
		count++;
	}
	Print (2, "Number of New External Locations = ") << count;
}
