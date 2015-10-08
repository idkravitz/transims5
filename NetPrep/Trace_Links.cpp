//*********************************************************
//	Trace_Links.cpp - build a list of thru links
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Trace_Links
//---------------------------------------------------------

int NetPrep::Trace_Links (int dir_index, bool from_flag, Integers &links, bool &end_flag)
{
	int link, lnk, dir, node, length, bearing, bear, diff;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	List_Data *list_ptr;
	Int_Itr itr;

	length = 0;
	end_flag = false;

	while (dir_index >= 0) {
		dir_ptr = &dir_array [dir_index];

		link = dir_ptr->Link ();
		if (select_links [link] == -1) {
			end_flag = true;
			break;
		}

		//---- check for loops ----

		for (itr = links.begin (); itr != links.end (); itr++) {
			if (*itr == link) break;
		}
		if (itr != links.end ()) break;

		//---- add the link to the list ----

		links.push_back (dir_index);

		link_ptr = &link_array [link];
		length += link_ptr->Length ();

		dir_index = dir_ptr->First_Turn (!from_flag);

		//---- check for selected cross streets ----

		dir = dir_ptr->Dir ();

		if (from_flag) {
			bearing = dir_ptr->Out_Bearing ();
			if (dir == 0) {
				node = link_ptr->Bnode ();
			} else {
				node = link_ptr->Anode ();
			}
		} else {
			bearing = dir_ptr->In_Bearing ();
			if (dir == 0) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
		}
		list_ptr = &node_list [node];

		for (dir = list_ptr->To_List (); dir >= 0; dir = dir_ptr->First_Connect_To ()) {
			dir_ptr = &dir_array [dir];

			lnk = dir_ptr->Link ();
			if (lnk == link) continue;

			bear = dir_ptr->Out_Bearing ();

			diff = compass.Difference (bear, bearing);

			if (diff >= cross_min && diff <= cross_max && select_links [lnk] != 0) {
				end_flag = true;
				return (length);
			}
		}
		for (dir = list_ptr->From_List (); dir >= 0; dir = dir_ptr->First_Connect_From ()) {
			dir_ptr = &dir_array [dir];

			lnk = dir_ptr->Link ();
			if (lnk == link) continue;

			bear = dir_ptr->In_Bearing ();

			diff = compass.Difference (bearing, bear);

			if (diff >= cross_min && diff <= cross_max && select_links [lnk] != 0) {
				end_flag = true;
				return (length);
			}
		}
	}
	return (length);
}
