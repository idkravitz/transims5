//*********************************************************
//	Total_Report.cpp - Create a Total Ridership Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Total_Report
//---------------------------------------------------------

void LineSum::Total_Report (Total_Report_Data &report)
{
	int pk_tot_on, op_tot_on, ab_on, ba_on, tot_on, mode;
	int mode_fld, line_fld, pk_len_fld, pk_time_fld, pk_on_ab_fld, pk_on_ba_fld, pk_on_fld, pk_max_ab_fld, pk_max_ba_fld;
	int op_len_fld, op_time_fld, op_on_ab_fld, op_on_ba_fld, op_on_fld, op_max_ab_fld, op_max_ba_fld;
	int day_on_ab_fld, day_on_ba_fld, day_on_fld, day_pmt_fld, day_pht_fld;

	bool found, all_flag;
	String name;

	Leg_Data *leg_ptr;
	Line_Map_Itr map_itr;
	Str_Itr str_itr;
		
	Total_Data total_rec, *total_ptr;
	Total_Array total_array;
	Total_Itr total_itr;
	
	memset (&total_rec, '\0', sizeof (total_rec));

	mode_fld = line_fld = pk_len_fld = pk_time_fld = pk_on_ab_fld = pk_on_ba_fld = pk_on_fld = pk_max_ab_fld = pk_max_ba_fld = -1;
	op_len_fld = op_time_fld = op_on_ab_fld = op_on_ba_fld = op_on_fld = op_max_ab_fld = op_max_ba_fld = -1;
	day_on_ab_fld = day_on_ba_fld = day_on_fld = day_pmt_fld = day_pht_fld = -1;

	if (report.file != 0) {
		mode_fld = report.file->Add_Field ("MODE", DB_INTEGER, 4);
		line_fld = report.file->Add_Field ("LINE", DB_STRING, 20);
		pk_len_fld = report.file->Add_Field ("PK_LEN", DB_DOUBLE, 10.2, MILES);
		pk_time_fld = report.file->Add_Field ("PK_TIME", DB_DOUBLE, 10.2, MINUTES);
		pk_on_ab_fld = report.file->Add_Field ("PK_ON_AB", DB_INTEGER, 8);
		pk_on_ba_fld = report.file->Add_Field ("PK_ON_BA", DB_INTEGER, 8);
		pk_on_fld = report.file->Add_Field ("PK_ON", DB_INTEGER, 8);
		pk_max_ab_fld = report.file->Add_Field ("PK_MAX_AB", DB_INTEGER, 8);
		pk_max_ba_fld = report.file->Add_Field ("PK_MAX_BA", DB_INTEGER, 8);
		op_len_fld = report.file->Add_Field ("OP_LEN", DB_DOUBLE, 10.2, MILES);
		op_time_fld = report.file->Add_Field ("OP_TIME", DB_DOUBLE, 10.2, MINUTES);
		op_on_ab_fld = report.file->Add_Field ("OP_ON_AB", DB_INTEGER, 8);
		op_on_ba_fld = report.file->Add_Field ("OP_ON_BA", DB_INTEGER, 8);
		op_on_fld = report.file->Add_Field ("OP_ON", DB_INTEGER, 8);
		op_max_ab_fld = report.file->Add_Field ("OP_MAX_AB", DB_INTEGER, 8);
		op_max_ba_fld = report.file->Add_Field ("OP_MAX_BA", DB_INTEGER, 8);
		day_on_ab_fld = report.file->Add_Field ("DAY_ON_AB", DB_INTEGER, 8);
		day_on_ba_fld = report.file->Add_Field ("DAY_ON_BA", DB_INTEGER, 8);
		day_on_fld = report.file->Add_Field ("DAY_ON", DB_INTEGER, 8);
		day_pmt_fld = report.file->Add_Field ("DAY_PMT", DB_DOUBLE, 12.1, MILES);
		day_pht_fld = report.file->Add_Field ("DAY_PHT", DB_DOUBLE, 12.1, HOURS);

		report.file->Write_Header ();
	}
	Show_Message (String ("Creating Total Report #%d -- Lines") % report.number);
	Set_Progress ();

	//---- report header ----

	New_Page ();
	Print (2, "Title: ") << report.title;

	Header_Number (TOTAL_REPORT);
	Total_Header ();

	//---- gather data for each line ----

	for (str_itr = report.lines.begin (); str_itr != report.lines.end (); str_itr++) {

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
			leg_ptr = &map_itr->second;

			//---- find the total record ----

			for (total_itr = total_array.begin (); total_itr != total_array.end (); total_itr++) {
				if (total_itr->line.Equals (name)) break;
			}
			if (total_itr == total_array.end ()) {
				total_rec.mode = map_itr->first.mode;
				total_rec.line = name;

				total_array.push_back (total_rec);

				total_ptr = &total_array.back ();
			} else {
				total_ptr = &(*total_itr);
			}
			total_ptr->pk.dist += leg_ptr->dist;
			total_ptr->pk.time += leg_ptr->time;
			total_ptr->pk.ab_on += leg_ptr->ab.on;
			total_ptr->pk.ba_on += leg_ptr->ba.on;
			if (total_ptr->pk.ab_max < leg_ptr->ab.ride) total_ptr->pk.ab_max = leg_ptr->ab.ride;
			if (total_ptr->pk.ba_max < leg_ptr->ba.ride) total_ptr->pk.ba_max = leg_ptr->ba.ride;

			tot_on = leg_ptr->ab.ride + leg_ptr->ba.ride;
			total_ptr->pmt += (double) leg_ptr->dist * tot_on / 100.0;
			total_ptr->pht += (double) leg_ptr->time * tot_on / 6000.0;
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
			leg_ptr = &map_itr->second;

			//---- find the total record ----

			for (total_itr = total_array.begin (); total_itr != total_array.end (); total_itr++) {
				if (total_itr->line.Equals (name)) break;
			}
			if (total_itr == total_array.end ()) {
				total_rec.mode = map_itr->first.mode;
				total_rec.line = name;

				total_array.push_back (total_rec);

				total_ptr = &total_array.back ();
			} else {
				total_ptr = &(*total_itr);
			}
			total_ptr->op.dist += leg_ptr->dist;
			total_ptr->op.time += leg_ptr->time;
			total_ptr->op.ab_on += leg_ptr->ab.on;
			total_ptr->op.ba_on += leg_ptr->ba.on;
			if (total_ptr->op.ab_max < leg_ptr->ab.ride) total_ptr->op.ab_max = leg_ptr->ab.ride;
			if (total_ptr->op.ba_max < leg_ptr->ba.ride) total_ptr->op.ba_max = leg_ptr->ba.ride;

			tot_on = leg_ptr->ab.ride + leg_ptr->ba.ride;
			total_ptr->pmt += (double) leg_ptr->dist * tot_on / 100.0;
			total_ptr->pht += (double) leg_ptr->time * tot_on / 6000.0;
		}
	}

	//---- exit if no data ----

	if (total_array.size () == 0) {
		Warning ("No Lines to Report");
		return;
	}
	memset (&total_rec, '\0', sizeof (total_rec));

	//---- print the report ----
	
	for (total_itr = total_array.begin (); total_itr != total_array.end (); total_itr++) {
		Show_Progress ();

		pk_tot_on = total_itr->pk.ab_on + total_itr->pk.ba_on;
		op_tot_on = total_itr->op.ab_on + total_itr->op.ba_on;
		ab_on = total_itr->pk.ab_on + total_itr->op.ab_on;
		ba_on = total_itr->pk.ba_on + total_itr->op.ba_on;
		tot_on = ab_on + ba_on;

		Print (1, String ("%3d   %-12.12s") % total_itr->mode % total_itr->line);

		Print (0, String ("%7.1lf%7.1lf%7d%7d%7d%7d%7d %7.1lf%7.1lf%7d%7d%7d%7d%7d%8d%8d%8d%10.0lf%9.0lf") %
			(double) (total_itr->pk.dist / 100.0) % (double) (total_itr->pk.time / 100.0) % 
			total_itr->pk.ab_on % total_itr->pk.ba_on % pk_tot_on % total_itr->pk.ab_max % total_itr->pk.ba_max %
			(double) (total_itr->op.dist / 100.0) % (double) (total_itr->op.time / 100.0) % 
			total_itr->op.ab_on % total_itr->op.ba_on % op_tot_on % total_itr->op.ab_max % total_itr->op.ba_max %
			ab_on % ba_on % tot_on % total_itr->pmt % total_itr->pht);

		if (report.file != 0) {
			report.file->Put_Field (mode_fld, total_itr->mode);
			report.file->Put_Field (line_fld, total_itr->line);
			report.file->Put_Field (pk_len_fld, total_itr->pk.dist * 52.8);
			report.file->Put_Field (pk_time_fld, total_itr->pk.time / 100.0);
			report.file->Put_Field (pk_on_ab_fld, total_itr->pk.ab_on);
			report.file->Put_Field (pk_on_ba_fld, total_itr->pk.ba_on);
			report.file->Put_Field (pk_on_fld, pk_tot_on);
			report.file->Put_Field (pk_max_ab_fld, total_itr->pk.ab_max);
			report.file->Put_Field (pk_max_ba_fld, total_itr->pk.ba_max);

			report.file->Put_Field (op_len_fld, total_itr->op.dist * 52.8);
			report.file->Put_Field (op_time_fld, total_itr->op.time / 100.0);
			report.file->Put_Field (op_on_ab_fld, total_itr->op.ab_on);
			report.file->Put_Field (op_on_ba_fld, total_itr->op.ba_on);
			report.file->Put_Field (op_on_fld, op_tot_on);
			report.file->Put_Field (op_max_ab_fld, total_itr->op.ab_max);
			report.file->Put_Field (op_max_ba_fld, total_itr->op.ba_max);

			report.file->Put_Field (day_on_ab_fld, ab_on);
			report.file->Put_Field (day_on_ba_fld, ba_on);
			report.file->Put_Field (day_on_fld, tot_on);
			report.file->Put_Field (day_pmt_fld, total_itr->pmt * 5280.0);
			report.file->Put_Field (day_pht_fld, total_itr->pht);

			report.file->Write ();
		}

		//---- calculate totals ----

		total_rec.pk.dist += total_itr->pk.dist;
		total_rec.pk.time += total_itr->pk.time;
		total_rec.pk.ab_on += total_itr->pk.ab_on;
		total_rec.pk.ba_on += total_itr->pk.ba_on;
		if (total_rec.pk.ab_max < total_itr->pk.ab_max) total_rec.pk.ab_max = total_itr->pk.ab_max;
		if (total_rec.pk.ba_max < total_itr->pk.ba_max) total_rec.pk.ba_max = total_itr->pk.ba_max;

		total_rec.op.dist += total_itr->op.dist;
		total_rec.op.time += total_itr->op.time;
		total_rec.op.ab_on += total_itr->op.ab_on;
		total_rec.op.ba_on += total_itr->op.ba_on;
		if (total_rec.op.ab_max < total_itr->op.ab_max) total_rec.op.ab_max = total_itr->op.ab_max;
		if (total_rec.op.ba_max < total_itr->op.ba_max) total_rec.op.ba_max = total_itr->op.ba_max;

		total_rec.pmt += total_itr->pmt;
		total_rec.pht += total_itr->pht;
	}
	End_Progress ();

	//---- print the totals ----

	pk_tot_on = total_rec.pk.ab_on + total_rec.pk.ba_on;
	op_tot_on = total_rec.op.ab_on + total_rec.op.ba_on;
	ab_on = total_rec.pk.ab_on + total_rec.op.ab_on;
	ba_on = total_rec.pk.ba_on + total_rec.op.ba_on;
	tot_on = ab_on + ba_on;

	Print (2, String ("Total %-4d        %7.1lf%7.1lf%7d%7d%7d%7d%7d %7.1lf%7.1lf%7d%7d%7d%7d%7d%8d%8d%8d%10.0lf%9.0lf") %
		total_array.size () % (double) (total_rec.pk.dist / 100.0) % (double) (total_rec.pk.time / 100.0) % 
		total_rec.pk.ab_on % total_rec.pk.ba_on % pk_tot_on % total_rec.pk.ab_max % total_rec.pk.ba_max %
		(double) (total_rec.op.dist / 100.0) % (double) (total_rec.op.time / 100.0) %
		total_rec.op.ab_on % total_rec.op.ba_on % op_tot_on % total_rec.op.ab_max % total_rec.op.ba_max %
		ab_on % ba_on % tot_on % total_rec.pmt % total_rec.pht);

	Header_Number (0);
}

//---------------------------------------------------------
//	Total_Header
//---------------------------------------------------------

void LineSum::Total_Header (void)
{
	Print (2, "                   ------------------- Peak Period ----------------  -------------------- Off Peak ------------------  --------------- Daily -------------------");
	Print (1, "                     Dist   Time ------Boardings----- ---Max Load--    Dist   Time ------Boardings----- ---Max Load--  -------Boardings------  ----Passenger----");
	Print (1, "Mode  Line         (miles) (min)   A->B   B->A  Total   A->B   B->A  (miles) (min)   A->B   B->A  Total   A->B   B->A    A->B    B->A   Total     Miles    Hours");
	Print (1);
}

/*** TOTAL Report ****************************************************************|**********************************************************************************

	Title: ssssssssssssssssss40ssssssssssssssssssss

	                   ------------------- Peak Period ----------------  -------------------- Off Peak ------------------  --------------- Daily ------------------- 
                         Dist   Time ------Boardings----- ---Max Load--    Dist   Time ------Boardings----- ---Max Load--  -------Boardings------  ----Passenger---- 
	Mode  Line         (miles) (min)   A->B   B->A  Total   A->B   B->A  (miles) (min)   A->B   B->A  Total   A->B   B->A    A->B    B->A   Total     Miles    Hours

	ddd   sssss12sssss ffff.f ffff.f dddddd dddddd dddddd dddddd dddddd  ffff.f ffff.f dddddd dddddd dddddd dddddd dddddd ddddddd ddddddd ddddddd fffffff.f ffffff.f

	Total dddd         ffffff ffffff dddddd dddddd dddddd dddddd dddddd  ffffff ffffff dddddd dddddd dddddd dddddd dddddd ddddddd ddddddd ddddddd fffffffff ffffffff
    
*********************************************************************************|**********************************************************************************/
