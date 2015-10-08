//*********************************************************
//	Line_Stats.cpp - Line Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Line_Group_Stats
//---------------------------------------------------------

void Validate::Line_Group_Stats (void)
{
	int group, line, riders, group_field, count_field;
	int sum_runs, sum_lines, tot_runs, tot_lines;
	double volume, count, error;
	Dtime time;
	String label;
	bool first;

	Group_Data *data, data_rec;
	Group_Map_Itr map_itr;
	Group_Map_Stat map_stat;
	Statistics stats;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Int_Set *list;
	Int_Set_Itr itr;
	Int_Map_Itr int_itr;

	memset (&data_rec, '\0', sizeof (data_rec));

	//---- read the count data ----

	group_field = line_count_file.Required_Field ("GROUP", "LINE", "LINEGROUP", "LINEGRP", "ID");
	count_field = line_count_file.Required_Field ("COUNT", "RIDERS", "PASSENGERS", "TOTAL", "VOLUME");

	while (line_count_file.Read ()) {

		group = line_count_file.Get_Integer (group_field);
		if (group <= 0) continue;

		riders = line_count_file.Get_Integer (count_field);

		map_stat = group_data.insert (Group_Map_Data (group, data_rec));
		
		data = &map_stat.first->second;
		data->index = group;
		data->count += riders;
	}

	//---- print the page header ----
	
	Header_Number (LINE_GROUP);

	if (!Break_Check ((int) group_data.size () + 11)) {
		Print (1);
		Line_Group_Header ();
	}

	//---- print each line group ----

	tot_runs = tot_lines = 0;

	for (map_itr = group_data.begin (); map_itr != group_data.end (); map_itr++) {
		data = &map_itr->second;
		if (data->count == 0) continue;

		group = data->index;

		list = line_equiv.Group_List (group);
		if (list == 0) continue;

		riders = sum_runs = sum_lines = 0;

		for (itr = list->begin (); itr != list->end (); itr++) {
			line = *itr;

			int_itr = line_map.find (line);
			if (int_itr == line_map.end ()) continue;

			line_ptr = &line_array [int_itr->second];
			first = true;
			
			//---- process run ----

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {

				//---- check the mid-run time period ----

				run_itr = --stop_itr->end ();
				time = run_itr->Schedule ();

				run_itr = stop_itr->begin ();
				time = (time + run_itr->Schedule ()) / 2;

				if (!sum_periods.In_Range (time)) continue;

				if (first) {
					sum_runs += (int) stop_itr->size ();
					sum_lines++;
					first = false;
				}

				//---- sum the boarding ----

				for (; run_itr != stop_itr->end (); run_itr++) {
					riders += run_itr->Board ();
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
		
		label = line_equiv.Group_Label (group);
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
//	Line_Group_Header
//---------------------------------------------------------

void Validate::Line_Group_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%30cSummary Statistics by Line Group\n") % BLANK);
	Print (0, String ("\n%41cNum.    Num.  ------Riders------ ---Difference---  ") % BLANK);
	Print (0, String ("\nGroup Description%23cLines    Runs  Estimate  Observed   Riders      %%\n") % BLANK % FINISH);
}

/**********************************************************|***********************************************************

                         Transit Summary Statistics by Line Group
									
                                   Num.    Num.  ------Riders------ ---Difference---
Line Group                        Lines    Runs  Estimate  Observed   Riders     %    

ssssssssssssssssssssssssssssssss dddddd ddddddd lllllllll lllllllll llllllll fffff.f

TOTAL                            dddddd ddddddd lllllllll lllllllll llllllll fffff.f

Avg.Abs.Error = ddddddd (fff.f%)   Std.Dev. = ddddddd   RMSE = ffff.f%   R Sq. = f.fff

***********************************************************|***********************************************************/ 
