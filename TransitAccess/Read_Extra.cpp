//*********************************************************
//	Read_Extra.cpp - read the extra link file
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Extra
//---------------------------------------------------------

void TransitAccess::Read_Extra (void)
{
	int anode, bnode, dist, mode;
	double dx, dy, ax, ay, bx, by, factor;
	String action;
	Zone_Map_Itr zone_itr;
	Node_Map_Itr node_itr;

	Show_Message (String ("Reading %s -- Record") % extra_link_file.File_Type ());
	Set_Progress ();
	
	ax = ay = bx = by = 0;

	while (extra_link_file.Read ()) {
		Show_Progress ();

		action = extra_link_file.Get_String (extra_action_fld);
		if (action [0] != '-' && action [0] != '+') continue;

		anode = extra_link_file.Get_Integer (extra_anode_fld);
		bnode = extra_link_file.Get_Integer (extra_bnode_fld);

		skip_links.insert (Int2_Key (anode, bnode));
		factor = 1.0;
		mode = 13;

		if (anode <= nzones) {
			zone_itr = zone_map.find (anode);
			if (zone_itr != zone_map.end ()) {
				zone_itr->second.del_flag = true;
				factor = zone_itr->second.weight;
				ax = zone_itr->second.x_coord;
				ay = zone_itr->second.y_coord;
				mode = 16;
			}
		} else if (action [0] == '+') {
			node_itr = node_map.find (anode);
			if (node_itr != node_map.end ()) {
				ax = node_itr->second.x_coord;
				ay = node_itr->second.y_coord;
			}
		}
		if (bnode <= nzones) {
			zone_itr = zone_map.find (bnode);
			if (zone_itr != zone_map.end ()) {
				zone_itr->second.del_flag = true;
				factor = zone_itr->second.weight;
				bx = zone_itr->second.x_coord;
				by = zone_itr->second.y_coord;
				mode = 16;
			}
		} else if (action [0] == '+') {
			node_itr = node_map.find (bnode);
			if (node_itr != node_map.end ()) {
				bx = node_itr->second.x_coord;
				by = node_itr->second.y_coord;
			}
		}
		if (action [0] != '+') continue;

		dx = ax - bx;
		dy = ay - by;

		dist = DTOI (factor * sqrt (dx * dx + dy * dy) / 52.8);

		action (" N=%d-%d MODE=%d SPEED=3 ONEWAY=%c DIST=%d") % 
			anode % bnode % mode % ((mode == 13) ? 'T' : 'F') % dist;

		if (mode == 13) {
			sidewalk_file.File () << "SUPPORT" << action << endl;
		} else {
			walk_acc_file.File () << "SUPPORT" << action << endl;
		}
		walk_link_file.File () << "SUPPLINK" << action << endl;
	}
	End_Progress ();
	extra_link_file.Close ();

	Print (2, "Number of Extra Link Records = ") << Progress_Count ();
}
