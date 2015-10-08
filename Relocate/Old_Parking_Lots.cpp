//*********************************************************
//	Old_Parking_Lots.cpp - read the old parking file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Old_Parking_Lots
//---------------------------------------------------------

void Relocate::Old_Parking_Lots (void)
{
	int i, num, link;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Parking_Data parking_rec;

	//---- store the parking data ----

	Show_Message (String ("Reading %s -- Record") % old_park_file.File_Type ());
	Set_Progress ();

	num = old_park_file.Num_Records ();
	old_park_array.reserve (num);

	while (old_park_file.Read (false)) {
		Show_Progress ();

		parking_rec.Clear ();
		parking_rec.Parking (old_park_file.Parking ());
		if (parking_rec.Parking () == 0) continue;

		//---- check/convert the link number and direction ----
		
		link = old_park_file.Link ();

		map_itr = old_link_map.find (link);
		if (map_itr == old_link_map.end ()) {
			Warning (String ("Parking Link %d was Not Found") % link);
			continue;
		}
		parking_rec.Link (map_itr->second);
		parking_rec.Dir (old_park_file.Dir ());
		parking_rec.Offset (Round (old_park_file.Offset ()));
		parking_rec.Type (old_park_file.Type ());

		num = old_park_file.Num_Nest ();

		for (i=1; i <= num; i++) {
			if (!old_park_file.Read (true)) {
				Error (String ("Number of Nested Records for Parking %d") % old_park_file.Parking ());
			}
			Show_Progress ();
		}
		map_stat = old_park_map.insert (Int_Map_Data (parking_rec.Parking (), (int) old_park_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Parking Number = ") << parking_rec.Parking ();
		} else {
			old_park_array.push_back (parking_rec);
		}
	}
	End_Progress ();
	old_park_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_park_file.File_Type () % Progress_Count ());

	num = (int) old_park_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_park_file.File_ID () % num);
	}
}
