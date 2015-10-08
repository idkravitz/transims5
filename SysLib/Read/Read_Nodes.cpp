//*********************************************************
//	Read_Nodes.cpp - read the node file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Nodes
//---------------------------------------------------------

void Data_Service::Read_Nodes (void)
{
	Node_File *file = (Node_File *) System_File_Handle (NODE);

	Int_Map_Stat map_stat;
	Node_Data node_rec;

	//---- store the node data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Nodes (*file);

	while (file->Read ()) {
		Show_Progress ();

		node_rec.Clear ();

		if (Get_Node_Data (*file, node_rec)) {
			map_stat = node_map.insert (Int_Map_Data (node_rec.Node (), (int) node_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Node Number = ") << node_rec.Node ();
				continue;
			} else {
				node_array.push_back (node_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	int num = (int) node_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (NODE);
}

//---------------------------------------------------------
//	Initialize_Nodes
//---------------------------------------------------------

void Data_Service::Initialize_Nodes (Node_File &file)
{
	int percent = System_Data_Reserve (NODE);

	if (node_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			node_array.reserve (num);
			if (num > (int) node_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool Data_Service::Get_Node_Data (Node_File &file, Node_Data &node_rec)
{
	//---- read and save the data ----

	node_rec.Node (file.Node ());
	node_rec.X (file.X ());
	node_rec.Y (file.Y ());
	node_rec.Z (file.Z ());
	node_rec.Subarea (file.Subarea ());
	node_rec.Partition (file.Partition ());
	node_rec.Notes (file.Notes ());

	return (true);
}
