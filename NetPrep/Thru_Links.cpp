//*********************************************************
//	Thru_Links.cpp - identify the thru link connections
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Thru_Links
//---------------------------------------------------------

void NetPrep::Thru_Links (void)
{
	int i, thru, link, index, bear_in, bear_out, bear, best_diff, diff, diff1, diff2;
	int type, dir_type, dir_lanes, dir_speed, thru_type, thru_lanes, thru_speed;

	Link_Itr link_itr;
	Dir_Data *dir_ptr, *dir2_ptr;
	List_Data *list_a, *list_b, *list_from, *list_to;
	List_Itr list_itr;

	Show_Message ("Building Thru Links -- Record");
	Set_Progress ();

	for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();

		if (link_itr->Length () == 0) continue;
			
		dir_type = link_itr->Type ();
		if (dir_type == BRIDGE || dir_type == TUNNEL || dir_type == OTHER) dir_type = MAJOR;

		//---- find the thru links for each direction ----

		list_a = &node_list [link_itr->Anode ()];
		list_b = &node_list [link_itr->Bnode ()];

		for (i=0; i < 2; i++) {
			if (i == 0) {
				index = link_itr->AB_Dir ();
				list_from = list_b;
				list_to = list_a;
			} else {
				index = link_itr->BA_Dir ();
				list_from = list_a;
				list_to = list_b;
			}
			if (index < 0) continue;

			dir_ptr = &dir_array [index];
			
			dir_lanes = dir_ptr->Lanes ();
			dir_speed = dir_ptr->Speed ();

			bear_in = dir_ptr->In_Bearing ();
			bear_out = dir_ptr->Out_Bearing ();

			//---- process the to end ----

			thru = -1;
			best_diff = thru_diff;
			thru_type = thru_lanes = thru_speed = 0;

			for (index = list_from->From_List (); index >= 0; index = dir2_ptr->First_Connect_From ()) {
				dir2_ptr = &dir_array [index];

				if (dir2_ptr->Link () == link) continue;

				bear = dir2_ptr->In_Bearing ();

				diff = compass.Difference (bear_out, bear);

				type = link_array [dir2_ptr->Link ()].Type ();
				if (type == BRIDGE || type == TUNNEL || type == OTHER) type = MAJOR;

				if (diff > thru_diff) {
					if (dir_type != RAMP || type != FREEWAY || type != EXPRESSWAY) continue;
				}

				//---- compare two options ----

				if (thru >= 0) {

					//---- check the facility types ----

					if (dir_type == RAMP) {
						if (type == FREEWAY || type == EXPRESSWAY) {
							diff1 = 0;
						} else {
							diff1 = abs (type - dir_type);
						}
						if (thru_type == FREEWAY || thru_type == EXPRESSWAY) {
							diff2 = 0;
						} else {
							diff2 = abs (thru_type - dir_type);
						}
					} else {
						diff1 = abs (type - dir_type);
						diff2 = abs (thru_type - dir_type);
					}
					if (diff1 > diff2) continue;

					if (diff1 == diff2) {

						//---- check the number of lanes ----

						diff1 = abs (dir2_ptr->Lanes () - dir_lanes);
						diff2 = abs (thru_lanes - dir_lanes);

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (dir2_ptr->Speed () - dir_speed);
							diff2 = abs (thru_speed - dir_speed);

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (diff > best_diff) continue;
							}
						}
					}
				}
				thru = index;
				best_diff = diff;
				thru_type = type;
				thru_lanes = dir2_ptr->Lanes ();
				thru_speed = dir2_ptr->Speed ();
			}
			dir_ptr->First_Turn_To (thru);

			//---- process the from end ----

			thru = -1;
			best_diff = thru_diff;
			thru_type = thru_lanes = thru_speed = 0;

			for (index = list_to->To_List (); index >= 0; index = dir2_ptr->First_Connect_To ()) {
				dir2_ptr = &dir_array [index];

				if (dir2_ptr->Link () == link) continue;

				bear = dir2_ptr->Out_Bearing ();

				diff = compass.Difference (bear, bear_in);

				type = link_array [dir2_ptr->Link ()].Type ();
				if (type == BRIDGE || type == TUNNEL || type == OTHER) type = MAJOR;

				if (diff > thru_diff) {
					if (dir_type != RAMP || type != FREEWAY || type != EXPRESSWAY) continue;
				}

				//---- compare two options ----

				if (thru >= 0) {

					//---- check the facility types ----

					if (dir_type == RAMP) {
						if (type == FREEWAY || type == EXPRESSWAY) {
							diff1 = 0;
						} else {
							diff1 = abs (type - dir_type);
						}
						if (thru_type == FREEWAY || thru_type == EXPRESSWAY) {
							diff2 = 0;
						} else {
							diff2 = abs (thru_type - dir_type);
						}
					} else {
						diff1 = abs (type - dir_type);
						diff2 = abs (thru_type - dir_type);
					}
					if (diff1 > diff2) continue;

					if (diff1 == diff2) {

						//---- check the number of lanes ----

						diff1 = abs (dir2_ptr->Lanes () - dir_lanes);
						diff2 = abs (thru_lanes - dir_lanes);

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (dir2_ptr->Speed () - dir_speed);
							diff2 = abs (thru_speed - dir_speed);

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (diff > best_diff) continue;
							}
						}
					}
				}
				thru = index;
				best_diff = diff;
				thru_type = type;
				thru_lanes = dir2_ptr->Lanes ();
				thru_speed = dir2_ptr->Speed ();
			}
			dir_ptr->First_Turn_From (thru);
		}
	}
	End_Progress ();
}
