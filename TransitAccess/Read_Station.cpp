//*********************************************************
//	Read_Station.cpp - read the station file
//*********************************************************

#include "TransitAccess.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Station
//---------------------------------------------------------

void TransitAccess::Read_Station (void)
{
	int num, fld, capacity, cost, stop, node, access, dist;
	double dx, dy, time;

	Station_Data station_data;
	String text;
	Node_Data node_data;
	Node_Map_Itr itr;
	Mode_Itr mode_itr;
	Int_Itr node_itr;
	Integers bus_nodes;

	Show_Message (String ("Reading %s -- Record") % station_file.File_Type ());
	Set_Progress ();

	access = 0;

	if (node_flag) {
		memset (&node_data, '\0', sizeof (node_data));
	}

	while (station_file.Read ()) {
		Show_Progress ();
		
		station_data.use = station_file.Get_Bool (station_use_fld);
		if (!station_data.use) continue;

		text = station_file.Get_String (station_mode_fld);
		text.To_Upper ();

		station_data.mode = (int) text [0];
		station_data.type = station_file.Get_Integer (station_type_fld);
		station_data.pnr = station_file.Get_Bool (station_pnr_fld);
		station_data.skim = station_file.Get_Integer (station_skim_fld);
		station_data.zone = station_file.Get_Integer (station_zone_fld);
		station_data.stop = station_file.Get_Integer (station_stop_fld);
		station_data.park = station_file.Get_Integer (station_park_fld);
		station_data.node = 0;
		station_data.x_coord = station_file.Get_Integer (station_x_fld);
		station_data.y_coord = station_file.Get_Integer (station_y_fld);
		station_data.name = station_file.Get_String (station_name_fld);
		station_data.missing = 0;

		if (station_data.skim > ndes) {
			Warning (String ("Station Skim Destination %d is Out of Range (1..%d)") % station_data.skim % ndes);
			station_data.skim = 0;
		}
		if (station_data.zone > nzones) {
			Warning (String ("Station Zone %d is Out of Range (1..%d)") % station_data.zone % nzones);
			station_data.zone = 0;
			station_data.cbd_dist = 0;
		} else {
			dx = station_data.x_coord - cbd_x;
			dy = station_data.y_coord - cbd_y;

			station_data.cbd_dist = DTOI (sqrt (dx * dx + dy * dy));
		}

		//---- read bus nodes ----

		bus_nodes.clear ();

		for (node_itr = station_node_flds.begin (); node_itr != station_node_flds.end (); node_itr++) {
			node = station_file.Get_Integer (*node_itr);
			if (node > 0) {
				bus_nodes.push_back (node);
			}
		}
		if (bus_nodes.size () > 0) {
			station_data.node = bus_nodes [0];
		}

		//---- save the station data ----

		station_array.push_back (station_data);

		//---- station access data ----
			
		stop = station_data.stop;

		if (station_acc_flag && stop > 0) {
			access = station_file.Get_Integer (station_acc_fld);

			if (access != 0) {
				station_access.insert (Int_Map_Data (stop, access));
			}
		}

		//---- add the station to the node data ----

		node_data.x_coord = station_data.x_coord;
		node_data.y_coord = station_data.y_coord;

		if (station_data.stop > 0 && mode_codes.size () > 0) {
			Node_Map_Stat map_stat;

			for (node_itr = mode_codes.begin (); node_itr != mode_codes.end (); node_itr++) {
				if (station_data.mode == *node_itr) {
					map_stat = node_map.insert (Node_Map_Data (station_data.stop, node_data));
					map_stat.first->second.use = 2;
					break;
				}
			}
		}

		//---- save the node coordinates ----

		if (node_flag) {
			if (station_data.stop > 0) {
				new_node_map.insert (Node_Map_Data (station_data.stop, node_data));
			}
			if (station_data.pnr == 1 && station_data.park > 0) {
				new_node_map.insert (Node_Map_Data (station_data.park, node_data));
			} else if (station_data.stop == 0 && station_data.node > 0) {
				new_node_map.insert (Node_Map_Data (station_data.node, node_data));
			}
		}

		//---- save the station support data ----

		for (mode_itr = mode_array.begin (); mode_itr != mode_array.end (); mode_itr++) {
			if (station_data.mode != mode_itr->mode) continue;

			//---- save coordinates of the stop ----

			stop = station_data.stop;
			if (stop > 0) {
				text (";---- Station=%d, %s ----") % stop % station_data.name;
				mode_itr->mode_file->File () << text << endl;
				text ("XY NODE=%d X=%d Y=%d") % stop % station_data.x_coord % station_data.y_coord;
				mode_itr->mode_file->File () << text << endl;
				mode_itr->count++;

				if (mode_itr->walk_time.size () > 0) {
					time = mode_itr->walk_time.Best (0) * 100.0;
				} else {
					time = 100.0;
				}
				if (station_acc_flag && access != 0) {
					time += access;
					if (time < 0) time = 0;
				}
				dist = DTOI (time * 3.0 / 60.0);

				for (node_itr = bus_nodes.begin (); node_itr != bus_nodes.end (); node_itr++) {
					text ("SUPPORT N=%d-%d ONEWAY=F MODE=12 DIST=%d SPEED=3") % stop % *node_itr % dist;
					mode_itr->mode_file->File () << text << endl;
					mode_itr->count++;					
				}
			} else {
				stop = station_data.node;
				if (stop == 0) continue;
				text (";---- Stop=%d, %s ----") % stop % station_data.name;
				mode_itr->mode_file->File () << text << endl;
			}

			//---- connect the parking lot to the stop ----

			if (mode_itr->pnr && station_data.pnr && station_data.park != 0) {
				text ("XY NODE=%d X=%d Y=%d  ;---- parking ----") % station_data.park % station_data.x_coord % station_data.y_coord;
				mode_itr->mode_file->File () << text << endl;
				mode_itr->count++;

				text ("SUPPORT N=%d-%d ONEWAY=T MODE=15 DIST=10 SPEED=3") % station_data.park % stop;
				mode_itr->mode_file->File () << text << endl;
				mode_itr->count++;

				//---- create walk links to station parking lots ----

				if (cost_flag) {
					capacity = station_file.Get_Integer (station_cap_fld) / 500 + 1;

					time = mode_itr->walk_time.Best (capacity);

					fld = station_cost_flds.Best (mode_itr->skim);
					cost = MAX ((station_file.Get_Integer (fld) / 2), 1);

					time += cost * time_value;

					fld = station_time_flds.Best (mode_itr->skim);
					time += station_file.Get_Integer (fld) / 100.0;

					text ("SUPPLINK N=%d-%d ONEWAY=T MODE=15 DIST=%d TIME=%.2lf") % station_data.park % stop % cost % time;
					mode_itr->mode_file->File () << text << endl;
					mode_itr->count++;
				}
			}
		}
	}
	End_Progress ();
	station_file.Close ();

	Print (2, "Number of Station Records = ") << Progress_Count ();

	num = (int) station_array.size ();

	if (num != Progress_Count ()) {
		Print (1, "Number of Stations Used = ") << num;
	}

	//---- write the node data ----

	if (node_flag) {
		for (itr = new_node_map.begin (); itr != new_node_map.end (); itr++) {
			new_node_file.Put_Field (new_node_fld, itr->first);
			new_node_file.Put_Field (new_x_fld, itr->second.x_coord);
			new_node_file.Put_Field (new_y_fld, itr->second.y_coord);

			new_node_file.Write ();
		}
		Print (2, "Number of New Node Records = ") << new_node_map.size ();
	}

	//---- write stop zones ----

	if (stop_zone_flag) {
		Station_Itr stop_itr;
		num = 0;

		for (stop_itr = station_array.begin (); stop_itr != station_array.end (); stop_itr++) {
			if (stop_itr->mode == stop_zone_mode) {
				stop = stop_itr->stop;

				text (";---- Station=%d, %s ----") % stop % stop_itr->name;
				stop_zone_file.File () << text << endl;

				stop -= stop_zone_offset;
				text ("XY NODE=%d X=%d Y=%d") % stop % stop_itr->x_coord % stop_itr->y_coord;
				stop_zone_file.File () << text << endl;

				text ("LINK NODES=%d-%d REVCD=2 DIST=0 ONEWAY=F SPEED=30") % stop % stop_itr->stop;
				stop_zone_file.File () << text << endl;
				num += 3;
			}
		}
		Print (2, "Number of New Stop Zone Records = ") << num;
		stop_zone_file.Close ();
	}

	//---- add a section delimiter message ---

	for (mode_itr = mode_array.begin (); mode_itr != mode_array.end (); mode_itr++) {
		if (mode_itr->skim >= 0) {
			mode_itr->mode_file->File () << ";---- Drive Access Links ----" << endl;
		}
	}
}
