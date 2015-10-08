//*********************************************************
//	Link_Rider.cpp - Create a Link Rider File
//*********************************************************

#include "LineSum.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Link_Rider
//---------------------------------------------------------

void LineSum::Link_Rider (Link_Rider_Data &data)
{
	int node, mode, dir, ab_day_ride, ba_day_ride, pk_ride, op_ride, day_ride, anode, bnode;
	double ab_pk_runs, ba_pk_runs, pk_runs, op_runs, peak_hour, load_fac, ab_load, ba_load;
	bool shape_found, offset_flag;
	String question;

	int aname_fld, bname_fld, anode_fld, bnode_fld;
	int ab_pk_ride_fld, ab_pk_freq_fld, ab_op_ride_fld, ab_op_freq_fld, ab_day_ride_fld;
	int ba_pk_ride_fld, ba_pk_freq_fld, ba_op_ride_fld, ba_op_freq_fld, ba_day_ride_fld;
	int pk_ride_fld, pk_freq_fld, pk_dist_fld, pk_time_fld, pk_pmt_fld, pk_pht_fld, pk_vmt_fld, pk_vht_fld;
	int op_ride_fld, op_freq_fld, op_dist_fld, op_time_fld, op_pmt_fld, op_pht_fld, op_vmt_fld, op_vht_fld;
	int day_ride_fld, day_pmt_fld, day_pht_fld, day_vmt_fld, day_vht_fld;
	int pk_hour_fld, ab_fac_fld, ba_fac_fld, load_fac_fld, max_fac_fld, max_line_fld; 

	Leg_Data *leg_ptr;
	Line_Map_Itr map_itr;
	Str_Itr str_itr;
	Str_Map_Itr name_itr;
	String name;
	Int_Itr int_itr;
	Int2_Key ab_key, ba_key;
	Range_Array_Itr range_itr;
	XYZ_Point point;
	I2_Points_Map_Itr shape_itr;
	Points *points;
	Points_Itr pts_itr;
	Points_RItr pts_ritr;

	//---- node xy map data ----

	XY_Map xy_map;
	XY_Point xy_rec;
	XY_Map_Itr xy_itr;
	XY_Map_Stat xy_stat;

	Rider_Map link_map;
	Rider_Data link_rec, *link_ptr;
	Rider_Map_Itr link_itr;
	Rider_Map_Stat map_stat;

	question = "?";
	memset (&link_rec, '\0', sizeof (link_rec));
	offset_flag = (data.offset > 0.0);
	ab_pk_runs = ba_pk_runs = 0;
	
	Show_Message (String ("Creating Link Rider File #%d -- Records") % data.number);
	Set_Progress ();

	anode_fld = data.file->Add_Field ("ANODE", DB_INTEGER, 10);
	bnode_fld = data.file->Add_Field ("BNODE", DB_INTEGER, 10);
	if (!offset_flag) {
		ab_pk_ride_fld = data.file->Add_Field ("AB_PK_RIDE", DB_INTEGER, 10);
		ab_pk_freq_fld = data.file->Add_Field ("AB_PK_FREQ", DB_DOUBLE, 6.2, MINUTES);
		ab_op_ride_fld = data.file->Add_Field ("AB_OP_RIDE", DB_INTEGER, 10);
		ab_op_freq_fld = data.file->Add_Field ("AB_OP_FREQ", DB_DOUBLE, 6.2, MINUTES);
		ab_day_ride_fld = data.file->Add_Field ("AB_DAY_RIDE", DB_INTEGER, 10);
		ba_pk_ride_fld = data.file->Add_Field ("BA_PK_RIDE", DB_INTEGER, 10);
		ba_pk_freq_fld = data.file->Add_Field ("BA_PK_FREQ", DB_DOUBLE, 6.2, MINUTES);
		ba_op_ride_fld = data.file->Add_Field ("BA_OP_RIDE", DB_INTEGER, 10);
		ba_op_freq_fld = data.file->Add_Field ("BA_OP_FREQ", DB_DOUBLE, 6.2, MINUTES);
		ba_day_ride_fld = data.file->Add_Field ("BA_DAY_RIDE", DB_INTEGER, 10);
	} else {
		ab_pk_ride_fld = ab_pk_freq_fld = ab_op_ride_fld = ab_op_freq_fld = ab_day_ride_fld = -1;
		ba_pk_ride_fld = ba_pk_freq_fld = ba_op_ride_fld = ba_op_freq_fld = ba_day_ride_fld = -1;
	}
	pk_ride_fld = data.file->Add_Field ("PK_RIDE", DB_INTEGER, 10);
	pk_freq_fld = data.file->Add_Field ("PK_FREQ", DB_DOUBLE, 6.2, MINUTES);
	pk_dist_fld = data.file->Add_Field ("PK_DIST", DB_DOUBLE, 6.2, MILES);
	pk_time_fld = data.file->Add_Field ("PK_TIME", DB_DOUBLE, 6.2, MINUTES);
	pk_pmt_fld = data.file->Add_Field ("PK_PMT", DB_DOUBLE, 10.1, MILES);
	pk_pht_fld = data.file->Add_Field ("PK_PHT", DB_DOUBLE, 10.1, HOURS);
	pk_vmt_fld = data.file->Add_Field ("PK_VMT", DB_DOUBLE, 10.2, MILES);
	pk_vht_fld = data.file->Add_Field ("PK_VHT", DB_DOUBLE, 10.2, HOURS);

	op_ride_fld = data.file->Add_Field ("OP_RIDE", DB_INTEGER, 10);
	op_freq_fld = data.file->Add_Field ("OP_FREQ", DB_DOUBLE, 6.2, MINUTES);
	op_dist_fld = data.file->Add_Field ("OP_DIST", DB_DOUBLE, 6.2, MILES);
	op_time_fld = data.file->Add_Field ("OP_TIME", DB_DOUBLE, 6.2, MINUTES);
	op_pmt_fld = data.file->Add_Field ("OP_PMT", DB_DOUBLE, 10.1, MILES);
	op_pht_fld = data.file->Add_Field ("OP_PHT", DB_DOUBLE, 10.1, HOURS);
	op_vmt_fld = data.file->Add_Field ("OP_VMT", DB_DOUBLE, 10.2, MILES);
	op_vht_fld = data.file->Add_Field ("OP_VHT", DB_DOUBLE, 10.2, HOURS);

	day_ride_fld = data.file->Add_Field ("DAY_RIDE", DB_INTEGER, 10);
	day_pmt_fld = data.file->Add_Field ("DAY_PMT", DB_DOUBLE, 10.1, MILES);
	day_pht_fld = data.file->Add_Field ("DAY_PHT", DB_DOUBLE, 10.1, HOURS);
	day_vmt_fld = data.file->Add_Field ("DAY_VMT", DB_DOUBLE, 10.2, MILES);
	day_vht_fld = data.file->Add_Field ("DAY_VHT", DB_DOUBLE, 10.2, HOURS);

	pk_hour_fld = data.file->Add_Field ("PEAK_HOUR", DB_DOUBLE, 10.1);
	if (!offset_flag) {
		ab_fac_fld = data.file->Add_Field ("AB_FACTOR", DB_DOUBLE, 8.2);
		ba_fac_fld = data.file->Add_Field ("BA_FACTOR", DB_DOUBLE, 8.2);
	} else {
		ab_fac_fld = ba_fac_fld = -1;
	}
	load_fac_fld = data.file->Add_Field ("LOAD_FAC", DB_DOUBLE, 8.2);
	max_fac_fld = data.file->Add_Field ("MAX_FACTOR", DB_DOUBLE, 8.2);
	max_line_fld = data.file->Add_Field ("MAX_LINE", DB_STRING, 20);

	if (station_flag) {
		aname_fld = data.file->Add_Field ("FROM", DB_STRING, 40);
		bname_fld = data.file->Add_Field ("TO", DB_STRING, 40);
	} else {
		aname_fld = bname_fld = -1;
	}
	data.file->Write_Header ();

	//---- read the node coordinates ----

	if (data.arcview_flag) {
		while (data.xy_file->Read ()) {
			node = data.xy_file->Get_Integer (data.node_fld);
			xy_rec.x = data.xy_file->Get_Double (data.xcoord_fld);
			xy_rec.y = data.xy_file->Get_Double (data.ycoord_fld);

			xy_stat = xy_map.insert (XY_Map_Data (node, xy_rec));

			if (!xy_stat.second) {
				Warning ("Duplicate Node Number ") << node;
			}
		}
		data.xy_file->Close ();
	}

	//---- gather peak ridership ----

	for (map_itr = peak_map.begin (); map_itr != peak_map.end (); map_itr++) {
		mode = map_itr->first.mode;

		if (!data.all_modes && !data.modes.In_Range (mode)) continue;

		name = map_itr->first.name;

		if (!data.all_lines) {
			for (str_itr = data.lines.begin (); str_itr != data.lines.end (); str_itr++) {
				if (name.In_Range (*str_itr)) break;
			}
			if (str_itr == data.lines.end ()) continue;
		}
		leg_ptr = &map_itr->second;
		
		Show_Progress ();
	
		//---- search for the link ----

		dir = 0;
		ab_key.first = abs (leg_ptr->b);
		ab_key.second = abs (leg_ptr->a);

		link_itr = link_map.find (ab_key);

		if (link_itr != link_map.end ()) {
			dir = 2;
		} else {
			ab_key.first = abs (leg_ptr->a);
			ab_key.second = abs (leg_ptr->b);

			link_itr = link_map.find (ab_key);

			if (link_itr != link_map.end ()) {
				dir = 1;
			}
		}
		if (dir == 0) {
			map_stat = link_map.insert (Rider_Map_Data (ab_key, link_rec));
			link_itr = map_stat.first;
			link_itr->second.pk_time = leg_ptr->time;
			link_itr->second.pk_dist = leg_ptr->dist;
			dir = 1;
		}
		link_ptr = &link_itr->second;
		ab_load = ba_load = 0.0;

		if (leg_ptr->ab.ride > 0) {
			if (dir == 1) {
				link_ptr->ab_pk_ride += leg_ptr->ab.ride;
				link_ptr->ab_pk_runs += leg_ptr->runs;
				link_ptr->ab_pk_service += leg_ptr->service;

				if (leg_ptr->service > 0) {
					pk_runs = leg_ptr->service;
				} else {
					pk_runs = leg_ptr->runs * data.peak_hours;
				}
				ab_load = leg_ptr->ab.ride * data.peak_fac / (pk_runs * data.peak_cap);
			} else {
				link_ptr->ba_pk_ride += leg_ptr->ab.ride;
				link_ptr->ba_pk_runs += leg_ptr->runs;
				link_ptr->ba_pk_service += leg_ptr->service;

				if (leg_ptr->service > 0) {
					pk_runs = leg_ptr->service;
				} else {
					pk_runs = leg_ptr->runs * data.peak_hours;
				}
				ba_load = leg_ptr->ab.ride * data.peak_fac / (pk_runs * data.peak_cap);
			}
		}
		if (leg_ptr->ba.ride > 0) {
			if (dir == 1) {
				link_ptr->ba_pk_ride += leg_ptr->ba.ride;
				link_ptr->ba_pk_runs += leg_ptr->runs;
				link_ptr->ba_pk_service += leg_ptr->service;

				if (leg_ptr->service > 0) {
					pk_runs = leg_ptr->service;
				} else {
					pk_runs = leg_ptr->runs * data.peak_hours;
				}
				ba_load = leg_ptr->ba.ride * data.peak_fac / (pk_runs * data.peak_cap);
			} else {
				link_ptr->ab_pk_ride += leg_ptr->ba.ride;
				link_ptr->ab_pk_runs += leg_ptr->runs;
				link_ptr->ab_pk_service += leg_ptr->service;

				if (leg_ptr->service > 0) {
					pk_runs = leg_ptr->service;
				} else {
					pk_runs = leg_ptr->runs * data.peak_hours;
				}
				ab_load = leg_ptr->ba.ride * data.peak_fac / (pk_runs * data.peak_cap);
			}
		}
		if (ba_load > ab_load) ab_load = ba_load;

		if (ab_load > link_ptr->max_load_fac) {
			link_ptr->max_load_fac = ab_load;
			link_ptr->max_line = name;
		}
	}

	//---- gather offpeak ridership ----

	for (map_itr = offpeak_map.begin (); map_itr != offpeak_map.end (); map_itr++) {
		mode = map_itr->first.mode;

		if (!data.all_modes && !data.modes.In_Range (mode)) continue;

		name = map_itr->first.name;

		if (!data.all_lines) {
			for (str_itr = data.lines.begin (); str_itr != data.lines.end (); str_itr++) {
				if (name.In_Range (*str_itr)) break;
			}
			if (str_itr == data.lines.end ()) continue;
		}
		leg_ptr = &map_itr->second;
		
		Show_Progress ();
	
		//---- search for the link ----

		dir = 0;
		ab_key.first = abs (leg_ptr->b);
		ab_key.second = abs (leg_ptr->a);

		link_itr = link_map.find (ab_key);

		if (link_itr != link_map.end ()) {
			dir = 2;
		} else {
			ab_key.first = abs (leg_ptr->a);
			ab_key.second = abs (leg_ptr->b);

			link_itr = link_map.find (ab_key);

			if (link_itr != link_map.end ()) {
				dir = 1;
			}
		}
		if (dir == 0) {
			map_stat = link_map.insert (Rider_Map_Data (ab_key, link_rec));
			link_itr = map_stat.first;
			link_itr->second.op_time = leg_ptr->time;
			link_itr->second.op_dist = leg_ptr->dist;
			dir = 1;
		} else if (link_itr->second.op_time == 0) {
			link_itr->second.op_time = leg_ptr->time;
			link_itr->second.op_dist = leg_ptr->dist;			
		}
		link_ptr = &link_itr->second;

		if (leg_ptr->ab.ride > 0) {
			if (dir == 1) {
				link_ptr->ab_op_ride += leg_ptr->ab.ride;
				link_ptr->ab_op_runs += leg_ptr->runs;
				link_ptr->ab_op_service += leg_ptr->service;
			} else {
				link_ptr->ba_op_ride += leg_ptr->ab.ride;
				link_ptr->ba_op_runs += leg_ptr->runs;
				link_ptr->ba_op_service += leg_ptr->service;
			}
		}
		if (leg_ptr->ba.ride > 0) {
			if (dir == 1) {
				link_ptr->ba_op_ride += leg_ptr->ba.ride;
				link_ptr->ba_op_runs += leg_ptr->runs;
				link_ptr->ab_op_service += leg_ptr->service;
			} else {
				link_ptr->ab_op_ride += leg_ptr->ba.ride;
				link_ptr->ab_op_runs += leg_ptr->runs;
				link_ptr->ab_op_service += leg_ptr->service;
			}
		}
	}

	//---- exit if no data ----

	if (link_map.size () == 0) {
		Warning ("No Link Data to Report");
		return;
	}

	//---- output the links ----

	for (link_itr = link_map.begin (); link_itr != link_map.end (); link_itr++) {
		for (dir=0; dir < 2; dir++) {
			if (!offset_flag && dir > 0) break;

			ab_key = link_itr->first;
			link_ptr = &link_itr->second;

			if (dir == 0) {
				anode = ab_key.first;
				bnode = ab_key.second;
			} else {
				anode = ab_key.second;
				bnode = ab_key.first;
				ab_key.first = anode;
				ab_key.second = bnode;
			}
			data.file->Put_Field (anode_fld, anode);
			data.file->Put_Field (bnode_fld, bnode);

			if (offset_flag) {
				if (dir == 0) {
					pk_ride = link_ptr->ab_pk_ride;
					op_ride = link_ptr->ab_op_ride;
					pk_runs = link_ptr->ab_pk_runs;
					op_runs = link_ptr->ab_op_runs;
				} else {
					pk_ride = link_ptr->ba_pk_ride;
					op_ride = link_ptr->ba_op_ride;
					pk_runs = link_ptr->ba_pk_runs;
					op_runs = link_ptr->ba_op_runs;
				}
				if (pk_runs == 0.0 && op_runs == 0.0) continue;
			} else {
				ab_day_ride = link_ptr->ab_pk_ride + link_ptr->ab_op_ride;
				ba_day_ride = link_ptr->ba_pk_ride + link_ptr->ba_op_ride;
				pk_ride = link_ptr->ab_pk_ride + link_ptr->ba_pk_ride;
				op_ride = link_ptr->ab_op_ride + link_ptr->ba_op_ride;
				pk_runs = link_ptr->ab_pk_runs + link_ptr->ba_pk_runs;
				op_runs = link_ptr->ab_op_runs + link_ptr->ba_op_runs;

				data.file->Put_Field (ab_pk_ride_fld, link_ptr->ab_pk_ride);
				data.file->Put_Field (ab_pk_freq_fld, (link_ptr->ab_pk_runs > 0.0) ? 60.0 / link_ptr->ab_pk_runs : 0.0);
				data.file->Put_Field (ab_op_ride_fld, link_ptr->ab_op_ride);
				data.file->Put_Field (ab_op_freq_fld, (link_ptr->ab_op_runs > 0.0) ? 60.0 / link_ptr->ab_op_runs : 0.0);
				data.file->Put_Field (ab_day_ride_fld, ab_day_ride);

				data.file->Put_Field (ba_pk_ride_fld, link_ptr->ba_pk_ride);
				data.file->Put_Field (ba_pk_freq_fld, (link_ptr->ba_pk_runs > 0.0) ? 60.0 / link_ptr->ba_pk_runs : 0.0);
				data.file->Put_Field (ba_op_ride_fld, link_ptr->ba_op_ride);
				data.file->Put_Field (ba_op_freq_fld, (link_ptr->ba_op_runs > 0.0) ? 60.0 / link_ptr->ba_op_runs : 0.0);
				data.file->Put_Field (ba_day_ride_fld, ba_day_ride);
			}
			day_ride = pk_ride + op_ride;

			data.file->Put_Field (pk_ride_fld, pk_ride);
			data.file->Put_Field (pk_freq_fld, (pk_runs > 0.0) ? 60.0 / pk_runs : 0.0);
			data.file->Put_Field (pk_time_fld, link_ptr->pk_time / 100.0);
			data.file->Put_Field (pk_dist_fld, link_ptr->pk_dist * 52.8);
			data.file->Put_Field (pk_pmt_fld, pk_ride * link_ptr->pk_dist * 52.8);	
			data.file->Put_Field (pk_pht_fld, pk_ride * link_ptr->pk_time / 6000.0);
		
			data.file->Put_Field (op_ride_fld, op_ride);
			data.file->Put_Field (op_freq_fld, (op_runs > 0.0) ? 60.0 / op_runs : 0.0);
			data.file->Put_Field (op_time_fld, link_ptr->op_time / 100.0);
			data.file->Put_Field (op_dist_fld, link_ptr->op_dist * 52.8);
			data.file->Put_Field (op_pmt_fld, op_ride * link_ptr->op_dist * 52.8);	
			data.file->Put_Field (op_pht_fld, op_ride * link_ptr->op_time / 6000.0);

			data.file->Put_Field (day_ride_fld, day_ride);
			data.file->Put_Field (day_pmt_fld, (pk_ride * link_ptr->pk_dist + op_ride * link_ptr->op_dist) * 52.8);	
			data.file->Put_Field (day_pht_fld, (pk_ride * link_ptr->pk_time + op_ride * link_ptr->op_time) / 6000.0);

			if (offset_flag) {
				pk_runs *= data.peak_hours;
				op_runs *= data.offpeak;

				if (dir == 0) {
					if (link_ptr->ab_pk_service > 0) {
						pk_runs = link_ptr->ab_pk_service;
					}
					if (link_ptr->ab_op_service > 0) {
						op_runs = link_ptr->ab_op_service;
					}
				} else {
					if (link_ptr->ba_pk_service > 0) {
						pk_runs = link_ptr->ba_pk_service;
					}
					if (link_ptr->ba_op_service > 0) {
						op_runs = link_ptr->ba_op_service;
					}
				}
			} else {
				if (link_ptr->ab_pk_service > 0) {
					ab_pk_runs = link_ptr->ab_pk_service;
				} else {
					ab_pk_runs = link_ptr->ab_pk_runs * data.peak_hours;
				}
				if (link_ptr->ba_pk_service > 0) {
					ba_pk_runs = link_ptr->ba_pk_service;
				} else {
					ba_pk_runs = link_ptr->ba_pk_runs * data.peak_hours;
				}
				pk_runs = ab_pk_runs + ba_pk_runs;

				if (link_ptr->ab_op_service > 0) {
					op_runs = link_ptr->ab_op_service;
				} else {
					op_runs = link_ptr->ab_op_runs * data.offpeak;
				}
				if (link_ptr->ba_op_service > 0) {
					op_runs += link_ptr->ba_op_service;
				} else {
					op_runs += link_ptr->ba_op_runs * data.offpeak;
				}
			}
			data.file->Put_Field (pk_vmt_fld, pk_runs * link_ptr->pk_dist * 52.8);	
			data.file->Put_Field (pk_vht_fld, pk_runs * link_ptr->pk_time / 6000.0);
			data.file->Put_Field (op_vmt_fld, op_runs * link_ptr->op_dist * 52.8);	
			data.file->Put_Field (op_vht_fld, op_runs * link_ptr->op_time / 6000.0);
			data.file->Put_Field (day_vmt_fld, (pk_runs * link_ptr->pk_dist + op_runs * link_ptr->op_dist) * 52.8);	
			data.file->Put_Field (day_vht_fld, (pk_runs * link_ptr->pk_time + op_runs * link_ptr->op_time) / 6000.0);

			if (offset_flag) {
				if (dir == 0) {
					peak_hour = link_ptr->ab_pk_ride * data.peak_fac / data.peak_hours;

					if (pk_runs > 0.0) {
						load_fac = link_ptr->ab_pk_ride * data.peak_fac / (pk_runs * data.peak_cap);
					} else {
						load_fac = 0.0;
					}
					data.file->Put_Field (pk_hour_fld, peak_hour);
					data.file->Put_Field (load_fac_fld, load_fac);
				} else {
					peak_hour = link_ptr->ba_pk_ride * data.peak_fac / data.peak_hours;

					if (pk_runs > 0.0) {
						load_fac = link_ptr->ba_pk_ride * data.peak_fac / (pk_runs * data.peak_cap);
					} else {
						load_fac = 0.0;
					}
					data.file->Put_Field (pk_hour_fld, peak_hour);
					data.file->Put_Field (load_fac_fld, load_fac);
				}
			} else {
				peak_hour = MAX (link_ptr->ab_pk_ride, link_ptr->ba_pk_ride) * data.peak_fac / data.peak_hours;

				if (ab_pk_runs > 0.0) {
					ab_load = link_ptr->ab_pk_ride * data.peak_fac / (ab_pk_runs * data.peak_cap);
				} else {
					ab_load = 0.0;
				}
				if (ba_pk_runs > 0.0) {
					ba_load = link_ptr->ba_pk_ride * data.peak_fac / (ba_pk_runs * data.peak_cap);
				} else {
					ba_load = 0.0;
				}
				load_fac = MAX (ab_load, ba_load);

				data.file->Put_Field (pk_hour_fld, peak_hour);
				data.file->Put_Field (ab_fac_fld, ab_load);
				data.file->Put_Field (ba_fac_fld, ba_load);
				data.file->Put_Field (load_fac_fld, load_fac);
			}
			data.file->Put_Field (max_fac_fld, link_ptr->max_load_fac);
			data.file->Put_Field (max_line_fld, link_ptr->max_line);

			if (station_flag) {
				name_itr = stop_names.find (anode);

				if (name_itr == stop_names.end ()) {
					data.file->Put_Field (aname_fld, question);
				} else {
					data.file->Put_Field (aname_fld, name_itr->second);
				}
				name_itr = stop_names.find (bnode);

				if (name_itr == stop_names.end ()) {
					data.file->Put_Field (bname_fld, question);
				} else {
					data.file->Put_Field (bname_fld, name_itr->second);
				}
			}
			if (data.arcview_flag) {
				data.arc_file->clear ();
				shape_found = false;

				//---- look for a link shape ----

				if (shape_flag) {
					shape_itr = points_map.find (ab_key);

					if (shape_itr == points_map.end ()) {
						ba_key.first = ab_key.second;
						ba_key.second = ab_key.first;

						shape_itr = points_map.find (ba_key);

						if (shape_itr != points_map.end ()) {
							shape_found = true;
							points = &(shape_itr->second);

							for (pts_ritr = points->rbegin (); pts_ritr != points->rend (); pts_ritr++) {
								data.arc_file->push_back (*pts_ritr);
							}
						}
					} else {
						shape_found = true;
						points = &(shape_itr->second);

						for (pts_itr = points->begin (); pts_itr != points->end (); pts_itr++) {
							data.arc_file->push_back (*pts_itr);
						}
					}
				}

				//---- use the node coordinates ----

				if (!shape_found) {
					xy_itr = xy_map.find (ab_key.first);

					if (xy_itr != xy_map.end ()) {
						point.x = xy_itr->second.x;
						point.y = xy_itr->second.y;
						data.arc_file->push_back (point);
					}
					xy_itr = xy_map.find (ab_key.second);

					if (xy_itr != xy_map.end ()) {
						point.x = xy_itr->second.x;
						point.y = xy_itr->second.y;
						data.arc_file->push_back (point);
					}
				}
				if (data.arc_file->size () < 2) {
					Warning (String ("Coordinates Missing for Link %d-%d") % ab_key.first % ab_key.second);
				} else {
					if (offset_flag) {
						Shift_Shape (*(data.arc_file), data.offset);
					}
					if (!data.arc_file->Write_Record ()) {
						Error ("Writing Arcview File");
					}
				}
			} else {
				if (!data.file->Write ()) {
					Error ("Writing Link Ridership");
				}
			}
		}
	}
	if (data.arcview_flag) {
		data.arc_file->Close ();
	} else {
		data.file->Close ();
	}
	End_Progress ();
}

