//*********************************************************
//	Write_Lines.cpp - write a new transit route file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Lines
//---------------------------------------------------------

void Data_Service::Write_Lines (void)
{
	Line_File *file = (Line_File *) System_File_Handle (NEW_TRANSIT_ROUTE);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = line_map.begin (); itr != line_map.end (); itr++) {
		Show_Progress ();

		count += Put_Line_Data (*file, line_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	line_array.Route_Records (count);
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Line_Data
//---------------------------------------------------------

int Data_Service::Put_Line_Data (Line_File &file, Line_Data &data)
{
	Line_Stop_Itr stop_itr;
	Stop_Data *stop_ptr;
	Veh_Type_Data *veh_type_ptr;

	int count = 0;

	file.Route (data.Route ());
	file.Mode (data.Mode ());
	file.Stops ((int) data.size ());
	file.Name (data.Name ());
	file.Notes (data.Notes ());

	veh_type_ptr = &veh_type_array [data.Type ()];
	file.Type (veh_type_ptr->Type ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (stop_itr = data.begin (); stop_itr != data.end (); stop_itr++) {
		stop_ptr = &stop_array [stop_itr->Stop ()];
		file.Stop (stop_ptr->Stop ());
		file.Zone (stop_itr->Zone ());
		file.Time_Flag ((stop_itr->Time_Flag ()) ? 1 : 0);

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
