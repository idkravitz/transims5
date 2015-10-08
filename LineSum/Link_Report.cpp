//*********************************************************
//	Link_Report.cpp - Create a Link Ridership Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Link_Report
//---------------------------------------------------------

void LineSum::Link_Report (Link_Report_Data &report)
{
	int i, anode, bnode, mode, dir;
	int ab_pk_tot, ab_op_tot, ba_pk_tot, ba_op_tot, ab_tot, ba_tot, pk_tot, op_tot, total;
	int ab_pk_max, ab_op_max, ba_pk_max, ba_op_max, ab_max, ba_max, pk_max, op_max, maximum;	
	int ab_pk_pmt, ab_op_pmt, ba_pk_pmt, ba_op_pmt, ab_pmt, ba_pmt, pk_pmt, op_pmt, pmt;
	int ab_pk_pht, ab_op_pht, ba_pk_pht, ba_op_pht, ab_pht, ba_pht, pk_pht, op_pht, pht;

	Leg_Data *leg_ptr;
	Line_Map_Itr map_itr;
	Str_Itr str_itr;
	Str_Map_Itr name_itr;
	String name;
	Int_Itr int_itr, b_itr;
	Range_Array_Itr range_itr;

	Link_Map link_map;
	Sort_Key sort_key;
	Link_Data link_rec, *link_ptr;
	Link_Map_Itr link_itr;
	Link_Map_Stat map_stat;

	memset (&link_rec, '\0', sizeof (link_rec));
	
	Show_Message (String ("Creating Link Report #%d -- Records") % report.number);
	Set_Progress ();

	//---- report header ----

	New_Page ();
	Print (2, "Title: ") << report.title;
	Print (1, "Links: ") << report.links;

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

	//---- flag connected links ----

	for (i=0, int_itr = report.nodes.begin (); int_itr != report.nodes.end (); int_itr += 2, i++) {
		anode = *int_itr;

		for (b_itr = report.nodes.begin (); b_itr != report.nodes.end (); b_itr++) {
			bnode = *(++b_itr);
			if (bnode == anode) {
				*(int_itr) = -anode;
				break;
			}
		}
	}

	Header_Number (LINK_REPORT);
	Link_Header ();

	ab_pk_tot = ab_op_tot = ba_pk_tot = ba_op_tot = ab_tot = ba_tot = pk_tot = op_tot = total = 0;
	ab_pk_max = ab_op_max = ba_pk_max = ba_op_max = ab_max = ba_max = pk_max = op_max = maximum = 0;	
	ab_pk_pmt = ab_op_pmt = ba_pk_pmt = ba_op_pmt = ab_pmt = ba_pmt = pk_pmt = op_pmt = pmt = 0;
	ab_pk_pht = ab_op_pht = ba_pk_pht = ba_op_pht = ab_pht = ba_pht = pk_pht = op_pht = pht = 0;

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
		dir = anode = bnode = 0;
		leg_ptr = &map_itr->second;
		
		for (int_itr = report.nodes.begin (); int_itr != report.nodes.end (); int_itr++) {
			anode = *int_itr;
			bnode = *(++int_itr);

			if (abs (leg_ptr->a) == abs (anode) && abs (leg_ptr->b) == bnode) {
				dir = 1;
				break;
			} else if (abs (leg_ptr->a) == bnode && abs (leg_ptr->b) == abs (anode)) {
				dir = 2;
				break;
			}
		}
		if (dir == 0) continue;

		Show_Progress ();
	
		//---- search for the link ----

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		map_stat = link_map.insert (Link_Map_Data (sort_key, link_rec));

		if (dir == 1) {
			if (anode < 0) {
				map_stat.first->second.ab_pk_ride += leg_ptr->ab.on;
				ab_pk_tot += leg_ptr->ab.on;
			} else {
				map_stat.first->second.ab_pk_ride += leg_ptr->ab.ride;
				ab_pk_tot += leg_ptr->ab.ride;
			}
			ab_pk_pmt += leg_ptr->ab.ride * leg_ptr->dist;
			ab_pk_pht += leg_ptr->ab.ride * leg_ptr->time;

			if (!report.dir_flag) {
				if (anode < 0) {
					map_stat.first->second.ba_pk_ride += leg_ptr->ba.off;
					ba_pk_tot += leg_ptr->ba.off;
				} else {
					map_stat.first->second.ba_pk_ride += leg_ptr->ba.ride;
					ba_pk_tot += leg_ptr->ba.ride;
				}
				ba_pk_pmt += leg_ptr->ba.ride * leg_ptr->dist;
				ba_pk_pht += leg_ptr->ba.ride * leg_ptr->time;
			}
		} else {
			if (anode < 0) {
				map_stat.first->second.ab_pk_ride += leg_ptr->ba.on;
				ab_pk_tot += leg_ptr->ba.on;
			} else {
				map_stat.first->second.ab_pk_ride += leg_ptr->ba.ride;
				ab_pk_tot += leg_ptr->ba.ride;
			}
			ab_pk_pmt += leg_ptr->ba.ride * leg_ptr->dist;
			ab_pk_pht += leg_ptr->ba.ride * leg_ptr->time;

			if (!report.dir_flag) {
				if (anode < 0) {
					map_stat.first->second.ba_pk_ride += leg_ptr->ab.off;
					ba_pk_tot += leg_ptr->ab.off;
				} else {
					map_stat.first->second.ba_pk_ride += leg_ptr->ab.ride;
					ba_pk_tot += leg_ptr->ab.ride;
				}
				ba_pk_pmt += leg_ptr->ab.ride * leg_ptr->dist;
				ba_pk_pht += leg_ptr->ab.ride * leg_ptr->time;
			}
		}
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
		dir = anode = bnode = 0;
		leg_ptr = &map_itr->second;
		
		for (int_itr = report.nodes.begin (); int_itr != report.nodes.end (); int_itr++) {
			anode = *int_itr;
			bnode = *(++int_itr);

			if (abs (leg_ptr->a) == abs (anode) && abs (leg_ptr->b) == bnode) {
				dir = 1;
				break;
			} else if (abs (leg_ptr->a) == bnode && abs (leg_ptr->b) == abs (anode)) {
				dir = 2;
				break;
			}
		}
		if (dir == 0) continue;

		Show_Progress ();
	
		//---- search for the link ----

		sort_key.mode = (short) mode;
		sort_key.leg = 0;
		sort_key.name = name;

		map_stat = link_map.insert (Link_Map_Data (sort_key, link_rec));
		
		if (dir == 1) {
			if (anode < 0) {
				map_stat.first->second.ab_op_ride += leg_ptr->ab.on;
				ab_op_tot += leg_ptr->ab.on;
			} else {
				map_stat.first->second.ab_op_ride += leg_ptr->ab.ride;
				ab_op_tot += leg_ptr->ab.ride;
			}
			ab_op_pmt += leg_ptr->ab.ride * leg_ptr->dist;
			ab_op_pht += leg_ptr->ab.ride * leg_ptr->time;

			if (!report.dir_flag) {
				if (anode < 0) {
					map_stat.first->second.ba_op_ride += leg_ptr->ba.off;
					ba_op_tot += leg_ptr->ba.off;
				} else {
					map_stat.first->second.ba_op_ride += leg_ptr->ba.ride;
					ba_op_tot += leg_ptr->ba.ride;
				}
				ba_op_pmt += leg_ptr->ba.ride * leg_ptr->dist;
				ba_op_pht += leg_ptr->ba.ride * leg_ptr->time;
			}
		} else {
			if (anode < 0) {
				map_stat.first->second.ab_op_ride += leg_ptr->ba.on;
				ab_op_tot += leg_ptr->ba.on;
			} else {
				map_stat.first->second.ab_op_ride += leg_ptr->ba.ride;
				ab_op_tot += leg_ptr->ba.ride;
			}
			ab_op_pmt += leg_ptr->ba.ride * leg_ptr->dist;
			ab_op_pht += leg_ptr->ba.ride * leg_ptr->time;

			if (!report.dir_flag) {
				if (anode < 0) {
					map_stat.first->second.ba_op_ride += leg_ptr->ab.off;
					ba_op_tot += leg_ptr->ab.off;
				} else {
					map_stat.first->second.ba_op_ride += leg_ptr->ab.ride;
					ba_op_tot += leg_ptr->ab.ride;
				}
				ba_op_pmt += leg_ptr->ab.ride * leg_ptr->dist;
				ba_op_pht += leg_ptr->ab.ride * leg_ptr->time;
			}
		}
	}

	//---- exit if no data ----

	if (link_map.size () == 0) {
		Warning ("No Link Data to Report");
		return;
	}

	//---- print the report ----

	for (link_itr = link_map.begin (); link_itr != link_map.end (); link_itr++) {
		sort_key = link_itr->first;
		link_ptr = &link_itr->second;
		
		ab_tot = link_ptr->ab_pk_ride + link_ptr->ab_op_ride;
		ba_tot = link_ptr->ba_pk_ride + link_ptr->ba_op_ride;
		pk_tot = link_ptr->ab_pk_ride + link_ptr->ba_pk_ride;
		op_tot = link_ptr->ab_op_ride + link_ptr->ba_op_ride;
		total = pk_tot + op_tot;

		Print (1, String ("%3d    %-12.12s  %7d  %7d  %7d   %7d  %7d  %7d   %7d  %7d  %7d") %
			(int) sort_key.mode % sort_key.name % link_ptr->ab_pk_ride % link_ptr->ab_op_ride % ab_tot %
			link_ptr->ba_pk_ride % link_ptr->ba_op_ride % ba_tot % pk_tot % op_tot % total);

		if (link_ptr->ab_pk_ride > ab_pk_max) ab_pk_max = link_ptr->ab_pk_ride;
		if (link_ptr->ab_op_ride > ab_op_max) ab_op_max = link_ptr->ab_op_ride;
		if (ab_tot > ab_max) ab_max = ab_tot;
		if (pk_tot > pk_max) pk_max = pk_tot;

		if (link_ptr->ba_pk_ride > ba_pk_max) ba_pk_max = link_ptr->ba_pk_ride;
		if (link_ptr->ba_op_ride > ba_op_max) ba_op_max = link_ptr->ba_op_ride;
		if (ba_tot > ba_max) ba_max = ba_tot;
		if (op_tot > op_max) op_max = op_tot;

		if (total > maximum) maximum = total;
	}

	//---- print the totals ----

	ab_tot = ab_pk_tot + ab_op_tot;
	ba_tot = ba_pk_tot + ba_op_tot;
	pk_tot = ab_pk_tot + ba_pk_tot;
	op_tot = ab_op_tot + ba_op_tot;
	total = pk_tot + op_tot;

	Print (2, String ("Total  %-3d           %7d  %7d  %7d   %7d  %7d  %7d   %7d  %7d  %7d") %
		(int) link_map.size () % ab_pk_tot % ab_op_tot % ab_tot %
		ba_pk_tot % ba_op_tot % ba_tot % pk_tot % op_tot % total);

	Print (2, String ("Maximum              %7d  %7d  %7d   %7d  %7d  %7d   %7d  %7d  %7d") %
		ab_pk_max % ab_op_max % ab_max % ba_pk_max % ba_op_max % ba_max % pk_max % op_max % maximum);

	ab_pmt = ab_pk_pmt + ab_op_pmt;
	ba_pmt = ba_pk_pmt + ba_op_pmt;
	pk_pmt = ab_pk_pmt + ba_pk_pmt;
	op_pmt = ab_op_pmt + ba_op_pmt;
	pmt = pk_pmt + op_pmt;

	ab_pk_pmt = (ab_pk_pmt + 50) / 100;
	ab_op_pmt = (ab_op_pmt + 50) / 100;
	ab_pmt = (ab_pmt + 50) / 100;
	ba_pk_pmt = (ba_pk_pmt + 50) / 100;
	ba_op_pmt = (ba_op_pmt + 50) / 100;
	ba_pmt = (ba_pmt + 50) / 100;
	pk_pmt = (pk_pmt + 50) / 100;
	op_pmt = (op_pmt + 50) / 100;
	pmt = (pmt + 50) / 100;

	Print (1, String ("Passenger Miles      %7d  %7d  %7d   %7d  %7d  %7d   %7d  %7d  %7d") %
		ab_pk_pmt % ab_op_pmt % ab_pmt % ba_pk_pmt % ba_op_pmt % ba_pmt % pk_pmt % op_pmt % pmt);
	
	ab_pht = ab_pk_pht + ab_op_pht;
	ba_pht = ba_pk_pht + ba_op_pht;
	pk_pht = ab_pk_pht + ba_pk_pht;
	op_pht = ab_op_pht + ba_op_pht;
	pht = pk_pht + op_pht;

	ab_pk_pht = (ab_pk_pht + 3000) / 6000;
	ab_op_pht = (ab_op_pht + 3000) / 6000;
	ab_pht = (ab_pht + 3000) / 6000;
	ba_pk_pht = (ba_pk_pht + 3000) / 6000;
	ba_op_pht = (ba_op_pht + 3000) / 6000;
	ba_pht = (ba_pht + 3000) / 6000;
	pk_pht = (pk_pht + 3000) / 6000;
	op_pht = (op_pht + 3000) / 6000;
	pht = (pht + 3000) / 6000;

	Print (1, String ("Passenger Hours      %7d  %7d  %7d   %7d  %7d  %7d   %7d  %7d  %7d") %
		ab_pk_pht % ab_op_pht % ab_pht % ba_pk_pht % ba_op_pht % ba_pht % pk_pht % op_pht % pht);

	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Header
//---------------------------------------------------------

void LineSum::Link_Header (void)
{
	Print (2, "                      -----A->B Direction-----    -----B->A Direction-----    --------- Total --------");
	Print (1, "Mode   Line             Peak  Off-Peak   Daily      Peak  Off-Peak   Daily      Peak  Off-Peak   Daily");
	Print (1);
}

/*** LINK Report ****************************************************************|**********************************************************************************

	Title: ssssssssssssssssss128sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	Links: ddddd-ddddd, ddddd-ddddd, ddddd-ddddd, ....
	Modes: dd-dd, dd-dd
	Lines: sssss..sssss

	                      -----A->B Direction-----    -----B->A Direction-----    --------- Total --------
	Mode   Line             Peak  Off-Peak   Daily      Peak  Off-Peak   Daily      Peak  Off-Peak   Daily 

	ddd    ssss12ssssss   dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd 
	ddd    ssss12ssssss   dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd 
	ddd    ssss12ssssss   dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd 
	ddd    ssss12ssssss   dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd 

	Total  ddd            dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd

	Maximum               dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd
	Passenger Miles       dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd
	Passenger Hours       dddddd   dddddd   dddddd    dddddd   dddddd   dddddd    dddddd   dddddd   dddddd
     
*********************************************************************************|**********************************************************************************/

