//*********************************************************
//	Read_Link.cpp - read the walk link file
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Link
//---------------------------------------------------------

void TransitAccess::Read_Link (void)
{
	int anode, bnode, dist, zone, type;
	double len;
	String text;
	Zone_Map_Itr zone_itr;
	Node_Map_Itr node_itr;

	//---- walk link data ----

	Show_Message (String ("Reading %s -- Record") % link_data_file.File_Type ());
	Set_Progress ();

	while (link_data_file.Read ()) {
		Show_Progress ();

		anode = link_data_file.Get_Integer (link_anode_fld);
		bnode = link_data_file.Get_Integer (link_bnode_fld);

		if (skip_links.find (Int2_Key (anode, bnode)) != skip_links.end ()) continue;
		if (skip_links.find (Int2_Key (bnode, anode)) != skip_links.end ()) continue;

		len = link_data_file.Get_Double (link_len_fld);
		zone = link_data_file.Get_Integer (link_zone_fld);
		type = link_data_file.Get_Integer (link_type_fld);

		if (!type_range.In_Range (type)) continue;

		zone_itr = zone_map.find (zone);
		if (zone_itr == zone_map.end ()) continue;
		if (zone_itr->second.walk == 0) continue;

		dist = DTOI (len * 100.0 * zone_itr->second.weight);

		node_itr = node_map.find (anode);
		if (node_itr != node_map.end ()) {
			node_itr->second.use = 1;
		}
		node_itr = node_map.find (bnode);
		if (node_itr != node_map.end ()) {
			node_itr->second.use = 1;
		}
	
		text (" N=%d-%d MODE=13 SPEED=3 ONEWAY=T DIST=%d") % anode % bnode % dist;

		sidewalk_file.File () << "SUPPORT" << text << endl;

		walk_link_file.File () << "SUPPLINK" << text << endl;
	}
	End_Progress ();
	link_data_file.Close ();

	Print (2, "Number of Link Data Records = ") << Progress_Count ();
}
