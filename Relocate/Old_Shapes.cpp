//*********************************************************
//	Old_Shapes.cpp - read the old shape point file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Old_Shapes
//---------------------------------------------------------

void Relocate::Old_Shapes (void)
{
	int i, num;	
	Shape_Data shape_rec;
	Int_Map_Stat map_stat;

	//---- store the shape point data ----

	Show_Message (String ("Reading %s -- Record") % old_shape_file.File_Type ());
	Set_Progress ();

	while (old_shape_file.Read (false)) {
		Show_Progress ();

		shape_rec.Clear ();

		shape_rec.Link (old_shape_file.Link ());
		if (shape_rec.Link () <= 0) continue;

		num = old_shape_file.Points ();
		if (num < 1) continue;

		shape_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!old_shape_file.Read (true)) {
				Error (String ("Number of Nested Records for Link %d") % old_shape_file.Link ());
			}
			Show_Progress ();

			//---- process a point record ----

			XYZ point;

			point.x = Round (old_shape_file.X ());
			point.y = Round (old_shape_file.Y ());
			point.z = Round (old_shape_file.Z ());

			shape_rec.push_back (point);
		}

		map_stat = old_shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) old_shape_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Link Number = ") << shape_rec.Link ();
		} else {
			old_shape_array.push_back (shape_rec);
		}
	}
	End_Progress ();
	old_shape_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_shape_file.File_Type () % Progress_Count ());

	num = (int) old_shape_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of Link %s Records = %d") % old_shape_file.File_ID () % num);
	}
}
