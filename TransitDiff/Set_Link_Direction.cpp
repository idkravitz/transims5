//*********************************************************
//	Set_Link_Direction.cpp - system data input service
//*********************************************************

#include "TransitDiff.hpp"

//-----------------------------------------------------------
//	Set_Link_Direction
//-----------------------------------------------------------

Link_Data * TransitDiff::Set_Link_Direction (Db_Header *file, int &link, int &dir, int &offset, bool flag)
{
	Link_Data *link_ptr = 0;
	Int_Map_Itr map_itr;

	int node = dir;

	if (link < 0) {
		dir = 1;
		link = -link;
	} else {
		dir = 0;
	}
	map_itr = compare_link_map.find (link);
	if (map_itr == compare_link_map.end ()) {
		Error (String ("%s Link %d was Not Found in the Link file") % file->File_ID () % link);
	}
	link = map_itr->second;

	link_ptr = &compare_link_array [link];

	if (dir == 0 && file->LinkDir_Type () != LINK_SIGN) {
		if (file->LinkDir_Type () == LINK_NODE) {
			dir = 0;

			map_itr = compare_node_map.find (node);
			if (map_itr == compare_node_map.end ()) {
				Error (String ("%s Node %d was Not Found in the Node file") % file->File_ID () % node);
			}
			node = map_itr->second;

			dir = -1;
			if (flag) {
				if (link_ptr->Bnode () == node) {
					dir = 1;
				} else if (link_ptr->Anode () == node) {
					dir = 0;
				}
			} else if (link_ptr->Anode () == node) {
				dir = 1;
			} else if (link_ptr->Bnode () == node) {
				dir = 0;
			}
			if (dir < 0) {
				node = map_itr->first;
				Error (String ("%s Node %d is Not on Link %d") % file->File_ID () % node % link_ptr->Link ());
			}
		} else {
			dir = node;
		}
	}
	if (offset < 0 || offset > link_ptr->Length ()) {
		Warning (String ("%s %d Offset %.1lf is Out of Range (0..%.1lf)") %
			file->File_ID () % Progress_Count () % UnRound (offset) % UnRound (link_ptr->Length ()));

		if (offset < 0) {
			offset = 0;
		} else {
			offset = link_ptr->Length ();
		}
	}
	if (file->Version () <= 40) {
		offset = link_ptr->Length () - offset;
	}
	return (link_ptr);
}
