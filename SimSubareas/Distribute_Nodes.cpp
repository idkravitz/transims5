//*********************************************************
//	Distribute_Nodes.cpp - assign nodes to subareas
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	Distribute_Nodes
//---------------------------------------------------------

void SimSubareas::Distribute_Nodes (void)
{
	int i, index, total, num_ring, num_wedge, sub_count, subarea, num;
	double x, y, dx, dy, diff, best, max_diff;
	Node_Itr node_itr;
	Node_Data *center_ptr;
	Int_Map_Itr map_itr;
	Integers dist_distrib, ring, wedge_count, target;
	Int_Itr dist_itr, wedge_itr;

	if (num_subareas == 1) {
		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			node_itr->Subarea (0);
		}
		return;
	}

	//---- get the center node ----

	if (center == 0) {
		if (node_array.size () == 0) {
			Error ("The Node File is Empty");
		}

		//---- find the geographic center ----

		x = y = 0.0;

		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			x += node_itr->X ();
			y += node_itr->Y ();
		}
		x /= node_array.size ();
		y /= node_array.size ();

		best = 0.0;
		center_ptr = 0;

		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			dx = node_itr->X () - x;
			dy = node_itr->Y () - y;

			diff = dx * dx + dy * dy;

			if (center == 0 || diff < best) {
				center = node_itr->Node ();
				center_ptr = &(*node_itr);
				best = diff;
			}
		}
		if (center == 0) {
			Error ("A Center Node was Not Found");
		}
		Write (2, "Center Node Number = ") << center;
		
	} else {
		map_itr = node_map.find (center);

		if (map_itr == node_map.end ()) {
			Error (String ("Center Node %d was Not Found") % center);
		}
		center_ptr = &node_array [map_itr->second];
	}

	//---- find the furtherest node ----

	max_diff = 0.0;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		dx = node_itr->X () - center_ptr->X ();
		dy = node_itr->Y () - center_ptr->Y ();

		diff = dx * dx + dy * dy;

		if (diff > max_diff) max_diff = diff;
	}

	//---- build a node distance distribution ----

	dist_distrib.assign (1000, 0);
	max_diff /= 999;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		dx = node_itr->X () - center_ptr->X ();
		dy = node_itr->Y () - center_ptr->Y ();

		index = (int) ((dx * dx + dy * dy) / max_diff);
		dist_distrib [index]++;
	}

	//---- find the ring indices ----

	sub_count = ((int) node_array.size () + num_subareas / 2) / num_subareas;
	if (sub_count < 1) sub_count = 1;

	num_ring = (num_subareas - 1) / 8 + 1;
	num_wedge = (num_subareas - 1 + num_ring - 1) / num_ring;

	total = sub_count;
	target.push_back (total);
	wedge_count.push_back (1);
	num = 1;

	for (i=1; i <= num_ring; i++) {
		total += sub_count * num_wedge;
		num += num_wedge;
		if (num > num_subareas) {
			num = num - num_subareas;
			total -= num * sub_count;
			wedge_count.push_back (num_wedge - num);
		} else {
			wedge_count.push_back (num_wedge);
		}
		target.push_back (total);
	}
	target [num_ring] = (int) node_array.size ();
	total = index = 0;

	for (i=0, dist_itr = dist_distrib.begin (); dist_itr != dist_distrib.end (); dist_itr++, i++) {
		total += *dist_itr;

		if (total >= target [index]) {
			ring.push_back (i);
			if (++index == (int) target.size ()) break;
		}
	}

	//---- assign nodes to a ring and wedge ----

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		dx = node_itr->X () - center_ptr->X ();
		dy = node_itr->Y () - center_ptr->Y ();

		index = (int) ((dx * dx + dy * dy) / max_diff);
		subarea = -1;

		wedge_itr = wedge_count.begin ();

		for (dist_itr = ring.begin (); dist_itr != ring.end (); dist_itr++, wedge_itr++) {
			if (index <= *dist_itr) {
				if (*wedge_itr > 1) {
					compass.Set_Points (*wedge_itr);
					subarea += compass.Direction (dx, dy);
				}
				node_itr->Subarea (++subarea);
				break;
			} else {
				subarea += *wedge_itr;
			}
		}
	}
}
