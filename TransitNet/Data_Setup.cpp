//*********************************************************
//	Data_Setup.cpp - set the node and link data
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Data_Setup
//---------------------------------------------------------

void TransitNet::Data_Setup (void)
{
	int i, dir, *list, node, zone, at, type;
	double dx, dy, closest, distance;

	Link_Data *link_ptr;
	Link_Itr link_itr;
	Stop_Itr stop_itr;
	Int_Map *stop_list;
	Zone_Itr zone_itr;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;

	//---- initialize the stop list ----

	if ((int) dir_stop_array.size () == 0) {
		Int_Map list_rec;
		dir_stop_array.assign (dir_array.size (), list_rec);
	}

	//---- add stops to the stop list ----

	for (nstop = 0, stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++, nstop++) {
		link_ptr = &link_array [stop_itr->Link ()];

		if (stop_itr->Dir () == 0) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = link_ptr->BA_Dir ();
		}
		if (dir < 0) continue;

		stop_list = &dir_stop_array [dir];

		stop_list->insert (Int_Map_Data (stop_itr->Offset (), nstop));

		if (stop_itr->Stop () > max_stop) max_stop = stop_itr->Stop ();
	}

	//---- max access link ----

	max_access = 0;
	if (access_map.size () > 0) {
		map_itr = --access_map.end ();
		max_access = map_itr->first;
	}

	//---- build the link list ----

	Show_Message ("Link Data Processing -- Record");
	Set_Progress ();

	node_list.assign (node_array.size (), -1);
	dir_list.assign (dir_array.size (), -1);

	local_access.assign (dir_array.size (), 0);
	fare_zone.assign (dir_array.size (), 0);

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Length () == 0) continue;

		type = link_itr->Type ();
		if (!facility_flag [type]) type = 0;

		for (i=0; i < 2; i++) {
			if (i == 0) {
				dir = link_itr->AB_Dir ();
				node = link_itr->Anode ();
			} else {
				dir = link_itr->BA_Dir ();
				node = link_itr->Bnode ();
			}
			if (dir < 0) continue;

			//---- build the node list ----

			list = &node_list [node];

			dir_list [dir] = *list;
			*list = dir;

			node_ptr = &node_array [node];

			//---- find the closest zone ----

			closest = 0;
			zone = 0;
			at = 2;

			for (zone_itr = zone_array.begin (); zone_itr != zone_array.end (); zone_itr++) {
				dx = zone_itr->X () - node_ptr->X ();
				dy = zone_itr->Y () - node_ptr->Y ();

				distance = dx * dx + dy * dy;

				if (zone == 0 || distance < closest) {
					zone = zone_itr->Zone ();
					closest = distance;
					at = zone_itr->Area_Type ();
				}
			}
			if (at_flag) {
				link_itr->Area_Type (at);
			}

			//---- set the fare zone ----

			if (equiv_flag && zone > 0) {
				fare_zone [dir] = zone_equiv.Zone_Group (zone);
			} else {
				fare_zone [dir] = zone;
			}

			//---- set the local access flag ----

			local_access [dir] = type;
		}
	}
	End_Progress ();
}
