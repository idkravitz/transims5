//*********************************************************
//	Read_Line.cpp - read the compare transit route file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void TransitDiff::Read_Line (void)
{
	int i, num, route;
	bool keep_flag;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Line_Data line_rec;

	//---- store the route data ----

	Show_Message (String ("Reading %s -- Record") % compare_line_file.File_Type ());
	Set_Progress ();

	while (compare_line_file.Read (false)) {
		Show_Progress ();

		line_rec.Clear ();
		keep_flag = true;

		route = compare_line_file.Route ();
		if (route == 0) keep_flag = false;
		line_rec.Route (route);

		if (compare_line_file.Stops () < 2) keep_flag = false;

		line_rec.Mode (compare_line_file.Mode ());
		line_rec.Type (compare_line_file.Type ());
		line_rec.Name (compare_line_file.Name ());

		map_itr = veh_type_map.find (line_rec.Type ());
		if (map_itr == veh_type_map.end ()) {
			Warning (String ("Transit Route %d Vehicle Type %d was Not Found") % route % line_rec.Type ());
		} else {
			line_rec.Type (map_itr->second);
		}

		num = compare_line_file.Num_Nest ();
		if (num > 0) line_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!compare_line_file.Read (true)) {
				Error (String ("Number of Stop Records for Route %d") % compare_line_file.Route ());
			}
			Show_Progress ();

			//---- process a stop record ----

			int zone, stop;
			Line_Stop line_stop;
			Int_Map_Itr itr;

			stop = compare_line_file.Stop ();

			itr = compare_stop_map.find (stop);
			if (itr == compare_stop_map.end ()) {
				Warning (String ("Transit Stop %d on Route %d was Not Found") % stop % route);
				continue;
			}
			line_stop.Stop (itr->second);
			zone = compare_line_file.Zone ();

			line_stop.Zone (zone);
			line_stop.Time_Flag ((compare_line_file.Time_Flag () > 0));
			
			line_rec.push_back (line_stop);
		}
		if (keep_flag) {
			map_stat = compare_line_map.insert (Int_Map_Data (line_rec.Route (), (int) compare_line_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Route Number = ") << line_rec.Route ();
			} else {
				compare_line_array.push_back (line_rec);
			}
		}
	}
	End_Progress ();
	compare_line_file.Close ();

	Print (2, String ("Number of %s Records = %d") % compare_line_file.File_Type () % Progress_Count ());

	num = (int) compare_line_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_line_file.File_ID () % num);
	}
}

