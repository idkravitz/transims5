//*********************************************************
//	Split_Loops.cpp - split a loop into two links
//*********************************************************

#include "NetPrep.hpp"

#include <math.h>

//---------------------------------------------------------
//	Split_Loops
//---------------------------------------------------------

void NetPrep::Split_Loops (void)
{
	int num_splits = 0;
	int index, x1, y1, x2, y2, id, node;
	double dx, dy, len, length, dist;

	Node_Data node_rec, *node_ptr;
	Link_Itr link_itr;
	Link_Data link_rec;
	Dir_Data *dir_ptr, dir_rec;
	Shape_Data *shape_ptr, shape_rec;
	XYZ xyz;
	XYZ_Itr xyz_itr, mid_itr;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;

	Show_Message ("Splitting Large Loops -- Record");
	Set_Progress ();
	
	for (id=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, id++) {
		Show_Progress ();

		if (link_itr->Anode () != link_itr->Bnode ()) continue;

		//---- update the shape records ----

		map_itr = shape_map.find (link_itr->Link ());

		if (map_itr == shape_map.end ()) continue;

		shape_ptr = &shape_array [map_itr->second];

		shape_rec.Clear ();
		shape_rec.Link (new_link);

		length = link_itr->Length () / 2.0;
		node_ptr = &node_array [link_itr->Anode ()];

		x1 = node_ptr->X ();
		y1 = node_ptr->Y ();
		len = 0;

		//---- find the mid-point ----

		for (xyz_itr = mid_itr = shape_ptr->begin (); ; xyz_itr++) {
			if (xyz_itr != shape_ptr->end ()) {
				x2 = xyz_itr->x;
				y2 = xyz_itr->y;
			} else {
				node_ptr = &node_array [link_itr->Bnode ()];
				x2 = node_ptr->X ();
				y2 = node_ptr->Y ();
			}
			dx = x2 - x1;
			dy = y2 - y1;

			dist = sqrt (dx * dx + dy * dy);

			len += dist;
			if (len >= length || xyz_itr == shape_ptr->end ()) {
				mid_itr = xyz_itr;

				if (dist > 0) {
					node_rec.X (x1);
					node_rec.Y (y1);
				} else if (len == length) {
					node_rec.X (x2);
					node_rec.Y (y2);
				} else {
					dist = (len - length) / dist;
					node_rec.X ((int) (x1 + dist * dx + 0.5));
					node_rec.Y ((int) (y1 + dist * dy + 0.5));	
				}
				break;
			}
			x1 = x2;
			y1 = y2;
		}

		if (mid_itr != shape_ptr->end ()) {
			shape_rec.assign (mid_itr, shape_ptr->end ());

			shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) shape_array.size ()));
			shape_array.push_back (shape_rec);

			shape_ptr->erase (mid_itr);
		}

		//---- add the new node ----

		node_rec.Node (new_node);
		node_rec.Z (0);
		node_rec.Count (2);
		node = (int) node_array.size ();

		map_stat = node_map.insert (Int_Map_Data (new_node, node));
		if (!map_stat.second) {
			Warning ("Duplicate Node Number = ") << new_node;
		} else {
			node_array.push_back (node_rec);
		}

		//---- split the link record ----

		link_itr->Length (link_itr->Length () / 2);

		link_rec = *link_itr;

		link_itr->Bnode (node);

		link_rec.Link (new_link);
		link_rec.Anode (node);

		index = (int) link_array.size ();

		//---- get the ab link ----

		if (link_itr->AB_Dir () >= 0) {
			dir_ptr = &dir_array [link_itr->AB_Dir ()];

			dir_ptr->Time0 (dir_ptr->Time0 () / 2);

			dir_rec = *dir_ptr;
			dir_rec.Link (index);

			link_rec.AB_Dir ((int) dir_array.size ());

			dir_array.push_back (dir_rec);
		}

		//---- get the ba link ----

		if (link_itr->BA_Dir () >= 0) {
			dir_ptr = &dir_array [link_itr->BA_Dir ()];

			dir_ptr->Time0 (dir_ptr->Time0 () / 2);

			dir_rec = *dir_ptr;
			dir_rec.Link (index);

			link_rec.BA_Dir ((int) dir_array.size ());

			dir_array.push_back (dir_rec);
		}
		map_stat = link_map.insert (Int_Map_Data (link_rec.Link (), index));

		if (!map_stat.second) {
			Warning ("Duplicate Link Number = ") << link_rec.Link ();
		} else {
			link_array.push_back (link_rec);
			link_itr = link_array.begin () + id;

			if (link_node_flag) {
				Link_Nodes rec;
				rec.link = link_rec.Link ();
				link_node_array.push_back (rec);
			}
		}
		new_node++;
		new_link++;
		num_splits++;
	}
	End_Progress ();

	Print (2, "Number of Large Loop Splits = ") << num_splits;
}
