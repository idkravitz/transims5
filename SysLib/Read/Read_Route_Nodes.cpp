//*********************************************************
//	Read_Route_Nodes.cpp - read the route nodes file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Route_Nodes
//---------------------------------------------------------

void Data_Service::Read_Route_Nodes (void)
{
	Route_Nodes_File *file = (Route_Nodes_File *) System_File_Handle (ROUTE_NODES);
	
	int i, num;
	bool keep_flag;
	Int_Map_Stat map_stat;
	Route_Header route_rec;

	//---- store the route nodes data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Route_Nodes (*file);

	while (file->Read (false)) {
		Show_Progress ();

		route_rec.Clear ();

		keep_flag = Get_Route_Nodes_Data (*file, route_rec);

		num = file->Num_Nest ();
		if (num > 0) route_rec.nodes.reserve (num);

		route_rec.periods.reserve (file->Num_Periods ());
	
		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Nested Records for Route %d") % file->Route ());
			}
			Show_Progress ();

			Get_Route_Nodes_Data (*file, route_rec);
		}
		if (keep_flag) {
			map_stat = route_map.insert (Int_Map_Data (route_rec.Route (), (int) route_nodes_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Route Number = ") << route_rec.Route ();
			} else {
				route_nodes_array.push_back (route_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) route_nodes_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (ROUTE_NODES);
}

//---------------------------------------------------------
//	Initialize_Route_Nodes
//---------------------------------------------------------

void Data_Service::Initialize_Route_Nodes (Route_Nodes_File &file)
{
	Required_File_Check (file, NODE);

	int percent = System_Data_Reserve (ROUTE_NODES);

	if (route_nodes_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 1.5);
		}
		if (num > 1) {
			route_nodes_array.reserve (num);
			if (num > (int) route_nodes_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Route_Nodes_Data
//---------------------------------------------------------

bool Data_Service::Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data_rec)
{
	static Dtime dwell = 0;
	static Dtime time = 0;

	//---- process a header line ----

	if (!file.Nested ()) {
		data_rec.Route (file.Route ());
		if (data_rec.Route () == 0) return (false);

		//---- set the parking type ----
		
		data_rec.Mode (file.Mode ());
		data_rec.Veh_Type (file.Veh_Type ());

		if (file.Version () <= 40) {
			data_rec.Veh_Type (VehType40_Map (data_rec.Veh_Type (), 0));
		}
		if (veh_type_array.size () > 0) {
			if (data_rec.Veh_Type () > 0) {
				Int_Map_Itr map_itr = veh_type_map.find (data_rec.Veh_Type ());
				if (map_itr == veh_type_map.end ()) {
					Warning (String ("Transit Vehicle Type %d was Not Found") % data_rec.Veh_Type ());
					data_rec.Veh_Type (-1);
				} else {
					data_rec.Veh_Type (map_itr->second);
				}
			} else {
				data_rec.Veh_Type (-1);
			}
		}

		data_rec.Name (file.Name ());
		data_rec.Notes (file.Notes ());

		for (int i=0; i < file.Num_Periods (); i++) {
			Route_Period period_rec;

			period_rec.Headway (file.Headway (i));
			period_rec.Offset (file.Offset (i));
			period_rec.TTime (file.TTime (i));

			data_rec.periods.push_back (period_rec);
		}
		dwell = time = 0;
		return (true);
	}

	//---- process a nested record ----

	Route_Node node_rec;
	Int_Map_Itr itr;
	int node;

	node = file.Node ();
	if (node < 0) {
		node_rec.Type (NO_STOP);
		node = -node;
	} else if (file.Type_Flag ()) {
		node_rec.Type (file.Type ());
	} else {
		node_rec.Type (STOP);
	}
	itr = node_map.find (node);
	if (itr == node_map.end ()) {
		Warning (String ("Route %d Node %d was Not Found") % data_rec.Route () % node);
		dwell += file.Dwell ();
		time += file.Time ();
		return (false);
	}
	node_rec.Node (itr->second);
	node_rec.Dwell (file.Dwell () + dwell);
	node_rec.Time (file.Time () + time);
	node_rec.Speed (file.Speed ());
	dwell = time = 0;

	data_rec.nodes.push_back (node_rec);
	return (true);
}
