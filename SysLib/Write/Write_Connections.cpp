//*********************************************************
//	Write_Connections.cpp - write a new connection file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Connections
//---------------------------------------------------------

void Data_Service::Write_Connections (void)
{
	Connect_File *file = (Connect_File *) System_File_Handle (NEW_CONNECTION);
	
	int dir, index, count;

	Connect_Data *connect_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr itr;

	count = 0;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		link_ptr = &link_array [itr->second];

		for (dir=0; dir < 2; dir++) {
			index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
			if (index < 0) continue;

			Show_Progress ();

			dir_ptr = &dir_array [index];

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				count += Put_Connect_Data (*file, *connect_ptr);
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type ()  % count);
}

//---------------------------------------------------------
//	Put_Connect_Data
//---------------------------------------------------------

int Data_Service::Put_Connect_Data (Connect_File &file, Connect_Data &data)
{
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());
	file.Lanes (Make_Lane_Range (dir_ptr, data.Low_Lane (), data.High_Lane ()));

	dir_ptr = &dir_array [data.To_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];
	
	file.To_Link (link_ptr->Link ());
	file.To_Lanes (Make_Lane_Range (dir_ptr, data.To_Low_Lane (), data.To_High_Lane ()));

	file.Type (data.Type ());
	file.Penalty (data.Penalty ());
	file.Speed (UnRound (data.Speed ()));
	file.Capacity (data.Capacity ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
