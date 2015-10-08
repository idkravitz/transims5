//*********************************************************
//	Local_Spacing.cpp - remove extra local links
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Local_Spacing
//---------------------------------------------------------

void NetPrep::Local_Spacing (void)
{
	int link, lnk, dir, num_drop, num_loop, node, length, left_dir, right_dir;
	int thru_link, bearing, bear, diff, change;
	bool left_flag, right_flag, other_flag, process_flag;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	List_Data *list_ptr;
	Integers links, backwards;
	Int_Itr sel_itr, itr;
	Int_RItr int_ritr;

	Crossing_Data *cross_ptr;
	Spacing_Data spacing;

	spacing.Clear ();

	Show_Message ("Checking Local Spacing -- Record");

	select_links.assign (link_array.size (), 0);

	for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();

		if (link_itr->Length () == 0) continue;

		if (link_itr->Type () != LOCAL) {
			if (link_itr->Type () == EXTERNAL) {
				lnk = -1;
			} else {
				lnk = 1;
			}
			*(&select_links [link]) = lnk;
		}
	}

	num_drop = num_loop = 0;
	process_flag = true;

	//---- search for short selected links ----

	while (process_flag) {
		Set_Progress ();
		process_flag = false;
		num_loop++;
		next_loop = num_loop + 1;

		for (link = 0, sel_itr = select_links.begin (); sel_itr != select_links.end (); sel_itr++, link++) {
			Show_Progress ();

			if (*sel_itr != num_loop) continue;

			//---- identify the link segment ----

			link_ptr = &link_array [link];
			if (link_ptr->Length () == 0) continue;

			process_flag = true;

			if (link_ptr->AB_Dir () >= 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			links.clear ();

			//---- trace backwards to a selected link ----

			backwards.clear ();
			length = Trace_Links (dir, false, backwards, left_flag) - link_ptr->Length ();

			if (backwards.size () > 1) {
				for (int_ritr = backwards.rbegin (); int_ritr != backwards.rend (); int_ritr++) {
					if (*int_ritr != dir) links.push_back (*int_ritr);
				}
			}

			//---- trace forward to a selected link ----

			length += Trace_Links (dir, true, links, left_flag);

			if (links.size () == 0) continue;

			//---- check for local cross streets at the origin node ----

			dir = links [0];
			dir_ptr = &dir_array [dir];

			bearing = dir_ptr->In_Bearing ();
			thru_link = dir_ptr->Link ();

			link_ptr = &link_array [thru_link];

			if (dir_ptr->Dir () == 0) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			list_ptr = &node_list [node];

			left_dir = right_dir = -1;
			left_flag = right_flag = other_flag = false;

			for (dir = list_ptr->To_List (); dir >= 0; dir = dir_ptr->First_Connect_To ()) {
				dir_ptr = &dir_array [dir];

				lnk = dir_ptr->Link ();
				if (lnk == thru_link) continue;

				bear = dir_ptr->Out_Bearing ();

				change = compass.Change (bear, bearing);
				diff = abs (change);

				if (diff >= cross_min && diff <= cross_max) {
					if (select_links [lnk] == 0) {
						if (change > 0) {
							right_dir = dir;
						} else {
							left_dir = dir;
						}
					} else {
						other_flag = true;
					}
				}
			}
			for (dir = list_ptr->From_List (); dir >= 0; dir = dir_ptr->First_Connect_From ()) {
				dir_ptr = &dir_array [dir];

				lnk = dir_ptr->Link ();
				if (lnk == thru_link) continue;

				bear = compass.Flip (dir_ptr->In_Bearing ());

				change = compass.Change (bear, bearing);
				diff = abs (change);

				if (diff >= cross_min && diff <= cross_max) {
					if (select_links [lnk] == 0) {
						if (change > 0) {
							right_dir = dir;
							right_flag = true;
						} else {
							left_dir = dir;
							left_flag = true;
						}
					} else {
						other_flag = true;
					}
				}
			}

			//---- create the left and right cross streets ----

			spacing.Clear ();

			if (left_dir >= 0) {
				spacing.left1 = cross_ptr = new Crossing_Data ();
				cross_ptr->Clear ();
				cross_ptr->length = Trace_Links (left_dir, left_flag, cross_ptr->links, cross_ptr->end_flag);
				cross_ptr->node = node;
				cross_ptr->flag = other_flag;
			}
			if (right_dir >= 0) {
				spacing.right1 = cross_ptr = new Crossing_Data ();
				cross_ptr->Clear ();
				cross_ptr->length = Trace_Links (right_dir, right_flag, cross_ptr->links, cross_ptr->end_flag);
				cross_ptr->node = node;
				cross_ptr->flag = other_flag;
			}

			//---- process the links on the segment ----

			for (itr = links.begin (); itr != links.end (); itr++) {
				dir = *itr;
				
				dir_ptr = &dir_array [dir];
				bearing = dir_ptr->Out_Bearing ();
				thru_link = dir_ptr->Link ();
				select_links [thru_link] = 2;

				link_ptr = &link_array [thru_link];

				if (dir_ptr->Dir () == 0) {
					node = link_ptr->Bnode ();
				} else {
					node = link_ptr->Anode ();
				}
				list_ptr = &node_list [node];

				left_dir = right_dir = -1;
				left_flag = right_flag = other_flag = false;

				for (dir = list_ptr->To_List (); dir >= 0; dir = dir_ptr->First_Connect_To ()) {
					dir_ptr = &dir_array [dir];

					lnk = dir_ptr->Link ();
					if (lnk == thru_link) continue;

					bear = compass.Flip (dir_ptr->Out_Bearing ());

					change = compass.Change (bearing, bear);
					diff = abs (change);

					if (diff >= cross_min && diff <= cross_max) {
						if (select_links [lnk] == 0) {
							if (change > 0) {
								right_dir = dir;
							} else {
								left_dir = dir;
							}
						} else {
							other_flag = true;
						}
					}
				}
				for (dir = list_ptr->From_List (); dir >= 0; dir = dir_ptr->First_Connect_From ()) {
					dir_ptr = &dir_array [dir];

					lnk = dir_ptr->Link ();
					if (lnk == thru_link) continue;

					bear = dir_ptr->In_Bearing ();

					change = compass.Change (bearing, bear);
					diff = abs (change);

					if (diff >= cross_min && diff <= cross_max) {
						if (select_links [lnk] == 0) {
							if (change > 0) {
								right_dir = dir;
								right_flag = true;
							} else {
								left_dir = dir;
								left_flag = true;
							}
						} else {
							other_flag = true;
						}
					}
				}

				//---- create the left cross street ----

				if (left_dir >= 0) {
					if (spacing.left1 == 0) {
						spacing.left1 = cross_ptr = new Crossing_Data ();
					} else if (spacing.left2 == 0) {
						spacing.left2 = cross_ptr = new Crossing_Data ();
						spacing.left_len12 += link_ptr->Length ();
						spacing.left_at12 = link_ptr->Area_Type ();
					} else {
						spacing.left3 = cross_ptr = new Crossing_Data ();
						spacing.left_len23 += link_ptr->Length ();
						spacing.left_at23 = link_ptr->Area_Type ();
					}
					cross_ptr->Clear ();
					cross_ptr->length = Trace_Links (left_dir, left_flag, cross_ptr->links, cross_ptr->end_flag);
					cross_ptr->node = node;
					cross_ptr->flag = other_flag;
				} else if (spacing.left2 == 0) {
					spacing.left_len12 += link_ptr->Length ();
					spacing.left_at12 = link_ptr->Area_Type ();
				} else {
					spacing.left_len23 += link_ptr->Length ();
					spacing.left_at23 = link_ptr->Area_Type ();
				}

				//---- create the right cross street ----

				if (right_dir >= 0) {
					if (spacing.right1 == 0) {
						spacing.right1 = cross_ptr = new Crossing_Data ();
					} else if (spacing.right2 == 0) {
						spacing.right2 = cross_ptr = new Crossing_Data ();
						spacing.right_len12 += link_ptr->Length ();
						spacing.right_at12 = link_ptr->Area_Type ();
					} else {
						spacing.right3 = cross_ptr = new Crossing_Data ();
						spacing.right_len23 += link_ptr->Length ();
						spacing.right_at23 = link_ptr->Area_Type ();
					}
					cross_ptr->Clear ();
					cross_ptr->length = Trace_Links (right_dir, right_flag, cross_ptr->links, cross_ptr->end_flag);
					cross_ptr->node = node;
					cross_ptr->flag = other_flag;
				} else if (spacing.right2 == 0) {
					spacing.right_len12 += link_ptr->Length ();
					spacing.right_at12 = link_ptr->Area_Type ();
				} else {
					spacing.right_len23 += link_ptr->Length ();
					spacing.right_at23 = link_ptr->Area_Type ();
				}

				//---- check for cross street processing ----

				if (spacing.left3 != 0 || spacing.right3 != 0) {
					num_drop += Space_Processing (spacing);
				}
			}
			num_drop += Space_Processing (spacing, true);
		}
	}

	//---- remove extra local links ----

	for (link = 0, sel_itr = select_links.begin (); sel_itr != select_links.end (); sel_itr++, link++) {
		Show_Progress ();

		if (*sel_itr != 0) continue;

		link_ptr = &link_array [link];

		if (link_ptr->Length () > 0) {
			Delete_Link (link);
			num_drop++;
		}
	}
	End_Progress ();

	Print (2, "Number of Local Links Dropped = ") << num_drop;
	Print (1, "Number of Processing Loops = ") << num_loop;
}
