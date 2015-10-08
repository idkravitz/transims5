//*********************************************************
//	Write_Stops.cpp - write a new transit stop file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Stops
//---------------------------------------------------------

void Data_Service::Write_Stops (void)
{
	Stop_File *file = (Stop_File *) System_File_Handle (NEW_TRANSIT_STOP);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = stop_map.begin (); itr != stop_map.end (); itr++) {
		Show_Progress ();

		count += Put_Stop_Data (*file, stop_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Stop_Data
//---------------------------------------------------------

int Data_Service::Put_Stop_Data (Stop_File &file, Stop_Data &data)
{
	Link_Data *link_ptr;

	link_ptr = &link_array [data.Link ()];

	file.Stop (data.Stop ());
	file.Name (data.Name ());
	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());
	file.Offset (UnRound (data.Offset ()));
	file.Use (data.Use ());
	file.Type (data.Type ());
	file.Space (data.Space ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
