//*********************************************************
//	Get_Route_Nodes.cpp - read the route nodes file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Route_Nodes_Data
//---------------------------------------------------------

bool ArcNet::Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data_rec)
{
	if (arcview_route_nodes.Is_Open ()) {

		//---- process a header line ----

		if (!file.Nested ()) {
			arcview_route_nodes.Copy_Fields (file);

			arcview_route_nodes.clear ();

			data_rec.Route (file.Route ());
			if (data_rec.Route () == 0) return (false);
			return (true);
		}

		//---- process a nested record ----

		int node;
		Int_Map_Itr itr;
		Node_Data *node_ptr;
		XYZ_Point point;

		node = abs (file.Node ());
	
		itr = node_map.find (node);
		if (itr == node_map.end ()) {
			Warning (String ("Route %d Node %d was Not Found") % file.Route () % node);
			return (false);
		}
		node_ptr = &node_array [itr->second];

		point.x = UnRound (node_ptr->X ());
		point.y = UnRound (node_ptr->Y ());
		point.z = UnRound (node_ptr->Z ());
		
		arcview_route_nodes.push_back (point);

		if ((int) arcview_route_nodes.size () == file.Num_Nest ()) {
			if (!arcview_route_nodes.Write_Record ()) {
				Error (String ("Writing %s") % arcview_route_nodes.File_Type ());
			}
		}
	}
	return (true);
}
