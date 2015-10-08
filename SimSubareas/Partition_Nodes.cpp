//*********************************************************
//	Partition_Nodes.cpp - assign subarea nodes to partitions
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	Partition_Nodes
//---------------------------------------------------------

void SimSubareas::Partition_Nodes (int subarea)
{
	int i, index, center, total, num_ring, num_wedge, part, sub_size, part_count, num;
	double x, y, dx, dy, diff, best, max_diff;
	Node_Itr node_itr;
	Node_Data *center_ptr;
	Int_Map_Itr map_itr;
	Integers dist_distrib, ring, wedge_count, target;
	Int_Itr dist_itr, wedge_itr;

	//---- find the geographic center ----

	x = y = 0.0;
	sub_size = 0;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == subarea) {
			x += node_itr->X ();
			y += node_itr->Y ();
			sub_size++;
		}
	}
	if (sub_size == 0) return;
	x /= sub_size;
	y /= sub_size;

	best = 0.0;
	center_ptr = 0;
	center = 0;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == subarea) {
			dx = node_itr->X () - x;
			dy = node_itr->Y () - y;

			diff = dx * dx + dy * dy;

			if (center == 0 || diff < best) {
				center = node_itr->Node ();
				center_ptr = &(*node_itr);
				best = diff;
			}
		}
	}

	//---- find the furtherest node ----

	max_diff = 0.0;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == subarea) {
			dx = node_itr->X () - center_ptr->X ();
			dy = node_itr->Y () - center_ptr->Y ();

			diff = dx * dx + dy * dy;

			if (diff > max_diff) max_diff = diff;
		}
	}

	//---- build a node distance distribution ----

	dist_distrib.assign (1000, 0);
	max_diff /= 999;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () == subarea) {
			dx = node_itr->X () - center_ptr->X ();
			dy = node_itr->Y () - center_ptr->Y ();

			index = (int) ((dx * dx + dy * dy) / max_diff);
			dist_distrib [index]++;
		}
	}

	//---- find the ring indices ----

	part_count = (sub_size + num_parts / 2) / num_parts;
	if (part_count < 1) part_count = 1;

	num_ring = (num_parts - 1) / 8 + 1;
	num_wedge = (num_parts - 1 + num_ring - 1) / num_ring;

	total = part_count;
	target.push_back (total);
	wedge_count.push_back (1);
	num = 1;

	for (i=1; i <= num_ring; i++) {
		total += part_count * num_wedge;
		num += num_wedge;
		if (num > num_parts) {
			num = num - num_parts;
			total -= num * part_count;
			wedge_count.push_back (num_wedge - num);
		} else {
			wedge_count.push_back (num_wedge);
		}
		target.push_back (total);
	}
	target [num_ring] = sub_size;
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
		if (node_itr->Subarea () == subarea) {
			dx = node_itr->X () - center_ptr->X ();
			dy = node_itr->Y () - center_ptr->Y ();

			index = (int) ((dx * dx + dy * dy) / max_diff);
			part = -1;

			wedge_itr = wedge_count.begin ();

			for (dist_itr = ring.begin (); dist_itr != ring.end (); dist_itr++, wedge_itr++) {
				if (index <= *dist_itr) {
					if (*wedge_itr > 1) {
						compass.Set_Points (*wedge_itr);
						part += compass.Direction (dx, dy);
					}
					node_itr->Partition (++part);
					break;
				} else {
					part += *wedge_itr;
				}
			}
		}
	}
}

