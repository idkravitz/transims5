//*********************************************************
//	Set_Subarea.cpp - mark links within the subarea polygon
//*********************************************************

#include "ArcSnapshot.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Set_Subarea
//---------------------------------------------------------

void ArcSnapshot::Set_Subarea (void)
{
	Link_Itr link_itr;		
	Node_Data *node_ptr;
	
	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		node_ptr = &node_array [link_itr->Anode ()];

		if (!In_Polygon (subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
			node_ptr = &node_array [link_itr->Bnode ()];

			if (!In_Polygon (subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
				link_itr->Use (0);
			}
		}
	}
}
