//*********************************************************
//	Read_Stop.cpp - read the compare transit stop file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Stop
//---------------------------------------------------------

void TransitDiff::Read_Stop (void)
{
	int num, link, dir, offset, use;
	Stop_Data stop_rec;
	Int_Map_Stat map_stat;

	//---- store the transit stop data ----

	Show_Message (String ("Reading %s -- Record") % compare_stop_file.File_Type ());
	Set_Progress ();

	while (compare_stop_file.Read ()) {
		Show_Progress ();

		stop_rec.Clear ();

		stop_rec.Stop (compare_stop_file.Stop ());
		if (stop_rec.Stop () == 0) continue;

		//---- check/convert the link number ----

		link = compare_stop_file.Link ();
		dir = compare_stop_file.Dir ();
		offset = Round (compare_stop_file.Offset ());

		Set_Link_Direction (&compare_stop_file, link, dir, offset);

		stop_rec.Link (link);
		stop_rec.Dir (dir);
		stop_rec.Offset (offset);

		//----- optional fields ----

		use = compare_stop_file.Use ();

		if (use == 0) {
			String text ("BUS/RAIL");
			use = Use_Code (text);
		}
		stop_rec.Use (use);

		if (compare_stop_file.Type_Flag ()) {
			stop_rec.Type (compare_stop_file.Type ());
		} else {
			stop_rec.Type (STOP);
		}
		stop_rec.Space (compare_stop_file.Space ());

		stop_rec.Name (compare_stop_file.Name ());

		map_stat = compare_stop_map.insert (Int_Map_Data (stop_rec.Stop (), (int) compare_stop_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Stop Number = ") << stop_rec.Stop ();
			continue;
		} else {
			compare_stop_array.push_back (stop_rec);
		}
	}
	End_Progress ();
	compare_stop_file.Close ();

	Print (2, String ("Number of %s Records = %d") % compare_stop_file.File_Type () % Progress_Count ());

	num = (int) compare_stop_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_stop_file.File_ID () % num);
	}
}
