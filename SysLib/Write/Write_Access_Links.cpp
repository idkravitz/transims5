//*********************************************************
//	Write_Access_Links.cpp - write a new access link file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Access_Links
//---------------------------------------------------------

void Data_Service::Write_Access_Links (void)
{
	Access_File *file = (Access_File *) System_File_Handle (NEW_ACCESS_LINK);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = access_map.begin (); itr != access_map.end (); itr++) {
		Show_Progress ();

		count += Put_Access_Data (*file, access_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Access_Data
//---------------------------------------------------------

int Data_Service::Put_Access_Data (Access_File &file, Access_Data &data)
{
	int id, type;

	file.Link (data.Link ());

	id = data.From_ID ();
	type = data.From_Type ();

	if (type == LOCATION_ID) {
		id = location_array [id].Location ();
	} else if (type == PARKING_ID) {
		id = parking_array [id].Parking ();
	} else if (type == STOP_ID) {
		id = stop_array [id].Stop ();
	} else if (type == NODE_ID) {
		id = node_array [id].Node ();
	}
	file.From_ID (id);
	file.From_Type (type);

	id = data.To_ID ();
	type = data.To_Type ();

	if (type == LOCATION_ID) {
		id = location_array [id].Location ();
	} else if (type == PARKING_ID) {
		id = parking_array [id].Parking ();
	} else if (type == STOP_ID) {
		id = stop_array [id].Stop ();
	} else if (type == NODE_ID) {
		id = node_array [id].Node ();
	}
	file.To_ID (id);
	file.To_Type (type);

	file.Dir (data.Dir ());
	file.Time (data.Time ());
	file.Cost (data.Cost ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
