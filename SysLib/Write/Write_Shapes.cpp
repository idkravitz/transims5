//*********************************************************
//	Write_Shapes.cpp - write a new shape file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Shapes
//---------------------------------------------------------

int Data_Service::Write_Shapes (void)
{
	Shape_File *file = (Shape_File *) System_File_Handle (NEW_SHAPE);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = shape_map.begin (); itr != shape_map.end (); itr++) {
		Show_Progress ();

		count += Put_Shape_Data (*file, shape_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);

	return (count);
}

//---------------------------------------------------------
//	Put_Shape_Data
//---------------------------------------------------------

int Data_Service::Put_Shape_Data (Shape_File &file, Shape_Data &data)
{
	Int_Map_Itr itr;
	XYZ_Itr pt_itr;

	int count = 0;

	file.Link (data.Link ());
	file.Points ((int) data.size ());
	file.Notes (data.Notes ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (pt_itr = data.begin (); pt_itr != data.end (); pt_itr++) {
		file.X (UnRound (pt_itr->x));
		file.Y (UnRound (pt_itr->y));
		file.Z (UnRound (pt_itr->z));

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
