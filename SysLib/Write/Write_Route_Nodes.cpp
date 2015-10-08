//*********************************************************
//	Write_Route_Nodes.cpp - write a new route nodes file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Route_Nodes
//---------------------------------------------------------

void Data_Service::Write_Route_Nodes (void)
{
	Route_Nodes_File *file = (Route_Nodes_File *) System_File_Handle (NEW_ROUTE_NODES);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = route_map.begin (); itr != route_map.end (); itr++) {
		Show_Progress ();

		count += Put_Route_Nodes_Data (*file, route_nodes_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Route_Nodes_Data
//---------------------------------------------------------

int Data_Service::Put_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data)
{
	Route_Node_Itr node_itr;
	Route_Period_Itr period_itr;
	Node_Data *node_ptr;
	Veh_Type_Data *veh_type_ptr;

	int i, node, dwell, count = 0;
	file.Route (data.Route ());
	file.Mode (data.Mode ());

	if (veh_type_array.size () > 0 && data.Veh_Type () >= 0) {
		veh_type_ptr = &veh_type_array [data.Veh_Type ()];
		file.Veh_Type (veh_type_ptr->Type ());
	} else {
		file.Veh_Type (data.Veh_Type ());
	}
	file.Name (data.Name ());
	file.Notes (data.Notes ());
	file.Nodes ((int) data.nodes.size ());

	for (i=0, period_itr = data.periods.begin (); period_itr != data.periods.end (); period_itr++, i++) {
		file.Headway (i, period_itr->Headway ());
		file.Offset (i, period_itr->Offset ());
		file.TTime (i, period_itr->TTime ());
	}
	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (node_itr = data.nodes.begin (); node_itr != data.nodes.end (); node_itr++) {
		node_ptr = &node_array [node_itr->Node ()];
		node = node_ptr->Node ();
		if (node_itr->Type () == NO_STOP && !file.Type_Flag ()) node = -node;
		dwell = node_itr->Dwell ();
		if (node_itr->Type () != NO_STOP && file.Dwell_Flag () && dwell <= 0) dwell = Dtime (5, SECONDS);

		file.Node (node);
		file.Type (node_itr->Type ());
		file.Dwell (dwell);
		file.Time (node_itr->Time ());
		file.Speed (node_itr->Speed ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
