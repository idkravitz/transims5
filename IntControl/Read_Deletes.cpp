//*********************************************************
//	Read_Deletes.cpp - read the delete node control file
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Deletes
//---------------------------------------------------------

void IntControl::Read_Deletes (void)
{
	int node, index, nnode, nsign, sign_node, nsignal, signal_node;

	Node_Data *node_ptr;
	Node_Itr node_itr;
	Dir_Data *dir_ptr;
	Signal_Data *signal_ptr;
	Int_Map_Itr map_itr;
	List_Data *list;
	Int_Itr int_itr;

	//---- store the delete data ----

	Show_Message (String ("Reading %s -- Record") % delete_file.File_Type ());
	Set_Progress ();

	nnode = nsign = sign_node = nsignal = signal_node = 0;

	while (delete_file.Read ()) {
		Show_Progress ();

		node = String (delete_file.Record_String ()).Integer ();
		if (node == 0) continue;

		nnode++;

		map_itr = node_map.find (node);
		if (map_itr == node_map.end ()) {
			Warning (String ("Delete Node %d was Not Found") % node);
			continue;
		}
		node_ptr = &node_array [map_itr->second];

		if (node_ptr->Control () == -1) {
			Warning (String ("Delete Node %d has No Traffic Controls") % node);
		} else if (node_ptr->Control () == -2) {
			list = &node_list [map_itr->second];
			sign_node++;

			for (index = list->To_List (); index >= 0; index = dir_ptr->First_Turn ()) {
				dir_ptr = &dir_array [index];

				if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == YIELD_SIGN) {
					nsign++;
					dir_ptr->Sign (NO_CONTROL);
				}
			}
			node_ptr->Control (-1);
		} else if (node_ptr->Control () >= 0) {
			signal_ptr = &signal_array [node_ptr->Control ()];

			map_itr = signal_map.find (signal_ptr->Signal ());
			signal_map.erase (map_itr);
			nsignal++;

			for (int_itr = signal_ptr->nodes.begin (); int_itr != signal_ptr->nodes.end (); int_itr++) {
				node_ptr = &node_array [*int_itr];

				if (node_ptr->Node () == node) {
					node_ptr->Control (-1);
				} else {
					node_ptr->Control (-3);
				}
				signal_node++;
			}
		} else if (node_ptr->Control () == -3) {
			node_ptr->Control (-1);
		}
	}
	End_Progress ();

	delete_file.Close ();

	Print (2, String ("Number of %s Records = %d") % delete_file.File_Type () % nnode);
	Print (1, String ("%d Signs Approaching %d Nodes were Deleted") % nsign % sign_node);
	Print (1, String ("%d Nodes included in %d Signals were Deleted") % signal_node % nsignal);

	nsignal = 0;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Control () == -3) {
			node_itr->Control (-1);

			if (nsignal == 0) {
				Print (2, "Signal Data was Deleted from Additional Nodes...");
			}
			if (!(nsignal % 10)) {
				Print (1, "\t");
			}
			nsignal++;
			Print (0, String ("%d ") % node_itr->Node ());
		}
	}
	if (nsignal) {
		Warning (String ("%d Additional Signal Nodes were Deleted") % nsignal);
	}
}
