//*********************************************************
//	Input_Links.cpp - convert the input link data
//*********************************************************

#include "NetPrep.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Input_Links
//---------------------------------------------------------

void NetPrep::Input_Links (void)
{
	int i, anode, bnode, xa, ya, xb, yb, index, lanes, num, ft, at, spd, min_spd, num_detail;
	double length, x1, y1, x2, y2, z1, z2, dx, dy, dz, ratio, speed, len;

	Db_Header *file;
	Link_File *new_file;

	Node_Data *anode_ptr, *bnode_ptr, node_rec;
	Node_Itr node_itr;
	Zone_Data *zone_ptr;
	Link_Data link_rec, *link_ptr;
	Dir_Data dir_rec, *dir_ptr;
	Shape_Data shape_rec;

	XYZ xyz;
	Points points;
	Points_Itr pt_itr;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Int2_Map_Itr ab_itr;

	new_file = (Link_File *) System_File_Base (NEW_LINK);

	if (link_shape_flag) {
		file = &link_shape_file;
	} else {
		file = &link_file;
	}
	num_detail = 0;
	min_spd = Round (1);


	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	while (((link_shape_flag) ? link_shape_file.Read_Record () : link_file.Read_Record ())) {
		Show_Progress ();

		//---- copy fields and execute user program ----

		new_file->Reset_Record ();

		new_file->Copy_Fields (*file);

		if (convert_flag) {
			if (detail_flag) {
				detail_file.Reset_Record ();
			}
			if (convert.Execute () <= 0) continue;
		}
		link_rec.Clear ();

		//--- check the anode ----

		anode = new_file->Node_A ();

		if (anode == 0) {
			if (link_shape_flag) {
				pt_itr = link_shape_file.begin ();

				x1 = Round (pt_itr->x);
				y1 = Round (pt_itr->y);

				len = resolution;

				for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
					dx = node_itr->X () - x1;
					dy = node_itr->Y () - y1;
					length = sqrt (dx * dx + dy * dy);

					if (length < len) {
						len = length;
						anode = node_itr->Node ();
						if (len == 0.0) break;
					}
				}
				if (anode == 0) {
					anode = new_node++;
					node_rec.Node (anode);
					node_rec.X (UnRound (x1));
					node_rec.Y (UnRound (y1));
					node_rec.Z (0);

					node_map.insert (Int_Map_Data (anode, (int) node_array.size ()));
					node_array.push_back (node_rec);
				}
			} else {
				Error ("Node_A Number is Zero");
			}
		}
		if (int_zone_flag && int_zone_range.In_Range (anode)) {
			if (new_zone_flag && at_fld >= 0) {
				at = file->Get_Integer (at_fld);
				if (at > 0) {
					map_itr = zone_map.find (anode);

					if (map_itr != zone_map.end ()) {
						zone_ptr = &zone_array [map_itr->second];
						zone_ptr->Area_Type (at);
					}
				}
			}
			if (!connector_flag) continue;

			new_file->Type (LOCAL);
			if (new_file->Lanes_AB () > 2) new_file->Lanes_AB (2);
			if (new_file->Lanes_BA () > 2) new_file->Lanes_BA (2);
		}

		//--- check the bnode ----

		bnode = new_file->Node_B ();

		if (bnode == 0) {
			if (link_shape_flag) {
				pt_itr = --link_shape_file.end ();

				x1 = Round (pt_itr->x);
				y1 = Round (pt_itr->y);

				len = resolution;

				for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
					dx = node_itr->X () - x1;
					dy = node_itr->Y () - y1;
					length = sqrt (dx * dx + dy * dy);

					if (length < len) {
						len = length;
						bnode = node_itr->Node ();
						if (len == 0.0) break;
					}
				}
				if (bnode == 0) {
					bnode = new_node++;
					node_rec.Node (bnode);
					node_rec.X (UnRound (x1));
					node_rec.Y (UnRound (y1));
					node_rec.Z (0);

					node_map.insert (Int_Map_Data (bnode, (int) node_array.size ()));
					node_array.push_back (node_rec);
				}
			} else {
				Error ("Node_B Number is Zero");
			}
		}
		if (int_zone_flag && int_zone_range.In_Range (bnode)) {
			if (new_zone_flag && at_fld >= 0) {
				at = file->Get_Integer (at_fld);
				if (at > 0) {
					map_itr = zone_map.find (bnode);

					if (map_itr != zone_map.end ()) {
						zone_ptr = &zone_array [map_itr->second];
						zone_ptr->Area_Type (at);
					}
				}
			}
			if (!connector_flag) continue;

			new_file->Type (LOCAL);
			if (new_file->Lanes_AB () > 2) new_file->Lanes_AB (2);
			if (new_file->Lanes_BA () > 2) new_file->Lanes_BA (2);
		}

		//---- get the node pointers ----

		map_itr = node_map.find (anode);

		if (map_itr == node_map.end ()) {
			if (!drop_node_flag || !drop_node_range.In_Range (anode)) {
				Warning (String ("Node_A %d was Not Found in the Node File") % anode);
			}
			continue;
		}
		anode_ptr = &node_array [map_itr->second];

		link_rec.Anode (map_itr->second);
		xa = anode_ptr->X ();
		ya = anode_ptr->Y ();
		map_itr = node_map.find (bnode);

		if (map_itr == node_map.end ()) {
			if (!drop_node_flag || !drop_node_range.In_Range (bnode)) {
				Warning (String ("Node_B %d was Not Found in the Node File") % bnode);
			}
			continue;
		}
		bnode_ptr = &node_array [map_itr->second];

		link_rec.Bnode (map_itr->second);
		xb = bnode_ptr->X ();
		yb = bnode_ptr->Y ();

		//---- identify the area type ----

		if ((zone_flag || zone_shape_flag) && link_rec.Area_Type () == 0) {
			i = Closest_Zone (link_rec.Anode ());
			if (i >= 0) {
				Zone_Data *zone_ptr = &zone_array [i];
				link_rec.Area_Type (zone_ptr->Area_Type ());
			}
		}

		//---- check for twoway links ----

		ab_itr = ab_map.find (Int2_Key (bnode, anode));

		if (ab_itr == ab_map.end ()) {

			//---- create a new record ----

			if (new_file->Link () == 0) {
				new_file->Link (new_link++);
			}
			if (drop_link_flag && drop_link_range.In_Range (new_file->Link ())) continue;

			if (new_file->Link () >= new_link) {
				new_link = new_file->Link () + 1;
			}

			//---- write the link detail file ----

			if (detail_flag) {
				if (detail_file.Link () != 0) {
					detail_file.Link (new_file->Link ());
					detail_file.Dir (0);

					if (!detail_file.Write ()) {
						Error (String ("Writing %s") % detail_file.File_Type ());
					}
					num_detail++;
				}
			}

			//---- process link shapes ----

			if (link_shape_flag) {

				//---- copy points into a point array ----

				num = link_shape_file.Num_Points ();
				points.clear ();
				points.reserve (num);

				for (pt_itr = link_shape_file.begin (); pt_itr != link_shape_file.end (); pt_itr++) {
					pt_itr->x = UnRound (Round (pt_itr->x));
					pt_itr->y = UnRound (Round (pt_itr->y));
					pt_itr->z = UnRound (Round (pt_itr->z));

					points.push_back (*pt_itr);
				}

				//---- smooth the shape points ----

				if (min_length > 0) {
					Smooth_Shape (points, max_angle, min_length);
				}

				//---- process the point data ----

				length = 0.0;
				x1 = y1 = z1 = 0.0;
				num = (int) points.size ();

				if (shape_flag) {
					shape_rec.Clear ();
					shape_rec.Link (new_file->Link ());
				}

				for (i=1, pt_itr = points.begin (); i <= num; i++, pt_itr++) {
					x2 = pt_itr->x;
					y2 = pt_itr->y;
					z2 = pt_itr->z;

					if (i > 1) {
						dx = x2 - x1;
						dy = y2 - y1;
						dz = z2 - z1;

						length += sqrt (dx * dx + dy * dy + dz * dz);

						//---- write the shape record ----

						if (i < num && shape_flag) {
							xyz.x = Round (x2);
							xyz.y = Round (y2);
							xyz.z = Round (x2);

							shape_rec.push_back (xyz);
						}
					}
					x1 = x2; 
					y1 = y2;
					z1 = z2;
				}
				if (shape_flag && shape_rec.size () > 0) {
					index = (int) shape_array.size ();
					map_stat = shape_map.insert (Int_Map_Data (shape_rec.Link (), index));

					if (!map_stat.second) {
						Warning ("Duplicate Link Number = ") << shape_rec.Link ();
					} else {
						shape_array.push_back (shape_rec);
						link_rec.Shape (index);
					}
				}
			} else {

				//---- check the link length ----

				dx = (double) xb - xa;
				dy = (double) yb - ya;

				length = UnRound (sqrt (dx * dx + dy * dy));
			}
			if (new_file->Length () <= 0) {
				new_file->Length (DTOI (length));
			} else if (length_flag) {
				ratio = length / (double) new_file->Length ();
				if (ratio < 1.0 || ratio > length_ratio) {
					new_file->Length (DTOI (length));
					num_ratio++;
				}
			}

			//---- check for short loops ----

			if (anode == bnode) {
				if (!split_flag || new_file->Length () < split_length) {
					num_loops++;
					continue;
				}
				loop_flag = true;
			}

			//---- translate the data fields ----

			link_rec.Link (new_file->Link ());
			link_rec.Length (Round (new_file->Length ()));
			link_rec.Aoffset (Round (new_file->Setback_A ()));
			link_rec.Boffset (Round (new_file->Setback_B ()));
			link_rec.Type (new_file->Type ());
			link_rec.Divided (new_file->Divided ());
			link_rec.Area_Type (new_file->Area_Type ());
			link_rec.Use (new_file->Use ());
			link_rec.Grade (Round (new_file->Grade ()));
			if (Notes_Name_Flag ()) {
				link_rec.Name (new_file->Name ());
				link_rec.Notes (new_file->Notes ());
			}
			link_rec.BA_Dir (-1);

			//---- process the AB direction ----

			lanes = new_file->Lanes_AB ();

			if (lanes > 0) {
				dir_rec.Link ((int) link_array.size ());
				dir_rec.Dir (0);
				dir_rec.Lanes (lanes);
				dir_rec.In_Bearing (new_file->Bearing_A ());
				dir_rec.Out_Bearing (new_file->Bearing_B ());

				if (spdcap_flag) {
					if (fac_fld >= 0 && at_fld >= 0) {
						at = file->Get_Integer (at_fld);
						ft = file->Get_Integer (fac_fld);

						if (at < 0 || at >= MAX_AT) goto area_type_error;
						if (ft < 0 || ft >= MAX_FUNCL) goto facility_error;

						if (link_rec.Area_Type () == 0) link_rec.Area_Type (at);

						spd = spd_map [at] [ft] [lanes];

						dir_rec.Speed (spd);
						dir_rec.Capacity (cap_map [at] [ft] [lanes] * lanes);
					} else {
						index = (int) new_file->Speed_AB ();
						if (index < 0 || index > 99) goto speed_error;
						dir_rec.Speed (spd_map [index % 10] [index / 10] [lanes]);

						index = (int) new_file->Fspd_AB ();
						if (index < 0 || index > 99) goto speed_error;
						spd = spd_map [index % 10] [index / 10] [lanes];

						index = new_file->Cap_AB ();
						if (index < 0 || index > 99) goto capacity_error;
						dir_rec.Capacity (cap_map [index % 10] [index / 10] [lanes] * lanes);
					}
				} else {
					spd = Round (Internal_Units (new_file->Fspd_AB (), ((units_flag) ? KPH : MPH)));
					speed = Internal_Units (new_file->Speed_AB (), ((units_flag) ? KPH : MPH));

					dir_rec.Speed (Round (speed));
					dir_rec.Capacity (new_file->Cap_AB ());
				}

				//---- check the speeds ----

				if (dir_rec.Speed () < min_spd && spd < min_spd) {
					Warning (String ("Link %d Speed %.1lf is Out of Range") % link_rec.Link () % UnRound (dir_rec.Speed ()));
					dir_rec.Time0 (1);
				} else {
					if (dir_rec.Speed () < min_spd) {
						if (speed_flag) {
							speed = spd * spd_fac.Best (link_rec.Type ());
							if (spd_inc > 0) {
								speed = ((int) (speed + spd_inc * 0.9) / spd_inc) * spd_inc;
							}
							dir_rec.Speed ((int) speed);
						} else {
							dir_rec.Speed (spd);
						}
					} else if (spd < min_spd) {
						spd = dir_rec.Speed ();
						if (speed_flag) {
							spd = (int) (spd / spd_fac.Best (link_rec.Type ()));
						}
					}
					if (spd > dir_rec.Speed ()) {
						dir_rec.Speed (spd + min_spd);
					}
					dir_rec.Time0 ((double) link_rec.Length () / spd + 0.09);
				}

				//---- save the link record ----

				index = (int) dir_array.size ();

				ab_stat = ab_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), index));

				if (!ab_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << link_rec.Link ();
					continue;
				}
				link_rec.AB_Dir (index);

				dir_array.push_back (dir_rec);
			}

			//---- check for B->A data ----

			lanes = new_file->Lanes_BA ();

			if (lanes > 0) {
				dir_rec.Dir (1);
				dir_rec.Lanes (lanes);

				if (spdcap_flag) {
					if (fac_fld >= 0 && at_fld >= 0) {
						at = file->Get_Integer (at_fld);
						ft = file->Get_Integer (fac_fld);

						if (at < 0 || at >= MAX_AT) goto area_type_error;
						if (ft < 0 || ft >= MAX_FUNCL) goto facility_error;

						if (link_rec.Area_Type () == 0) link_rec.Area_Type (at);

						spd = spd_map [at] [ft] [lanes];

						dir_rec.Speed (spd);
						dir_rec.Capacity (cap_map [at] [ft] [lanes] * lanes);
					} else {
						index = (int) new_file->Speed_BA ();
						if (index < 0 || index > 99) goto speed_error;
						dir_rec.Speed (spd_map [index % 10] [index / 10] [lanes]);

						index = (int) new_file->Fspd_BA ();
						if (index < 0 || index > 99) goto speed_error;
						spd = spd_map [index % 10] [index / 10] [lanes];

						index = new_file->Cap_BA ();
						if (index < 0 || index > 99) goto capacity_error;
						dir_rec.Capacity (cap_map [index % 10] [index / 10] [lanes] * lanes);
					}
				} else {
					spd = Round (Internal_Units (new_file->Fspd_BA (), ((units_flag) ? KPH : MPH)));
					speed = Internal_Units (new_file->Speed_BA (), ((units_flag) ? KPH : MPH));

					dir_rec.Speed (Round (speed));
					dir_rec.Capacity (new_file->Cap_AB ());
				}

				//---- check the speeds ----

				if (dir_rec.Speed () < min_spd && spd < min_spd) {
					Warning (String ("Link %d Speed %.1lf is Out of Range") % link_rec.Link () % UnRound (dir_rec.Speed ()));
					dir_rec.Time0 (1);
				} else {
					if (dir_rec.Speed () < min_spd) {
						if (speed_flag) {
							speed = spd * spd_fac.Best (link_rec.Type ());
							if (spd_inc > 0) {
								speed = ((int) (speed + spd_inc * 0.9) / spd_inc) * spd_inc;
							}
							dir_rec.Speed ((int) speed);
						} else {
							dir_rec.Speed (spd);
						}
					} else if (spd < min_spd) {
						spd = dir_rec.Speed ();
						if (speed_flag) {
							spd = (int) (spd / spd_fac.Best (link_rec.Type ()));
						}
					}
					if (spd > dir_rec.Speed ()) {
						dir_rec.Speed (spd + min_spd);
					}
					dir_rec.Time0 ((double) link_rec.Length () / spd + 0.09);
				}

				//---- save the link direction record ----

				link_rec.BA_Dir ((int) dir_array.size ());

				dir_array.push_back (dir_rec);
			}

			//---- check for lanes ----

			if (link_rec.AB_Dir () < 0 && link_rec.BA_Dir () < 0) {
				Warning (String ("Link %d has No Lanes") % link_rec.Link ());
				continue;
			}

			//---- save the link record ----

			map_stat = link_map.insert (Int_Map_Data (link_rec.Link (), (int) link_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << link_rec.Link ();
			} else {
				link_array.push_back (link_rec);
				anode_ptr->Add_Count ();
				bnode_ptr->Add_Count ();

				if (link_node_flag) {
					Link_Nodes rec;
					rec.link = link_rec.Link ();
					rec.nodes.push_back (anode_ptr->Node ());
					rec.nodes.push_back (bnode_ptr->Node ());

					link_node_array.push_back (rec);
				}

				if (new_zone_flag && link_rec.Type () == EXTERNAL) {
					Node_Data *ptr;

					if (anode_ptr->Node () < bnode_ptr->Node ()) {
						ptr = anode_ptr;
					} else {
						ptr = bnode_ptr;
					}
					map_itr = zone_map.find (ptr->Node ());

					if (map_itr == zone_map.end ()) {
						Zone_Data zone_rec;

						zone_rec.Zone (ptr->Node ());
						zone_rec.X (ptr->X ());
						zone_rec.Y (ptr->Y ());
						zone_rec.Z (ptr->Z ());
						zone_rec.Area_Type (link_rec.Area_Type ());

						zone_map.insert (Int_Map_Data (ptr->Node (), (int) zone_array.size ()));
						zone_array.push_back (zone_rec);
					}
				}
			}

		} else {

			//---- update existing record ----

			dir_ptr = &dir_array [ab_itr->second];

			link_ptr = &link_array [dir_ptr->Link ()];

			//---- write the link detail file ----

			if (detail_flag) {
				if (detail_file.Link () != 0) {
					detail_file.Link (link_ptr->Link ());
					detail_file.Dir (1);

					if (!detail_file.Write ()) {
						Error (String ("Writing %s") % detail_file.File_Type ());
					}
					num_detail++;
				}
			}

			//---- translate the data fields ----

			dir_rec.Link (dir_ptr->Link ());
			dir_rec.Dir (1);
			dir_rec.Lanes (new_file->Lanes_AB ());

			if (spdcap_flag) {
				lanes = new_file->Lanes_AB ();

				if (fac_fld >= 0 && at_fld >= 0) {
					at = file->Get_Integer (at_fld);
					ft = file->Get_Integer (fac_fld);

					if (at < 0 || at >= MAX_AT) goto area_type_error;
					if (ft < 0 || ft >= MAX_FUNCL) goto facility_error;

					spd = spd_map [at] [ft] [lanes];

					dir_rec.Speed (spd);
					dir_rec.Capacity (cap_map [at] [ft] [lanes] * lanes);
				} else {
					index = (int) new_file->Speed_AB ();
					if (index < 0 || index > 99) goto speed_error;
					dir_rec.Speed (spd_map [index % 10] [index / 10] [lanes]);

					index = (int) new_file->Fspd_AB ();
					if (index < 0 || index > 99) goto speed_error;
					spd = spd_map [index % 10] [index / 10] [lanes];

					index = new_file->Cap_AB ();
					if (index < 0 || index > 99) goto capacity_error;
					dir_rec.Capacity (cap_map [index % 10] [index / 10] [lanes] * lanes);
				}
			} else {
				spd = Round (Internal_Units (new_file->Fspd_AB (), ((units_flag) ? KPH : MPH)));
				speed = Internal_Units (new_file->Speed_AB (), ((units_flag) ? KPH : MPH));

				dir_rec.Speed (Round (speed));
				dir_rec.Capacity (new_file->Cap_AB ());
			}

			//---- check the speeds ----

			if (dir_rec.Speed () < min_spd && spd < min_spd) {
				Warning (String ("Link %d Speed %.1lf is Out of Range") % link_ptr->Link () % UnRound (dir_rec.Speed ()));
				dir_rec.Time0 (1);
			} else {
				if (dir_rec.Speed () < min_spd) {
					if (speed_flag) {
						speed = spd * spd_fac.Best (link_ptr->Type ());
						if (spd_inc > 0) {
							speed = ((int) (speed + spd_inc * 0.9) / spd_inc) * spd_inc;
						}
						dir_rec.Speed ((int) speed);
					} else {
						dir_rec.Speed (spd);
					}
				} else if (spd < min_spd) {
					spd = dir_rec.Speed ();
					if (speed_flag) {
						spd = (int) (spd / spd_fac.Best (link_ptr->Type ()));
					}
				}
				if (spd > dir_rec.Speed ()) {
					dir_rec.Speed (spd + min_spd);
				}
				dir_rec.Time0 ((double) link_ptr->Length () / spd + 0.09);
			}

			//---- save the link record ----

			link_ptr->BA_Dir ((int) dir_array.size ());

			dir_array.push_back (dir_rec);
		}
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (link_shape_flag) {
		link_shape_file.Close ();
	} else {
		link_file.Close ();
	}
	if (detail_flag) {
		Print (2, String ("Number of %s Record = %d") % detail_file.File_Type () % num_detail);
		detail_file.Close ();
	}
	return;

speed_error:
	Error (String ("Speed Class %d is Out of Range (0..99)") % index);
	return;

capacity_error:
	Error (String ("Capacity Class %d is Out of Range (0..99)") % index);
	return;

facility_error:
	Error (String ("Facility Index %d is Out of Range (0..%d)") % ft % (MAX_FUNCL - 1));
	return;

area_type_error:
	Error (String ("Area Type Index %d is Out of Range (0..%d)" ) % at % (MAX_AT - 1));
	return;
}
