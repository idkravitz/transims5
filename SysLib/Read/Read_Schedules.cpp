//*********************************************************
//	Read_Schedules.cpp - read the transit schedule file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Schedules
//---------------------------------------------------------

void Data_Service::Read_Schedules (void)
{
	int i, j, nrun, nstop, index, npoints, num, stops, count;
	int route, stop, last_route, first_stop;
	Dtime time, tim, t;
	bool keep_flag;

	Stop_Time_Itr stop_itr;
	Schedule_Data sched_rec;
	Line_Data *line_ptr = 0;
	Line_Stop *stop_ptr = 0;
	Line_Stop *prev_ptr = 0;
	Line_Run run_rec, *run_ptr = 0;
	Int_Map_Itr map_itr;

	Schedule_File *file = (Schedule_File *) System_File_Handle (TRANSIT_SCHEDULE);

	//---- store the transit schedule data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Schedules (*file);
	count = npoints = stops = 0;
	last_route = first_stop = -1;
	nrun = nstop = 1;

	while (file->Read (false)) {
		Show_Progress ();

		//---- process the old file format ----

		if (file->Version () <= 40) {

			route = file->Route ();
			if (route == 0) continue;

			//---- check the stop id ----

			stop = file->Stop ();

			map_itr = stop_map.find (stop);
			if (map_itr == stop_map.end ()) {
				Warning (String ("Schedule Stop %d on Route %d was Not Found") % stop % file->Route ());
				continue;
			}
			stop = map_itr->second;

			time = file->Time (0);

			//---- check for a new route ----

			if (route != last_route) {
				first_stop = stop;
				last_route = route;
				nrun = nstop = i = 1;

				map_itr = line_map.find (route);
				if (map_itr == line_map.end ()) {
					Warning (String ("Transit Route %d was Not Found") % route);
					continue;
				}
				line_ptr = &line_array [map_itr->second];

				stops = (int) line_ptr->size ();

				stop_ptr = &(line_ptr->at (0));

			} else {

				//---- find the stop id on the route ----

				for (i=nstop; i <= stops; i++) {
					stop_ptr = &(line_ptr->at (i - 1));

					if (stop_ptr->Stop () == stop) break;
				}
				if (i > stops) {
					if (stop == first_stop) nrun++;
					nstop = i = 1;

					stop_ptr = &(line_ptr->at (i - 1));
				}
			}

			//---- interpolate time points ----

			if (i > nstop) {
				if (nstop == 1) {
					Warning (String ("Schedule Stop %d is Not the First Stop on Route %d") % 
						file->Stop () % route);
					continue;
				}
				index = i;

				prev_ptr = &(line_ptr->at (nstop - 2));

				run_ptr = &(prev_ptr->at (nrun - 1));
				tim = run_ptr->Schedule ();

				t = (time - tim) / (index - nstop + 1);

				for (i=nstop; i < index; i++) {
					tim += t;
					run_rec.Schedule (tim);
					prev_ptr = &(line_ptr->at (i - 1));
					prev_ptr->push_back (run_rec);
					npoints++;
				}
				nstop = index;
			}

			//---- add the time point ----

			run_rec.Schedule (time);
			stop_ptr->push_back (run_rec);
			count++;
			nstop++;
			continue;
		}

		//---- new file format processing ----

		sched_rec.Clear ();

		keep_flag = Get_Schedule_Data (*file, sched_rec);

		num = file->Num_Nest ();

		sched_rec.clear ();
		if (num > 0) sched_rec.reserve (num);

		for (i=0; i < num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Link Records for Route %d") % file->Route ());
			}
			Show_Progress ();

			Get_Schedule_Data (*file, sched_rec);
		}

		//---- write the schedule data to line data ----

		if (keep_flag) {
			line_ptr = &line_array [sched_rec.Route ()];

			stops = (int) line_ptr->size ();

			for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
				if (sched_rec.Run (j) == 0) break;

				stop_ptr = &(line_ptr->at (0));
				nrun = (int) stop_ptr->size ();

				if (sched_rec.Run (j) != nrun + 1) {
					Warning (String ("Schedule Route %d Run Number is Out of Order (%d vs %d)") %
						line_ptr->Route () % sched_rec.Run (j) % (nrun + 1));
					break;
				}
				nstop = 1;

				for (stop_itr = sched_rec.begin (); stop_itr != sched_rec.end (); stop_itr++) {
					time = stop_itr->Time (j);

					//---- find the stop id on the route ----

					for (i=nstop; i <= stops; i++) {
						stop_ptr = &(line_ptr->at (i - 1));

						if (stop_ptr->Stop () == stop_itr->Stop ()) break;
					}
					if (i > stops) {
						Warning (String ("Schedule Route %d Stop %d was Not Found") % 
							line_ptr->Route () % stop_array [stop_itr->Stop ()].Stop ());
						break;
					}

					//---- interpolate time points ----

					if (i > nstop) {
						if (nstop == 1) {
							Warning (String ("Schedule Stop %d is Not the First Stop on Route %d") % 
								stop_array [stop_itr->Stop ()].Stop () % line_ptr->Route ());
							break;
						}
						index = i;

						prev_ptr = &(line_ptr->at (nstop - 2));

						run_ptr = &(prev_ptr->at (nrun - 1));
						tim = run_ptr->Schedule ();

						t = (time - tim) / (index - nstop + 1);

						for (i=nstop; i < index; i++) {
							tim += t;
							run_rec.Schedule (tim);

							prev_ptr = &(line_ptr->at (i - 1));
							prev_ptr->push_back (run_rec);
							npoints++;
						}
						nstop = index;
					}

					//---- add the time point ----

					run_rec.Schedule (time);
					stop_ptr->push_back (run_rec);
					count++;
					nstop++;
				}
			}
		}
	}
	End_Progress ();
	file->Close ();

	line_array.Schedule_Records (Progress_Count ());

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % count);
	}
	if (npoints > 0) {
		Print (1, String ("Number of Interpolated Time Points = %d") % npoints);
	}
	if (count > 0) System_Data_True (TRANSIT_SCHEDULE);
	return;
}

//---------------------------------------------------------
//	Initialize_Schedules
//---------------------------------------------------------

void Data_Service::Initialize_Schedules (Schedule_File &file)
{
	Required_File_Check (file, TRANSIT_ROUTE);
	Required_File_Check (file, TRANSIT_STOP);
}

//---------------------------------------------------------
//	Get_Schedule_Data
//---------------------------------------------------------

bool Data_Service::Get_Schedule_Data (Schedule_File &file, Schedule_Data &sched_rec)
{
	int i, route, stop;

	Stop_Time stop_rec;
	Int_Map_Itr map_itr;

	//---- process a header line ----

	if (!file.Nested ()) {

		//--- convert the route ID;

		route = file.Route ();
		if (route == 0) return (false);

		map_itr = line_map.find (route);
		if (map_itr == line_map.end ()) {
			Warning (String ("Schedule Route %d was Not Found") % route);
			return (false);
		}
		sched_rec.Route (map_itr->second);

		//---- process the run numbers ----

		for (i=0; i < NUM_SCHEDULE_COLUMNS; i++) {
			sched_rec.Run (i, file.Run (i));
		}
		return (true);
	}

	//---- convert the stop id ----

	stop = file.Stop ();

	map_itr = stop_map.find (stop);
	if (map_itr == stop_map.end ()) {
		Warning (String ("Schedule Stop %d on Route %d was Not Found") % stop % file.Route ());
		return (false);
	}
	stop_rec.Stop (map_itr->second);

	for (i=0; i < NUM_SCHEDULE_COLUMNS; i++) {
		stop_rec.Time (i, file.Time (i));
	}
	sched_rec.push_back (stop_rec);
	return (true);
}
