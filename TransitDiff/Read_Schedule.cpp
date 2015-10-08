//*********************************************************
//	Read_Schedule.cpp - read the compare transit schedule file
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Read_Schedule
//---------------------------------------------------------

void TransitDiff::Read_Schedule (void)
{
	int i, j, nrun, nstop, index, npoints, num, stops, count, route, stop;
	Dtime time, tim, t;
	bool keep_flag;

	Stop_Time stop_rec;
	Stop_Time_Itr stop_itr;
	Schedule_Data sched_rec;
	Line_Data *line_ptr = 0;
	Line_Stop *stop_ptr = 0;
	Line_Stop *prev_ptr = 0;
	Line_Run run_rec, *run_ptr = 0;
	Int_Map_Itr map_itr;

	//---- store the transit schedule data ----

	Show_Message (String ("Reading %s -- Record") % compare_schedule_file.File_Type ());
	Set_Progress ();

	count = npoints = stops = 0;
	nrun = nstop = 1;

	while (compare_schedule_file.Read (false)) {
		Show_Progress ();

		//---- new file format processing ----

		sched_rec.Clear ();
		keep_flag = true;

		route = compare_schedule_file.Route ();
		if (route == 0) keep_flag = false;

		map_itr = compare_line_map.find (route);
		if (map_itr == compare_line_map.end ()) {
			Warning (String ("Schedule Route %d was Not Found") % route);
			keep_flag = false;
		}
		sched_rec.Route (map_itr->second);

		//---- process the run numbers ----

		for (i=1; i <= NUM_SCHEDULE_COLUMNS; i++) {
			sched_rec.Run (i, compare_schedule_file.Run (i));
		}

		num = compare_schedule_file.Num_Nest ();

		sched_rec.clear ();
		if (num > 0) sched_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!compare_schedule_file.Read (true)) {
				Error (String ("Number of Link Records for Route %d") % compare_schedule_file.Route ());
			}
			Show_Progress ();

			//---- convert the stop id ----

			stop = compare_schedule_file.Stop ();

			map_itr = compare_stop_map.find (stop);
			if (map_itr == compare_stop_map.end ()) {
				Warning (String ("Schedule Stop %d on Route %d was Not Found") % stop % compare_schedule_file.Route ());
				continue;
			}
			stop_rec.Stop (map_itr->second);

			for (j=1; j <= NUM_SCHEDULE_COLUMNS; j++) {
				stop_rec.Time (j, compare_schedule_file.Time (j));
			}
			sched_rec.push_back (stop_rec);
		}

		//---- write the schedule data to line data ----

		if (keep_flag) {
			line_ptr = &compare_line_array [sched_rec.Route ()];

			stops = (int) line_ptr->size ();

			for (j=1; j <= NUM_SCHEDULE_COLUMNS; j++) {
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
							line_ptr->Route () % compare_stop_array [stop_itr->Stop ()].Stop ());
						break;
					}

					//---- interpolate time points ----

					if (i > nstop) {
						if (nstop == 1) {
							Warning (String ("Schedule Stop %d is Not the First Stop on Route %d") % 
								compare_stop_array [stop_itr->Stop ()].Stop () % line_ptr->Route ());
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
	compare_schedule_file.Close ();

	compare_line_array.Schedule_Records (Progress_Count ());

	Print (2, String ("Number of %s Records = %d") % compare_schedule_file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % compare_schedule_file.File_ID () % count);
	}
	if (npoints > 0) {
		Print (1, String ("Number of Interpolated Time Points = %d") % npoints);
	}
	return;
}
