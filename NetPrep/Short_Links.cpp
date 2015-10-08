//*********************************************************
//	Short_Links.cpp - remove short links
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Short_Links
//---------------------------------------------------------

void NetPrep::Short_Links (void)
{
	int num_drop, index, link, alink, blink, anode, bnode, type, use, dir1, dir2, dir3, dir4;
	int bearing, bear, diff, best_diff;
	bool flag;

	Node_Data *anode_ptr, *bnode_ptr;
	Link_Data *alink_ptr, *blink_ptr;
	Dir_Data *dir_ptr, *dir2_ptr;
	Link_Itr link_itr;
	List_Data *list;

	Show_Message ("Dropping Short Links -- Record");
	Set_Progress ();

	num_drop = type = use = dir1 = dir2 = dir3 = dir4 = 0;

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();

		if (link_itr->Type () == EXTERNAL || link_itr->Length () == 0 || link_itr->Length () > short_links) continue;
		if (keep_link_flag && keep_link_range.In_Range (link_itr->Link ())) continue;

		anode = link_itr->Anode ();
		bnode = link_itr->Bnode ();

		anode_ptr = &node_array [anode];
		bnode_ptr = &node_array [bnode];

		if (anode_ptr->Count () > 2 && bnode_ptr->Count () > 2) continue;

		alink = blink = -1;
		alink_ptr = blink_ptr = 0;

		//---- get the anode connection ----

		list = &node_list [anode];

		if (anode_ptr->Count () <= 2) {
			for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
				dir_ptr = &dir_array [index];

				if (dir_ptr->Link () != link) {
					alink = dir_ptr->Link ();
				}
			}
			if (alink < 0) {
				for (index = list->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () != link) {
						alink = dir_ptr->Link ();
					}
				}
			}
		} else {
			best_diff = straight_diff + 1;

			if (link_itr->AB_Dir () >= 0) {
				dir_ptr = &dir_array [link_itr->AB_Dir ()];
				bearing = dir_ptr->In_Bearing ();

				for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () == link) continue;

					bear = dir_ptr->Out_Bearing ();
					diff = compass.Difference (bear, bearing);

					if (diff < best_diff) {
						best_diff = diff;
						alink = dir_ptr->Link ();
					}
				}
			} else {
				dir_ptr = &dir_array [link_itr->BA_Dir ()];
				bearing = dir_ptr->Out_Bearing ();

				for (index = list->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () == link) continue;

					bear = dir_ptr->In_Bearing ();
					diff = compass.Difference (bearing, bear);

					if (diff < best_diff) {
						best_diff = diff;
						alink = dir_ptr->Link ();
					}
				}
			}
		}

		//---- get the bnode connection ----

		list = &node_list [bnode];

		if (bnode_ptr->Count () <= 2) {
			for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
				dir_ptr = &dir_array [index];

				if (dir_ptr->Link () != link) {
					blink = dir_ptr->Link ();
				}
			}
			if (blink < 0) {
				for (index = list->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () != link) {
						blink = dir_ptr->Link ();
					}
				}
			}
		} else {
			best_diff = straight_diff + 1;

			if (link_itr->AB_Dir () >= 0) {
				dir_ptr = &dir_array [link_itr->AB_Dir ()];
				bearing = dir_ptr->Out_Bearing ();

				for (index = list->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () == link) continue;

					bear = dir_ptr->In_Bearing ();
					diff = compass.Difference (bearing, bear);

					if (diff < best_diff) {
						best_diff = diff;
						blink = dir_ptr->Link ();
					}
				}
			} else {
				dir_ptr = &dir_array [link_itr->BA_Dir ()];
				bearing = dir_ptr->In_Bearing ();

				for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
					dir_ptr = &dir_array [index];

					if (dir_ptr->Link () == link) continue;

					bear = dir_ptr->Out_Bearing ();
					diff = compass.Difference (bear, bearing);

					if (diff < best_diff) {
						best_diff = diff;
						blink = dir_ptr->Link ();
					}
				}
			}
		}
		if (alink < 0 && blink < 0) continue;

		if (alink >= 0) {
			alink_ptr = &link_array [alink];
		}
		if (blink >= 0) {
			blink_ptr = &link_array [blink];
		}

		//---- compare the links ----

		flag = false;

		if (Compare_Links (alink_ptr, blink_ptr)) {
			if (collapse_flag) {
				flag = true;
				type = alink_ptr->Type ();
				use = alink_ptr->Use ();

				if (alink_ptr->Bnode () == link_itr->Anode ()) {
					dir1 = alink_ptr->AB_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = alink_ptr->BA_Dir ();
					dir4 = link_itr->BA_Dir ();
				} else if (blink_ptr->Anode () == link_itr->Bnode ()) {
					dir1 = blink_ptr->AB_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = blink_ptr->BA_Dir ();
					dir4 = link_itr->BA_Dir ();
				} else {
					dir1 = alink_ptr->BA_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = alink_ptr->AB_Dir ();
					dir4 = link_itr->BA_Dir ();
				}
			} else {
			}
		} else if (Compare_Links (alink_ptr, &(*link_itr), anode, true)) {

			if (collapse_flag) {
				flag = true;
				type = alink_ptr->Type ();
				use = alink_ptr->Use ();

				if (alink_ptr->Bnode () == link_itr->Anode ()) {
					dir1 = alink_ptr->AB_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = alink_ptr->BA_Dir ();
					dir4 = link_itr->BA_Dir ();
				} else {
					dir1 = alink_ptr->BA_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = alink_ptr->AB_Dir ();
					dir4 = link_itr->BA_Dir ();
				}
			} else {
			}
		} else if (Compare_Links (&(*link_itr), blink_ptr, bnode, true)) {
			if (collapse_flag) {
				flag = true;
				type = blink_ptr->Type ();
				use = blink_ptr->Use ();

				if (blink_ptr->Anode () == link_itr->Bnode ()) {
					dir1 = blink_ptr->AB_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = blink_ptr->BA_Dir ();
					dir4 = link_itr->BA_Dir ();
				} else {
					dir1 = blink_ptr->BA_Dir ();
					dir2 = link_itr->AB_Dir ();
					dir3 = blink_ptr->AB_Dir ();
					dir4 = link_itr->BA_Dir ();
				}
			} else {
			}
		} else {
			Warning ("Short Link ") << link_itr->Link () << " Not Dropped";
		}

		//---- update the fields ----

		if (flag) {
			link_itr->Type (type);
			link_itr->Use (use);

			if (dir1 >= 0 && dir2 >= 0) {
				dir_ptr = &dir_array [dir1];
				dir2_ptr = &dir_array [dir2];

				dir2_ptr->Lanes (dir_ptr->Lanes ());
				dir2_ptr->Speed (dir_ptr->Speed ());
				dir2_ptr->Capacity (dir_ptr->Capacity ());
			}
			if (dir3 >= 0 && dir4 >= 0) {
				dir_ptr = &dir_array [dir3];
				dir2_ptr = &dir_array [dir4];

				dir2_ptr->Lanes (dir_ptr->Lanes ());
				dir2_ptr->Speed (dir_ptr->Speed ());
				dir2_ptr->Capacity (dir_ptr->Capacity ());
			}
			num_drop++;
		}
	}
	End_Progress ();

	Print (2, "Number of Short Links Dropped = ") << num_drop;
}
