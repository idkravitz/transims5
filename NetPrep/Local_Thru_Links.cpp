//*********************************************************
//	Local_Thru_Links.cpp - build a local thru segments
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Local_Thru_Links
//---------------------------------------------------------

void NetPrep::Local_Thru_Links (void)
{
	int link, dir_index, index, lnk, length, len, at_len, num, count, first_index;
	int bearing, bear, diff;
	bool end1_flag, end2_flag, cross1_flag, cross2_flag, first;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	List_Data *list_ptr;
	Integers links; 
	Int_Itr itr;

	Show_Message ("Building Local Thru Segments -- Record");
	Set_Progress ();

	count = 0;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Length () == 0 || link_itr->Type () != LOCAL) continue;

		length = len = num = 0;
		links.clear ();
		end1_flag = end2_flag = cross1_flag = cross2_flag = false;

		//---- pick a link direction ----

		first_index = link_itr->AB_Dir ();
		if (first_index < 0) {
			first_index = link_itr->BA_Dir ();
		}

		//---- check the link shape ----

		dir_ptr = &dir_array [first_index];
		bearing = dir_ptr->In_Bearing ();
		bear = dir_ptr->Out_Bearing ();
		diff = compass.Difference (bearing, bear);
		if (diff > forward_diff) continue;

		//---- build forward ----

		dir_index = first_index;

		while (dir_index >= 0) {
			dir_ptr = &dir_array [dir_index];

			//---- check the bearing ----

			bear = dir_ptr->Out_Bearing ();
			diff = compass.Difference (bearing, bear);
			if (diff > forward_diff) break;

			//---- check the facility type ----

			link = dir_ptr->Link ();
			link_ptr = &link_array [link];

			if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
				end1_flag = true;
				break;
			}

			//---- check for loops ----

			for (itr = links.begin (); itr != links.end (); itr++) {
				if (*itr == link) break;
			}
			if (itr != links.end ()) break;

			//---- add the link to the list ----

			links.push_back (link);

			length += link_ptr->Length ();
			at_len = thru_length.Best (link_ptr->Area_Type ());

			if (at_len > 0) {
				len += at_len;
				num++;
			}
			dir_index = dir_ptr->First_Turn ();

			if (dir_index >= 0) continue;

			//---- check for cross streets ----

			if (dir_ptr->Dir () == 0) {
				list_ptr = &node_list [link_ptr->Bnode ()];
			} else {
				list_ptr = &node_list [link_ptr->Anode ()];
			}

			for (index = list_ptr->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
				dir_ptr = &dir_array [index];

				lnk = dir_ptr->Link ();
				if (lnk == link) continue;

				link_ptr = &link_array [lnk];
				if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
					cross1_flag = true;
					break;
				}
			}
			if (cross1_flag) break;

			for (index = list_ptr->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
				dir_ptr = &dir_array [index];

				lnk = dir_ptr->Link ();
				if (lnk == link) continue;

				link_ptr = &link_array [lnk];
				if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
					cross1_flag = true;
					break;
				}
			}
		}

		//---- build backward ----

		dir_index = first_index;
		first = true;

		while (dir_index >= 0) {
			dir_ptr = &dir_array [dir_index];

			//---- check the bearing ----

			bear = dir_ptr->In_Bearing ();
			diff = compass.Difference (bear, bearing);
			if (diff > forward_diff) break;

			//---- check the facility type ----

			link = dir_ptr->Link ();
			link_ptr = &link_array [link];

			if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
				end2_flag = true;
				break;
			}
			if (!first) {

				//---- check for loops ----

				for (itr = links.begin (); itr != links.end (); itr++) {
					if (*itr == link) break;
				}
				if (itr != links.end ()) break;

				//---- add the link to the list ----

				links.push_back (link);

				length += link_ptr->Length ();
				at_len = thru_length.Best (link_ptr->Area_Type ());

				if (at_len > 0) {
					len += at_len;
					num++;
				}
			} else {
				first = false;
			}
			dir_index = dir_ptr->First_Turn (false);

			if (dir_index >= 0) continue;

			//---- check for cross streets ----

			if (dir_ptr->Dir () == 0) {
				list_ptr = &node_list [link_ptr->Anode ()];
			} else {
				list_ptr = &node_list [link_ptr->Bnode ()];
			}

			for (index = list_ptr->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
				dir_ptr = &dir_array [index];

				lnk = dir_ptr->Link ();
				if (lnk == link) continue;

				link_ptr = &link_array [lnk];

				if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
					cross2_flag = true;
					break;
				}
			}
			if (cross2_flag) break;

			for (index = list_ptr->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
				dir_ptr = &dir_array [index];

				lnk = dir_ptr->Link ();
				if (lnk == link) continue;

				link_ptr = &link_array [lnk];

				if (link_ptr->Type () != LOCAL && link_ptr->Type () != LOCAL_THRU) {
					cross2_flag = true;
					break;
				}
			}
		}

		//---- check the length criteria ----

		if (num == 0 || num < (int) (links.size () / 2)) continue;
		len /= num;

		if (length < len) {
			if (!end1_flag || !end2_flag) {
				if (!end1_flag || !cross2_flag) {
					if (!end2_flag || !cross1_flag) {
						continue;
					}
				}
			}
		}

		//---- change the links to local thru ----

		for (itr = links.begin (); itr != links.end (); itr++) {
			link_ptr = &link_array [*itr];
			link_ptr->Type (LOCAL_THRU);
			count++;
		}
	}
	End_Progress ();

	Print (2, "Number of New Local Thru Links = ") << count;
}
