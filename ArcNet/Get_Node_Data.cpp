//*********************************************************
//	Get_Node_Data.cpp - additional node processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool ArcNet::Get_Node_Data (Node_File &file, Node_Data &node_rec)
{
	if (Data_Service::Get_Node_Data (file, node_rec)) {
		if (arcview_node.Is_Open ()) {
			arcview_node.Copy_Fields (file);

			XYZ_Point *point = arcview_node.Get_Points ();

			point->x = file.X ();
			point->y = file.Y ();
			point->z = file.Z ();

			if (!arcview_node.Write_Record ()) {
				Error (String ("Writing %s") % arcview_node.File_Type ());
			}
		}
		return (true);
	}
	return (false);
}
