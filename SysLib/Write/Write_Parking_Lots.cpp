//*********************************************************
//	Write_Parking_Lots.cpp - write a new parking file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Parking_Lots
//---------------------------------------------------------

void Data_Service::Write_Parking_Lots (void)
{
	Parking_File *file = (Parking_File *) System_File_Handle (NEW_PARKING);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = parking_map.begin (); itr != parking_map.end (); itr++) {
		Show_Progress ();

		count += Put_Parking_Data (*file, parking_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Parking_Data
//---------------------------------------------------------

int Data_Service::Put_Parking_Data (Parking_File &file, Parking_Data &data)
{
	Park_Nest_Itr nest_itr;
	Link_Data *link_ptr;

	int count = 0;

	link_ptr = &link_array [data.Link ()];

	file.Parking (data.Parking ());
	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());
	file.Offset (UnRound (data.Offset ()));
	file.Type (data.Type ());
	file.Num_Nest ((int) data.size ());
	file.Notes (data.Notes ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (nest_itr = data.begin (); nest_itr != data.end (); nest_itr++) {
		file.Use (nest_itr->Use ());
		file.Start (nest_itr->Start ());
		file.End (nest_itr->End ());
		file.Space (nest_itr->Space ());
		file.Time_In (nest_itr->Time_In ());
		file.Time_Out (nest_itr->Time_Out ());
		file.Hourly (nest_itr->Hourly ());
		file.Daily (nest_itr->Daily ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
