//*********************************************************
//	Compare_Links.cpp - compare link attributes
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Compare_Links
//---------------------------------------------------------

bool NetPrep::Compare_Links (Link_Data *link_ptr, Link_Data *link2_ptr, int node, bool near_flag)
{
	int dir1, dir2, bearing1, bearing2;
	int diff, short_length, anode, bnode;
	double dx, dy;
	bool twoway_flag;

	Dir_Data *dir_ptr, *dir2_ptr;
	Node_Data *anode_ptr, *bnode_ptr;

	//---- compare the links ----
	
	if (link_ptr == 0 || link2_ptr == 0) return (false);
	if (link_ptr->Type () != link2_ptr->Type () || link_ptr->Type () == EXTERNAL) return (false);
	if (link_ptr->Use () != link2_ptr->Use ()) return (false);

	dir1 = (link_ptr->AB_Dir () >= 0) ? 1 : 0;
	dir1 += (link_ptr->BA_Dir () >= 0) ? 1 : 0;

	dir2 = (link2_ptr->AB_Dir () >= 0) ? 1 : 0;
	dir2 += (link2_ptr->BA_Dir () >= 0) ? 1 : 0;
	
	if (dir1 != dir2) return (false);

	twoway_flag = (dir1 > 1);
	short_length = Round (Internal_Units (200.0, METERS));

	//---- check link to link2 direction ----

	if (link_ptr->Bnode () == node || (node < 0 && link_ptr->AB_Dir () >= 0)) {
		dir1 = link_ptr->AB_Dir ();
		anode = link_ptr->Anode ();
	} else {
		dir1 = link_ptr->BA_Dir ();
		anode = link_ptr->Bnode ();
	}
	if (link2_ptr->Anode () == node || (node < 0 && link2_ptr->AB_Dir () >= 0)) {
		dir2 = link2_ptr->AB_Dir ();
		bnode = link2_ptr->Bnode ();
	} else {
		dir2 = link2_ptr->BA_Dir ();
		bnode = link2_ptr->Anode ();
	}

	if (dir1 >= 0 && dir2 >= 0 && anode != bnode) {
		dir_ptr = &dir_array [dir1];
		dir2_ptr = &dir_array [dir2];

		if (dir_ptr->Lanes () != dir2_ptr->Lanes ()) return (false);
		if (!near_flag && abs (dir_ptr->Speed () - dir2_ptr->Speed ()) > Round (0.9)) return (false);
		if (!near_flag && abs (dir_ptr->Capacity () - dir2_ptr->Capacity ()) > 100) return (false); 

		//---- check for shape nodes ----

		if (!shape_flag) {
			anode_ptr = &node_array [link_ptr->Anode ()];
			bnode_ptr = &node_array [link_ptr->Bnode ()];
			
			dx = bnode_ptr->X () - anode_ptr->X ();
			dy = bnode_ptr->Y () - anode_ptr->Y ();

			if (dir_ptr->Dir () == 0) {
				bearing1 = compass.Direction (dx, dy);
			} else {
				bearing1 = compass.Direction (-dx, -dy);
			}
			anode_ptr = &node_array [link2_ptr->Anode ()];
			bnode_ptr = &node_array [link2_ptr->Bnode ()];
			
			dx = bnode_ptr->X () - anode_ptr->X ();
			dy = bnode_ptr->Y () - anode_ptr->Y ();
			
			if (dir2_ptr->Dir () == 0) {
				bearing2 = compass.Direction (dx, dy);
			} else {
				bearing2 = compass.Direction (-dx, -dy);
			}
			diff = compass.Difference (bearing1, bearing2);

			if (diff > thru_diff) return (false);

			if (diff > straight_diff && link_ptr->Length () > short_length && 
				link2_ptr->Length () > short_length) return (false);
		}
	} else if (dir1 >= 0 || dir2 >= 0) {
		return (false);
	}

	if (node < 0) return (true);

	//---- check link2 to link direction ----

	if (link_ptr->Anode () == node) {
		dir1 = link_ptr->AB_Dir ();
		bnode = link_ptr->Bnode ();
	} else {
		dir1 = link_ptr->BA_Dir ();
		bnode = link_ptr->Anode ();
	}
	if (link2_ptr->Bnode () == node) {
		dir2 = link2_ptr->AB_Dir ();
		anode = link2_ptr->Anode ();
	} else {
		dir2 = link2_ptr->BA_Dir ();
		anode = link2_ptr->Bnode ();
	}
	if (dir1 >= 0 && dir2 >= 0 && anode != bnode) {
		dir_ptr = &dir_array [dir1];
		dir2_ptr = &dir_array [dir2];

		if (dir_ptr->Lanes () != dir2_ptr->Lanes ()) return (false);
		if (!near_flag && abs (dir_ptr->Speed () - dir2_ptr->Speed ()) > Round (0.9)) return (false);
		if (!near_flag && abs (dir_ptr->Capacity () - dir2_ptr->Capacity ()) > 100) return (false); 

		//---- check for shape nodes ----

		if (!shape_flag && !twoway_flag) {
			anode_ptr = &node_array [link_ptr->Anode ()];
			bnode_ptr = &node_array [link_ptr->Bnode ()];
			
			dx = bnode_ptr->X () - anode_ptr->X ();
			dy = bnode_ptr->Y () - anode_ptr->Y ();

			if (dir_ptr->Dir () == 0) {
				bearing1 = compass.Direction (dx, dy);
			} else {
				bearing1 = compass.Direction (-dx, -dy);
			}
			anode_ptr = &node_array [link2_ptr->Anode ()];
			bnode_ptr = &node_array [link2_ptr->Bnode ()];
			
			dx = bnode_ptr->X () - anode_ptr->X ();
			dy = bnode_ptr->Y () - anode_ptr->Y ();
			
			if (dir2_ptr->Dir () == 0) {
				bearing2 = compass.Direction (dx, dy);
			} else {
				bearing2 = compass.Direction (-dx, -dy);
			}
			diff = compass.Difference (bearing2, bearing1);

			if (diff > thru_diff) return (false);

			if (diff > straight_diff && link_ptr->Length () > short_length && 
				link2_ptr->Length () > short_length) return (false);
		}
	} else if (dir1 >= 0 || dir2 >= 0) {
		return (false);
	}
	return (true);
}
