//*********************************************************
//	Read_Node.cpp - read the node file
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Read_Node
//---------------------------------------------------------

void TransitAccess::Read_Node (void)
{
	int node;
	Node_Data node_data;
	Node_Map_Stat map_stat;
	Node_Map_Itr node_itr;
	Data_Range_Itr range_itr;

	Show_Message (String ("Reading %s -- Record") % node_file.File_Type ());
	Set_Progress ();

	while (node_file.Read ()) {
		Show_Progress ();

		node = node_file.Get_Integer (node_fld);
		if (node <= nzones) continue;

		node_data.x_coord = node_file.Get_Integer (node_x_fld);
		node_data.y_coord = node_file.Get_Integer (node_y_fld);
		node_data.use = 0;

		if (node_data.x_coord == 0 && node_data.y_coord == 0) continue;

		map_stat = node_map.insert (Node_Map_Data (node, node_data));

		if (!map_stat.second) {
			Warning (String ("Duplicate Node Number = %d") % node);
		}
	}
	End_Progress ();
	node_file.Close ();

	Print (2, "Number of Node Records = ") << Progress_Count ();

	node = (int) node_map.size ();

	if (node != Progress_Count ()) {
		Print (1, "Number of Nodes Kept = ") << node;
	}
}
