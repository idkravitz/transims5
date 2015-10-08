//*********************************************************
//	Read_Node.cpp - read the compare node file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Node
//---------------------------------------------------------

void TransitDiff::Read_Node (void)
{
	Int_Map_Stat map_stat;
	Node_Data node_rec;

	//---- store the node data ----

	Show_Message (String ("Reading %s -- Record") % compare_node_file.File_Type ());
	Set_Progress ();

	while (compare_node_file.Read ()) {
		Show_Progress ();

		node_rec.Clear ();

		node_rec.Node (compare_node_file.Node ());
		node_rec.X (compare_node_file.X ());
		node_rec.Y (compare_node_file.Y ());
		node_rec.Z (compare_node_file.Z ());
		node_rec.Notes (compare_node_file.Notes ());

		map_stat = compare_node_map.insert (Int_Map_Data (node_rec.Node (), (int) compare_node_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Node Number = ") << node_rec.Node ();
			continue;
		} else {
			compare_node_array.push_back (node_rec);
		}
	}
	End_Progress ();
	compare_node_file.Close ();

	Print (2, String ("Number of %s Records = %d") % compare_node_file.File_Type () % Progress_Count ());

	int num = (int) compare_node_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_node_file.File_ID () % num);
	}
}
