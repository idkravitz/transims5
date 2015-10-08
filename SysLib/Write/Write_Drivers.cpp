//*********************************************************
//	Write_Drivers.cpp - write a new transit driver file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Drivers
//---------------------------------------------------------

void Data_Service::Write_Drivers (void)
{
	Driver_File *file = (Driver_File *) System_File_Handle (NEW_TRANSIT_DRIVER);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = line_map.begin (); itr != line_map.end (); itr++) {
		Show_Progress ();

		count += Put_Driver_Data (*file, line_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	line_array.Driver_Records (count);
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Driver_Data
//---------------------------------------------------------

int Data_Service::Put_Driver_Data (Driver_File &file, Line_Data &data)
{
	Driver_Itr driver_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Veh_Type_Data *veh_type_ptr;

	int count = 0;

	file.Route (data.Route ());
	file.Links ((int) data.driver_array.size ());
	file.Notes (data.Notes ());
	
	if (veh_type_array.size () > 0) {
		veh_type_ptr = &veh_type_array [data.Type ()];
		file.Type (veh_type_ptr->Type ());
	} else {
		file.Type (data.Type ());
	}
	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (driver_itr = data.driver_array.begin (); driver_itr != data.driver_array.end (); driver_itr++) {
		dir_ptr = &dir_array [*driver_itr];
		link_ptr = &link_array [dir_ptr->Link ()];

		file.Link (link_ptr->Link ());
		file.Dir (dir_ptr->Dir ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
