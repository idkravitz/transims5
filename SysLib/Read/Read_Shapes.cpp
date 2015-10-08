//*********************************************************
//	Read_Shapes.cpp - read the shape point file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Shapes
//---------------------------------------------------------

void Data_Service::Read_Shapes (void)
{
	Shape_File *file = (Shape_File *) System_File_Handle (SHAPE);
	
	int i, num;	
	bool keep_flag;
	Shape_Data shape_rec;
	Int_Map_Stat map_stat;

	//---- store the shape point data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Shapes (*file);

	while (file->Read (false)) {
		Show_Progress ();

		shape_rec.Clear ();

		keep_flag = Get_Shape_Data (*file, shape_rec);

		num = file->Num_Nest ();
		if (num > 0) shape_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Nested Records for Link %d") % file->Link ());
			}
			Show_Progress ();

			Get_Shape_Data (*file, shape_rec);
		}
		if (keep_flag) {
			map_stat = shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) shape_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << shape_rec.Link ();
			} else {
				shape_array.push_back (shape_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) shape_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of Link %s Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (SHAPE);
}

//---------------------------------------------------------
//	Initialize_Nodes
//---------------------------------------------------------

void Data_Service::Initialize_Shapes (Shape_File &file)
{
	int percent = System_Data_Reserve (SHAPE);

	if (shape_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else {
			num /= 20;
		}
		if (num > 1) {
			shape_array.reserve (num);
			if (num > (int) shape_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Shape_Data
//---------------------------------------------------------

bool Data_Service::Get_Shape_Data (Shape_File &file, Shape_Data &shape_rec)
{

	//---- process a header line ----

	if (!file.Nested ()) {
		shape_rec.Link (file.Link ());
		if (shape_rec.Link () <= 0) return (false);

		if (file.Points () < 1) return (false);

		shape_rec.Notes (file.Notes ());

	} else {

		//---- process a point record ----

		XYZ point;

		point.x = Round (file.X ());
		point.y = Round (file.Y ());
		point.z = Round (file.Z ());

		shape_rec.push_back (point);
	}
	return (true);
}
