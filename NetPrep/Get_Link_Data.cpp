//*********************************************************
//	Get_Link_Data.cpp - additonal link processing
//*********************************************************

#include "NetPrep.hpp"

#include <math.h>

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool NetPrep::Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba)
{
	Link_File *new_file;

	if (convert_flag && !link_flag && !link_shape_flag) {
		new_file = (Link_File *) System_File_Base (NEW_LINK);

		new_file->Copy_Fields (file);

		if (detail_flag) {
			detail_file.Reset_Record ();
		}
		if (!convert.Execute ()) return (false);

		//---- write the link detail file ----

		if (detail_flag) {
			if (detail_file.Link () != 0) {
				detail_file.Link (new_file->Link ());
				detail_file.Dir (0);

				if (detail_file.Write ()) {
					Error (String ("Writing %s") % detail_file.File_Type ());
				}
			}
		}
	} else {
		new_file = &file;
	}
	if (Data_Service::Get_Link_Data (*new_file, data, ab, ba)) {
		if (data.Link () >= new_link) new_link = data.Link () + 1;

		if (drop_link_flag && drop_link_range.In_Range (data.Link ())) return (false);

		//---- check for short loops ----

		if (data.Anode () == data.Bnode ()) {
			if (!split_flag || data.Length () < split_length) {
				num_loops++;
				return (false);
			}
			loop_flag = true;
		}

		//---- identify the area type ----

		if (!file.Area_Type_Flag () && System_File_Flag (ZONE)) {
			int i = Closest_Zone (data.Anode ());
			if (i >= 0) {
				Zone_Data *zone_ptr = &zone_array [i];
				data.Area_Type (zone_ptr->Area_Type ());
			}
		}

		//---- check speed fields ----

		if (speed_flag && (!file.Speed_Flag () || !file.Fspd_Flag ())) {
			if (!file.Speed_Flag ()) {
				if (file.Fspd_Flag ()) {
					double speed = Scale (file.Fspd_AB ()) * spd_fac.Best (data.Type ());

					if (spd_inc > 0) {
						speed = ((int) (speed + spd_inc * 0.9) / spd_inc) * spd_inc;
					}
					ab.Speed ((int) speed);

					speed = Scale (file.Fspd_BA ()) * spd_fac.Best (data.Type ());

					if (spd_inc > 0) {
						speed = ((int) (speed + spd_inc * 0.9) / spd_inc) * spd_inc;
					}
					ba.Speed ((int) speed);
				}
			} else {
				if (ab.Speed () > 0) {
					ab.Time0 (spd_fac.Best (data.Type ()) * data.Length () / ab.Speed () + 0.09);
				}
				if (ba.Speed () > 0) {
					ba.Time0 (spd_fac.Best (data.Type ()) * data.Length () / ba.Speed () + 0.09);
				}
			}
		}

		//---- increment the node count ----

		int anode, bnode;
		Node_Data *node_ptr;

		node_ptr = &node_array [data.Anode ()];
		node_ptr->Add_Count ();
		anode = node_ptr->Node ();

		node_ptr = &node_array [data.Bnode ()];
		node_ptr->Add_Count ();
		bnode = node_ptr->Node ();

		//---- add to the link node list ----

		if (link_node_flag) {
			Link_Nodes rec;

			rec.link = data.Link ();
			rec.nodes.push_back (anode);
			rec.nodes.push_back (bnode);

			link_node_array.push_back (rec);
		}

		//---- process link shapes ----

		if (length_flag) {
			double xa, ya, xb, yb, x1, y1, x2, y2, length, dx, dy, ratio;

			node_ptr = &node_array [data.Anode ()];

			xa = node_ptr->X ();
			ya = node_ptr->Y ();

			node_ptr = &node_array [data.Bnode ()];

			xb = node_ptr->X ();
			yb = node_ptr->Y ();

			//---- get the shape record ----

			length = 0;

			if (System_File_Flag (SHAPE) && data.Shape () >= 0) {
				XYZ_Itr pt_itr;

				Shape_Data *shape_ptr = &shape_array [data.Shape ()];

				x1 = xa;
				y1 = ya;

				for (pt_itr = shape_ptr->begin (); pt_itr != shape_ptr->end (); pt_itr++) {
					x2 = pt_itr->x;
					y2 = pt_itr->y;

					dx = x2 - x1;
					dy = y2 - y1;

					length += sqrt (dx * dx + dy * dy);

					x1 = x2;
					y1 = y2;
				}

				dx = xb - x1;
				dy = yb - y1;

				length += sqrt (dx * dx + dy * dy);
			} else {
				dx = xb - xa;
				dy = yb - ya;

				length = sqrt (dx * dx + dy * dy);
			}
			if (data.Length () < 1) data.Length (1);
			if (length < 1.0) length = 1.0;

			ratio = length / (double) data.Length ();

			if (ratio < 1.0 || ratio > length_ratio) {
				int time;
				data.Length (DTOI (length));	
				num_ratio++;

				ab.Time0 (DTOI (ab.Time0 () * ratio));
				ba.Time0 (DTOI (ba.Time0 () * ratio));

				if (ab.Speed () > 0) {
					time = Round ((double) data.Length () / ab.Speed ());
					if (ab.Time0 () < time) ab.Time0 (time);
				}
				if (ba.Speed () > 0) {
					time = Round ((double) data.Length () / ba.Speed ());
					if (ba.Time0 () < time) ba.Time0 (time);
				}
			}
		}
		return (true);
	}
	return (false);
}
