//*********************************************************
//	Write_Signals.cpp - write a new signal file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Signals
//---------------------------------------------------------

void Data_Service::Write_Signals (void)
{
	Signal_File *file = (Signal_File *) System_File_Handle (NEW_SIGNAL);

	int count = 0;
	Int_Map_Itr map_itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (map_itr = signal_map.begin (); map_itr != signal_map.end (); map_itr++) {
		Show_Progress ();

		count += Put_Signal_Data (*file, signal_array [map_itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Signal_Data
//---------------------------------------------------------

int Data_Service::Put_Signal_Data (Signal_File &file, Signal_Data &data)
{
	String nodes;
	Int_Itr node_itr;
	Int_Map_Itr map_itr;
	Signal_Time_Itr time_itr;
	Node_Data *node_ptr;

	int count = 0;

	file.Signal (data.Signal ());
	file.Group (data.Group ());

	nodes.erase ();

	for (node_itr = data.nodes.begin (); node_itr != data.nodes.end (); node_itr++) {
		node_ptr = &node_array [*node_itr];
		if (nodes.empty ()) {
			nodes = String ("%d") % node_ptr->Node ();
		} else {
			nodes += String ("|%d") % node_ptr->Node ();
		}
	}
	file.Nodes (nodes);
	file.Times ((int) data.size ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (time_itr = data.begin (); time_itr != data.end (); time_itr++) {
		file.Start (time_itr->Start ());
		file.End (time_itr->End ());
		file.Timing (time_itr->Timing ());
		file.Phasing (time_itr->Phasing ());
		file.Notes (time_itr->Notes ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}

