//*********************************************************
//	Node_Path.cpp - build a path between two nodes
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Node_Path
//---------------------------------------------------------

void TransitNet::Node_Path (int node1, int node2, Use_Type use)
{
	int anode, bnode, best_cum, cum_a, cum_b, dir_index, index;

	Node_Path_Data *node_ptr, *first_ptr, *last_ptr, path_root;
	Path_Leg_Data path_rec;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Data *connect_ptr;

	path_leg_array.clear ();

	index = node_list [node1];
	if (index < 0) return;

	node_path_array.assign (node_array.size (), path_root);

	first_ptr = &path_root;
	first_ptr->Next_List (node1);
	last_ptr = &node_path_array [node1];

	//---- add links leaving the origin node ----

	for (; index >= 0; index = dir_list [index]) {
		dir_ptr = &dir_array [index];
		link_ptr = &link_array [dir_ptr->Link ()];

		if (!Use_Permission (link_ptr->Use (), use)) continue;

		if (dir_ptr->Dir () == 0) {
			bnode = link_ptr->Bnode ();
		} else {
			bnode = link_ptr->Anode ();
		}
		cum_b = link_ptr->Length ();

		node_ptr = &node_path_array [bnode];

		if (node_ptr->Next_List () == -1 && last_ptr != node_ptr) {
			last_ptr->Next_List (bnode);
			last_ptr = node_ptr;
		}
		node_ptr->Distance (cum_b);
		node_ptr->From_Node (node1);
		node_ptr->Dir_Index (index);
	}
	if (last_ptr == &node_path_array [node1]) return;

	//---- build a path to the destination node ----

	best_cum = MAX_INTEGER;

	for (;;) {
		anode = first_ptr->Next_List ();
		if (anode < 0) break;

		first_ptr->Next_List (-2);

		first_ptr = &node_path_array [anode];
		if (first_ptr->Dir_Index () < 0) continue;

		//---- check the cumulative impedance ----

		cum_a = first_ptr->Distance ();

		if (cum_a >= best_cum) continue;

		//---- identify the approach link ----

		dir_ptr = &dir_array [first_ptr->Dir_Index ()];

		//---- process each link leaving the node ----

		for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {

			connect_ptr = &connect_array [index];

			dir_index = connect_ptr->To_Index ();

			dir_ptr = &dir_array [dir_index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (!Use_Permission (link_ptr->Use (), use)) continue;

			if (dir_ptr->Dir () == 0) {
				bnode = link_ptr->Bnode ();
			} else {
				bnode = link_ptr->Anode ();
			}
			if (bnode == node1) continue;

			//---- check the cumulative distance ----

			cum_b = cum_a + link_ptr->Length ();
			if (cum_b > best_cum) continue;

			node_ptr = &node_path_array [bnode];

			if (cum_b >= node_ptr->Distance () && node_ptr->Distance () > 0) continue;

			if (bnode == node2) {
				best_cum = cum_b;
			}

			//---- add to the tree ----

			if (node_ptr->Next_List () == -2) {
				node_ptr->Next_List (first_ptr->Next_List ());
				first_ptr->Next_List (bnode);
			} else if (node_ptr->Next_List () == -1 && last_ptr != node_ptr) {
				last_ptr->Next_List (bnode);
				last_ptr = node_ptr;
			}
			node_ptr->Distance (cum_b);
			node_ptr->From_Node (anode);
			node_ptr->Dir_Index (dir_index);
		}
	}

	//---- trace the path ----

	anode = node2;
	while (anode != node1 && anode > 0) {
		node_ptr = &node_path_array [anode];
		path_rec.Node (anode);

		anode = node_ptr->From_Node ();
		if (anode < 0) break;

		path_rec.Dir_Index (node_ptr->Dir_Index ());

		path_leg_array.push_back (path_rec);
	}
}
