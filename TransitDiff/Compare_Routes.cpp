//*********************************************************
//	Compare_Routes.cpp - compare the transit routes
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Compare_Routes
//---------------------------------------------------------

void TransitDiff::Compare_Routes (void)
{
	int stops, stops2, ttime, ttime2, runs, runs2, links, links2, fld;

	Int_Map_Itr map_itr, map2_itr;
	Line_Data *line_ptr, *line2_ptr;
	Line_Stop *first_stop, *last_stop;
	Line_Run_Itr first_run_itr, last_run_itr;
	
	Show_Message ("Compare Routes -- Record");
	Set_Progress ();

	//---- check each input route ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		stops = stops2 = runs = runs2 = ttime = ttime2 = links = links2 = 0;

		line_ptr = &line_array [map_itr->second];
		stops = (int) line_ptr->size ();
		links = (int) line_ptr->driver_array.size ();

		//---- summarize the base data ----

		first_stop = &line_ptr->front ();
		last_stop = &line_ptr->back ();

		for (first_run_itr = first_stop->begin (), last_run_itr = last_stop->begin ();
			first_run_itr != first_stop->end () && last_run_itr != last_stop->end (); 
			first_run_itr++, last_run_itr++) {

			ttime += (last_run_itr->Schedule () - first_run_itr->Schedule ());
			runs++;
		}
		if (runs > 0) ttime /= runs;
		ttime = Resolve (ttime);

		fld = 1;

		diff_file.Put_Field (fld++, line_ptr->Route ());
		diff_file.Put_Field (fld++, stops);
		diff_file.Put_Field (fld++, runs);
		diff_file.Put_Field (fld++, ttime);
		if (driver_flag) {
			diff_file.Put_Field (fld++, links);
		}

		//---- match the route numbers ----

		map2_itr = compare_line_map.find (map_itr->first);
		if (map2_itr != compare_line_map.end ()) {
			line2_ptr = &compare_line_array [map2_itr->second];

			stops2 = (int) line2_ptr->size ();
			links2 = (int) line2_ptr->driver_array.size ();

			//---- summarize the compare data ----

			first_stop = &line2_ptr->front ();
			last_stop = &line2_ptr->back ();

			for (first_run_itr = first_stop->begin (), last_run_itr = last_stop->begin ();
				first_run_itr != first_stop->end () && last_run_itr != last_stop->end (); 
				first_run_itr++, last_run_itr++) {

				ttime2 += (last_run_itr->Schedule () - first_run_itr->Schedule ());
				runs2++;
			}
			if (runs2 > 0) ttime2 /= runs2;
			ttime2 = Resolve (ttime2);
		}

		//---- check for differences ----

		if (stops != stops2 || runs != runs2 || ttime != ttime2 || links != links2) {
			diff_file.Put_Field (fld++, stops2);
			diff_file.Put_Field (fld++, runs2);
			diff_file.Put_Field (fld++, ttime2);
			if (driver_flag) {
				diff_file.Put_Field (fld++, links2);
			}
			stops2 -= stops;
			runs2 -= runs;
			ttime2 -= ttime;
			links2 -= links;

			diff_file.Put_Field (fld++, stops2);
			diff_file.Put_Field (fld++, runs2);
			diff_file.Put_Field (fld++, ttime2);
			if (driver_flag) {
				diff_file.Put_Field (fld++, links2);
			}
			diff_file.Write ();
		}
	}

	//---- check for new compare routes ----

	for (map2_itr = compare_line_map.begin (); map2_itr != compare_line_map.end (); map2_itr++) {
		Show_Progress ();

		map_itr = line_map.find (map2_itr->first);
		if (map_itr == line_map.end ()) {
			line2_ptr = &compare_line_array [map2_itr->second];

			stops2 = (int) line2_ptr->size ();
			links2 = (int) line2_ptr->driver_array.size ();
			runs2 = ttime2 = 0;

			//---- summarize the compare data ----

			first_stop = &line2_ptr->front ();
			last_stop = &line2_ptr->back ();

			for (first_run_itr = first_stop->begin (), last_run_itr = last_stop->begin ();
				first_run_itr != first_stop->end () && last_run_itr != last_stop->end (); 
				first_run_itr++, last_run_itr++) {

				ttime2 += (last_run_itr->Schedule () - first_run_itr->Schedule ());
				runs2++;
			}
			if (runs2 > 0) ttime2 /= runs2;
			ttime2 = Resolve (ttime2);

			fld = 1;
			diff_file.Put_Field (fld++, line2_ptr->Route ());
			diff_file.Put_Field (fld++, 0);
			diff_file.Put_Field (fld++, 0);
			diff_file.Put_Field (fld++, 0);
			if (driver_flag) {
				diff_file.Put_Field (fld++, 0);
			}
			diff_file.Put_Field (fld++, stops2);
			diff_file.Put_Field (fld++, runs2);
			diff_file.Put_Field (fld++, ttime2);
			if (driver_flag) {
				diff_file.Put_Field (fld++, links2);
			}
			diff_file.Put_Field (fld++, stops2);
			diff_file.Put_Field (fld++, runs2);
			diff_file.Put_Field (fld++, ttime2);
			if (driver_flag) {
				diff_file.Put_Field (fld++, links2);
			}
			diff_file.Write ();
		}
	}
	End_Progress ();
}

//---------------------------------------------------------
//	First_Header
//---------------------------------------------------------

void TransitDiff::First_Header (void)
{
	Print (1, "First Report");
	Print (2, String ("%6cNode   X-Coordinate   Y-Coordinate") % BLANK);
	Print (1);
}

/*********************************************|***********************************************

	First Report

	      Node   X-Coordinate   Y-Coordinate

	xxxxxxxxxx xxxxxxxxxxxx.x xxxxxxxxxxxx.x
    
**********************************************|***********************************************/ 
