//*********************************************************
//	Station_Access.cpp - add access to transit stations
//*********************************************************

#include "TransitNet.hpp"

#include "Shape_Tools.hpp"
#include <math.h>

//---------------------------------------------------------
//	Station_Access
//---------------------------------------------------------

void TransitNet::Station_Access (void)
{
	int dir, index, node, max_walk, offset, speed, num, best;
	Dtime time;
	double dx, dy, closest, distance;
	bool walk_flag;

	Link_Data *link_ptr, *link2_ptr;
	Dir_Data *dir_ptr;
	Stop_Itr stop_itr;
	Node_Itr node_itr;
	Int_Map_Stat map_stat;
	Access_Data access_rec;
	Points points;
	XYZ_Point pt;

	max_walk = Round (Internal_Units (200, METERS));
	speed = Round (Internal_Units (1.0, MPS));

	Show_Message ("Adding Station Access -- Record");
	Set_Progress ();

	//---- find transit stops on non-walk links ----

	for (nstop = 0, stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++, nstop++) {
		if (stop_itr->Stop () <= new_stop) continue;

		link_ptr = &link_array [stop_itr->Link ()];

		walk_flag = Use_Permission (link_ptr->Use (), WALK);

		if (stop_itr->Type () == STOP && walk_flag) continue;

		Show_Progress ();

		if (stop_itr->Offset () < link_ptr->Length () / 2) {
			if (stop_itr->Dir () == 0) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			offset = stop_itr->Offset ();
		} else {
			if (stop_itr->Dir () == 0) {
				node = link_ptr->Bnode ();
			} else {
				node = link_ptr->Anode ();
			}
			offset = link_ptr->Length () - stop_itr->Offset ();
		}
		if (offset < max_walk) {
			if (!walk_flag) {
				time.Seconds (offset / speed);

				access_rec.Link (++max_access);
				access_rec.From_Type (STOP_ID);
				access_rec.From_ID (nstop);
				access_rec.To_Type (NODE_ID);
				access_rec.To_ID (node);
				access_rec.Dir (2);
				access_rec.Time (time);
				access_rec.Cost (0);
				access_rec.Notes ("Station Access");

				access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));
				access_array.push_back (access_rec);
			}
			num = 0;
			for (dir = node_list [node]; dir >= 0; dir = dir_list [dir]) {
				dir_ptr = &dir_array [dir];
				if (dir_ptr->Link () == stop_itr->Link ()) continue;

				link2_ptr = &link_array [dir_ptr->Link ()];
				if (!Use_Permission (link2_ptr->Use (), WALK)) continue;
				num++;
				if (dir_stop_array [dir].size () > 0) num++;
			}
			if (num >= 2) continue;
		}

		//---- find nodes close to the station ----

		Link_Shape (link_ptr, stop_itr->Dir (), points, UnRound (stop_itr->Offset ()), 0.0);

		pt = points [0];

		closest = max_walk + 1;
		best = -1;

		for (index=0, node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++, index++) {
			if (index == node) continue;

			dx = UnRound (node_itr->X ()) - pt.x;
			dy = UnRound (node_itr->Y ()) - pt.y;

			distance = sqrt (dx * dx + dy * dy);

			if (distance < closest) {
				num = 0;
				for (dir = node_list [index]; dir >= 0; dir = dir_list [dir]) {
					dir_ptr = &dir_array [dir];
					if (dir_ptr->Link () == stop_itr->Link ()) continue;

					link2_ptr = &link_array [dir_ptr->Link ()];
					if (!Use_Permission (link2_ptr->Use (), WALK)) continue;
					num++;
					if (dir_stop_array [dir].size () > 0) num++;
				}
				if (num >= 2) {
					closest = distance;
					best = index;
				}
			}
		}
		if (best >= 0) {
			time.Seconds (Round (closest) / speed);

			access_rec.Link (++max_access);
			access_rec.From_Type (STOP_ID);
			access_rec.From_ID (nstop);
			access_rec.To_Type (NODE_ID);
			access_rec.To_ID (best);
			access_rec.Dir (2);
			access_rec.Time (time);
			access_rec.Cost (0);
			access_rec.Notes ("Station Access");

			access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));
			access_array.push_back (access_rec);
		}
	}
	End_Progress ();
}
