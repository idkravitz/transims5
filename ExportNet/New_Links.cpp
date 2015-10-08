//*********************************************************
//	New_Links.cpp - insert new link records
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	New_Links
//---------------------------------------------------------

void ExportNet::New_Links (void)
{
	int i, node, link, new_link, count, length, index, park_len, speed, time, dir;
	double offset, len, factor;

	Int_Map_Itr map_itr;
	Link_Data link_data, link_rec;
	Split_Itr split_itr, split2_itr;
	Link_Split_Itr link_itr;
	Points points;
	Dir_Data dir_ab, dir_ba, dir_rec;
	Shape_Data shape_rec;
	Points_Itr pt_itr;
	XYZ xyz;
	
	park_len = Round (Internal_Units (20, METERS)); 
	speed = Round (Internal_Units (5, MPS));
	time = Round (Internal_Units (20/5, SECONDS));

	map_itr = --link_map.end ();
	link = map_itr->first;
	new_link = ((link + 1) / 100 + 1) * 100;
	count = 0;

	//---- split links ----

	Set_Progress ();

	for (link=0, link_itr = link_splits.begin (); link_itr != link_splits.end (); link_itr++, link++) {
		if (link_itr->size () == 0) continue;
		Show_Progress ();

		link_data = link_array [link];

		if (link_data.AB_Dir () >= 0) {
			dir_ab = dir_array [link_data.AB_Dir ()];
		}
		if (link_data.BA_Dir () >= 0) {
			dir_ba = dir_array [link_data.BA_Dir ()];
		}
		offset = 0.0;

		for (split_itr = split2_itr = link_itr->begin (); split_itr != link_itr->end (); split2_itr = split_itr++) {
			link_rec = link_data;
			link_rec.Name (link_data.Name ());

			index = (int) link_array.size ();
			link_rec.Divided (3);

			split_itr->link = index;
			link_rec.Link (new_link++);

			if ((split_itr + 1) != link_itr->end ()) {
				link_rec.Bnode (split_itr->node);
				length = split_itr->offset;
				link_rec.Boffset (0);
			} else {
				length = link_rec.Length ();
			}
			if (split_itr != split2_itr) {
				link_rec.Anode (split2_itr->node);
				length -= split2_itr->offset;
				link_rec.Aoffset (0);
				offset = UnRound (split2_itr->offset);
			}
			len = UnRound (length);
			link_rec.Length (length);
			factor = (double) length / link_data.Length ();

			//---- split the link shape ----

			if (link_data.Shape () >= 0) {
				Link_Shape (&link_data, 0, points, offset, len);

				if (points.size () > 2) {
					shape_rec.Clear ();
					shape_rec.Link (link_rec.Link ());

					for (pt_itr = points.begin () + 1; pt_itr != points.end (); pt_itr++) {
						xyz.x = Round (pt_itr->x);
						xyz.y = Round (pt_itr->y);
						xyz.z = Round (pt_itr->z);

						shape_rec.push_back (xyz);
					}
					link_rec.Shape ((int) shape_array.size ());
					shape_map.insert (Int_Map_Data (shape_rec.Link (), link_rec.Shape ()));
					shape_array.push_back (shape_rec);
				} else {
					link_rec.Shape (-1);
				}
			}

			//---- process each direction ----

			if (link_data.AB_Dir () >= 0) {
				dir_rec = dir_ab;
				dir_rec.Link (index);
				dir_rec.First_Connect_From (-1);
				dir_rec.First_Connect_To (-1);

				if ((split_itr + 1) != link_itr->end ()) {
					dir_rec.First_Pocket (-1);
				}
				dir_rec.Time0 ((int) (dir_rec.Time0 () * factor + 0.5));

				dir = (int) dir_array.size ();
				link_rec.AB_Dir (dir);

				dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir));
				dir_array.push_back (dir_rec);
			} else {
				link_rec.AB_Dir (-1);
			}

			if (link_data.BA_Dir () >= 0) {
				dir_rec = dir_ba;
				dir_rec.Link (index);
				dir_rec.First_Connect_From (-1);
				dir_rec.First_Connect_To (-1);
			
				if (split_itr != split2_itr) {
					dir_rec.First_Pocket (-1);
				}
				dir_rec.Time0 ((int) (dir_rec.Time0 () * factor + 0.5));
				
				dir = (int) dir_array.size ();
				link_rec.BA_Dir (dir);

				dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir));
				dir_array.push_back (dir_rec);
			} else {
				link_rec.BA_Dir (-1);
			}

			//---- insert the link ----

			link_map.insert (Int_Map_Data (link_rec.Link (), index));
			link_array.push_back (link_rec);
			count++;
		}
	}

	//---- add parking connection links ----

	Set_Progress ();

	for (link=0, link_itr = link_splits.begin (); link_itr != link_splits.end (); link_itr++, link++) {
		if (link_itr->size () == 0) continue;
		Show_Progress ();

		for (split_itr = split2_itr = link_itr->begin (); split_itr != link_itr->end (); split2_itr = split_itr++) {

			for (i=0; i < 2; i++) {
				index = (int) link_array.size ();

				if (i == 0) {
					node = split_itr->park_ab;
					if (node < 0) continue;
					split_itr->link_ab = index;
				} else {
					node = split_itr->park_ba;
					if (node < 0) continue;
					split_itr->link_ba = index;
				}
				link_rec.Clear ();
				link_rec.Divided (3);

				link_rec.Link (new_link++);
				link_rec.Anode (split_itr->node);
				link_rec.Bnode (node);
				link_rec.Length (park_len);
				link_rec.Use (ANY);
				link_rec.Type (LOCAL);
				link_rec.Name ("Parking");
				
				//---- insert directional data ----

				dir_rec.Clear ();
				dir_rec.Link (index);
				dir_rec.Dir (0);
				dir_rec.Lanes (1);
				dir_rec.Speed (speed);
				dir_rec.Capacity (300);
				dir_rec.Time0 (time);
				dir_rec.Sign (YIELD_SIGN);

				dir = (int) dir_array.size ();
				link_rec.AB_Dir (dir);

				dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir));
				dir_array.push_back (dir_rec);

				dir_rec.Dir (1);
				dir_rec.Sign (NO_CONTROL);
			
				dir = (int) dir_array.size ();
				link_rec.BA_Dir (dir);

				dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir));
				dir_array.push_back (dir_rec);

				//---- insert the link ----

				link_map.insert (Int_Map_Data (link_rec.Link (), index));
				link_array.push_back (link_rec);
				count++;
			}
		}
	}
	Print (2, "Number of New Link Records = ") << count;
}

