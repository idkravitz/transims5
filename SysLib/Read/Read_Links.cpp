//*********************************************************
//	Read_Links.cpp - read the link file
//*********************************************************

#include "Data_Service.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Links
//---------------------------------------------------------

void Data_Service::Read_Links (void)
{
	int num, anode, bnode, index;
	Link_File *file = (Link_File *) System_File_Handle (LINK);
	
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Link_Data link_rec;
	Dir_Data ab_rec, ba_rec;

	//---- store the link data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Links (*file);

	anode = bnode = 0;

	while (file->Read ()) {
		Show_Progress ();

		link_rec.Clear ();
		ab_rec.Clear ();
		ba_rec.Clear ();

		if (AB_Map_Flag ()) {
			anode = file->Node_A ();
			bnode = file->Node_B ();
		}
		if (Get_Link_Data (*file, link_rec, ab_rec, ba_rec)) {
			map_stat = link_map.insert (Int_Map_Data (link_rec.Link (), (int) link_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << link_rec.Link ();
				continue;
			} else {
				link_array.push_back (link_rec);
			}

			//---- insert the AB direction ----

			if (link_rec.AB_Dir () >= 0) {
				index = (int) dir_array.size ();

				map_stat = dir_map.insert (Int_Map_Data (ab_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ab_rec.Link_Dir ();
					continue;
				} else {
					dir_array.push_back (ab_rec);
				}
				if (index != link_rec.AB_Dir ()) goto num_error;

				if (AB_Map_Flag ()) {
					ab_stat = ab_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), index));

					if (!ab_stat.second) {
						Warning (String ("Duplicate Anode-Bnode Key = %d-%d") % anode % bnode);
					}
				}
			}
			if (link_rec.BA_Dir () >= 0) {
				index = (int) dir_array.size ();

				map_stat = dir_map.insert (Int_Map_Data (ba_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ba_rec.Link_Dir ();
					continue;
				} else {
					dir_array.push_back (ba_rec);
				}
				if (index != link_rec.BA_Dir ()) goto num_error;

				if (AB_Map_Flag ()) {
					ab_stat = ab_map.insert (Int2_Map_Data (Int2_Key (bnode, anode), index));

					if (!ab_stat.second) {
						Warning (String ("Duplicate Anode-Bnode Key = %d-%d") % bnode % anode);
					}
				}
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) link_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (LINK);

	num = (int) dir_array.size ();

	if (num) Print (1, "Number of Directional Links = ") << num;
	return;

num_error:
	Error (String ("%s Direction Numbering") % file->File_ID ());
}

//---------------------------------------------------------
//	Initialize_Links
//---------------------------------------------------------

void Data_Service::Initialize_Links (Link_File &file)
{
	Required_File_Check (file, NODE);

	int percent = System_Data_Reserve (LINK);

	if (link_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			link_array.reserve (num);
			if (num > (int) link_array.capacity ()) Mem_Error (file.File_ID ());

			num *= 2;
			dir_array.reserve (num);
			if (num > (int) dir_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool Data_Service::Get_Link_Data (Link_File &file, Link_Data &link_rec, Dir_Data &ab_rec, Dir_Data &ba_rec)
{
	int lvalue, link, dir, lane_cap, ab_in, ab_out;
	string text;

	Int_Map_Itr map_itr;

	//---- set the link number ----

	link = (int) link_array.size ();

	lvalue = file.Link ();
	if (lvalue == 0) lvalue = link;

	link_rec.Link (lvalue);
	link_rec.Name (file.Name ());

	//---- convert the anode ----

	lvalue = file.Node_A ();
	if (lvalue == 0) return (false);

	map_itr = node_map.find (lvalue);
	if (map_itr == node_map.end ()) goto node_error;

	link_rec.Anode (map_itr->second);

	//---- convert the bnode ----

	lvalue = file.Node_B ();
	if (lvalue == 0) return (false);
	
	map_itr = node_map.find (lvalue);
	if (map_itr == node_map.end ()) goto node_error;

	link_rec.Bnode (map_itr->second);

	//----  check the link lengths ----

	link_rec.Length (file.Length ());
	link_rec.Aoffset (file.Setback_A ());
	link_rec.Boffset (file.Setback_B ());

	if (link_rec.Length () - link_rec.Aoffset () - link_rec.Boffset () < 0) {
		Warning (String ("Link %d Length %.1lf and Setbacks %.1lf, %.1lf are Incompatible") %
			link_rec.Link () % UnRound (link_rec.Length ()) % 
			UnRound (link_rec.Aoffset ()) % UnRound (link_rec.Boffset ()));
	}

	//---- facility type ----

	link_rec.Type (file.Type ());

	if (link_rec.Type () == FREEWAY) {
		lane_cap = 2000;
	} else if (link_rec.Type () == EXPRESSWAY || link_rec.Type () == PRINCIPAL) {
		lane_cap = 1400;
	} else {
		lane_cap = 800;
	}

	//---- use permission ----

	link_rec.Use (file.Use ());

	//---- other attributes ----

	link_rec.Area_Type (file.Area_Type ());
	if (link_rec.Area_Type () == 0) link_rec.Area_Type (2);
	link_rec.Divided (file.Divided ());
	link_rec.Grade (file.Grade ());

	//---- shape index ----

	map_itr = shape_map.find (link_rec.Link ());
	if (map_itr == shape_map.end ()) {
		link_rec.Shape (-1);
	} else {
		link_rec.Shape (map_itr->second);
	}
	link_rec.Notes (file.Notes ());

	//---- calculate link bearings ----

	ab_in = file.Bearing_A ();
	ab_out = file.Bearing_B ();

	if (file.Bearing_Flag () && (ab_in != 0 || ab_out != 0)) {
		int num_points = compass.Num_Points ();

		//---- adjust the points to the user-specified resolution ----

		if (num_points != 360 && num_points > 0) {
			ab_in = (ab_in * num_points + 180) / 360;
			ab_out = (ab_out * num_points + 180) / 360;
		}

	} else {
		Link_Bearings (link_rec, ab_in, ab_out);
	}

	//---- A->B direction ----

	dir = (int) dir_array.size ();

	lvalue = file.Lanes_AB ();

	if (lvalue > 0) {
		link_rec.AB_Dir (dir++);

		ab_rec.Link (link);
		ab_rec.Dir (0);

		ab_rec.Lanes (lvalue);

		lvalue = Round (file.Fspd_AB ());

		if (lvalue == 0) {
			lvalue = Round (file.Speed_AB ());

			if (lvalue <= 0) {
				lvalue = Round (25.0 * ((Metric_Flag ()) ? MPHTOMPS : MPHTOFPS));
			}
		}
		ab_rec.Time0 ((double) link_rec.Length () / lvalue + 0.09);

		ab_rec.Speed (file.Speed_AB ());

		if (ab_rec.Speed () == 0) {
			ab_rec.Speed (lvalue);
		}
		lvalue = file.Cap_AB ();
		if (lvalue == 0) lvalue = ab_rec.Lanes () * lane_cap;

		ab_rec.Capacity (lvalue);

		ab_rec.In_Bearing (ab_in);
		ab_rec.Out_Bearing (ab_out);
	} else {
		link_rec.AB_Dir (-1);
	}

	//---- B->A direction ----

	lvalue = file.Lanes_BA ();

	if (lvalue > 0) {
		link_rec.BA_Dir (dir);

		ba_rec.Link (link);
		ba_rec.Dir (1);

		ba_rec.Lanes (lvalue);

		lvalue = Round (file.Fspd_BA ());

		if (lvalue == 0) {
			lvalue = Round (file.Speed_BA ());

			if (lvalue <= 0) {
				lvalue = Round (25.0 * ((Metric_Flag ()) ? MPHTOMPS : MPHTOFPS));
			}
		}
		ba_rec.Time0 ((double) link_rec.Length () / lvalue + 0.09);

		ba_rec.Speed (file.Speed_BA ());

		if (ba_rec.Speed () == 0) {
			ba_rec.Speed (lvalue);
		}
		lvalue = file.Cap_BA ();
		if (lvalue == 0) lvalue = ba_rec.Lanes () * lane_cap;

		ba_rec.Capacity (lvalue);

		ba_rec.In_Bearing (compass.Flip (ab_out));
		ba_rec.Out_Bearing (compass.Flip (ab_in));
	} else {
		link_rec.BA_Dir (-1);
	}

	//---- set pocket lanes ----

	if (file.Version () <= 40) {
		ab_rec.Left (file.Left_AB ());
		ab_rec.Right (file.Right_AB ());
		ba_rec.Left (file.Left_BA ());
		ba_rec.Right (file.Right_BA ());
	}
	return ((link_rec.AB_Dir () >= 0 || link_rec.BA_Dir () >= 0));

node_error:
	Warning (String ("Link %d Node %d was Not Found") % link_rec.Link () % lvalue);
	return (false);
}
