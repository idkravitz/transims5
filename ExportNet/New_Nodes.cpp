//*********************************************************
//	New_Nodes.cpp - insert new node records
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	New_Nodes
//---------------------------------------------------------

void ExportNet::New_Nodes (void)
{
	int node, link, count, index;
	double factor, side;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Split_Itr split_itr, end_itr;
	Link_Split_Itr link_itr;
	Node_Data node_rec;
	Points points;

	side = Internal_Units (20, METERS); 

	//---- insert nodes ----

	map_itr = --node_map.end ();
	node = map_itr->first;
	node = ((node + 1) / 100 + 1) * 100;
	index = (int) node_array.size ();

	node_rec.Subarea (1);
	count = 0;
	Set_Progress ();

	for (link=0, link_itr = link_splits.begin (); link_itr != link_splits.end (); link_itr++, link++) {
		if (link_itr->size () == 0) continue;
		Show_Progress ();

		link_ptr = &link_array [link];

		end_itr = link_itr->end () - 1;

		for (split_itr = link_itr->begin (); split_itr != end_itr; split_itr++) {
			split_itr->node = index;

			node_rec.Node (node++);

			factor = UnRound (split_itr->offset);

			Link_Shape (link_ptr, 0, points, factor, 0.0, 0.0);

			node_rec.X (Round (points [0].x));
			node_rec.Y (Round (points [0].y));

			node_map.insert (Int_Map_Data (node_rec.Node (), index++));
			node_array.push_back (node_rec);
			count++;

			//---- parking node in ab direction ----

			if (split_itr->park_ab < 0) {
				split_itr->park_ab = index;

				node_rec.Node (node++);

				Link_Shape (link_ptr, 0, points, factor, 0.0, side);

				node_rec.X (Round (points [0].x));
				node_rec.Y (Round (points [0].y));

				node_map.insert (Int_Map_Data (node_rec.Node (), index++));
				node_array.push_back (node_rec);
				count++;
			} else {
				split_itr->park_ab = split_itr->link_ab = -1;
			}

			//---- parking node in ba direction ----

			if (split_itr->park_ba < 0) {
				split_itr->park_ba = index;

				node_rec.Node (node++);

				factor = UnRound (link_ptr->Length ()) - factor;

				Link_Shape (link_ptr, 1, points, factor, 0.0, side);

				node_rec.X (Round (points [0].x));
				node_rec.Y (Round (points [0].y));

				node_map.insert (Int_Map_Data (node_rec.Node (), index++));
				node_array.push_back (node_rec);
				count++;
			} else {
				split_itr->park_ba = split_itr->link_ba = -1;
			}
		}
	}
	Print (2, "Number of New Node Records = ") << count;
}

