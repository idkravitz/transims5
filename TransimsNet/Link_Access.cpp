//*********************************************************
//	Link_Access.cpp - add activity locations and parking
//*********************************************************

#include "TransimsNet.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Access
//---------------------------------------------------------

void TransimsNet::Link_Access (void)
{
	int i, j, link, num_pts, max_pts, min_len, offset, area_type, z, zone, zone_num, setback;
	bool walk_flag, drive_flag, ab_flag, dir_flag;
	double off, side, dx, dy, diff, best_diff;

	Link_Itr link_itr;
	Node_Data *node_ptr;
	Location_Data loc_rec;
	Parking_Data park_rec;
	Int_Map_Stat map_stat;
	Int_Map_Itr int_itr;
	Access_Index map_index;
	Access_Warrant_Map_Itr map_itr;
	Access_Warrant *warrant_ptr;
	Park_Detail_Itr detail_itr;
	Parking_Nest park_nest;
	Zone_Itr zone_itr;
	Points_Map_Itr boundary_itr;
	Points point;
	Points_Itr point_itr;

	Show_Message (String ("Creating Link Access Points -- Record"));
	Set_Progress ();

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();

		//---- apply the default link setback ----

		if (link_setback > 0) {
			if (link_itr->Type () == EXTERNAL) {
				node_ptr = &node_array [link_itr->Anode ()];

				if (node_ptr->Node () > Max_Zone_Number ()) {
					link_itr->Aoffset (MIN (link_setback, link_itr->Length () / 2));
				} else {
					link_itr->Boffset (MIN (link_setback, link_itr->Length () / 2));
				}
			} else {
				link_itr->Aoffset (MIN (link_setback, link_itr->Length () / 2));
				link_itr->Boffset (MIN (link_setback, link_itr->Length () / 2));
			}
		}
		area_type = link_itr->Area_Type ();

		map_index.Facility (link_itr->Type ());
		map_index.Area_Type (area_type);

		map_itr = access_warrant_map.find (map_index);

		if (map_itr == access_warrant_map.end ()) {
			if (link_itr->Type () != EXTERNAL) continue;
			min_len = link_itr->Length ();
			warrant_ptr = 0;
			setback = loc_setback;
		} else {
			warrant_ptr = &access_warrants [map_itr->second];
			min_len = warrant_ptr->Min_Length ();
			setback = warrant_ptr->Setback ();
		}

		//---- check the access criteria ----

		if (link_itr->Length () < min_len) continue;

		walk_flag = Use_Permission (link_itr->Use (), WALK);
		drive_flag = Use_Permission (link_itr->Use (), CAR);

		if (!walk_flag && !drive_flag) continue;

		loc_rec.Link (link);
		loc_rec.Setback (setback);

		park_rec.Link (link);

		//---- check for an external link ----

		if (link_itr->Type () == EXTERNAL) {
			num_pts = 1;
			min_len = external_offset;
			if (link_itr->Length () < external_offset * 2) min_len = link_itr->Length () / 2;

			node_ptr = &node_array [link_itr->Anode ()];
			zone = node_ptr->Node ();

			node_ptr = &node_array [link_itr->Bnode ()];

			if (zone < node_ptr->Node ()) {
				ab_flag = true;
			} else {
				ab_flag = false;
				zone = node_ptr->Node ();
			}
			int_itr = zone_map.find (zone);
			if (int_itr == zone_map.end ()) {
				Warning (String ("External Link %d Zone %d is Out of Range") % link_itr->Link () % zone);
				zone_num = -1;
			} else {
				zone_num = int_itr->second;
			}
			park_rec.Type (BOUNDARY);
			park_rec.Notes ("External Station");
			dir_flag = true;
		} else {

			//---- get the number of points ----

			max_pts = warrant_ptr->Max_Points ();

			if (min_len > 0) {
				num_pts = (link_itr->Length () + min_len / 2) / min_len - 1;
				if (num_pts > max_pts) num_pts = max_pts;
				if (num_pts < 1) num_pts = 1;
			} else if (max_pts > 0) {
				num_pts = max_pts;
			} else {
				num_pts = 1;
			}
			max_pts = num_pts + 1;
			min_len = (link_itr->Length () + max_pts / 2) / max_pts;
			ab_flag = true;
			zone_num = -1;
			park_rec.Type (LOT);
			park_rec.Notes ("Parking Lot");
			dir_flag = (link_itr->Type () == EXPRESSWAY || link_itr->Type () == FRONTAGE || 
						link_itr->Divided () > 0);

			loc_rec.Notes ("Activity Location");
		}

		//---- insert activity locations ----

		for (i=offset=0; i < num_pts; i++) {
			offset += min_len;

			for (j=0; j < 2; j++) {
				if (j == 0) {
					if (dir_flag && link_itr->AB_Dir () < 0) continue;

					if (ab_flag) {
						loc_rec.Offset (offset);
						if (link_itr->Type () == EXTERNAL) loc_rec.Notes ("External Origin");
					} else {
						loc_rec.Offset (link_itr->Length () - offset);
						if (link_itr->Type () == EXTERNAL) loc_rec.Notes ("External Destination");
					}
				} else {
					if (dir_flag && link_itr->BA_Dir () < 0) continue;

					if (ab_flag) {
						loc_rec.Offset (link_itr->Length () - offset);
						if (link_itr->Type () == EXTERNAL) loc_rec.Notes ("External Destination");
					} else {
						loc_rec.Offset (offset);
						if (link_itr->Type () == EXTERNAL) loc_rec.Notes ("External Origin");
					}
				}
				loc_rec.Dir (j);
				loc_rec.Location (++location_id);

				//---- add zone ----
					
				loc_rec.Zone (zone_num);

				if (zone_num < 0) {
					off = UnRound (loc_rec.Offset ());
					side = UnRound (loc_rec.Setback ());

					Link_Shape (&(*link_itr), j, point, off, 0.0, side);
					point_itr = point.begin ();

					if (boundary_flag) {
						for (boundary_itr = boundary_map.begin (); boundary_itr != boundary_map.end (); boundary_itr++) {
							if (In_Polygon (boundary_itr->second, point_itr->x, point_itr->y)) {
								loc_rec.Zone (boundary_itr->first);
								break;
							}
						}
					} else {
						best_diff = 0.0;
						zone = -1;

						for (z=0, zone_itr = zone_array.begin (); zone_itr != zone_array.end (); zone_itr++, z++) {
							if (ext_zone_flag && ext_zone_range.In_Range (zone_itr->Zone ())) continue;

							dx = UnRound (zone_itr->X ()) - point_itr->x;
							dy = UnRound (zone_itr->Y ()) - point_itr->y;

							diff = sqrt (dx * dx + dy * dy);

							if (zone < 0 || diff < best_diff) {
								zone = z;
								best_diff = diff;
							}
						}
						loc_rec.Zone (zone);
					}
				}

				//---- insert the location record ----

				map_stat = location_map.insert (Int_Map_Data (location_id, (int) location_array.size ()));

				if (!map_stat.second) continue;

				location_array.push_back (loc_rec);

				//---- add a parking lot ----

				if (drive_flag) {
					park_rec.Parking (++parking_id);

					park_rec.Dir (j);
					park_rec.Offset (loc_rec.Offset ());

					map_stat = parking_map.insert (Int_Map_Data (parking_id, (int) parking_array.size ()));

					if (!map_stat.second) continue;

					if (details_flag) {
						park_rec.clear ();

						for (detail_itr = parking_details.begin (); detail_itr != parking_details.end (); detail_itr++) {
							if (detail_itr->Area_Type1 () <= area_type && area_type <= detail_itr->Area_Type2 ()) {
								park_nest.Start (detail_itr->Start ());
								park_nest.End (detail_itr->End ());
								park_nest.Use (detail_itr->Use ());
								park_nest.Time_In (detail_itr->Time_In ());
								park_nest.Time_Out (detail_itr->Time_Out ());
								park_nest.Hourly (detail_itr->Hourly ());
								park_nest.Daily (detail_itr->Daily ());

								park_rec.push_back (park_nest);
							}
						}
					}
					parking_array.push_back (park_rec);
				}
			}
		}
	}
	End_Progress ();
	return;
}
