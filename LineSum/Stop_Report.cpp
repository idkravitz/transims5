//*********************************************************
//	Stop_Report.cpp - Create a Stop Ridership Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Stop_Report
//---------------------------------------------------------

void LineSum::Stop_Report (Stop_Report_Data &report)
{
	int i, stop, mode, dir, arrive, on, off, depart, max_arrive, max_on, max_off, max_depart;
	int pk_on, pk_off, op_on, op_off, pk_xfer, op_xfer;
	String name;

	Range_Array_Itr range_itr;
	Leg_Data *leg_ptr;
	Line_Map_Itr map_itr;
	Access_Map_Itr access_itr;
	Str_Itr str_itr;
	Str_Map_Itr name_itr;

	Sort_Key sort_key;
	Stop_Data stop_rec, *stop_ptr, max_rec, tot_rec;
	Stop_Map stop_map;
	Stop_Map_Itr stop_itr;
	Stop_Map_Stat stop_stat;
	
	memset (&stop_rec, '\0', sizeof (stop_rec));
	memset (&max_rec, '\0', sizeof (max_rec));
	memset (&tot_rec, '\0', sizeof (tot_rec));
	max_arrive = max_depart = max_off = max_on = pk_on = pk_off = op_on = op_off = pk_xfer = op_xfer = 0;

	Show_Message (String ("Creating Stop Report #%d -- Stops") % report.number);
	Set_Progress ();

	//---- report header ----

	New_Page ();
	Print (2, "Title: ") << report.title;
	Print (1, "Stops:");

	for (i=0, range_itr = report.stops.begin (); range_itr != report.stops.end (); range_itr++, i++) {
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
		} else {
			name_itr = stop_names.find (range_itr->Low ());

			if (name_itr != stop_names.end ()) {
				Print (0, String (" (%s)") % name_itr->second);
			}
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
	if (!report.all_lines) {
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
	}
	Header_Number (STOP_REPORT);
	Stop_Header ();

	//---- gather peak ridership ----

	for (map_itr = peak_map.begin (); map_itr != peak_map.end (); map_itr++) {
		mode = map_itr->first.mode;
	
		if (!report.all_modes && !report.modes.In_Range (mode)) continue;

		name = map_itr->first.name;

		if (!report.all_lines) {
			for (str_itr = report.lines.begin (); str_itr != report.lines.end (); str_itr++) {
				if (name.In_Range (*str_itr)) break;
			}
			if (str_itr == report.lines.end ()) continue;
		}
		dir = 0;
		leg_ptr = &map_itr->second;

		stop = abs (leg_ptr->b);

		if (report.stops.In_Range (stop)) {
			dir = 1;
		} else {
			stop = abs (leg_ptr->a);

			if (report.stops.In_Range (stop)) {
				dir = 2;
			}
		}
		if (dir == 0) continue;
		Show_Progress ();

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		stop_stat = stop_map.insert (Stop_Map_Data (sort_key, stop_rec));

		stop_ptr = &stop_stat.first->second;

		if (dir == 1) {
			stop_ptr->pk.arrive += leg_ptr->ab.ride;
			stop_ptr->pk.depart += leg_ptr->ba.ride;
			stop_ptr->pk.off += leg_ptr->ab.off;
			stop_ptr->pk.on += leg_ptr->ba.on;
		} else {
			stop_ptr->pk.arrive += leg_ptr->ba.ride;
			stop_ptr->pk.depart += leg_ptr->ab.ride;
			stop_ptr->pk.off += leg_ptr->ba.off;
			stop_ptr->pk.on += leg_ptr->ab.on;
		}
	}

	//---- gather peak access data ----

	for (access_itr = access_map.begin (); access_itr != access_map.end (); access_itr++) {
		mode = access_itr->first.mode;

		if (!report.all_modes && !report.modes.In_Range (mode)) continue;

		stop = access_itr->first.stop;

		if (!report.stops.In_Range (stop)) continue;
		
		Show_Progress ();

		on = access_itr->second.pk_on;
		off = access_itr->second.pk_off;

		stop = access_itr->first.node;

		if (report.stops.In_Range (stop)) {
			on /= 2;
			off /= 2;
		}
		pk_on += on;
		pk_off += off;

		if (report.transfers.In_Range (mode)) {
			pk_xfer += (on + off) / 2;
		}
		name ("%d") % mode;

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		stop_stat = stop_map.insert (Stop_Map_Data (sort_key, stop_rec));

		stop_ptr = &stop_stat.first->second;

		stop_ptr->pk.arrive += on;
		stop_ptr->pk.depart += off;
	}

	//---- gather offpeak ridership ----

	for (map_itr = offpeak_map.begin (); map_itr != offpeak_map.end (); map_itr++) {
		mode = map_itr->first.mode;

		if (!report.all_modes && !report.modes.In_Range (mode)) continue;

		name = map_itr->first.name;

		if (!report.all_lines) {
			for (str_itr = report.lines.begin (); str_itr != report.lines.end (); str_itr++) {
				if (name.In_Range (*str_itr)) break;
			}
			if (str_itr == report.lines.end ()) continue;
		}
		dir = 0;
		leg_ptr = &map_itr->second;

		stop = abs (leg_ptr->b);

		if (report.stops.In_Range (stop)) {
			dir = 1;
		} else {
			stop = abs (leg_ptr->a);

			if (report.stops.In_Range (stop)) {
				dir = 2;
			}
		}
		if (dir == 0) continue;
		Show_Progress ();

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		stop_stat = stop_map.insert (Stop_Map_Data (sort_key, stop_rec));

		stop_ptr = &stop_stat.first->second;

		if (dir == 1) {
			stop_ptr->op.arrive += leg_ptr->ab.ride;
			stop_ptr->op.depart += leg_ptr->ba.ride;
			stop_ptr->op.off += leg_ptr->ab.off;
			stop_ptr->op.on += leg_ptr->ba.on;
		} else {
			stop_ptr->op.arrive += leg_ptr->ba.ride;
			stop_ptr->op.depart += leg_ptr->ab.ride;
			stop_ptr->op.off += leg_ptr->ba.off;
			stop_ptr->op.on += leg_ptr->ab.on;
		}
	}

	//---- gather offpeak access data ----

	for (access_itr = access_map.begin (); access_itr != access_map.end (); access_itr++) {
		mode = access_itr->first.mode;

		if (!report.all_modes && !report.modes.In_Range (mode)) continue;

		stop = access_itr->first.stop;

		if (!report.stops.In_Range (stop)) continue;

		Show_Progress ();

		on = access_itr->second.op_on;
		off = access_itr->second.op_off;

		stop = access_itr->first.node;

		if (report.stops.In_Range (stop)) {
			on /= 2;
			off /= 2;
		}
		op_on += on;
		op_off += off;

		if (report.transfers.In_Range (mode)) {
			op_xfer += (on + off) / 2;
		}
		name ("%d") % mode;

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		stop_stat = stop_map.insert (Stop_Map_Data (sort_key, stop_rec));
		
		stop_ptr = &stop_stat.first->second;

		stop_ptr->op.arrive += on;
		stop_ptr->op.depart += off;
	}
	End_Progress ();

	//---- exit if no data ----

	if (stop_map.size () == 0) {
		Warning ("No Stops to Report");
		return;
	}

	//---- print the report ----

	for (stop_itr = stop_map.begin (); stop_itr != stop_map.end (); stop_itr++) {
		mode = stop_itr->first.mode;
		name = stop_itr->first.name;

		stop_ptr = &stop_itr->second;

		arrive = stop_ptr->pk.arrive + stop_ptr->op.arrive;
		depart = stop_ptr->pk.depart + stop_ptr->op.depart;
		off = stop_ptr->pk.off + stop_ptr->op.off;
		on = stop_ptr->pk.on + stop_ptr->op.on;

		Print (1, String ("%3d    %-12.12s") % mode % name);

		Print (0, String ("  %8d%8d%8d%8d  %8d%8d%8d%8d  %8d%8d%8d%8d") %
			stop_ptr->pk.arrive % stop_ptr->pk.off % stop_ptr->pk.on % stop_ptr->pk.depart %
			stop_ptr->op.arrive % stop_ptr->op.off % stop_ptr->op.on % stop_ptr->op.depart %
			arrive % off % on % depart);

		tot_rec.pk.arrive += stop_ptr->pk.arrive;
		tot_rec.pk.depart += stop_ptr->pk.depart;
		tot_rec.pk.off += stop_ptr->pk.off;
		tot_rec.pk.on += stop_ptr->pk.on;

		tot_rec.op.arrive += stop_ptr->op.arrive;
		tot_rec.op.depart += stop_ptr->op.depart;
		tot_rec.op.off += stop_ptr->op.off;
		tot_rec.op.on += stop_ptr->op.on;

		if (stop_ptr->pk.arrive > max_rec.pk.arrive) max_rec.pk.arrive = stop_ptr->pk.arrive;
		if (stop_ptr->pk.depart > max_rec.pk.depart) max_rec.pk.depart = stop_ptr->pk.depart;
		if (stop_ptr->pk.off > max_rec.pk.off) max_rec.pk.off = stop_ptr->pk.off;
		if (stop_ptr->pk.on > max_rec.pk.on) max_rec.pk.on = stop_ptr->pk.on;

		if (stop_ptr->op.arrive > max_rec.op.arrive) max_rec.op.arrive = stop_ptr->op.arrive;
		if (stop_ptr->op.depart > max_rec.op.depart) max_rec.op.depart = stop_ptr->op.depart;
		if (stop_ptr->op.off > max_rec.op.off) max_rec.op.off = stop_ptr->op.off;
		if (stop_ptr->op.on > max_rec.op.on) max_rec.op.on = stop_ptr->op.on;

		if (arrive > max_arrive) max_arrive = arrive;
		if (depart > max_depart) max_depart = depart;
		if (off > max_off) max_off = off;
		if (on > max_on) max_on = on;
	}

	//---- print total ----

	arrive = tot_rec.pk.arrive + tot_rec.op.arrive;
	depart = tot_rec.pk.depart + tot_rec.op.depart;
	off = tot_rec.pk.off + tot_rec.op.off;
	on = tot_rec.pk.on + tot_rec.op.on;

	Print (2, String ("Total  %-4d          %8d%8d%8d%8d  %8d%8d%8d%8d  %8d%8d%8d%8d") % stop_map.size () %
		tot_rec.pk.arrive % tot_rec.pk.off % tot_rec.pk.on % tot_rec.pk.depart %
		tot_rec.op.arrive % tot_rec.op.off % tot_rec.op.on % tot_rec.op.depart %
		arrive % off % on % depart);

	if (report.transfers.size () > 0) {
		pk_xfer += (tot_rec.pk.on - pk_on + tot_rec.pk.off - pk_off) / 2;
		op_xfer += (tot_rec.op.on - op_on + tot_rec.op.off - op_off) / 2;

		Print (1, String ("Transfers                            %8d                          %8d                          %8d") %
			pk_xfer % op_xfer % (pk_xfer + op_xfer));
	}
	Print (1, String ("Maximum              %8d%8d%8d%8d  %8d%8d%8d%8d  %8d%8d%8d%8d") %
		max_rec.pk.arrive % max_rec.pk.off % max_rec.pk.on % max_rec.pk.depart %
		max_rec.op.arrive % max_rec.op.off % max_rec.op.on % max_rec.op.depart %
		max_arrive % max_off % max_on % max_depart);

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Header
//---------------------------------------------------------

void LineSum::Stop_Header (void)
{
	Print (2, "                      --------------Peak-------------   ------------Off-Peak-----------   -------------Daily-------------");
	Print (1, "Mode   Line            Arrive    Off      On   Depart    Arrive    Off      On   Depart    Arrive    Off      On   Depart");
	Print (1);
}

/*** STOP Report ****************************************************************|**********************************************************************************

	Title: ssssssssssssssssss
	Stops: ddddd, ddddd, ddddd, ...
	Modes: dd-dd, dd-dd
	Lines: ssssssssssss, ssssssssssss

	                      --------------Peak-------------   ------------Off-Peak-----------   -------------Daily------------- 
	Mode   Line            Arrive    Off      On   Depart    Arrive    Off      On   Depart    Arrive    Off      On   Depart

	ddd    ssss12ssssss   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd
	ddd    ssss12ssssss   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd
	ddd    ssss12ssssss   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd
	ddd    ssss12ssssss   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd

	Total  ddd            ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd
    Transfers                             ddddddd                           ddddddd                           ddddddd
	Maximum               ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd   ddddddd ddddddd ddddddd ddddddd


*********************************************************************************|**********************************************************************************/
