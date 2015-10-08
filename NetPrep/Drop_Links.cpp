//*********************************************************
//	Drop_Links.cpp - remove dead end links
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Drop_Links
//---------------------------------------------------------

void NetPrep::Drop_Links (void)
{
	int num_drop = 0;

	Node_Data *anode_ptr, *bnode_ptr;
	Link_Itr link_itr;

	Show_Message ("Dropping Dead End Links -- Record");
	Set_Progress ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Type () != EXTERNAL && link_itr->Length () > 0 && 
			link_itr->Length () < drop_length) {

			anode_ptr = &node_array [link_itr->Anode ()];
			bnode_ptr = &node_array [link_itr->Bnode ()];

			if (anode_ptr->Count () == 1 || bnode_ptr->Count () == 1) {
				link_itr->Length (0);
				num_drop++;

				anode_ptr->Drop_Count ();
				bnode_ptr->Drop_Count ();
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Dead End Links Dropped = ") << num_drop;
}
