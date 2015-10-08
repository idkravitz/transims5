//*********************************************************
//	Connections.cpp - build the link connection list
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Connections
//---------------------------------------------------------

void TransimsNet::Connections (void)
{
	int index, dir_index, to_index, thru_index, bear_in, bear_out, thru, change, diff, best_diff;
	int diff1, diff2, type, dir_type, dir_lanes, dir_speed, thru_type, thru_lanes, thru_speed, num;
	bool left_flag, ext_flag;

	Link_Itr link_itr;
	Link_Data *link_ptr, *to_link_ptr;
	Node_Data *node_ptr;
	Dir_Data *dir_ptr, *to_ptr;
	List_Data list_rec, *list_a, *list_b, *next_list, *to_list;
	List_Itr list_itr;
	Int_Map bear_map;
	Int_Map_Stat bear_stat;
	Int_Map_Itr map_itr, thru_itr;
	Connect_Data connect_rec, *connect_ptr;

	Show_Message ("Building Link Connections -- Record");
	Set_Progress ();

	//---- allocate node list ----

	if (node_list.size () == 0) {
		node_list.assign (node_array.size (), list_rec);
		turn_list.assign (dir_array.size (), list_rec);
	} else {
		for (list_itr = node_list.begin (); list_itr != node_list.end (); list_itr++) {
			list_itr->Clear ();
		}
		for (list_itr = turn_list.begin (); list_itr != turn_list.end (); list_itr++) {
			list_itr->Clear ();
		}
	}

	//---- add link directions to the node list ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Length () == 0) continue;

		list_a = &node_list [link_itr->Anode ()];
		list_b = &node_list [link_itr->Bnode ()];

		if (link_itr->AB_Dir () >= 0) {
			index = link_itr->AB_Dir ();
			next_list = &turn_list [index];

			next_list->From_List (list_a->From_List ());
			next_list->To_List (list_b->To_List ());

			list_a->From_List (index);
			list_b->To_List (index);
		}
		if (link_itr->BA_Dir () >= 0) {
			index = link_itr->BA_Dir ();
			next_list = &turn_list [index];

			next_list->From_List (list_b->From_List ());
			next_list->To_List (list_a->To_List ());

			list_b->From_List (index);
			list_a->To_List (index);
		}
	}

	//---- create connection records between links ----

	for (list_itr = node_list.begin (); list_itr != node_list.end (); list_itr++) {

		//---- process each approach link ----

		for (dir_index = list_itr->To_List (); dir_index >= 0; dir_index = next_list->To_List ()) {
			next_list = &turn_list [dir_index];

			dir_ptr = &dir_array [dir_index];
			link_ptr = &link_array [dir_ptr->Link ()];

			//---- process all links leaving the node ----

			bear_in = dir_ptr->Out_Bearing ();
			bear_map.clear ();
			index = -1;
			num = 0;

			for (to_index = list_itr->From_List (); to_index >= 0; to_index = to_list->From_List ()) {
				to_list = &turn_list [to_index];
				to_ptr = &dir_array [to_index];
				bear_out = to_ptr->In_Bearing ();

				num++;
				if (to_ptr->Link () == dir_ptr->Link ()) index = to_index;

				change = compass.Change (bear_in, bear_out);
				if (abs (change) <= max_angle) {
					bear_stat = bear_map.insert (Int_Map_Data (change, to_index));
					if (!bear_stat.second) {
						Warning ("Duplicate Connection Angle between Link ") << link_ptr->Link () << " and " << link_array [to_ptr->Link ()].Link ();
						to_ptr->In_Bearing (++bear_out);
						bear_map.insert (Int_Map_Data (++change, to_index));
					}
				}
			}

			//---- check for dead-ends ----

			if (bear_map.size () == 0) {
				if (link_ptr->Type () == EXTERNAL) continue;

				if (uturn_flag && index >= 0 && num == 1) {
					bear_map.insert (Int_Map_Data (uturn_angle, index));
				} else {
					if (dir_ptr->Dir () == 0) {
						num = link_ptr->Bnode ();
					} else {
						num = link_ptr->Anode ();
					}
					node_ptr = &node_array [num];
					Warning ("No Exit from Link ") << link_ptr->Link () << " at Node " << node_ptr->Node ();
					continue;
				}
			}

			//---- identify the thru movements ----

			thru = -1;
			best_diff = thru_diff;
			dir_type = link_ptr->Type ();
			ext_flag = (dir_type == EXTERNAL);
			if (dir_type == BRIDGE || dir_type == TUNNEL || dir_type == OTHER) dir_type = MAJOR;
			dir_lanes = dir_ptr->Lanes ();
			dir_speed = dir_ptr->Speed ();
			thru_type = thru_lanes = thru_speed = 0;
			thru_itr = bear_map.end ();

			for (index=0, map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++, index++) {
				change = map_itr->first;

				diff = abs (change);
				if (diff > thru_diff && dir_type != RAMP) continue;

				to_index = map_itr->second;
				to_ptr = &dir_array [to_index];

				type = link_array [to_ptr->Link ()].Type ();
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

						diff1 = abs (to_ptr->Lanes () - dir_lanes);
						diff2 = abs (thru_lanes - dir_lanes);

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (to_ptr->Speed () - dir_speed);
							diff2 = abs (thru_speed - dir_speed);

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (diff > abs (best_diff)) continue;
							}
						}
					}
				}

				//---- save the thru data ---

				thru = index;
				best_diff = change;
				thru_type = type;
				thru_lanes = to_ptr->Lanes ();
				thru_speed = to_ptr->Speed ();
				thru_itr = map_itr;
			}

			//---- map external link attributes ----

			if (thru >= 0) {
				if (ext_flag) {
					to_ptr = &dir_array [thru_itr->second];
					to_link_ptr = &link_array [to_ptr->Link ()];

					dir_ptr->Lanes (to_ptr->Lanes ());
					dir_ptr->Speed (to_ptr->Speed ());
					dir_ptr->Capacity (to_ptr->Capacity ());
					dir_ptr->Time0 (DTOI (to_ptr->Time0 () * link_ptr->Length () / to_link_ptr->Length ()));
					link_ptr->Divided (to_link_ptr->Divided ());
					link_ptr->Use (to_link_ptr->Use ());
				}
				if (thru_type == EXTERNAL) {
					to_ptr = &dir_array [thru_itr->second];
					to_link_ptr = &link_array [to_ptr->Link ()];

					to_ptr->Lanes (dir_ptr->Lanes ());
					to_ptr->Speed (dir_ptr->Speed ());
					to_ptr->Capacity (dir_ptr->Capacity ());
					to_ptr->Time0 (DTOI (dir_ptr->Time0 () * to_link_ptr->Length () / link_ptr->Length ()));
					to_link_ptr->Divided (link_ptr->Divided ());
					to_link_ptr->Use (link_ptr->Use ());
				}
			}

			//---- save link connection records in sorted order ----

			thru_index = -1;

			for (index=0, map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++, index++) {

				change = map_itr->first;
				to_index = map_itr->second;
				to_ptr = &dir_array [to_index];

				connect_rec.Dir_Index (dir_index);
				connect_rec.To_Index (to_index);
				num = (int) connect_array.size ();

				if (thru == index) {
					connect_rec.Type (THRU);
					thru_index = num;
				} else if (dir_ptr->Link () == to_ptr->Link ()) {
					connect_rec.Type (UTURN);
				} else {
					if (thru >= 0) {
						left_flag = (index < thru);
					} else {
						left_flag = (change < 0);
					}
					if (abs (change) > thru_diff) {
						connect_rec.Type ((left_flag) ? LEFT : RIGHT);
					} else {
						connect_rec.Type ((left_flag) ? L_SPLIT : R_SPLIT);
					}
				}

				//---- insert the connection record ----

				connect_rec.Next_To (dir_ptr->First_Connect_To ());
				dir_ptr->First_Connect_To (num);

				connect_rec.Next_From (to_ptr->First_Connect_From ());
				to_ptr->First_Connect_From (num);

				connect_map.insert (Int2_Map_Data (Int2_Key (dir_index, to_index), num));
				connect_array.push_back (connect_rec);
				nconnect++;
			}

			//---- check if the thru movements is a merge ----

			if (thru_index >= 0) {
				connect_ptr = &connect_array [thru_index];

				to_ptr = &dir_array [connect_ptr->To_Index ()];
				bear_out = to_ptr->In_Bearing ();
				thru = -1;
				left_flag = false;

				for (to_index = list_itr->To_List (); to_index >= 0; to_index = to_list->To_List ()) {
					to_list = &turn_list [to_index];
					to_ptr = &dir_array [to_index];

					if (to_index == connect_ptr->Dir_Index ()) continue;

					bear_in = to_ptr->Out_Bearing ();

					change = compass.Change (bear_in, bear_out);
					diff = abs (change);
					if (diff > thru_diff) continue;

					//---- check the facility types ----

					type = link_array [to_ptr->Link ()].Type ();
					if (type == BRIDGE || type == TUNNEL || type == OTHER) type = MAJOR;

					diff1 = abs (type - thru_type);
					diff2 = abs (thru_type - dir_type);

					if (diff1 > diff2) continue;

					if (diff1 == diff2) {

						//---- check the number of lanes ----

						diff1 = abs (to_ptr->Lanes () - thru_lanes);
						diff2 = abs (thru_lanes - dir_lanes);

						if (diff1 > diff2) continue;

						if (diff1 == diff2) {

							//---- check speeds ----

							diff1 = abs (to_ptr->Speed () - thru_speed);
							diff2 = abs (thru_speed - dir_speed);

							if (diff1 > diff2) continue;

							if (diff1 == diff2) {
								if (diff > abs (best_diff)) continue;
							}
						}
					}
					thru = to_index;
					left_flag = (best_diff < change);
				}

				//---- correct the movement type ----

				if (thru >= 0) {
					connect_ptr->Type ((left_flag) ? L_MERGE : R_MERGE);
				}
			}
		}
	}
	End_Progress ();
}
