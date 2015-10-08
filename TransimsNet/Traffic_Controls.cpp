//*********************************************************
//	Traffic_Controls.cpp - create traffic controls
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Traffic_Controls
//---------------------------------------------------------

void TransimsNet::Traffic_Controls (void)
{
	int i, j, index, dir_index, to_index, node, type, id;
	int primary, secondary, area_type, link, bearing, bear, divided_index, divided_type;
	int street [10] [4], num_street, num_approach, num_one_way, num_thru;
	bool flag, ramp_turn, divided_flag;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *to_ptr;
	Connect_Data *connect_ptr;
	Int_Map_Stat map_stat;
	Control_Index map_index;
	Control_Warrant_Map_Itr map_itr;
	Control_Warrant *warrant_ptr;
	Signal_Data signal_rec, *signal_ptr;
	Link_Detail_Map_Itr detail_itr;
	List_Itr list_itr;
	List_Data *next_list;

	Show_Message (String ("Creating Traffic Controls -- Record"));
	Set_Progress ();

	//---- process each node ----

	for (node=0, list_itr = node_list.begin (); list_itr != node_list.end (); list_itr++, node++) {
		node_ptr = &node_array [node];

		if (node_ptr->Control () != -1) continue;

		memset (street, '\0', sizeof (street));
		num_street = num_approach = num_one_way = num_thru = divided_index = divided_type = 0;
		ramp_turn = divided_flag = false;

		//---- process each approach link ----

		for (dir_index = list_itr->To_List (); dir_index >= 0; dir_index = next_list->To_List ()) {
			next_list = &turn_list [dir_index];
			dir_ptr = &dir_array [dir_index];

			if (link_detail_flag) {
				detail_itr = detail_map.find (dir_index);
				if (detail_itr != detail_map.end ()) {
					if (detail_itr->second.Control () != BLANK_CONTROL) {
						if (detail_itr->second.Control () == SIGNAL_GROUP) {
							index = (int) signal_array.size ();
							node_ptr->Control (index);
							nsignal++;

							if (signal_id_flag) {
								id = node_ptr->Node ();
							} else {
								id = nsignal;
							}
							signal_rec.Clear ();
							signal_rec.Signal (id);
							signal_rec.Group (detail_itr->second.Group ());
							signal_rec.nodes.push_back (node);

							signal_map.insert (Int_Map_Data (id, index));

							signal_array.push_back (signal_rec);							
						} else {
							node_ptr->Control (-2);
							dir_ptr->Sign (detail_itr->second.Control ());

							if (detail_itr->second.Control () == YIELD_SIGN) {
								nyield++;
							} else if (detail_itr->second.Control () != NO_CONTROL) {
								nstop++;
							}
							nsign++;
						}
						break;
					}
				}
			}
			link = dir_ptr->Link ();

			link_ptr = &link_array [link];
			num_approach++;

			type = link_ptr->Type ();
			if (type == BRIDGE || type == TUNNEL) type = MAJOR;

			if (link_ptr->AB_Dir () < 0 || link_ptr->BA_Dir () < 0) {
				num_one_way++;

				if (type >= PRINCIPAL && type <= LOCAL && link_ptr->Divided ()) {
					divided_flag = true;
					divided_index = dir_index;
					divided_type = type;
				}
			}

			//---- check for ramp turns ----

			if (type == RAMP && !ramp_turn) {
				for (to_index = dir_ptr->First_Connect_To (); to_index >= 0; to_index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [to_index];

					if (connect_ptr->Type () != THRU && connect_ptr->Type () != L_MERGE && connect_ptr->Type () != R_MERGE) {
						ramp_turn = true;
						break;
					}
				}
			}

			//---- check for new links ----

			flag = false;

			for (i=0; i < num_street; i++) {
				for (j=0; j < 2; j++) {
					if (street [i] [j] == link) {
						if (type < street [i] [2]) street [i] [2] = type;
						if (link_ptr->Area_Type () < street [i] [3]) street [i] [3] = link_ptr->Area_Type ();
						flag = true;
						break;
					}
				}
			}
			if (flag) continue;
			
			//---- add a new link ----

			i = num_street++;
			street [i] [0] = link;
			street [i] [1] = -1;
			street [i] [2] = type;
			street [i] [3] = link_ptr->Area_Type ();
			num_approach++;

			//---- find the thru link ----

			for (to_index = dir_ptr->First_Connect_To (); to_index >= 0; to_index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [to_index];

				if (connect_ptr->Type () == THRU || connect_ptr->Type () == L_MERGE || connect_ptr->Type () == R_MERGE) {
					to_ptr = &dir_array [connect_ptr->To_Index ()];
					street [i] [1] = to_ptr->Link ();
					if (connect_ptr->Type () == THRU) break;
				}
			}
			if (street [i] [1] >= 0) num_thru++;
		}

		//---- process each departure link ----

		for (to_index = list_itr->From_List (); to_index >= 0; to_index = next_list->From_List ()) {
			next_list = &turn_list [to_index];
			dir_ptr = &dir_array [to_index];

			link = dir_ptr->Link ();
			link_ptr = &link_array [link];

			//---- check for new links ----

			flag = false;

			for (i=0; i < num_street; i++) {
				for (j=0; j < 2; j++) {
					if (street [i] [j] == link) {
						flag = true;
						break;
					}
				}
			}
			if (flag) continue;
			
			type = link_ptr->Type ();
			if (type == BRIDGE || type == TUNNEL) type = MAJOR;

			//---- add a new link ----

			i = num_street++;
			street [i] [0] = -1;
			street [i] [1] = link;
			street [i] [2] = type;
			street [1] [3] = link_ptr->Area_Type ();
		}

		//---- check the intersection type ----

		if (num_street <= 1 || num_approach <= 1 || (num_approach - num_one_way) <= 1 ||
			(num_street == 2 && num_thru == 0)) {
			continue;
		}

		//---- identify the warrant codes ----
		
		primary = secondary = area_type = index = 0;

		for (i=0; i < num_street; i++) {
			if (street [i] [0] >= 0 && (primary == 0 || street [i] [2] < primary)) {
				secondary = primary;
				primary = street [i] [2];
				area_type = street [i] [3];
				index = i;
			} else if (secondary == 0 || street [i] [2] < secondary) {
				secondary = street [i] [2];
			}
		}
		map_index.Primary (primary);
		map_index.Secondary (secondary);
		map_index.Area_Type (area_type);

		map_itr = control_warrant_map.find (map_index);

		if (map_itr == control_warrant_map.end ()) continue;

		warrant_ptr = &control_warrants [map_itr->second];

		if (warrant_ptr->Control_Type () == NO_CONTROL) continue;

		//---- save the control record ----

		if (warrant_ptr->Control_Type () == SIGNAL_GROUP) {

			//---- check for ramp merge ----

			if (secondary == RAMP && !ramp_turn) {
				node_ptr->Control (-2);

				for (i=0; i < num_street; i++) {
					for (j=0; j < 2; j++) {
						link = street [i] [j];
						if (link < 0) continue;

						link_ptr = &link_array [link];

						if (link_ptr->Bnode () == node) {
							dir_index = link_ptr->AB_Dir ();
							link_ptr->Boffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
						} else {
							dir_index = link_ptr->BA_Dir ();
							link_ptr->Aoffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
						}
						if (dir_index < 0) continue;

						dir_ptr = &dir_array [dir_index];

						if (i == index) continue;
						nsign++;

						dir_ptr->Sign (YIELD_SIGN);
						nyield++;
					}
				}
				continue;
			}

			//---- insert a new signal ----

			index = (int) signal_array.size ();
			node_ptr->Control (index);
			nsignal++;

			if (signal_id_flag) {
				id = node_ptr->Node ();
			} else {
				id = nsignal;
			}
			signal_rec.Clear ();
			signal_rec.Signal (id);
			signal_rec.Group (warrant_ptr->Group ());
			signal_rec.nodes.push_back (node);

			signal_map.insert (Int_Map_Data (id, index));

			signal_array.push_back (signal_rec);

			//---- set the setbacks ----

			for (i=0; i < num_street; i++) {
				for (j=0; j < 2; j++) {
					link = street [i] [j];
					if (link < 0) continue;

					link_ptr = &link_array [link];

					if (link_ptr->Bnode () == node) {
						link_ptr->Boffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
					} else {
						link_ptr->Aoffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
					}
				}
			}

			//---- check for a divided signal ----

			if (divided_flag) {
				dir_ptr = &dir_array [divided_index];
				bearing = compass.Flip (dir_ptr->Out_Bearing ());

				//---- find the left link ----

				dir_index = -1;
				for (to_index = dir_ptr->First_Connect_To (); to_index >= 0; to_index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [to_index];

					if (connect_ptr->Type () == LEFT) {
						dir_index = connect_ptr->To_Index ();
						break;
					}
				}
				if (dir_index < 0) continue;

				dir_ptr = &dir_array [dir_index];

				//---- find another left link ----

				dir_index = -1;
				for (to_index = dir_ptr->First_Connect_To (); to_index >= 0; to_index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [to_index];

					if (connect_ptr->Type () == LEFT) {
						dir_index = connect_ptr->To_Index ();
						break;
					}
				}
				if (dir_index < 0) continue;

				//---- check the link attributes ----

				dir_ptr = &dir_array [dir_index];

				bear = dir_ptr->In_Bearing ();
				if (compass.Difference (bearing, bear) > straight_diff) continue;

				link_ptr = &link_array [dir_ptr->Link ()];
				if (link_ptr->Divided () == 0) continue;
				if (link_ptr->AB_Dir () >= 0 && link_ptr->BA_Dir () >= 0) continue;

				type = link_ptr->Type ();
				if (type == BRIDGE || type == TUNNEL) type = MAJOR;
				if (type != divided_type) continue;
				
				if (dir_ptr->Dir () == 0) {
					i = link_ptr->Anode ();
				} else {
					i = link_ptr->Bnode ();
				}
				node_ptr = &node_array [i];

				if (node_ptr->Control () != -1) {
					Warning ("Divided Signal Warrant Conflicts as Node ") << node_ptr->Node ();
					continue;
				}
				node_ptr->Control (index);
				signal_ptr = &signal_array [index];
				signal_ptr->nodes.push_back (i);
			}

		} else {	//---- signs ----

			node_ptr->Control (-2);

			for (i=0; i < num_street; i++) {
				for (j=0; j < 2; j++) {
					link = street [i] [j];
					if (link < 0) continue;

					link_ptr = &link_array [link];

					if (link_ptr->Bnode () == node) {
						dir_index = link_ptr->AB_Dir ();
						link_ptr->Boffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
					} else {
						dir_index = link_ptr->BA_Dir ();
						link_ptr->Aoffset (MIN (warrant_ptr->Setback (), link_ptr->Length () / 2));
					}
					if (dir_index < 0) continue;

					dir_ptr = &dir_array [dir_index];

					if (i == index && warrant_ptr->Control_Type () != ALL_STOP) continue;
					nsign++;

					dir_ptr->Sign (warrant_ptr->Control_Type ());

					if (warrant_ptr->Control_Type () == YIELD_SIGN) {
						nyield++;
					} else {
						nstop++;
					}
				}
			}
		}
	}
	End_Progress ();
}
