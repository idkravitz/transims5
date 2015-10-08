//*********************************************************
//	Read_Driver.cpp - read the compare transit driver file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Driver
//---------------------------------------------------------

void TransitDiff::Read_Driver (void)
{
	int i, num, count, route, type, link, dir, offset, dir_index;
	bool keep_flag;

	Line_Data *line_ptr;
	Driver_Data driver_rec;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;

	//---- store the transit driver data ----

	Show_Message (String ("Reading %s -- Record") % compare_driver_file.File_Type ());
	Set_Progress ();
	
	count = 0;

	while (compare_driver_file.Read (false)) {
		Show_Progress ();

		driver_rec.Clear ();

		keep_flag = true;

		route = compare_driver_file.Route ();
		if (route == 0) keep_flag = false;
		if (compare_driver_file.Links () < 1) keep_flag = false;

		map_itr = compare_line_map.find (route);
		if (map_itr == compare_line_map.end ()) {
			Warning (String ("Transit Route %d was Not Found") % route);
			keep_flag = false;
		}
		driver_rec.Route (map_itr->second);

		type = compare_driver_file.Type ();

		driver_rec.Type (type);

		if (driver_rec.Type () > 0) {
			map_itr = veh_type_map.find (driver_rec.Type ());
			if (map_itr == veh_type_map.end ()) {
				Warning (String ("Driver Route %d Vehicle Type %d was Not Found") % route % driver_rec.Type ());
				keep_flag = false;
			}
			driver_rec.Type (map_itr->second);
		}

		num = compare_driver_file.Num_Nest ();
		if (num > 0) driver_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!compare_driver_file.Read (true)) {
				Error (String ("Number of Link Records for Route %d") % compare_driver_file.Route ());
			}
			Show_Progress ();

			link = compare_driver_file.Link ();
			dir = compare_driver_file.Dir ();
			offset = 0;

			link_ptr = Set_Link_Direction (&compare_driver_file, link, dir, offset);

			if (link_ptr == 0) continue;

			if (dir) {
				dir_index = link_ptr->BA_Dir ();
			} else {
				dir_index = link_ptr->AB_Dir ();
			}
			if (dir_index < 0) {
				Line_Data *line_ptr = &compare_line_array [driver_rec.Route ()];

				route = line_ptr->Route ();
				link = compare_driver_file.Link ();

				Warning (String ("Route %d Link %d Direction %s was Not Found") % route % link % ((dir) ? "BA" : "AB"));
				continue;
			}
			driver_rec.push_back (dir_index);
		}
		if (keep_flag) {
			line_ptr = &compare_line_array [driver_rec.Route ()];

			if (driver_rec.Type () > 0 && line_ptr->Type () == 0) line_ptr->Type (driver_rec.Type ());

			line_ptr->driver_array.swap (driver_rec);
			count += (int) driver_rec.size ();
		}
	}
	End_Progress ();
	compare_driver_file.Close ();

	Print (2, String ("Number of %s Records = %d") % compare_driver_file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_driver_file.File_ID () % count);
	}
}
