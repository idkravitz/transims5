//*********************************************************
//	Select_Links.cpp - flag selected links
//*********************************************************

#include "LinkSum.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Select_Links
//---------------------------------------------------------

void LinkSum::Select_Links (void)
{
	int i, num, link, count;
	bool ab_flag, ba_flag;

	Node_Data *node_ptr;
	Link_Itr link_itr;
	Int_Set *group_ptr;
	Int_Set_Itr set_itr;

	Show_Message ("Selecting Links -- Record");
	Set_Progress ();

	count = 0;
	if (group_select) {
		num = link_equiv.Max_Group ();
	} else {
		num = 0;
	}

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();
		if (!select_facility [link_itr->Type ()]) {
			link_itr->Use (0);
			continue;
		}
		if (select_subarea) {
			node_ptr = &node_array [link_itr->Anode ()];

			if (!In_Polygon (subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
				node_ptr = &node_array [link_itr->Bnode ()];

				if (!In_Polygon (subarea_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
					link_itr->Use (0);
					continue;
				}
			}
		}
		if (group_select) {
			ab_flag = ba_flag = false;

			for (i=1; i <= num; i++) {
				group_ptr = link_equiv.Group_List (i);
				if (group_ptr == 0) continue;

				//---- compare each link in the link group ----

				for (set_itr = group_ptr->begin (); set_itr != group_ptr->end (); set_itr++) {
					link = abs (*set_itr);

					if (link == link_itr->Link ()) {
						if (*set_itr > 0) {
							ab_flag = true;
						} else {
							ba_flag = true;
						}
						if (ab_flag && ba_flag) break;
					}
				}
				if (ab_flag && ba_flag) break;
			}
			if (!ab_flag && !ba_flag) {
				link_itr->Use (0);
				continue;
			} else if (!ab_flag) {
				link_itr->Use (-2);
			} else if (!ba_flag) {
				link_itr->Use (-1);
			}
		}
		count++;
	}
	End_Progress ();

	Print (2, "Number of Selected Links = ") << count;
}
