//*********************************************************
//	Old_Links.cpp - read the old link file
//*********************************************************

#include "Relocate.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Old_Links
//---------------------------------------------------------

void Relocate::Old_Links (void)
{
	int num, link, dir, anode, bnode, lvalue, ab_in, ab_out;
	string text;

	Int_Map_Itr map_itr;	
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Link_Data link_rec;
	Dir_Data ab_rec, ba_rec;

	//---- store the link data ----

	Show_Message (String ("Reading %s -- Record") % old_link_file.File_Type ());
	Set_Progress ();

	num = old_link_file.Num_Records ();
	old_link_array.reserve (num);

	num *= 2;
	old_dir_array.reserve (num);

	anode = bnode = 0;

	while (old_link_file.Read ()) {
		Show_Progress ();

		link = old_link_file.Link ();
		if (link == 0) continue;

		link_rec.Clear ();
		ab_rec.Clear ();
		ba_rec.Clear ();

		link_rec.Link (link);

		anode = old_link_file.Node_A ();

		map_itr = old_node_map.find (anode);
		if (map_itr == old_node_map.end ()) {
			Warning (String ("Link %d Node %d was Not Found") % link % anode);
			continue;
		}
		link_rec.Anode (map_itr->second);

		bnode = old_link_file.Node_B ();

		map_itr = old_node_map.find (bnode);
		if (map_itr == old_node_map.end ()) {
			Warning (String ("Link %d Node %d was Not Found") % link % bnode);
			continue;
		}
		link_rec.Bnode (map_itr->second);

		//----  check the link lengths ----

		link_rec.Length (old_link_file.Length ());
		link_rec.Aoffset (old_link_file.Setback_A ());
		link_rec.Boffset (old_link_file.Setback_B ());
		link_rec.Type (old_link_file.Type ());
		link_rec.Use (old_link_file.Use ());
		link_rec.Area_Type (old_link_file.Area_Type ());

		//---- shape index ----

		map_itr = shape_map.find (link);
		if (map_itr == shape_map.end ()) {
			link_rec.Shape (-1);
		} else {
			link_rec.Shape (map_itr->second);
		}
		ab_in = old_link_file.Bearing_A ();
		ab_out = old_link_file.Bearing_B ();

		//---- A->B direction ----

		dir = (int) old_dir_array.size ();

		lvalue = old_link_file.Lanes_AB ();

		if (lvalue > 0) {
			link_rec.AB_Dir (dir++);

			ab_rec.Link (link);
			ab_rec.Dir (0);

			ab_rec.Lanes (lvalue);

			lvalue = Round (old_link_file.Fspd_AB ());
			ab_rec.Time0 ((double) link_rec.Length () / lvalue + 0.09);

			ab_rec.Speed (old_link_file.Speed_AB ());
			ab_rec.Capacity (old_link_file.Cap_AB ());

			ab_rec.In_Bearing (ab_in);
			ab_rec.Out_Bearing (ab_out);
		} else {
			link_rec.AB_Dir (-1);
		}

		//---- B->A direction ----

		lvalue = old_link_file.Lanes_BA ();

		if (lvalue > 0) {
			link_rec.BA_Dir (dir);

			ba_rec.Link (link);
			ba_rec.Dir (1);

			ba_rec.Lanes (lvalue);

			lvalue = Round (old_link_file.Fspd_BA ());
			ba_rec.Time0 ((double) link_rec.Length () / lvalue + 0.09);

			ba_rec.Speed (old_link_file.Speed_BA ());
			ba_rec.Capacity (old_link_file.Cap_BA ());

			ba_rec.In_Bearing (compass.Flip (ab_out));
			ba_rec.Out_Bearing (compass.Flip (ab_in));
		} else {
			link_rec.BA_Dir (-1);
		}

		//---- save the link records ----

		map_stat = old_link_map.insert (Int_Map_Data (link_rec.Link (), (int) old_link_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Link Number = ") << link_rec.Link ();
			continue;
		} else {
			old_link_array.push_back (link_rec);
		}

		//---- insert the AB direction ----

		if (link_rec.AB_Dir () >= 0) {
			lvalue = (int) old_dir_array.size ();

			map_stat = old_dir_map.insert (Int_Map_Data (ab_rec.Link_Dir (), lvalue));

			if (!map_stat.second) {
				Warning ("Duplicate Link Direction Number = ") << ab_rec.Link_Dir ();
				continue;
			} else {
				old_dir_array.push_back (ab_rec);
			}
			if (lvalue != link_rec.AB_Dir ()) {
				Error (String ("%s Direction Numbering") % old_link_file.File_ID ());
			}
			ab_stat = old_ab_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), lvalue));

			if (!ab_stat.second) {
				Warning (String ("Duplicate Anode-Bnode Key = %d-%d") % anode % bnode);
			}
		}
		if (link_rec.BA_Dir () >= 0) {
			lvalue = (int) old_dir_array.size ();

			map_stat = old_dir_map.insert (Int_Map_Data (ba_rec.Link_Dir (), lvalue));

			if (!map_stat.second) {
				Warning ("Duplicate Link Direction Number = ") << ba_rec.Link_Dir ();
				continue;
			} else {
				old_dir_array.push_back (ba_rec);
			}
			if (lvalue != link_rec.BA_Dir ()) {
				Error (String ("%s Direction Numbering") % old_link_file.File_ID ());
			}
			ab_stat = old_ab_map.insert (Int2_Map_Data (Int2_Key (bnode, anode), lvalue));

			if (!ab_stat.second) {
				Warning (String ("Duplicate Anode-Bnode Key = %d-%d") % bnode % anode);
			}
		}
	}
	End_Progress ();
	old_link_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_link_file.File_Type () % Progress_Count ());

	num = (int) old_link_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_link_file.File_ID () % num);
	}
	num = (int) dir_array.size ();

	if (num) Print (1, "Number of Directional Links = ") << num;
}

