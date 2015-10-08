//*********************************************************
//	Collapse_Nodes.cpp - remove shape nodes
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Collapse_Nodes
//---------------------------------------------------------

void NetPrep::Collapse_Nodes (void)
{
	int index, node, num_drop, link1, link2, dir1, dir2;

	Node_Itr node_itr;
	Link_Itr link_itr;
	Dir_Data *dir_ptr, *dir2_ptr;
	Link_Data *link_ptr, *link2_ptr;
	Int_Map_Itr map_itr;
	Int_List_Itr list_itr;
	Int_List_RItr list_ritr;
	Shape_Data *shape_ptr, *shape2_ptr, shape_rec;
	XYZ xyz;
	XYZ_Itr xyz_itr;
	XYZ_RItr xyz_ritr;
	List_Data *list_a, *list_b;
	Link_Nodes *link_node1, *link_node2;

	num_drop = 0;

	shape_ptr = 0;
	dir_ptr = dir2_ptr = 0;
	link_node1 = link_node2 = 0;

	Show_Message ("Collapsing Nodes -- Record");

	//---- scan the node file for limited link connections ----

	for (node = 0, node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++, node++) {
		Show_Progress ();

		if (node_itr->Count () != 2) continue;
		if (keep_node_flag && keep_node_range.In_Range (node_itr->Node ())) continue;

		//---- find the two link numbers ----

		link1 = link2 = -1;

		list_a = &node_list [node];

		for (index = list_a->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
			dir_ptr = &dir_array [index];

			if (link1 < 0 || dir_ptr->Dir () == 0) {
				link1 = dir_ptr->Link ();
			}
		}
		for (index = list_a->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
			dir_ptr = &dir_array [index];

			if ((link2 < 0 || dir_ptr->Dir () == 0) && dir_ptr->Link () != link1) {
				link2 = dir_ptr->Link ();
			}
		}
		if (link1 < 0 || link2 < 0) continue;

		//---- check the seleciton criteria ----

		link_ptr = &link_array [link1];
		link2_ptr = &link_array [link2];

		if (keep_link_flag && (keep_link_range.In_Range (link_ptr->Link ()) || 
			keep_link_range.In_Range (link2_ptr->Link ()))) continue;

		if (!Compare_Links (link_ptr, link2_ptr, node)) continue;

		//---- remove the node ----

		node_itr->Count (0);
		num_drop++;

		if (link_node_flag) {
			link_node1 = &link_node_array [link1];
			link_node2 = &link_node_array [link2];
		}

		//---- combine link1 to link2 direction ----

		if (link_ptr->Bnode () == node && link_ptr->AB_Dir () >= 0) {

			dir1 = link_ptr->AB_Dir ();
			dir_ptr = &dir_array [dir1];

			link_ptr->Length (link_ptr->Length () + link2_ptr->Length ());
			link2_ptr->Length (0);

			//---- add the deleted node to the approach shape ----

			if (shape_flag) {
				map_itr = shape_map.find (link_ptr->Link ());

				if (map_itr == shape_map.end ()) {
					shape_rec.Clear ();
					shape_rec.Link (link_ptr->Link ());

					index = (int) shape_array.size ();
					shape_map.insert (Int_Map_Data (shape_rec.Link (), index));
					shape_array.push_back (shape_rec);
				} else {
					index = map_itr->second;
				}
				shape_ptr = &shape_array [index];

				xyz.x = node_itr->X ();
				xyz.y = node_itr->Y ();
				xyz.z = node_itr->Z ();

				shape_ptr->push_back (xyz);
			}

			//---- add the departure data ----

			if (link2_ptr->Anode () == node) {
				dir2 = link2_ptr->AB_Dir ();
				dir2_ptr = &dir_array [dir2];

				dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
				link_ptr->Bnode (link2_ptr->Bnode ());

				list_b = &node_list [link2_ptr->Bnode ()];
			
				//---- update the to list at the bnode ----

				for (index = list_b->To_List (); index >= 0; index = dir2_ptr->First_Connect_To ()) {
					if (index == dir2) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir1);
						} else {
							dir2_ptr->First_Connect_To (dir1);
						}
					}
					dir2_ptr = &dir_array [index];

					if (index == dir2) {
						dir_ptr->First_Connect_To (dir2_ptr->First_Connect_To ());
						break;
					}
				}

				//---- update the from list at the bnode ----
				
				dir1 = link_ptr->BA_Dir ();
				dir2 = link2_ptr->BA_Dir ();

				if (dir1 >= 0 && dir2 >= 0) {

					dir_ptr = &dir_array [dir1];
					dir2_ptr = &dir_array [dir2];

					dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

					for (index = list_b->From_List (); index >= 0; index = dir2_ptr->First_Connect_From ()) {
						if (index == dir2) {
							if (index == list_b->From_List ()) {
								list_b->From_List (dir1);
							} else {
								dir2_ptr->First_Connect_From (dir1);
							}
						}
						dir2_ptr = &dir_array [index];

						if (index == dir2) {
							dir_ptr->First_Connect_From (dir2_ptr->First_Connect_From ());
							break;
						}
					}
				}

				//---- add the shape points ----

				if (shape_flag) {
					map_itr = shape_map.find (link2_ptr->Link ());

					if (map_itr != shape_map.end ()) {
						shape2_ptr = &shape_array [map_itr->second];

						shape_ptr->insert (shape_ptr->end (), shape2_ptr->begin (), shape2_ptr->end ());
					}
				}

				//---- merge the link nodes ----

				if (link_node_flag) {
					link_node1->nodes.insert (link_node1->nodes.end (), ++link_node2->nodes.begin (), link_node2->nodes.end ());
				}
			} else {
				dir2 = link2_ptr->BA_Dir ();
				dir2_ptr = &dir_array [dir2];

				dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
				link_ptr->Bnode (link2_ptr->Anode ());

				list_b = &node_list [link2_ptr->Anode ()];
		
				//---- update the to list at the anode ----

				for (index = list_b->To_List (); index >= 0; index = dir2_ptr->First_Connect_To ()) {
					if (index == dir2) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir1);
						} else {
							dir2_ptr->First_Connect_To (dir1);
						}
					}
					dir2_ptr = &dir_array [index];

					if (index == dir2) {
						dir_ptr->First_Connect_To (dir2_ptr->First_Connect_To ());
						break;
					}
				}

				//---- update the from list at the anode ----
				
				dir1 = link_ptr->BA_Dir ();
				dir2 = link2_ptr->AB_Dir ();

				if (dir1 >= 0 && dir2 >= 0) {
					dir_ptr = &dir_array [dir1];
					dir2_ptr = &dir_array [dir2];

					dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

					for (index = list_b->From_List (); index >= 0; index = dir2_ptr->First_Connect_From ()) {
						if (index == dir2) {
							if (index == list_b->From_List ()) {
								list_b->From_List (dir1);
							} else {
								dir2_ptr->First_Connect_From (dir1);
							}
						}
						dir2_ptr = &dir_array [index];

						if (index == dir2) {
							dir_ptr->First_Connect_From (dir2_ptr->First_Connect_From ());
							break;
						}
					}
				}

				//---- add the shape points ----

				if (shape_flag) {
					map_itr = shape_map.find (link2_ptr->Link ());

					if (map_itr != shape_map.end ()) {
						shape2_ptr = &shape_array [map_itr->second];

						for (xyz_ritr = shape2_ptr->rbegin (); xyz_ritr != shape2_ptr->rend (); xyz_ritr++) {
							shape_ptr->push_back (*xyz_ritr);
						}
					}
				}

				//---- merge the link nodes ----

				if (link_node_flag) {
					for (list_ritr = ++link_node2->nodes.rbegin (); list_ritr != link_node2->nodes.rend (); list_ritr++) {
						link_node1->nodes.push_back (*list_ritr);
					}
				}
			}
		
			//---- combine link2 to link1 direction ----

		} else if (link2_ptr->Bnode () == node && link2_ptr->AB_Dir () >= 0) {

			dir2 = link2_ptr->AB_Dir ();
			dir2_ptr = &dir_array [dir2];

			link2_ptr->Length (link2_ptr->Length () + link_ptr->Length ());
			link_ptr->Length (0);

			//---- add the deleted node to the approach shape ----

			if (shape_flag) {
				map_itr = shape_map.find (link2_ptr->Link ());

				if (map_itr == shape_map.end ()) {
					shape_rec.Clear ();
					shape_rec.Link (link2_ptr->Link ());

					index = (int) shape_array.size ();
					shape_map.insert (Int_Map_Data (shape_rec.Link (), index));
					shape_array.push_back (shape_rec);
				} else {
					index = map_itr->second;
				}
				shape_ptr = &shape_array [index];

				xyz.x = node_itr->X ();
				xyz.y = node_itr->Y ();
				xyz.z = node_itr->Z ();

				shape_ptr->push_back (xyz);
			}

			//---- add the departure data ----

			if (link_ptr->Anode () == node) {
				dir1 = link_ptr->AB_Dir ();
				dir_ptr = &dir_array [dir1];

				dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
				link2_ptr->Bnode (link_ptr->Bnode ());

				list_b = &node_list [link_ptr->Bnode ()];
				
				//---- update the to list at the bnode ----

				for (index = list_b->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
					if (index == dir1) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir2);
						} else {
							dir_ptr->First_Connect_To (dir2);
						}
					}
					dir_ptr = &dir_array [index];

					if (index == dir1) {
						dir2_ptr->First_Connect_To (dir_ptr->First_Connect_To ());
						break;
					}
				}

				//---- update the from list at the bnode ----

				dir2 = link2_ptr->BA_Dir ();
				dir1 = link_ptr->BA_Dir ();

				if (dir1 >= 0 && dir2 >= 0) {
					dir2_ptr = &dir_array [dir2];
					dir_ptr = &dir_array [dir1];

					dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

					for (index = list_b->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
						if (index == dir1) {
							if (index == list_b->From_List ()) {
								list_b->From_List (dir2);
							} else {
								dir_ptr->First_Connect_From (dir2);
							}
						}
						dir_ptr = &dir_array [index];

						if (index == dir1) {
							dir2_ptr->First_Connect_From (dir_ptr->First_Connect_From ());
							break;
						}
					}
				}

				//---- add the shape points ----

				if (shape_flag) {
					map_itr = shape_map.find (link_ptr->Link ());

					if (map_itr != shape_map.end ()) {
						shape2_ptr = &shape_array [map_itr->second];

						shape_ptr->insert (shape_ptr->end (), shape2_ptr->begin (), shape2_ptr->end ());
					}
				}

				//---- merge the link nodes ----

				if (link_node_flag) {
					link_node2->nodes.insert (link_node2->nodes.end (), ++link_node1->nodes.begin (), link_node1->nodes.end ());
				}
			} else {
				dir1 = link_ptr->BA_Dir ();
				dir_ptr = &dir_array [dir1];

				dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
				link2_ptr->Bnode (link_ptr->Anode ());

				list_b = &node_list [link_ptr->Anode ()];

				//---- update the to list at the anode ----

				for (index = list_b->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
					if (index == dir1) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir2);
						} else {
							dir_ptr->First_Connect_To (dir2);
						}
					}
					dir_ptr = &dir_array [index];

					if (index == dir1) {
						dir2_ptr->First_Connect_To (dir_ptr->First_Connect_To ());
						break;
					}
				}

				//---- update the from list at the anode ----

				dir2 = link2_ptr->BA_Dir ();
				dir1 = link_ptr->AB_Dir ();

				if (dir1 >= 0 && dir2 >= 0) {
					dir2_ptr = &dir_array [dir2];
					dir_ptr = &dir_array [dir1];

					dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

					for (index = list_b->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
						if (index == dir1) {
							if (index == list_b->From_List ()) {
								list_b->From_List (dir2);
							} else {
								dir_ptr->First_Connect_From (dir2);
							}
						}
						dir_ptr = &dir_array [index];

						if (index == dir1) {
							dir2_ptr->First_Connect_From (dir_ptr->First_Connect_From ());
							break;
						}
					}
				}

				//---- add the shape points ----

				if (shape_flag) {
					map_itr = shape_map.find (link_ptr->Link ());

					if (map_itr != shape_map.end ()) {
						shape2_ptr = &shape_array [map_itr->second];

						for (xyz_ritr = shape2_ptr->rbegin (); xyz_ritr != shape2_ptr->rend (); xyz_ritr++) {
							shape_ptr->push_back (*xyz_ritr);
						}
					}
				}

				//---- merge the link nodes ----

				if (link_node_flag) {
					for (list_ritr = ++link_node1->nodes.rbegin (); list_ritr != link_node1->nodes.rend (); list_ritr++) {
						link_node2->nodes.push_back (*list_ritr);
					}
				}
			}

			//---- flip link2 and insert before link1 ----

		} else if (link_ptr->Anode () == node && link_ptr->AB_Dir () >= 0) {

			dir1 = link_ptr->AB_Dir ();
			dir_ptr = &dir_array [dir1];

			link_ptr->Length (link_ptr->Length () + link2_ptr->Length ());
			link2_ptr->Length (0);

			//---- insert the deleted node in front of the departure shape ----

			if (shape_flag) {
				map_itr = shape_map.find (link_ptr->Link ());

				if (map_itr == shape_map.end ()) {
					shape_rec.Clear ();
					shape_rec.Link (link_ptr->Link ());

					index = (int) shape_array.size ();
					shape_map.insert (Int_Map_Data (shape_rec.Link (), index));
					shape_array.push_back (shape_rec);
				} else {
					index = map_itr->second;
				}
				shape_ptr = &shape_array [index];

				xyz.x = node_itr->X ();
				xyz.y = node_itr->Y ();
				xyz.z = node_itr->Z ();

				shape_ptr->insert (shape_ptr->begin (), xyz);
			}

			//---- add the approach data ----

			dir2 = link2_ptr->BA_Dir ();
			dir2_ptr = &dir_array [dir2];

			dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
			link_ptr->Anode (link2_ptr->Bnode ());

			list_b = &node_list [link2_ptr->Bnode ()];

			//---- update the from list at the bnode ----

			for (index = list_b->From_List (); index >= 0; index = dir2_ptr->First_Connect_From ()) {
				if (index == dir2) {
					if (index == list_b->From_List ()) {
						list_b->From_List (dir1);
					} else {
						dir2_ptr->First_Connect_From (dir1);
					}
				}
				dir2_ptr = &dir_array [index];

				if (index == dir2) {
					dir_ptr->First_Connect_From (dir2_ptr->First_Connect_From ());
					break;
				}
			}

			//---- update the to list at the bnode ----
			
			dir1 = link_ptr->BA_Dir ();
			dir2 = link2_ptr->AB_Dir ();

			if (dir1 >= 0 && dir2 >= 0) {

				dir_ptr = &dir_array [dir1];
				dir2_ptr = &dir_array [dir2];

				dir_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

				for (index = list_b->To_List (); index >= 0; index = dir2_ptr->First_Connect_To ()) {
					if (index == dir2) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir1);
						} else {
							dir2_ptr->First_Connect_To (dir1);
						}
					}
					dir2_ptr = &dir_array [index];

					if (index == dir2) {
						dir_ptr->First_Connect_To (dir2_ptr->First_Connect_To ());
						break;
					}
				}
			}

			//---- insert the shape points in front ----

			if (shape_flag) {
				map_itr = shape_map.find (link2_ptr->Link ());

				if (map_itr != shape_map.end ()) {
					shape2_ptr = &shape_array [map_itr->second];

					for (xyz_itr = shape2_ptr->begin (); xyz_itr != shape2_ptr->end (); xyz_itr++) {
						shape_ptr->insert (shape_ptr->begin (), *xyz_itr);
					}
				}
			}

			//---- merge the link nodes ----

			if (link_node_flag) {
				for (list_itr = ++link_node2->nodes.begin (); list_itr != link_node2->nodes.end (); list_itr++) {
					link_node1->nodes.push_front (*list_itr);
				}
			}

			//---- flip link1 and insert before link2 ----

		} else if (link2_ptr->Anode () == node && link2_ptr->AB_Dir () >= 0) {

			dir2 = link2_ptr->AB_Dir ();
			dir2_ptr = &dir_array [dir2];

			link2_ptr->Length (link_ptr->Length () + link2_ptr->Length ());
			link_ptr->Length (0);

			//---- insert the deleted node in front of the departure shape ----

			if (shape_flag) {
				map_itr = shape_map.find (link2_ptr->Link ());

				if (map_itr == shape_map.end ()) {
					shape_rec.Clear ();
					shape_rec.Link (link2_ptr->Link ());

					index = (int) shape_array.size ();
					shape_map.insert (Int_Map_Data (shape_rec.Link (), index));
					shape_array.push_back (shape_rec);
				} else {
					index = map_itr->second;
				}
				shape_ptr = &shape_array [index];

				xyz.x = node_itr->X ();
				xyz.y = node_itr->Y ();
				xyz.z = node_itr->Z ();

				shape_ptr->insert (shape_ptr->begin (), xyz);
			}

			//---- add the approach data ----

			dir1 = link_ptr->BA_Dir ();
			dir_ptr = &dir_array [dir1];

			dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());
			link2_ptr->Anode (link_ptr->Bnode ());

			list_b = &node_list [link_ptr->Bnode ()];

			//---- update the from list at the anode ----

			for (index = list_b->From_List (); index >= 0; index = dir_ptr->First_Connect_From ()) {
				if (index == dir1) {
					if (index == list_b->From_List ()) {
						list_b->From_List (dir2);
					} else {
						dir_ptr->First_Connect_From (dir2);
					}
				}
				dir_ptr = &dir_array [index];

				if (index == dir1) {
					dir2_ptr->First_Connect_From (dir_ptr->First_Connect_From ());
					break;
				}
			}

			//---- update the to list at the bnode ----
			
			dir1 = link_ptr->AB_Dir ();
			dir2 = link2_ptr->BA_Dir ();

			if (dir1 >= 0 && dir2 >= 0) {

				dir_ptr = &dir_array [dir1];
				dir2_ptr = &dir_array [dir2];

				dir2_ptr->Time0 (dir_ptr->Time0 () + dir2_ptr->Time0 ());

				for (index = list_b->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
					if (index == dir1) {
						if (index == list_b->To_List ()) {
							list_b->To_List (dir2);
						} else {
							dir_ptr->First_Connect_To (dir2);
						}
					}
					dir_ptr = &dir_array [index];

					if (index == dir1) {
						dir2_ptr->First_Connect_To (dir_ptr->First_Connect_To ());
						break;
					}
				}
			}

			//---- insert the shape points in front ----

			if (shape_flag) {
				map_itr = shape_map.find (link_ptr->Link ());

				if (map_itr != shape_map.end ()) {
					shape2_ptr = &shape_array [map_itr->second];

					for (xyz_itr = shape2_ptr->begin (); xyz_itr != shape2_ptr->end (); xyz_itr++) {
						shape_ptr->insert (shape_ptr->begin (), *xyz_itr);
					}
				}
			}

			//---- merge the link nodes ----

			if (link_node_flag) {
				for (list_itr = ++link_node1->nodes.begin (); list_itr != link_node1->nodes.end (); list_itr++) {
					link_node2->nodes.push_front (*list_itr);
				}
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Collapsed Nodes = ") << num_drop;
}
