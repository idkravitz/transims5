//*********************************************************
//	Build_Routes.cpp - Build routes from Route-Nodes
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Build_Routes
//---------------------------------------------------------

void TransitNet::Build_Routes (void)
{
	int n, n0, node, node0, route, mode, num_in, dir_index, nsplit, spacing, last_stop, type;
	int offset, headway, speed, change, length, aoffset, boffset, first_offset, last_offset;
	Dtime dwell, ttime, spd_time, cum_time, next_dwell, prev_dwell, next_ttime;
	Dtime offset_time, time_inc, low, high;
	double factor;
	bool first, bus_flag, left_flag, prev_left, put_flag, short_flag, first_flag, mid_flag, last_flag;
	Use_Type use;

	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Dir_Data *dir_ptr, *next_ptr;
	Link_Data *link_ptr;
	Route_Node_Itr node_itr;
	Route_Period_Itr period_itr;
	Route_Nodes_Itr route_itr; 
	Int2_Map_Itr ab_itr;
	Int2_Key ab_key;
	Path_Leg_Itr path_itr;
	Path_Leg_RItr path_ritr;
	Line_Data *line_ptr, line_rec;
	Veh_Type_Data *veh_type_ptr;
	Int_Map *stop_list;
	Stop_Data *stop_ptr, stop_rec;
	Line_Stop line_stop;
	Line_Stop_Itr line_stop_itr;
	Line_Run line_run;

	Dtimes stop_times;

	Route_Path_Data route_path;
	Route_Path_Array route_path_array;
	Route_Path_Itr route_path_itr, next_path_itr;

	//---- read the route file ----

	Show_Message ("Building Route Data -- Record");
	Set_Progress ();
	num_in = 0;

	for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {
		Show_Progress ();
		num_in++;

		route_path_array.clear ();

		route = route_itr->Route ();

		map_stat = line_map.insert (Int_Map_Data (route, (int) line_array.size ()));
		if (map_stat.second) {
			line_rec.Route (route);
			line_array.push_back (line_rec);
			line_ptr = &line_array.back ();
		} else {
			line_ptr = &line_array [map_stat.first->second];
			line_edit++;
			route_edit += (int) line_ptr->size () + 1;
			schedule_edit += (((int) line_ptr->begin ()->size () + 7) / 8) * ((int) line_ptr->size () + 1);
			driver_edit += (int) line_ptr->driver_array.size () + 1;
			line_ptr->Clear ();
		}
		mode = route_itr->Mode ();
		use = (mode <= EXPRESS_BUS) ? BUS : RAIL;

		line_ptr->Route (route);
		line_ptr->Mode (mode);

		type = route_itr->Veh_Type ();
		if (type < 0) {
			switch (mode) {
				case LOCAL_BUS:
					type = 4;
					break;
				case EXPRESS_BUS:
					type = 5;
					break;
				case TROLLEY:
					type = 6;
					break;
				case STREETCAR:
					type = 7;
					break;
				case LRT:
					type = 8;
					break;
				case RAPIDRAIL:
					type = 9;
					break;
				case REGIONRAIL:
					type = 10;
					break;
			}
			map_itr = veh_type_map.find (type);
			if (map_itr == veh_type_map.end ()) {
				Warning (String ("Route %d Vehicle Type %d was Not Found") % route % type);
			} else {
				type = map_itr->second;
			}
		}
		line_ptr->Type (type);

		veh_type_ptr = &veh_type_array [type];
		use = veh_type_ptr->Use ();

		//---- read the node records ----

		cum_time = 0;
		node0 = n0 = 0;
		first = true;

		for (node_itr = route_itr->nodes.begin (); node_itr != route_itr->nodes.end (); node_itr++) {
			node = node_itr->Node ();
			n = node_array [node].Node ();

			stop_type = node_itr->Type ();

			if (dwell_flag) {
				dwell = node_itr->Dwell ();
			} else {
				if (stop_type == NO_STOP) {
					dwell = 0;
				} else {
					dwell = min_dwell;
				}
			}
			if (first) {
				route_path.Node (node);
				route_path.Dir_Index (-1);
				route_path.Dwell (dwell);
				route_path.TTime (0);

				route_path_array.push_back (route_path);
				first = false;
			} else {
				ttime = node_itr->Time ();
				speed = node_itr->Speed ();

				//---- find the link(s) between route nodes ----

				ab_itr = ab_map.find (Int2_Key (n0, n));

				if (ab_itr == ab_map.end ()) {

					//---- attempt to build a minimum distance path ----

					Node_Path (node0, node, use);

					if ((int) path_leg_array.size () == 0) {

						//---- try to build an auto path ----

						Node_Path (node0, node, CAR);

						if ((int) path_leg_array.size () != 0) {
							Warning (String ("Path from %d and %d on Route %d includes Use Restrictions") % n0 % n % route);
						}
					}

					if ((int) path_leg_array.size () == 0) {
						Warning (String ("No Path between Nodes %d and %d on Route %d") % n0 % n % route);
						break;
					} else {

						//---- calculate the network travel time for the path ----

						cum_time = spd_time = 0;

						for (path_itr = path_leg_array.begin (); path_itr != path_leg_array.end (); path_itr++) {
							dir_ptr = &dir_array [path_itr->Dir_Index ()];

							cum_time += dir_ptr->Time0 ();

							if (speed_flag && speed > 0) {
								link_ptr = &link_array [dir_ptr->Link ()];

								spd_time += Round ((double) link_ptr->Length () / speed);
							}
						}

						//---- check the transit travel time ----

						if (ttime == 0) {
							if (spd_time > 0) {
								ttime = spd_time + dwell;
								cum_time = spd_time;
							} else {
								ttime = cum_time + dwell;
							}
						} else if (ttime < cum_time) {
							Warning (String ("Route %d Node %d to %d travel time %.1lf < %.1lf") % route % 
								n0 % node % ttime.Seconds () % cum_time.Seconds ());
						}

						//---- distribute the transit travel time to each link on the path ----

						if (cum_time < 1) cum_time = 1;
						factor = (double) ttime / cum_time;

						for (path_ritr = path_leg_array.rbegin (); path_ritr != path_leg_array.rend (); path_ritr++) {
							route_path.Node (path_ritr->Node ());
							route_path.Dir_Index (path_ritr->Dir_Index ());
							line_ptr->driver_array.push_back (path_ritr->Dir_Index ());

							dir_ptr = &dir_array [path_ritr->Dir_Index ()];

							ttime = DTOI (dir_ptr->Time0 () * factor);

							route_path.TTime (ttime);
							route_path.Dwell (dwell);

							route_path_array.push_back (route_path);
						}
					}

				} else {

					//---- calculate and check the transit travel time ----

					route_path.Node (node);
					route_path.Dir_Index (ab_itr->second);
					line_ptr->driver_array.push_back (ab_itr->second);

					dir_ptr = &dir_array [ab_itr->second];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (!Use_Permission (link_ptr->Use (), use)) {
						Warning (String ("Link %d does Not Permit %s Service") % 
							link_ptr->Link () % Transit_Code ((Transit_Type) mode));
					}
					if (ttime == 0) {
						if (speed_flag && speed > 0) {
							ttime = DTOI ((double) link_ptr->Length () / speed) + dwell;
						} else {
							ttime = dir_ptr->Time0 () + dwell;
						}
					} else if (ttime < dir_ptr->Time0 ()) {
						Warning (String ("Route %d Link %d travel time %.1lf < %.1lf") % route % 
							link_ptr->Link () % ttime.Seconds () % UnRound (dir_ptr->Time0 ()));
					}
					route_path.Dwell (dwell);
					route_path.TTime (ttime);

					route_path_array.push_back (route_path);
				}
			}
			route_path.Dwell (0);
			route_path.TTime (0);
			node0 = node;
			n0 = n;
		}

		if (route_path_array.size () <= 1) {
			Warning (String ("Route %d has Missing Node Records") % route);
			continue;
		}

		//---- assign stops to route links ----

		stop_times.clear ();

		bus_flag = (mode == LOCAL_BUS || mode == EXPRESS_BUS);

		dir_ptr = next_ptr = 0;
		put_flag = false;

		dir_index = 0;
		cum_time = next_dwell = next_ttime = dwell = 0;
		prev_left = left_flag = false;

		for (route_path_itr = route_path_array.begin (); route_path_itr != route_path_array.end (); route_path_itr++) {

			if (route_path_itr->Dir_Index () < 0) {
				dwell = route_path_itr->Dwell ();
				continue;
			}
			prev_dwell = dwell;
			first_flag = mid_flag = last_flag = false;
			dir_index = route_path_itr->Dir_Index ();

			if (dir_ptr == 0) {
				dir_ptr = &dir_array [dir_index];
				dwell = route_path_itr->Dwell ();
				ttime = route_path_itr->TTime ();

				first_flag = true;
				if (prev_dwell == 0) {		
					Warning ("A Stop was Added at the Beginning of Route ") << route;
				}
			} else {
				dir_ptr = next_ptr;
				dwell = next_dwell;
				ttime = next_ttime;
			}
			left_flag = false;

			next_path_itr = route_path_itr + 1;

			if (next_path_itr != route_path_array.end ()) {
				next_ptr = &dir_array [next_path_itr->Dir_Index ()];
				next_dwell = next_path_itr->Dwell ();
				next_ttime = next_path_itr->TTime ();

				if ((dir_ptr->Lanes () + dir_ptr->Right ()) > 1) {
					change = compass.Change (dir_ptr->Out_Bearing (), next_ptr->In_Bearing ());
					left_flag = (change <= left_turn);
				}
			} else {
				next_ptr = 0;
				next_dwell = next_ttime = 0;
				last_flag = true;
				if (dwell == 0) {
					Warning ("A Stop was Added at the End of Route ") << route;
				}
			}

			//---- set the stop criteria ----

			link_ptr = &link_array [dir_ptr->Link ()];

			length = link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ();
			short_flag = (length < (2 * stop_offset));

			if (!short_flag && prev_dwell > 0) {
				if (stop_type == FARSIDE || prev_left || !put_flag) {
					first_flag = true;
				}
			}
			if (bus_flag && !short_flag && prev_dwell > 0 && dwell > 0 && local_access [dir_index] != 0) {
				mid_flag = true;
			}
			if (!short_flag && dwell > 0) {
				if ((stop_type == NEARSIDE && !left_flag) || (!mid_flag && next_ptr != 0 && local_access [dir_index] == 0)) {
					last_flag = true;
				}
			}
			prev_left = left_flag;
			put_flag = false;

			if (first_flag || last_flag || mid_flag) {
				if (dir_ptr->Dir () == 0) {
					aoffset = link_ptr->Aoffset ();
					boffset = link_ptr->Boffset ();
				} else {
					aoffset = link_ptr->Boffset ();
					boffset = link_ptr->Aoffset ();
				}
				first_offset = aoffset + stop_offset;
				last_offset = link_ptr->Length () - boffset - stop_offset;

				offset_time = (int) ((double) first_offset * ttime / link_ptr->Length () + 0.5);
				
				if (bus_flag) {

					//---- check for special conditions ----

					if (local_access [dir_index] == 0 || short_flag) {
						Warning (String ("Route %d must Stop on Link %d") % route % link_ptr->Link ());

						if (length < (4 * stop_offset)) {
							nsplit = 0;
							time_inc = spacing = 0;
						} else {
							nsplit = 1;
							spacing = length - 2 * stop_offset;
							time_inc = (int) ((double) spacing * ttime / link_ptr->Length () + 0.5);
						}

					} else {

						//---- normal stop spacing ----

						spacing = Round (min_stop_spacing.Best (link_ptr->Area_Type ()));
						if (spacing == 0) spacing = length;
		
						nsplit = (length - (2 * stop_offset)) / spacing;
						if (nsplit < 1) nsplit = 1;
					
						spacing = (length - 2 * stop_offset) / nsplit;
						time_inc = (int) ((double) spacing * ttime / link_ptr->Length () + 0.5);
					}

				} else {	//---- rail line ----

					nsplit = 1;
					spacing = length - 2 * stop_offset;
					if (spacing < stop_offset) spacing = length / 2;
					time_inc = (int) ((double) spacing * ttime / link_ptr->Length () + 0.5);
				}
				nsplit++;

				//---- add stops to the route list ----
		
				stop_list = &dir_stop_array [dir_index];

				last_stop = 0;
				offset = 0;

				for (n=1; n <= nsplit; n++) {
					if (n == 1) {
						ttime -= offset_time;
						cum_time += offset_time;
						offset = first_offset;
						if (!first_flag) continue;
					} else if (n == nsplit) {
						ttime -= time_inc;
						cum_time += time_inc;
						offset = last_offset;
						if (!last_flag) continue;
					} else {
						ttime -= time_inc;
						cum_time += time_inc;
						offset += spacing;
						if (!mid_flag) continue;
					}

					//---- check for an existing stop ----

					for (map_itr = stop_list->begin (); map_itr != stop_list->end (); map_itr++) {
						if (map_itr->first >= offset - stop_offset) break;
						last_stop = map_itr->second;
					}

					if (map_itr != stop_list->end () && map_itr->first >= offset - stop_offset && 
						map_itr->first <= offset + stop_offset) {

						//---- use existing stop ----

						stop_ptr = &stop_array [map_itr->second];

						stop_ptr->Space (stop_ptr->Space () + 1);
						if (bus_flag) {
							stop_ptr->Use (stop_ptr->Use () | bus_code);
						} else {
							stop_ptr->Use (stop_ptr->Use () | rail_code);
						}
						last_stop = map_itr->second;

					} else {

						//---- create a new stop ----

						stop_rec.Stop (++max_stop);
						stop_rec.Link_Dir (dir_ptr->Link_Dir ());
						stop_rec.Offset (offset);

						stop_rec.Space (1);
						if (bus_flag) {
							stop_rec.Use (bus_code);
							stop_rec.Type (STOP);
						} else {
							stop_rec.Use (rail_code);
							stop_rec.Type (STATION);
						}
						last_stop = (int) stop_array.size ();

						stop_map.insert (Int_Map_Data (stop_rec.Stop (), last_stop));

						stop_array.push_back (stop_rec);

						//---- update the stop list ----
		
						stop_list->insert (Int_Map_Data (offset, last_stop));
					}

					//---- save the route stop ----

					cum_time += dwell;

					stop_times.push_back (cum_time);

					line_stop.Stop (last_stop);
					line_stop.Zone (fare_zone [dir_index]);

					line_ptr->push_back (line_stop);

					put_flag = true;
				}
			}
			cum_time += ttime;
		}

		//---- add schedule data ----

		if (cum_time < 1) cum_time = 1;

		for (n0=0, period_itr = route_itr->periods.begin (); period_itr != route_itr->periods.end (); period_itr++, n0++) {
			headway = period_itr->Headway ();
			if (headway <= 0) continue;

			offset = period_itr->Offset ();

			if (offset < 0) {
				offset = (int) (headway * random.Probability () + 0.5);
			}
			schedule_periods.Period_Range (n0, low, high);

			if (period_itr->TTime () > 0) {
				factor = (double) period_itr->TTime () / cum_time;
			} else {
				factor = 1.0;
			}

			for (ttime = low + offset; ttime < high; ttime += headway) {
				for (n=0, line_stop_itr = line_ptr->begin (); line_stop_itr != line_ptr->end (); line_stop_itr++, n++) {
					time_inc = ttime + stop_times [n] * factor;
					line_run.Schedule (time_inc.Round_Seconds ());

					line_stop_itr->push_back (line_run);
				}
			}
		}
	}
	End_Progress ();

	Print (1, "Number of Transit Routes = ") << num_in;
}
