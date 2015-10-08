//*********************************************************
//	Line_Report.cpp - Create a Line Ridership Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Line_Report
//---------------------------------------------------------

void LineSum::Line_Report (Line_Report_Data &report)
{
	int i, anode, bnode, dir, dir1, dir2, stop, mode;
	int ab_pk_tot, ab_op_tot, ba_pk_tot, ba_op_tot;
	int dist_tot, time_tot, dist_max, time_max;
	int ab_pk_pmt, ab_op_pmt, ba_pk_pmt, ba_op_pmt;
	int ab_pk_pht, ab_op_pht, ba_pk_pht, ba_op_pht;
	bool found, all_flag;

	Ride_Data ab, ba, ab_pk, ab_op, ba_pk, ba_op, ab_max, ba_max, tot_max, tot;
	Leg_Data *leg_ptr;
	Line_Map_Itr map_itr, first_itr;
	Str_Itr str_itr;
	Str_Map_Itr name_itr;
	String name;
	Range_Array_Itr range_itr;
		
	Line_Data line_rec, *line_ptr;
	Line_Array line_array;
	Line_Itr line_itr;
	
	Show_Message (String ("Creating Line Report #%d -- Lines") % report.number);
	Set_Progress ();

	//---- report header ----

	New_Page ();
	Print (2, "Title: ") << report.title;
	Print (1, "Lines:");

	for (i=0, str_itr = report.lines.begin (); str_itr != report.lines.end (); str_itr++, i++) {
		if (!(i % 10)) {
			if (i) {
				Print (1, "       ") << *str_itr;
			} else {
				Print (0, " ") << *str_itr;
			}
		} else {
			Print (0, ", ") << *str_itr;
		}
	}
	if (!report.all_modes) {
		Print (1, "Modes:");
		for (i=0, range_itr = report.modes.begin (); range_itr != report.modes.end (); range_itr++, i++) {
			if (!(i % 10)) {
				if (i) {
					Print (1, "       ") << range_itr->Low ();
				} else {
					Print (0, " ") << range_itr->Low ();
				}
			} else {
				Print (0, ", ") << range_itr->Low ();
			}
			if (range_itr->High () > range_itr->Low ()) {
				Print (0, "..") << range_itr->High ();
			}
		}
	}
	Header_Number (LINE_REPORT);
	Line_Header ();

	//---- gather data for each line ----

	for (str_itr = report.lines.begin (); str_itr != report.lines.end (); str_itr++) {
		Show_Progress ();

		//---- peak period data ----

		all_flag = str_itr->Equals ("ALL");
		found = false;

		for (map_itr = peak_map.begin (); map_itr != peak_map.end (); map_itr++) {
			mode = map_itr->first.mode;

			if (!report.all_modes && !report.modes.In_Range (mode)) continue;

			name = map_itr->first.name;

			if (!all_flag && !name.In_Range (*str_itr)) {
				if (found) break;
				continue;
			}
			found = true;

			//---- add the first set of legs ----

			if (line_array.size () == 0) {
				for (; map_itr != peak_map.end (); map_itr++) {
					if (!name.Equals (map_itr->first.name)) break;

					memset (&line_rec, '\0', sizeof (line_rec));

					leg_ptr = &map_itr->second;

					line_rec.a = leg_ptr->a;
					line_rec.b = leg_ptr->b;

					line_rec.time = leg_ptr->time;
					line_rec.dist = leg_ptr->dist;

					line_rec.ab.pk.on = leg_ptr->ab.on;
					line_rec.ab.pk.off = leg_ptr->ab.off;
					line_rec.ab.pk.ride = leg_ptr->ab.ride;

					line_rec.ba.pk.on = leg_ptr->ba.on;
					line_rec.ba.pk.off = leg_ptr->ba.off;
					line_rec.ba.pk.ride = leg_ptr->ba.ride;
					line_array.push_back (line_rec);
				}
				if (map_itr == peak_map.end ()) break;
				continue;
			}

			//---- determine the line orientation ----

			dir1 = dir2 = 0;

			for (first_itr = map_itr; map_itr != peak_map.end (); map_itr++) {
				if (!name.Equals (map_itr->first.name)) break;

				leg_ptr = &map_itr->second;

                anode = abs (leg_ptr->a);
				bnode = abs (leg_ptr->b);

				for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
					if (anode == abs (line_itr->a) && bnode == abs (line_itr->b)) {
						dir1++;
					} else if (anode == abs (line_itr->b) && bnode == abs (line_itr->a)) {
						dir2++;
					}
				}
			}
			dir = (dir1 >= dir2) ? 1 : 2;

			map_itr = first_itr;
			leg_ptr = &map_itr->second;
 
			//---- search for the link ----

			if (dir == 1) {
				anode = leg_ptr->a;
				bnode = leg_ptr->b;
			} else {
				anode = leg_ptr->b;
				bnode = leg_ptr->a;
			}
			for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
				if (abs (anode) == abs (line_itr->a) && abs (bnode) == abs (line_itr->b)) break;
			}

			//---- store the data ----

			if (line_itr == line_array.end ()) {
				for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
					if (abs (bnode) == abs (line_itr->a) && abs (anode) == abs (line_itr->b)) break;
				}
				if (line_itr == line_array.end ()) {
					memset (&line_rec, '\0', sizeof (line_rec));

					if (dir == 1) {
						line_rec.a = anode;
						line_rec.b = bnode;
					} else {
						line_rec.a = bnode;
						line_rec.b = anode;
					}
					line_rec.time = leg_ptr->time;
					line_rec.dist = leg_ptr->dist;

					line_rec.ab.pk = leg_ptr->ab;
					line_rec.ba.pk = leg_ptr->ba;

					line_array.push_back (line_rec);

				} else {
					if (bnode > line_itr->a) line_itr->a = bnode;
					if (anode > line_itr->b) line_itr->b = anode;
					if (line_itr->time == 0) line_itr->time = leg_ptr->time;
					if (line_itr->dist == 0) line_itr->dist = leg_ptr->dist;

					if (dir == 2) {
						line_itr->ab.pk.on += leg_ptr->ab.on;
						line_itr->ab.pk.off += leg_ptr->ab.off;
						line_itr->ab.pk.ride += leg_ptr->ab.ride;

						line_itr->ba.pk.on += leg_ptr->ba.on;
						line_itr->ba.pk.off += leg_ptr->ba.off;
						line_itr->ba.pk.ride += leg_ptr->ba.ride;
					} else {
						line_itr->ab.pk.on += leg_ptr->ba.on;
						line_itr->ab.pk.off += leg_ptr->ba.off;
						line_itr->ab.pk.ride += leg_ptr->ba.ride;

						line_itr->ba.pk.on += leg_ptr->ab.on;
						line_itr->ba.pk.off += leg_ptr->ab.off;
						line_itr->ba.pk.ride += leg_ptr->ab.ride;
					}
				}
			} else {
				if (anode > line_itr->a) line_itr->a = anode;
				if (bnode > line_itr->b) line_itr->b = bnode;
				if (line_itr->time == 0) line_itr->time = leg_ptr->time;
				if (line_itr->dist == 0) line_itr->dist = leg_ptr->dist;

				if (dir == 1) {
					line_itr->ab.pk.on += leg_ptr->ab.on;
					line_itr->ab.pk.off += leg_ptr->ab.off;
					line_itr->ab.pk.ride += leg_ptr->ab.ride;

					line_itr->ba.pk.on += leg_ptr->ba.on;
					line_itr->ba.pk.off += leg_ptr->ba.off;
					line_itr->ba.pk.ride += leg_ptr->ba.ride;
				} else {
					line_itr->ab.pk.on += leg_ptr->ba.on;
					line_itr->ab.pk.off += leg_ptr->ba.off;
					line_itr->ab.pk.ride += leg_ptr->ba.ride;

					line_itr->ba.pk.on += leg_ptr->ab.on;
					line_itr->ba.pk.off += leg_ptr->ab.off;
					line_itr->ba.pk.ride += leg_ptr->ab.ride;
				}
			}
		}

		//---- offpeak data ----

		found = false;

		for (map_itr = offpeak_map.begin (); map_itr != offpeak_map.end (); map_itr++) {
			mode = map_itr->first.mode;

			if (!report.all_modes && !report.modes.In_Range (mode)) continue;

			name = map_itr->first.name;

			if (!all_flag && !name.In_Range (*str_itr)) {
				if (found) break;
				continue;
			}
			found = true;

			//---- add the first set of legs ----

			if (line_array.size () == 0) {
				for (; map_itr != offpeak_map.end (); map_itr++) {
					if (!name.Equals (map_itr->first.name)) break;

					memset (&line_rec, '\0', sizeof (line_rec));

					leg_ptr = &map_itr->second;

					line_rec.a = leg_ptr->a;
					line_rec.b = leg_ptr->b;

					line_rec.time = leg_ptr->time;
					line_rec.dist = leg_ptr->dist;

					line_rec.ab.op = leg_ptr->ab;
					line_rec.ba.op = leg_ptr->ba;

					line_array.push_back (line_rec);
				}
				if (map_itr == offpeak_map.end ()) break;
				continue;
			}

			//---- determine the line orientation ----

			dir1 = dir2 = 0;

			for (first_itr = map_itr; map_itr != offpeak_map.end (); map_itr++) {
				if (!name.Equals (map_itr->first.name)) break;

				leg_ptr = &map_itr->second;

                anode = abs (leg_ptr->a);
				bnode = abs (leg_ptr->b);

				for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
					if (anode == abs (line_itr->a) && bnode == abs (line_itr->b)) {
						dir1++;
					} else if (anode == abs (line_itr->b) && bnode == abs (line_itr->a)) {
						dir2++;
					}
				}
			}
			dir = (dir1 >= dir2) ? 1 : 2;

			map_itr = first_itr;
			leg_ptr = &map_itr->second;
 
			//---- search for the link ----

			if (dir == 1) {
				anode = leg_ptr->a;
				bnode = leg_ptr->b;
			} else {
				anode = leg_ptr->b;
				bnode = leg_ptr->a;
			}
			for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
				if (abs (anode) == abs (line_itr->a) && abs (bnode) == abs (line_itr->b)) break;
			}

			//---- store the data ----

			if (line_itr == line_array.end ()) {
				for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
					if (abs (bnode) == abs (line_itr->a) && abs (anode) == abs (line_itr->b)) break;
				}
				if (line_itr == line_array.end ()) {
					memset (&line_rec, '\0', sizeof (line_rec));

					if (dir == 1) {
						line_rec.a = anode;
						line_rec.b = bnode;
					} else {
						line_rec.a = bnode;
						line_rec.b = anode;
					}
					line_rec.time = leg_ptr->time;
					line_rec.dist = leg_ptr->dist;

					line_rec.ab.op = leg_ptr->ab;
					line_rec.ba.op = leg_ptr->ba;

					line_array.push_back (line_rec);

				} else {
					if (bnode > line_itr->a) line_itr->a = bnode;
					if (anode > line_itr->b) line_itr->b = anode;
					if (line_itr->time == 0) line_itr->time = leg_ptr->time;
					if (line_itr->dist == 0) line_itr->dist = leg_ptr->dist;

					if (dir == 2) {
						line_itr->ab.op.on += leg_ptr->ab.on;
						line_itr->ab.op.off += leg_ptr->ab.off;
						line_itr->ab.op.ride += leg_ptr->ab.ride;

						line_itr->ba.op.on += leg_ptr->ba.on;
						line_itr->ba.op.off += leg_ptr->ba.off;
						line_itr->ba.op.ride += leg_ptr->ba.ride;
					} else {
						line_itr->ab.op.on += leg_ptr->ba.on;
						line_itr->ab.op.off += leg_ptr->ba.off;
						line_itr->ab.op.ride += leg_ptr->ba.ride;

						line_itr->ba.op.on += leg_ptr->ab.on;
						line_itr->ba.op.off += leg_ptr->ab.off;
						line_itr->ba.op.ride += leg_ptr->ab.ride;
					}
				}
			} else {
				if (anode > line_itr->a) line_itr->a = anode;
				if (bnode > line_itr->b) line_itr->b = bnode;
				if (line_itr->time == 0) line_itr->time = leg_ptr->time;
				if (line_itr->dist == 0) line_itr->dist = leg_ptr->dist;

				if (dir == 1) {
					line_itr->ab.op.on += leg_ptr->ab.on;
					line_itr->ab.op.off += leg_ptr->ab.off;
					line_itr->ab.op.ride += leg_ptr->ab.ride;

					line_itr->ba.op.on += leg_ptr->ba.on;
					line_itr->ba.op.off += leg_ptr->ba.off;
					line_itr->ba.op.ride += leg_ptr->ba.ride;
				} else {
					line_itr->ab.op.on += leg_ptr->ba.on;
					line_itr->ab.op.off += leg_ptr->ba.off;
					line_itr->ab.op.ride += leg_ptr->ba.ride;

					line_itr->ba.op.on += leg_ptr->ab.on;
					line_itr->ba.op.off += leg_ptr->ab.off;
					line_itr->ba.op.ride += leg_ptr->ab.ride;
				}
			}
		}
	}

	//---- exit if no data ----

	if (line_array.size () == 0) {
		Warning ("No Stops to Report");
		return;
	}

	//---- begin branches with a stop ----

	for (line_ptr = 0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		if (line_itr->a < 0) {
			if (line_ptr == 0 || abs (line_ptr->b) != -line_itr->a || line_ptr->b == -line_itr->a) {
				line_itr->a = -line_itr->a;
			}
		}
		line_ptr = &(*line_itr);
	}

	//---- combine distance and time for non-stop nodes ----

	if (!report.node_flag) {
		for (line_ptr = 0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			if (line_itr->a < 0) {
				if (line_ptr != 0) {
					line_ptr->dist = (short) (line_ptr->dist + line_itr->dist);
					line_ptr->time = (short) (line_ptr->time + line_itr->time);
				}
			} else {
				line_ptr = &(*line_itr);
			}
		}
	}

	//---- print the report ----

	ab_pk_tot = ab_op_tot = ba_pk_tot = ba_op_tot = 0;
	dist_tot = time_tot = dist_max = time_max = 0;
	ab_pk_pmt = ab_op_pmt = ba_pk_pmt = ba_op_pmt = 0;
	ab_pk_pht = ab_op_pht = ba_pk_pht = ba_op_pht = 0;

	memset (&ab_pk, '\0', sizeof (ab_pk));
	memset (&ab_op, '\0', sizeof (ab_op));
	memset (&ba_pk, '\0', sizeof (ba_pk));
	memset (&ba_op, '\0', sizeof (ba_op));
	memset (&ab_max, '\0', sizeof (ab_max));
	memset (&ba_max, '\0', sizeof (ba_max));
	memset (&tot_max, '\0', sizeof (tot_max));

	memset (&line_rec, '\0', sizeof (line_rec));
	line_ptr = &line_rec;

	for (line_itr = line_array.begin (); ; line_ptr = &(*line_itr), line_itr++) {

		//---- check for the end of the segment ----

		if (line_itr == line_array.end () || (line_ptr->b != 0 && abs (line_ptr->b) != abs (line_itr->a))) {
			ab.off = line_ptr->ab.pk.off + line_ptr->ab.op.off;
			ba.on = line_ptr->ba.pk.on + line_ptr->ba.op.on;
			ba_pk_tot += line_ptr->ba.pk.on;
			ba_op_tot += line_ptr->ba.op.on;

			if (ab.off > ab_max.off) ab_max.off = ab.off;
			if (ba.on > ba_max.on) ba_max.on = ba.on;

			if (line_ptr->ab.pk.off > ab_pk.off) ab_pk.off = line_ptr->ab.pk.off;
			if (line_ptr->ab.op.off > ab_op.off) ab_op.off = line_ptr->ab.op.off;
			if (line_ptr->ba.pk.on > ba_pk.on) ba_pk.on = line_ptr->ba.pk.on;
			if (line_ptr->ba.op.on > ba_op.on) ba_op.on = line_ptr->ba.op.on;

			stop = abs (line_ptr->b);
			name_itr = stop_names.find (stop);

			if (name_itr == stop_names.end ()) {
				Print (1, String ("%-10d") % stop);
			} else {
				Print (1, String ("%-10.10s") % name_itr->second);
			}
			Print (0, String ("%c%21c%6d%14c%6d%14c%6d%8c%6d%14c%6d%14c%6d%14c%6d%6d") %
				((line_ptr->b < 0) ? '-' : ' ') % BLANK % line_ptr->ab.pk.off % BLANK % 
				line_ptr->ab.op.off % BLANK % ab.off % BLANK % line_ptr->ba.pk.on % BLANK % 
				line_ptr->ba.op.on % BLANK % ba.on % BLANK % ba.on % ab.off);

			line_ptr = &line_rec;
		}
		if (line_itr == line_array.end ()) break;

		if (!report.node_flag && line_itr->a < 0) continue; 

		ab.off = line_ptr->ab.pk.off + line_ptr->ab.op.off;
		ab.on = line_itr->ab.pk.on + line_itr->ab.op.on;
		ab.ride = line_itr->ab.pk.ride + line_itr->ab.op.ride;

		ba.off = line_itr->ba.pk.off + line_itr->ba.op.off;
		ba.on = line_ptr->ba.pk.on + line_ptr->ba.op.on;
		ba.ride = line_itr->ba.pk.ride + line_itr->ba.op.ride;

		tot.off = ab.off + ba.off;
		tot.on = ab.on + ba.on;
		tot.ride = ab.ride + ba.ride;

		stop = abs (line_itr->a);
		name_itr = stop_names.find (stop);

		if (name_itr == stop_names.end ()) {
			Print (1, String ("%-10d") % stop);
		} else {
			Print (1, String ("%-10.10s") % name_itr->second);
		}
		Print (0, String ("%c%7.2f%7.2f %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d") %
			((line_itr->a < 0) ? '-' : ' ') % (line_itr->dist / 100.0) % (line_itr->time / 100.0) % 
			line_itr->ab.pk.on % line_ptr->ab.pk.off % line_itr->ab.pk.ride % 
			line_itr->ab.op.on % line_ptr->ab.op.off % line_itr->ab.op.ride % 
			ab.on % ab.off % ab.ride %
			line_ptr->ba.pk.on % line_itr->ba.pk.off % line_itr->ba.pk.ride % 
			line_ptr->ba.op.on % line_itr->ba.op.off % line_itr->ba.op.ride % 
			ba.on % ba.off % ba.ride %
			tot.on % tot.off % tot.ride);

		//---- calculate totals ----

		dist_tot += line_itr->dist;
		time_tot += line_itr->time;

		ab_pk_tot += line_itr->ab.pk.on;
		ab_op_tot += line_itr->ab.op.on;
		ba_pk_tot += line_ptr->ba.pk.on;
		ba_op_tot += line_ptr->ba.op.on;

		if (line_itr->dist > dist_max) dist_max = line_itr->dist;
		if (line_itr->time > time_max) time_max = line_itr->time;

		if (line_itr->ab.pk.on > ab_pk.on) ab_pk.on = line_itr->ab.pk.on;
		if (line_ptr->ab.pk.off > ab_pk.off) ab_pk.off = line_ptr->ab.pk.off;
		if (line_itr->ab.pk.ride > ab_pk.ride) ab_pk.ride = line_itr->ab.pk.ride;

		if (line_itr->ab.op.on > ab_op.on) ab_op.on = line_itr->ab.op.on;
		if (line_ptr->ab.op.off > ab_op.off) ab_op.off = line_ptr->ab.op.off;
		if (line_itr->ab.op.ride > ab_op.ride) ab_op.ride = line_itr->ab.op.ride;

		if (line_ptr->ba.pk.on > ba_pk.on) ba_pk.on = line_ptr->ba.pk.on;
		if (line_itr->ba.pk.off > ba_pk.off) ba_pk.off = line_itr->ba.pk.off;
		if (line_itr->ba.pk.ride > ba_pk.ride) ba_pk.ride = line_itr->ba.pk.ride;

		if (line_ptr->ba.op.on > ba_op.on) ba_op.on = line_ptr->ba.op.on;
		if (line_itr->ba.op.off > ba_op.off) ba_op.off = line_itr->ba.op.off;
		if (line_itr->ba.op.ride > ba_op.ride) ba_op.ride = line_itr->ba.op.ride;

		if (ab.on > ab_max.on) ab_max.on = ab.on;
		if (ab.off > ab_max.off) ab_max.off = ab.off;
		if (ab.ride > ab_max.ride) ab_max.ride = ab.ride;

		if (ba.on > ba_max.on) ba_max.on = ba.on;
		if (ba.off > ba_max.off) ba_max.off = ba.off;
		if (ba.ride > ba_max.ride) ba_max.ride = ba.ride;

		if (tot.on > tot_max.on) tot_max.on = tot.on;
		if (tot.off > tot_max.off) tot_max.off = tot.off;
		if (tot.ride > tot_max.ride) tot_max.ride = tot.ride;

		ab_pk_pmt += line_itr->ab.pk.ride * line_itr->dist;
		ab_op_pmt += line_itr->ab.op.ride * line_itr->dist;
		ba_pk_pmt += line_itr->ba.pk.ride * line_itr->dist;
		ba_op_pmt += line_itr->ba.op.ride * line_itr->dist;

		ab_pk_pht += line_itr->ab.pk.ride * line_itr->time;
		ab_op_pht += line_itr->ab.op.ride * line_itr->time;
		ba_pk_pht += line_itr->ba.pk.ride * line_itr->time;
		ba_op_pht += line_itr->ba.op.ride * line_itr->time;
	}

	//---- print the totals ----

	ab.off = ab_pk_tot + ab_op_tot;
	ba.off = ba_pk_tot + ba_op_tot;
	tot.off = ab.off + ba.off;

	Print (2, String ("Total      %7.2f%7.2f%13d%20d%20d%20d%20d%20d%20d") %
		(dist_tot / 100.0) % (time_tot / 100.0) %
		ab_pk_tot % ab_op_tot % ab.off % ba_pk_tot % ba_op_tot % ba.off % tot.off);

	//---- maximums ----

	Print (2, String ("Max        %7.2f%7.2f %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d %6d%6d%7d") %
		(dist_max / 100.0) % (time_max / 100.0) %
		ab_pk.on % ab_pk.off % ab_pk.ride % ab_op.on % ab_op.off % ab_op.ride %
		ab_max.on % ab_max.off % ab_max.ride %
		ba_pk.on % ba_pk.off % ba_pk.ride % ba_op.on % ba_op.off % ba_op.ride %
		ba_max.on % ba_max.off % ba_max.ride %
		tot_max.on % tot_max.off % tot_max.ride);

	//---- passenger miles ----

	ab_pk.ride = (ab_pk_pmt + 50) / 100;
	ab_op.ride = (ab_op_pmt + 50) / 100;
	ba_pk.ride = (ba_pk_pmt + 50) / 100;
	ba_op.ride = (ba_op_pmt + 50) / 100;
	ab.ride = ab_pk.ride + ab_op.ride;
	ba.ride = ba_pk.ride + ba_op.ride;
	tot.ride = ab.ride + ba.ride;

	Print (2, String ("Passenger Miles%23d%20d%20d%20d%20d%20d%20d") %
		ab_pk.ride % ab_op.ride % ab.ride % ba_pk.ride % ba_op.ride % ba.ride % tot.ride);

	//---- passenger houres ----
		
	ab_pk.ride = (ab_pk_pht + 3000) / 6000;
	ab_op.ride = (ab_op_pht + 3000) / 6000;
	ba_pk.ride = (ba_pk_pht + 3000) / 6000;
	ba_op.ride = (ba_op_pht + 3000) / 6000;
	ab.ride = ab_pk.ride + ab_op.ride;
	ba.ride = ba_pk.ride + ba_op.ride;
	tot.ride = ab.ride + ba.ride;

	Print (1, String ("Passenger Hours%23d%20d%20d%20d%20d%20d%20d") %
		ab_pk.ride % ab_op.ride % ab.ride % ba_pk.ride % ba_op.ride % ba.ride % tot.ride);

	//---- average trip distance ----

	ab_pk.ride = (ab_pk_tot) ? ab_pk_pmt / ab_pk_tot : 0;
	ab_op.ride = (ab_op_tot) ? ab_op_pmt / ab_op_tot : 0;
	ba_pk.ride = (ba_pk_tot) ? ba_pk_pmt / ba_pk_tot : 0;
	ba_op.ride = (ba_op_tot) ? ba_op_pmt / ba_op_tot : 0;
	ab.ride = ab_pk.ride + ab_op.ride;
	ba.ride = ba_pk.ride + ba_op.ride;
	tot.ride = ab.ride + ba.ride;

	Print (1, String ("Average Trip Length (miles)%11.1f%20.1f%20.1f%20.1f%20.1f%20.1f%20.1f") %
		(ab_pk.ride / 100.0) % (ab_op.ride / 100.0) % (ab.ride / 100.0) %
		(ba_pk.ride / 100.0) % (ba_op.ride / 100.0) % (ba.ride / 100.0) % (tot.ride / 100.0));

	//---- average trip time ----

	ab_pk.ride = (ab_pk_tot) ? ab_pk_pht / ab_pk_tot : 0;
	ab_op.ride = (ab_op_tot) ? ab_op_pht / ab_op_tot : 0;
	ba_pk.ride = (ba_pk_tot) ? ba_pk_pht / ba_pk_tot : 0;
	ba_op.ride = (ba_op_tot) ? ba_op_pht / ba_op_tot : 0;
	ab.ride = ab_pk.ride + ab_op.ride;
	ba.ride = ba_pk.ride + ba_op.ride;
	tot.ride = ab.ride + ba.ride;

	Print (1, String ("Average Trip Length (minutes)%9.1f%20.1f%20.1f%20.1f%20.1f%20.1f%20.1f") %
		(ab_pk.ride / 100.0) % (ab_op.ride / 100.0) % (ab.ride / 100.0) %
		(ba_pk.ride / 100.0) % (ba_op.ride / 100.0) % (ba.ride / 100.0) % (tot.ride / 100.0));

	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Header
//---------------------------------------------------------

void LineSum::Line_Header (void)
{
	Print (2, "                           ---------------- A->B Direction (Read Down) --------------  --------------- B->A Direction (Read Up) -----------------  -------Total------");
	Print (1, "              Dist   Time  -------Peak-------  -----Off-Peak-----  -------Daily------  -------Peak-------  -----Off-Peak-----  -------Daily------  -------Daily------");
	Print (1, "Stop        (miles) (min)     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride");
	Print (1);
}

/*** LINE Report ****************************************************************|***************************************************************************************

	Title: ssssssssssssssssss128sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	Lines: sssss12sssss, ssssss12ssss, sssss12sssss, sssss12sssss, ssssss12ssss, sssss12sssss, sssss12sssss, ssssss12ssss, sssss12sssss, sssss12sssss
	       ssssssssssss 
	Modes: dd-dd, dd-dd

	                           ---------------- A->B Direction (Read Down) --------------  --------------- B->A Direction (Read Up) -----------------  -------Total------
	              Dist   Time  -------Peak-------  -----Off-Peak-----  -------Daily------  -------Peak-------  -----Off-Peak-----  -------Daily------  -------Daily------
	Stop        (miles) (min)     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride     On   Off   Ride

	ddddd       fff.ff fff.ff  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd
	ssssssssss  fff.ff fff.ff  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd
	ddddd       fff.ff fff.ff  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd

	Total       fff.ff fff.ff       dddddd              dddddd              dddddd              dddddd              dddddd              dddddd              dddddd      

	Max         fff.ff fff.ff  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd  ddddd ddddd dddddd

	Passenger Miles                ddddddd             ddddddd             ddddddd             ddddddd             ddddddd             ddddddd             ddddddd
	Passenger Hours                ddddddd             ddddddd             ddddddd             ddddddd             ddddddd             ddddddd             ddddddd
	Average Trip Length (miles)    fffff.f             fffff.f             fffff.f             fffff.f             fffff.f             fffff.f             fffff.f
	Average Trip Length (minutes)  fffff.f             fffff.f             fffff.f             fffff.f             fffff.f             fffff.f             fffff.f
     
*********************************************************************************|***************************************************************************************/

