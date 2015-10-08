//*********************************************************
//	Draw_Route - draw a transit route file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Draw_Route
//---------------------------------------------------------

void ArcNet::Draw_Route (void)
{
	int i, j, num_legs, stop, stops, link_dir, dir_index, dir, overlap, runs;
	int route_field, stops_field, mode_field, type_field, name_field, notes_field;
	double offset, length, stop_off, link_len, side, side_offset;
	bool offset_flag;

	Db_Base *file;
	
	file = System_File_Base (TRANSIT_ROUTE);

	route_field = file->Required_Field (ROUTE_FIELD_NAMES);
	stops_field = file->Required_Field ("STOPS", "NSTOPS", "NUM_STOPS");
	mode_field = file->Required_Field (MODE_FIELD_NAMES);
	type_field = file->Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	name_field = file->Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	notes_field = file->Optional_Field ("NOTES");

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Itr link_itr;
	Line_Itr line_itr;
	XYZ_Point point;
	Int_Map_Itr map_itr;
	Points_Itr pt_itr;
	Point_Map_Itr stop_pt_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Stop_Data *stop_ptr;

	Show_Message ("Draw Transit Route -- Record");
	Set_Progress ();

	offset_flag = (route_offset != 0.0);
	side = 0.0;
	stop = 1;

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			link_itr->Aoffset (0);
			link_itr->Boffset (0);
		}
	}

	//---- process each transit route ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		stops = (int) line_itr->size ();

		arcview_route.Put_Field (route_field, line_itr->Route ());
		arcview_route.Put_Field (mode_field, line_itr->Mode ());
		arcview_route.Put_Field (type_field, line_itr->Type ());
		arcview_route.Put_Field (stops_field, stops);
		arcview_route.Put_Field (name_field, line_itr->Name ());
		arcview_route.Put_Field (notes_field, line_itr->Notes ());

		stop_itr = line_itr->begin ();

		arcview_route.Put_Field ("NUM_RUNS", (int) stop_itr->size ());

		//---- save the number of runs in each period ----

		if (schedule_flag) {
			for (i=1; i <= sched_breaks.Num_Ranges (); i++) {
				runs = 0;

				for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
					if (sched_breaks.In_Index (Resolve (run_itr->Schedule ())) == i) {
						runs++;
					}
				}
				arcview_route.Put_Field (run_field + i, runs);
			}
		}

		//---- create the route shape ----

		arcview_route.clear ();

		if (driver_flag) {

			num_legs = (int) line_itr->driver_array.size ();

			arcview_route.Put_Field ("NUM_LINKS", num_legs);

			//---- find the end index ----

			stop_itr = --(line_itr->end ());

			stop_ptr = &stop_array [stop_itr->Stop ()];

			map_itr = dir_map.find (stop_ptr->Link_Dir ());
			dir_index = map_itr->second;

			for (i=num_legs - 1; i >= 0; i--) {
				if (dir_index == line_itr->driver_array [i]) break;
			}
			if (i < 0) goto path_error;

			num_legs = i;

			//---- find the start index ----

			stop_itr = line_itr->begin ();

			stop_ptr = &stop_array [stop_itr->Stop ()];

			map_itr = dir_map.find (stop_ptr->Link_Dir ());
			dir_index = map_itr->second;

			for (i=0; i <= num_legs; i++) {
				if (dir_index == line_itr->driver_array [i]) break;
			}
			if (i > num_legs) goto path_error;

			//---- create transit legs ----

			for (j=1; i <= num_legs; i++) {
				dir_index = line_itr->driver_array [i];

				dir_ptr = &dir_array [dir_index];

				dir = dir_ptr->Dir ();
				link_dir = dir_ptr->Link_Dir ();

				link_ptr = &link_array [dir_ptr->Link ()];
				
				link_len = UnRound (link_ptr->Length ());
				side_offset = route_offset;

				if (offset_flag) {
					if (!overlap_flag) {
						if (dir == 0) {
							overlap = link_ptr->Aoffset () + 1;
							link_ptr->Aoffset (overlap);
						} else {
							overlap = link_ptr->Boffset () + 1;
							link_ptr->Boffset (overlap);
						}
						side = side_offset *= overlap;
					} else if (link_ptr->AB_Dir () >= 0 && link_ptr->BA_Dir () >= 0) {
						side = side_offset;
					} else {
						side = 0.0;
					}
				}
				if (side_offset > link_len / 3.0) {
					side_offset = link_len / 3.0;
				}
				offset = side_offset;

				//---- check for stops ----

				while (stop_ptr->Link_Dir () == link_dir) {

					stop_off = UnRound (stop_ptr->Offset ());
					length = stop_off - offset;

					Link_Shape (link_ptr, dir, points, offset, length, side);

					offset = stop_off;

					if (j != 1) {

						//---- draw the link to the stop ----

						pt_itr = points.begin ();

						if (side == 0.0 && pt_itr != points.end ()) {
							pt_itr++;
						}
						for (; pt_itr != points.end (); pt_itr++) {
							arcview_route.push_back (*pt_itr);
						}
						pt_itr = --points.end ();
					} else {
						pt_itr = points.begin ();
					}
					stop_pt_itr = stop_pt.find (stop_ptr->Stop ());

					if (stop_pt_itr != stop_pt.end ()) {
						arcview_route.push_back (stop_pt_itr->second);
					}

					//---- find the next stop -----

					if (++j > stops) break;

					arcview_route.push_back (*pt_itr);

					stop_itr++;

					stop = stop_itr->Stop ();

					stop_ptr = &stop_array [stop];
				}

				//---- complete the link ----

				if (i < num_legs) {
					length = link_len - offset - side_offset;

					Link_Shape (link_ptr, dir, points, offset, length, side);

					pt_itr = points.begin ();

					if (!offset_flag) {
						pt_itr++;
					}
					for (; pt_itr != points.end (); pt_itr++) {
						arcview_route.push_back (*pt_itr);
					}
				}
			}

		} else {

			for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
				stop_ptr = &stop_array [stop_itr->Stop ()];

				stop_pt_itr = stop_pt.find (stop_ptr->Stop ());
				if (stop_pt_itr == stop_pt.end ()) goto stop_error;

				arcview_route.push_back (stop_pt_itr->second);
			}
		}

		if (!arcview_route.Write_Record ()) {
			Error (String ("Writing %s") % arcview_route.File_Type ());
		}
	}
	End_Progress ();

	arcview_route.Close ();
	return;

stop_error:
	Error (String ("Route %d Stop %d was Not Found in the Stop File") % line_itr->Route () % stop);
	
path_error:
	Error (String ("Route %d Stop %d was Not Found on Driver Path") % line_itr->Route () % stop_ptr->Stop ());

}
