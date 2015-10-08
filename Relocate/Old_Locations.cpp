//*********************************************************
//	Old_Locations.cpp - read the old location file
//*********************************************************

#include "Relocate.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Old_Locations
//---------------------------------------------------------

void Relocate::Old_Locations (void)
{
	int num, link, dir;
	double setback, offset;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Location_Data location_rec;
	Link_Data *link_ptr;
	XYZ point;

	//---- store the location data ----

	Show_Message (String ("Reading %s -- Record") % old_loc_file.File_Type ());
	Set_Progress ();

	num = old_loc_file.Num_Records ();
	old_loc_array.reserve (num);

	while (old_loc_file.Read ()) {
		Show_Progress ();

		location_rec.Clear ();

		location_rec.Location (old_loc_file.Location ());
		if (location_rec.Location () == 0) continue;
		
		link = old_loc_file.Link ();

		map_itr = old_link_map.find (link);
		if (map_itr == old_link_map.end ()) {
			Warning (String ("Location Link %d was Not Found") % link);
			continue;
		}
		link_ptr = &old_link_array [map_itr->second];

		location_rec.Link (map_itr->second);

		dir = old_loc_file.Dir ();
		location_rec.Dir (dir);

		offset = old_loc_file.Offset ();
		location_rec.Offset (Round (offset));

		setback = old_loc_file.Setback ();
		location_rec.Setback (setback);

		point = Link_Offset (link_ptr, dir, offset, setback);

		location_rec.X (point.x);
		location_rec.Y (point.y);

		location_rec.Zone (old_loc_file.Zone ());

		//---- save the location record ----

		map_stat = old_loc_map.insert (Int_Map_Data (location_rec.Location (), (int) old_loc_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Location Number = ") << location_rec.Location ();
			continue;
		} else {
			old_loc_array.push_back (location_rec);
		}
	}
	End_Progress ();
	old_loc_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_loc_file.File_Type () % Progress_Count ());

	num = (int) old_loc_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_loc_file.File_ID () % num);
	}
}
