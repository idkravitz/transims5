//*********************************************************
//	Read_Route_Nodes.cpp - read transit route nodes
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Route_Nodes
//---------------------------------------------------------

void NewFormat::Read_Route_Nodes (void)
{
	String field;
	int i, node, route, route_fld, mode_fld, ttime_fld, name_fld, note_fld, node_fld, dwell_fld, time_fld, spd_fld;
	int head_fld [24], offset_fld [24], ttim_fld [24];

	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Route_Header route_rec, *route_ptr;

	//---- process the route header file ----

	Show_Message (String ("Reading %s -- Record") % route_header.File_Type ());
	Set_Progress ();

	route_fld = route_header.Required_Field ("ROUTE", "LINE");
	mode_fld = route_header.Required_Field ("MODE", "TYPE");
	ttime_fld = route_header.Optional_Field ("TTIME", "TIME", "MIN_TIME");
	name_fld = route_header.Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	note_fld = route_header.Optional_Field ("NOTES");
	
	for (i=1; i <= route_periods; i++) {
		field ("HEADWAY_%d") % i;
		head_fld [i-1] = route_header.Required_Field (field.c_str ());

		field ("OFFSET_%d") % i;
		offset_fld [i-1] = route_header.Optional_Field (field.c_str ());

		field ("TTIME_%d") % i;
		ttim_fld [i-1] = route_header.Optional_Field (field.c_str ());
		if (ttim_fld [i-1] < 0) ttim_fld [i-1] = ttime_fld;
	}

	while (route_header.Read ()) {
		Show_Progress ();
		route_rec.Clear ();

		route_rec.Route (route_header.Get_Integer (route_fld));
		if (route_rec.Route () == 0) continue;

		//---- set the parking type ----
		
		route_rec.Mode (Transit_Code (route_header.Get_String (mode_fld)));
		route_rec.Name (route_header.Get_String (name_fld));
		route_rec.Notes (route_header.Get_String (note_fld));

		switch (route_rec.Mode ()) {
			case LOCAL_BUS:
				route_rec.Veh_Type (4);
				break;
			case EXPRESS_BUS:
				route_rec.Veh_Type (5);
				break;
			case TROLLEY:
				route_rec.Veh_Type (6);
				break;
			case STREETCAR:
				route_rec.Veh_Type (7);
				break;
			case LRT:
				route_rec.Veh_Type (8);
				break;
			case RAPIDRAIL:
				route_rec.Veh_Type (9);
				break;
			case REGIONRAIL:
				route_rec.Veh_Type (10);
				break;
		}
		if (veh_type_array.size () > 0) {
			route_rec.Veh_Type (VehType40_Map (route_rec.Veh_Type (), 0));

			if (route_rec.Veh_Type () > 0) {
				Int_Map_Itr map_itr = veh_type_map.find (route_rec.Veh_Type ());
				if (map_itr == veh_type_map.end ()) {
					Warning (String ("Transit Vehicle Type %d was Not Found") % route_rec.Veh_Type ());
					route_rec.Veh_Type (0);
				} else {
					route_rec.Veh_Type (map_itr->second);
				}
			} else {
				route_rec.Veh_Type (0);
			}
		}

		for (i=0; i < route_periods; i++) {
			Route_Period period_rec;

			period_rec.Headway (route_header.Get_Time (head_fld [i]));
			period_rec.Offset (route_header.Get_Time (offset_fld [i]));
			period_rec.TTime (route_header.Get_Integer (ttim_fld [i]));

			route_rec.periods.push_back (period_rec);
		}
		map_stat = route_map.insert (Int_Map_Data (route_rec.Route (), (int) route_nodes_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Route Number = ") << route_rec.Route ();
		} else {
			route_nodes_array.push_back (route_rec);
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % route_header.File_Type () % Progress_Count ());

	//---- process the route nodes file ----

	Show_Message (String ("Reading %s -- Record") % route_nodes.File_Type ());
	Set_Progress ();

	route_fld = route_nodes.Required_Field ("ROUTE", "LINE");
	node_fld = route_nodes.Required_Field ("NODE", "STOP");
	dwell_fld = route_nodes.Optional_Field ("DWELL", "DWELL_TIME", "STOP_TIME", "STOPTIME");
	time_fld = route_nodes.Optional_Field ("TIME", "TTIME", "RUNTIME", "RUN_TIME");
	spd_fld = route_nodes.Optional_Field ("SPEED", "SPD", "RUNSPD", "RUN_SPD", "RUN_SPEED");

	while (route_nodes.Read ()) {
		Show_Progress ();

		route = route_nodes.Get_Integer (route_fld);

		map_itr = route_map.find (route);
		if (map_itr == route_map.end ()) {
			Warning (String ("Route Nodes Route %d was Not Found in the Route Header File") % route);
			continue;
		}
		route_ptr = &route_nodes_array [map_itr->second];

		Route_Node node_rec;

		node = route_nodes.Get_Integer (node_fld);
		if (node < 0) {
			node_rec.Type (NO_STOP);
			node = -node;
		} else {
			node_rec.Type (STOP);
		}
		map_itr = node_map.find (node);
		if (map_itr == node_map.end ()) {
			Warning (String ("Route %d Node %d was Not Found") % route % node);
			continue;
		}
		node_rec.Node (map_itr->second);
		node_rec.Dwell (Dtime (route_nodes.Get_Integer (dwell_fld), SECONDS));
		node_rec.Time (Dtime (route_nodes.Get_Integer (time_fld), SECONDS));
		node_rec.Speed (Internal_Units (route_nodes.Get_Double (spd_fld), MPS));

		route_ptr->nodes.push_back (node_rec);
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % route_nodes.File_Type () % Progress_Count ());
}
