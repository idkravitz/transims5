//*********************************************************
//	Old_Nodes.cpp - read the old node file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Old_Nodes
//---------------------------------------------------------

void Relocate::Old_Nodes (void)
{
	Int_Map_Stat map_stat;
	Node_Data node_rec;

	//---- store the old node data ----

	Show_Message (String ("Reading %s -- Record") % old_node_file.File_Type ());
	Set_Progress ();

	old_node_array.reserve (old_node_file.Num_Records ());

	while (old_node_file.Read ()) {
		Show_Progress ();

		//---- read and save the data ----

		node_rec.Clear ();
		node_rec.Node (old_node_file.Node ());
		node_rec.X (old_node_file.X ());
		node_rec.Y (old_node_file.Y ());
		node_rec.Z (old_node_file.Z ());
		node_rec.Subarea (old_node_file.Subarea ());
		node_rec.Partition (old_node_file.Partition ());

		map_stat = old_node_map.insert (Int_Map_Data (node_rec.Node (), (int) old_node_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Node Number = ") << node_rec.Node ();
			continue;
		} else {
			old_node_array.push_back (node_rec);
		}
	}
	End_Progress ();
	old_node_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_node_file.File_Type () % Progress_Count ());

	int num = (int) old_node_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_node_file.File_ID () % num);
	}
}
