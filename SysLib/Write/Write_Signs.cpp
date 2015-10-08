//*********************************************************
//	Write_Signs.cpp - write a new sign file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Signs
//---------------------------------------------------------

void Data_Service::Write_Signs (void)
{
	Sign_File *file = (Sign_File *) System_File_Handle (NEW_SIGN);

	int dir, index, count;

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
			if (dir_ptr->Sign () == NO_CONTROL) continue;

			count += Put_Sign_Data (*file, *dir_ptr);
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Sign_Data
//---------------------------------------------------------

int Data_Service::Put_Sign_Data (Sign_File &file, Dir_Data &data)
{
	Link_Data *link_ptr;

	if (data.Sign () == NO_CONTROL) return (0);

	link_ptr = &link_array [data.Link ()];

	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());
	file.Sign (data.Sign ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
