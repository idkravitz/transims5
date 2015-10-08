//*********************************************************
//	Stop_Stats.cpp - Stop Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Stop_Group_Stats
//---------------------------------------------------------

void Validate::Stop_Group_Stats (int type)
{
	int group, stop, s, riders, on, off, group_field, count_field;
	int sum_lines, sum_runs, tot_lines, tot_runs;
	double volume, count, error;
	Dtime time;
	String label;
	bool first;

	Group_Data *data, data_rec;
	Group_Map_Itr map_itr;
	Group_Map_Stat map_stat;
	Statistics stats;
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Stop_Data *stop_ptr;
	Int_Set *list;
	Int_Set_Itr itr;

	memset (&data_rec, '\0', sizeof (data_rec));

	//---- read the count data ----

	group_field = stop_count_file.Required_Field ("GROUP", "STOP", "STOPGROUP", "STOPGRP", "ID");

	if (type == STOP_GROUP) {
		count_field = stop_count_file.Required_Field ("TOTAL",  "ONOFF", "ON_OFF", "PASSENGERS", "COUNT");
	} else if (type == BOARD_GROUP) {
		count_field = stop_count_file.Required_Field ("BOARDINGS", "BOARD", "ON", "PASSENGERS", "COUNT");
	} else if (type == ALIGHT_GROUP) {
		count_field = stop_count_file.Required_Field ("ALIGHTINGS", "ALIGHT", "OFF", "PASSENGERS", "COUNT");
	} else {
		return;
	}
	stop_count_file.Rewind ();

	while (stop_count_file.Read ()) {

		group = stop_count_file.Get_Integer (group_field);
		if (group <= 0) continue;

		riders = stop_count_file.Get_Integer (count_field);

		map_stat = group_data.insert (Group_Map_Data (group, data_rec));
		
		data = &map_stat.first->second;
		data->index = group;
		data->count += riders;
	}

	//---- print the page header ----
	
	Header_Number (type);

	if (!Break_Check ((int) group_data.size () + 11)) {
		Print (1);
		Stop_Group_Header (type);
	}
	tot_runs = tot_lines = 0;

	//---- print each line group ----

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		if (data->count == 0) continue;

		group = data->index;

		list = stop_equiv.Group_List (group);
		if (list == 0) continue;

		riders = sum_runs = sum_lines = 0;

		//---- check routes for stops ----

		for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			first = true;

			//---- check the stop ----

			for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
				stop_ptr = &stop_array [stop_itr->Stop ()];
				stop = stop_ptr->Stop ();

				//---- check for stops in the stop group ----

				for (s=0, itr = list->begin (); itr != list->end (); itr++) {
					if (*itr == stop) {
						s = stop;
						break;
					}
				}
				if (s == 0) continue;
				
				//---- process run ----

				for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {

					//---- check time period ----

					time = run_itr->Schedule ();

					if (!sum_periods.In_Range (time)) continue;

					//---- sum stop ons/offs ----

					on = run_itr->Board ();
					off = run_itr->Alight ();

					if (type == STOP_GROUP) {
						riders += on + off;
					} else if (type == BOARD_GROUP) {
						riders += on;
					} else {
						riders += off;
					}
					sum_runs++;
					if (first) {
						sum_lines++;
						first = false;
					}
				}
			}
		}
		volume = riders;
		count = data->count;
		error = fabs (volume - count);

		data->count = 0;

		//---- sum to category ----

		data->number++;

		data->volume += volume;
		data->volume_sq += volume * volume;

		data->count += count;
		data->count_sq += count * count;

		data->error += error;
		data->error_sq += error * error;
		
		data->count_volume += count * volume;

		//---- add to total as well ----

		total.number++;
		tot_runs += sum_runs;
		tot_lines += sum_lines;

		total.volume += volume;
		total.volume_sq += volume * volume;

		total.count += count;
		total.count_sq += count * count;

		total.error += error;
		total.error_sq += error * error;
		
		total.count_volume += count * volume;

		//---- print the group data ----

		Calc_Stats (data, stats);
		
		label = stop_equiv.Group_Label (group);
		if (label.empty ()) {
			label = String ("Line Group %d") % group;
		}
		Page_Check (1);
		Print (0, String ("\n%-5d %-32.32s ") % data->index % label);
		Print (0, String ("%6d%8d%10.0lf%10.0lf%9.0lf %7.1lf") %
			sum_lines % sum_runs % data->volume % data->count % 
			(data->volume - data->count) % stats.per_diff);
	}
		
	//---- calculate the total statistics ----
	
	if (total.number > 0) {
		Calc_Stats (&total, stats);
		
		Page_Check (5);
		Print (0, String ("\n\nTOTAL%34c") % BLANK);
		Print (0, String ("%6d%8d%10.0lf%10.0lf%9.0lf %7.1lf\n") %
			tot_lines % tot_runs % total.volume % total.count % 
			(total.volume - total.count) % stats.per_diff);

		Print (0, String ("\n      Avg.Abs.Error = %.0lf (%.1lf%%)   Std.Dev. = %.0lf   RMSE = %.1lf%%   R Sq. = %.3lf\n") %
			stats.avg_error % stats.per_error % stats.std_dev % stats.rmse % stats.r_sq % FINISH);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void Validate::Stop_Group_Header (int type)
{
	Page_Check (5);
	Print (0, String ("\n%30cTransit Summary Statistics by ") % BLANK);
	if (type == STOP_GROUP) {
		Print (0, "Stop Group\n");
	} else if (type == BOARD_GROUP) {
		Print (0, "Boarding Group\n");
	} else {
		Print (0, "Alighting Group\n");
	}
	Print (0, String ("\n%41cNum.    Num.  ------Riders------ ---Difference---  ") % BLANK);
	Print (0, String ("\nGroup Description%23cLines    Runs  Estimate  Observed   Riders      %%\n") % BLANK % FINISH);
}

/**********************************************************|***********************************************************

                         Transit Summary Statistics by Stop Group
									
                                         Num.    Num.  ------Riders------ ---Difference---
Group Description                       Lines    Runs  Estimate  Observed   Riders     %    

ddddd ssssssssssssssssssssssssssssssss dddddd ddddddd lllllllll lllllllll llllllll fffff.f

TOTAL                                  dddddd ddddddd lllllllll lllllllll llllllll fffff.f

Avg.Abs.Error = ddddd (fff.f%)   Std.Dev. = dddddd   RMSE = ffff.f%   R Sq. = f.fff

***********************************************************|***********************************************************/ 
