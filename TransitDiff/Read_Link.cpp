//*********************************************************
//	Read_Link.cpp - read the compare link file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Link
//---------------------------------------------------------

void TransitDiff::Read_Link (void)
{
	int num, bnode, index, lvalue, link, dir, lane_cap, ab_in, ab_out;
	string text;

	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Link_Data link_rec;
	Dir_Data ab_rec, ba_rec;

	//---- store the link data ----

	Show_Message (String ("Reading %s -- Record") % compare_link_file.File_Type ());
	Set_Progress ();

	bnode = 0;

	while (compare_link_file.Read ()) {
		Show_Progress ();

		link_rec.Clear ();
		ab_rec.Clear ();
		ba_rec.Clear ();

		//---- set the link number ----

		link = compare_link_file.Link ();

		link_rec.Link (link);
		
		link_rec.Name (compare_link_file.Name ());

		//---- convert the anode ----

		lvalue = compare_link_file.Node_A ();
		if (lvalue == 0) continue;

		map_itr = compare_node_map.find (lvalue);
		if (map_itr == compare_node_map.end ()) goto node_error;

		link_rec.Anode (map_itr->second);

		//---- convert the bnode ----

		lvalue = compare_link_file.Node_B ();
		if (lvalue == 0) continue;
		
		map_itr = compare_node_map.find (lvalue);
		if (map_itr == compare_node_map.end ()) goto node_error;

		link_rec.Bnode (map_itr->second);

		//----  check the link lengths ----

		link_rec.Length (compare_link_file.Length ());
		link_rec.Aoffset (compare_link_file.Setback_A ());
		link_rec.Boffset (compare_link_file.Setback_B ());

		if (link_rec.Length () - link_rec.Aoffset () - link_rec.Boffset () < 0) {
			Warning (String ("Link %d Length %.1lf and Setbacks %.1lf, %.1lf are Incompatible") %
				link_rec.Link () % UnRound (link_rec.Length ()) % 
				UnRound (link_rec.Aoffset ()) % UnRound (link_rec.Boffset ()));
		}

		//---- facility type ----

		link_rec.Type (compare_link_file.Type ());

		if (link_rec.Type () == FREEWAY) {
			lane_cap = 2000;
		} else if (link_rec.Type () == EXPRESSWAY || link_rec.Type () == PRINCIPAL) {
			lane_cap = 1400;
		} else {
			lane_cap = 800;
		}

		//---- use permission ----

		link_rec.Use (compare_link_file.Use ());

		//---- shape index ----

		link_rec.Shape (-1);

		//---- calculate link bearings ----

		ab_in = compare_link_file.Bearing_A ();
		ab_out = compare_link_file.Bearing_B ();

		if (compare_link_file.Bearing_Flag () && (ab_in != 0 || ab_out != 0)) {
			int num_points = Resolve (compass.Num_Points ());

			//---- adjust the points to the user-specified resolution ----

			if (num_points != 360 && num_points > 0) {
				ab_in = (ab_in * num_points + 180) / 360;
				ab_out = (ab_out * num_points + 180) / 360;
			}

		}

		//---- A->B direction ----

		dir = (int) compare_dir_array.size ();

		lvalue = compare_link_file.Lanes_AB ();

		if (lvalue > 0) {
			link_rec.AB_Dir (dir++);

			ab_rec.Link (link);
			ab_rec.Dir (0);

			ab_rec.Lanes (lvalue);

			lvalue = Round (compare_link_file.Fspd_AB ());

			if (lvalue == 0) {
				lvalue = Round (compare_link_file.Speed_AB ());

				if (lvalue == 0) {
					lvalue = Round (25.0 * ((Metric_Flag ()) ? MPHTOMPS : MPHTOFPS));
				}
			}
			ab_rec.Time0 ((double) link_rec.Length () / lvalue);

			ab_rec.Speed (compare_link_file.Speed_AB ());

			if (ab_rec.Speed () == 0) {
				ab_rec.Speed (lvalue);
			}
			lvalue = compare_link_file.Cap_AB ();
			if (lvalue == 0) lvalue = ab_rec.Lanes () * lane_cap;

			ab_rec.Capacity (lvalue);

			ab_rec.In_Bearing (ab_in);
			ab_rec.Out_Bearing (ab_out);
		} else {
			link_rec.AB_Dir (-1);
		}

		//---- B->A direction ----

		lvalue = compare_link_file.Lanes_BA ();

		if (lvalue > 0) {
			link_rec.BA_Dir (dir);

			ba_rec.Link (link);
			ba_rec.Dir (1);

			ba_rec.Lanes (lvalue);

			lvalue = Round (compare_link_file.Fspd_BA ());

			if (lvalue == 0) {
				lvalue = Round (compare_link_file.Speed_BA ());

				if (lvalue == 0) {
					lvalue = Round (25.0 * ((Metric_Flag ()) ? MPHTOMPS : MPHTOFPS));
				}
			}
			ba_rec.Time0 ((double) link_rec.Length () / lvalue);

			ba_rec.Speed (compare_link_file.Speed_BA ());

			if (ba_rec.Speed () == 0) {
				ba_rec.Speed (lvalue);
			}
			lvalue = compare_link_file.Cap_BA ();
			if (lvalue == 0) lvalue = ba_rec.Lanes () * lane_cap;

			ba_rec.Capacity (lvalue);

			ba_rec.In_Bearing (compass.Flip (ab_out));
			ba_rec.Out_Bearing (compass.Flip (ab_in));
		} else {
			link_rec.BA_Dir (-1);
		}
		if (link_rec.AB_Dir () >= 0 || link_rec.BA_Dir () >= 0) {
			map_stat = compare_link_map.insert (Int_Map_Data (link_rec.Link (), (int) compare_link_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << link_rec.Link ();
				continue;
			} else {
				compare_link_array.push_back (link_rec);
			}

			//---- insert the AB direction ----

			if (link_rec.AB_Dir () >= 0) {
				index = (int) compare_dir_array.size ();

				map_stat = compare_dir_map.insert (Int_Map_Data (ab_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ab_rec.Link_Dir ();
					continue;
				} else {
					compare_dir_array.push_back (ab_rec);
				}
				if (index != link_rec.AB_Dir ()) goto num_error;
			}
			if (link_rec.BA_Dir () >= 0) {
				index = (int) compare_dir_array.size ();

				map_stat = compare_dir_map.insert (Int_Map_Data (ba_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ba_rec.Link_Dir ();
					continue;
				} else {
					compare_dir_array.push_back (ba_rec);
				}
				if (index != link_rec.BA_Dir ()) goto num_error;
			}
		}
	}
	End_Progress ();
	compare_link_file.Close ();

	Print (2, String ("Number of %s Records = %d") % compare_link_file.File_Type () % Progress_Count ());

	num = (int) compare_link_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_link_file.File_ID () % num);
	}
	num = (int) compare_dir_array.size ();

	if (num) Print (1, "Number of Directional Links = ") << num;
	return;

node_error:
	Warning (String ("Link %d Node %d was Not Found") % link_rec.Link () % lvalue);
	return;

num_error:
	Error (String ("%s Direction Numbering") % compare_link_file.File_ID ());
}
