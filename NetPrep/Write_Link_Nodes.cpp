//*********************************************************
//	Write_Link_Nodes.cpp - write a new link node list file
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Write_Link_Nodes
//---------------------------------------------------------

void NetPrep::Write_Link_Nodes (void)
{
	int count = 0;
	Int_Map_Itr itr;
	Int_List_Itr list_itr;
	Link_Nodes *ptr;
	Link_Data *link_ptr;

	Show_Message (String ("Writing %s -- Record") % link_node_file.File_Type ());
	Set_Progress ();

	fstream &file = link_node_file.File ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		Show_Progress ();

		link_ptr = &link_array [itr->second];

		if (!drop_flag || link_ptr->Length () > 0) {
			count++;

			ptr = &link_node_array [itr->second];

			file << ptr->link << " =";

			for (list_itr = ptr->nodes.begin (); list_itr != ptr->nodes.end (); list_itr++) {
				file << " " << *list_itr;
			}
			file << endl;
		}
	}
	Show_Progress (count);
	End_Progress ();
	link_node_file.Close ();
	
	Print (2, String ("%s Records = %d") % link_node_file.File_Type () % count);
}
