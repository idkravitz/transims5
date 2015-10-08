//*********************************************************
//	Write_Nodes.cpp - write a new node file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Nodes
//---------------------------------------------------------

void Data_Service::Write_Nodes (void)
{
	Node_File *file = (Node_File *) System_File_Handle (NEW_NODE);

	int count = 0;
	Int_Map_Itr itr;
	
	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = node_map.begin (); itr != node_map.end (); itr++) {
		Show_Progress ();

		count += Put_Node_Data (*file, node_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	Print (2, String ("%s Records = %d") % file->File_Type ()  % count);
}

//---------------------------------------------------------
//	Put_Node_Data
//---------------------------------------------------------

int Data_Service::Put_Node_Data (Node_File &file, Node_Data &data)
{
	file.Node (data.Node ());
	file.X (UnRound (data.X ()));
	file.Y (UnRound (data.Y ()));
	file.Z (UnRound (data.Z ()));
	file.Subarea (data.Subarea ());
	file.Partition (data.Partition ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
