//*********************************************************
//	Difference_Data.cpp - Difference Distribution Data
//*********************************************************

#include "Difference_Data.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Difference_Data constructor
//---------------------------------------------------------

Difference_Data::Difference_Data (void)
{
	num_periods = period = -1;
	num_distribution = NUM_DISTRIBUTION;
	min_difference = MIN_DIFFERENCE;
	total_distrib = period_distrib = total_sum = period_sum = false;
	output_flag = cost_flag = period_flag = active_flag = false;
	units_factor = Dtime (1.0, MINUTES);
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Difference_Data::Clear (void)
{
	num_periods = -1;
	summary_array.clear ();
	diff_distrib.clear ();
}

//---------------------------------------------------------
//	Report_Flags -- set report flags
//---------------------------------------------------------

bool Difference_Data::Report_Flags (bool total, bool distrib, bool tot_sum, bool per_sum)
{
	total_distrib = total;
	period_distrib = distrib;
	total_sum = tot_sum;
	period_sum = per_sum;

	if (cost_flag) {
		units_factor = 100.0;
		num_distribution = NUM_DISTRIBUTION * 6;
		min_difference = MIN_DIFFERENCE * 6;
	} else {
		units_factor = Dtime (1.0, MINUTES);
		num_distribution = NUM_DISTRIBUTION;
		min_difference = MIN_DIFFERENCE;
	}
	Initialize ();

	return (total_distrib || period_distrib || total_sum || period_sum);
}

//---------------------------------------------------------
//	Initialize -- allocate memory
//---------------------------------------------------------

void Difference_Data::Initialize (void)
{
	int num;	
	Packed_Integers int_data;
	PInts_Itr int_itr;
	Summary_Data sum_data;

	Clear ();

	active_flag = (total_distrib || period_distrib || total_sum || period_sum || output_flag);
	if (!active_flag) return;

	num_periods = periods.Num_Periods ();

	if (period_distrib || period_sum || output_flag) {
		num = num_periods + 1;
	} else {
		num = 1;
	}
	memset (&sum_data, '\0', sizeof (sum_data));

	summary_array.assign (num, sum_data);

	diff_distrib.assign (num, int_data);

	for (int_itr = diff_distrib.begin (); int_itr != diff_distrib.end (); int_itr++) {
		int_itr->assign (num_distribution, 0);
	}
}

//---------------------------------------------------------
//	Set_Periods -- set time periods 
//---------------------------------------------------------

bool Difference_Data::Set_Periods (Time_Periods &per)
{
	period_flag = periods.Copy_Periods (per);

	Initialize ();

	return (period_flag);
}

//---------------------------------------------------------
//	Open_Distribution -- difference distribution file
//---------------------------------------------------------

bool Difference_Data::Open_Distribution (string filename)
{
	if (!filename.empty ()) {
		output_flag = true;

		exe->Print (1);
		distrib_file.File_Type (String ("New %s Distribution File") % ((cost_flag) ? "Cost" : "Time"));

		if (!distrib_file.Create (exe->Project_Filename (filename))) {
			exe->File_Error ("Creating New Distribution File", distrib_file.Filename ());
			return (false);
		}
		Initialize ();
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Trip -- add data to the summary periods
//---------------------------------------------------------

void Difference_Data::Add_Trip (int tod, int current, int base)
{
	if (num_periods < 0 || current < 0 || base < 0) return;

	if (period_flag) {
		tod = periods.Period (tod);
		if (tod < 0) return;
	} else {
		tod = num_periods;
	}
	int diff = current - base;

	Summary_Data *sum_ptr;

	sum_ptr = &summary_array [num_periods]; 

	sum_ptr->num_diff++;
	sum_ptr->current += current;
	sum_ptr->base += base;
	sum_ptr->abs_diff += abs (diff);
	if (diff > sum_ptr->max_diff || sum_ptr->num_diff == 1) sum_ptr->max_diff = diff;
	if (diff < sum_ptr->min_diff || sum_ptr->num_diff == 1) sum_ptr->min_diff = diff;

	//---- period summary ----

	if (period_sum && tod < num_periods) {
		sum_ptr = &summary_array [tod];

		sum_ptr->num_diff++;
		sum_ptr->current += current;
		sum_ptr->base += base;
		sum_ptr->abs_diff += abs (diff);
		if (diff > sum_ptr->max_diff || sum_ptr->num_diff == 1) sum_ptr->max_diff = diff;
		if (diff < sum_ptr->min_diff || sum_ptr->num_diff == 1) sum_ptr->min_diff = diff;
	}

	//---- difference distribution ----

	if (diff < 0) {
		diff = -(int)(-diff / units_factor) - 1;
	} else if (diff > 0) {
		diff = (int)(diff / units_factor) + 1;
	}
	diff -= min_difference;

	if (diff < 0) diff = 0;
	if (diff >= num_distribution) diff = num_distribution - 1;

	diff_distrib [num_periods] [diff]++;

	if ((period_distrib || period_sum) && tod < num_periods) {
		diff_distrib [tod] [diff]++;
	}
}

//---------------------------------------------------------
//	Distribution_Range
//---------------------------------------------------------

void Difference_Data::Distribution_Range (int period, double percent, double &low, double &high)
{
	int i, j, total, start, end, num, cum0, cum1, cum2;

	low = high = 0.0;
	if (period < 0 || period > num_periods) return;

	//---- get the distribution total ----

	total = end = 0;
	start = num_distribution;

	for (i=0; i < num_distribution; i++) {
		num = diff_distrib [period] [i];
		if (num > 0) {
			total += num;
			if (start > i) start = i;
			end = i;
		}
	}
	if (total == 0) return;

	//--- calculate the percent criteria ----

	cum0 = (int) (total * (100.0 - percent) / 200.0 + 0.5);

	//---- get the lower end of the range ----

	cum1 = 0;

	for (i=start; i <= end; i++) {
		num = diff_distrib [period] [i];
		cum2 = cum1 + num;

		if (cum2 >= cum0) {
			j = i + min_difference;

			if (j < 0) {
				low = j + (double) (cum0 - cum1) / num;
			} else if (j > 0) {
				low = j - 1 + (double) (cum0 - cum1) / num;
			}
			break;
		}
		cum1 = cum2;
	}

	//---- get the higher end of the range ----

	cum1 = 0;

	for (i=end; i >= start; i--) {
		num = diff_distrib [period] [i];
		cum2 = cum1 + num;

		if (cum2 >= cum0) {
			j = i + min_difference;

			if (j < 0) {
				high = j + 1 - (double) (cum0 - cum1) / num;
			} else if (j > 0) {
				high = j - (double) (cum0 - cum1) / num;
			}
			break;
		}
		cum1 = cum2;
	}
}

//---------------------------------------------------------
//	Distribution_Report
//---------------------------------------------------------

void Difference_Data::Distribution_Report (int number, bool total_flag)
{
	int i, total, num, start, end, max, max_row, first, last, max_lines, low85, high85;
	double percent, low, high;
	string flag;
	string stars = "************************************************************";

	//---- set the period range ----

	if (total_flag) {
		if (!total_distrib) return;
		first = last = num_periods;
	} else {
		if (!period_distrib) return;
		first = 0;
		last = num_periods - 1;
	}

	//---- print each time period's distribution ----

	for (period = first; period <= last; period++) {
		total = end = max = max_row = 0;
		start = num_distribution;
		
		for (i=0; i < num_distribution; i++) {
			num = diff_distrib [period] [i];
			if (num > 0) {
				total += num;
				if (start > i) start = i;
				if (end < i) end = i;
				if (num > max) {
					max = num;
				}
			}
		}
		if (total == 0) continue;

		num = 0;

		for (i=start; i <= end; i++) {
			num += diff_distrib [period] [i];
			if (2 * num >= total) {
				max_row = i;
				break;
			}
		}
		scaling_factor = 100.0 * max / (60.0 * total);

		//---- find the 85th percentile rows ----

		Distribution_Range (period, 85.0, low, high);

		if (low < 0.0) {
			low85 = -((int) (-low + 0.5));
		} else {
			low85 = (int) (low + 0.5);
		}
		if (high < 0.0) {
			high85 = -((int) (-high + 0.5));
		} else {
			high85 = (int) (high + 0.5);
		}

		//---- limit output to one page ----

		max_lines = exe->Page_Lines () - 8;

		if (start < max_row - max_lines / 2) {
			start = max_row - max_lines / 2;
		}
		if (start + max_lines <= end) {
			end = start + max_lines - 1;
		}

		//---- print the report ----

		exe->Header_Number (number);
		exe->New_Page ();

		for (i=start; i <= end; i++) {
			num = i + min_difference;
			percent = 100.0 * diff_distrib [period] [i] / total;
			max = (int) (percent / scaling_factor + 0.5);

			flag = "  ";
			if (num == low85) flag = ">=";
			if (num == high85) flag = "<=";
			if (i == max_row) flag = "==";

			exe->Print (1, String ("  %4d %2s %8d %6.1lf%2s%*.*s") % num % flag % 
				diff_distrib [period] [i] % percent % 
				((num == 0) ? "--" : "  ") % max % max % stars);
		}
		exe->Header_Number (0);
	}
}

//---------------------------------------------------------
//	Distribution Header
//---------------------------------------------------------

void Difference_Data::Distribution_Header (bool total_flag)
{
	String buffer;

	if (total_flag) {
		buffer = "All Time Periods";
	} else {
		if (period >= num_periods) return;
		buffer = periods.Range_Format (period);
	}
	exe->Print (1, String ("%s Difference Distribution for %s") % ((cost_flag) ? "Cost" : "Time") % buffer);
	exe->Print (1);
	exe->Print (1, String ("Difference%16c----------------- Percent of Total Trips -------------------") % BLANK);
	exe->Print (1, String ("(%s)   Trips Percent ") % ((cost_flag) ? "dollars" : "minutes"));

	for (int i=1; i <= 12; i++) {
		exe->Print (0, String ("%5d") % (int) (i * 5 * scaling_factor + 0.5));
	}
	exe->Print (1);
}

/*********************************************|***********************************************

	Time Difference Distribution for %s
                            
	Difference                ----------------- Percent of Total Trips -------------------
	(minutes)   Trips Percent     5   10   15   20   25   35   45   50   55   60   65   70
	(dollars)

	   ddd <= ddddddd  fff.fssccccccccccccccccccccccccccccccccccc60ccccccccccccccccccccccc
      
**********************************************|***********************************************/ 

//---------------------------------------------------------
//	Period_Summary
//---------------------------------------------------------

void Difference_Data::Period_Summary (int number)
{
	int period;
	double low, high, factor, diff, base;
	String buffer;

	Summary_Data *sum_ptr;

	//---- print the report ----

	exe->Header_Number (number);

	if (!exe->Break_Check (num_periods + 6)) {
		exe->Print (1);
		Period_Header ();
	}

	for (period=0; period <= num_periods; period++) {

		if (period == num_periods) {
			exe->Print (1);
			buffer = "Total";
		} else {
			buffer = periods.Range_Format (period);
		}
		sum_ptr = &summary_array [period];

		Distribution_Range (period, 85.0, low, high);

		base = sum_ptr->base;
		diff = sum_ptr->current - base;
		if (base <= 0.0) base = 1.0;

		if (sum_ptr->num_diff < 1) {
			factor = 1.0;
		} else {
			factor = 1.0 / (units_factor * sum_ptr->num_diff);
		}
		exe->Print (1, String ("%-12.12s %9d %8.2lf%8.2lf %8.2lf%8.2lf%8.2lf%8.2lf %8.2lf%8.2lf") %
			buffer % sum_ptr->num_diff % (sum_ptr->current * factor) % (sum_ptr->base * factor) %
			(diff * factor) % (100.0 * diff / base) % (sum_ptr->abs_diff * factor) % 
			(100.0 * sum_ptr->abs_diff / base) % low % high);
	}
	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Period Header
//---------------------------------------------------------

void Difference_Data::Period_Header (void)
{
	exe->Print (1, String ("Time Period Summary (%s)") % ((cost_flag) ? "Dollars" : "Minutes"));
	exe->Print (2, String ("%16cNumber  --Travel %s--  ------- %s Difference -------  -- 85%% Range --") % BLANK %
		((cost_flag) ? "Cost" : "Time") % ((cost_flag) ? "Cost" : "Time") % FINISH);
	exe->Print (1, "Time Period      Trips   Current  Base   Average Percent Absolute Percent   Low     High");
	exe->Print (1);
}

/*********************************************|***********************************************

	Time Period Summary (Minutes) (Dollars)

	                Number  --Travel Time--  ------- Time Difference -------  -- 85% Range --
					Number  --Travel Cost--  ------- Cost Difference -------  -- 85% Range --
	Time Period     Trips   Current  Base   Average Percent Absolute Percent   Low     High

	dd:dd..dd:dd ddddddddd  ffff.ff ffff.ff  ffff.ff ffff.ff ffff.ff ffff.ff  ffff.ff ffff.ff
	
	Total        ddddddddd  ffff.ff ffff.ff  ffff.ff ffff.ff ffff.ff ffff.ff  ffff.ff ffff.ff
      
**********************************************|***********************************************/ 

//---------------------------------------------------------
//	Total_Summary
//---------------------------------------------------------

void Difference_Data::Total_Summary (void)
{
	if (num_periods < 0) return;

	double base, diff, factor, low, high, fac;
	string units, measure, total;
	Summary_Data *sum_ptr;

	sum_ptr = &summary_array [num_periods];

	//---- print the comparison summary ----

	base = sum_ptr->base;
	diff = sum_ptr->current - base;
	if (base == 0.0) base = 1.0;

	if (cost_flag) {
		fac = 100.0;
		measure = "Cost";
		units = "dollars";
		total = "dollars";
	} else {
		fac = Dtime (1.0, HOURS);
		measure = "Time";
		units = "minutes";
		total = "hours";
	}
	if (sum_ptr->num_diff < 1) {
		factor = 1.0;
	} else {
		factor = 1.0 / (units_factor * sum_ptr->num_diff);
	}
	Distribution_Range (0, 85.0, low, high);

	exe->Break_Check (10);

	exe->Print (2, "Number of Trips Compared = ") << sum_ptr->num_diff;
	exe->Print (1, String ("Average Input Travel %s = %.2lf %s") % measure % (sum_ptr->current * factor) % units);
	exe->Print (1, String ("Average Compare Travel %s = %.2lf %s") % measure % (sum_ptr->base * factor) % units);
	exe->Print (1, String ("Average Travel %s Difference = %.2lf %s (%.2lf%%)") %
		measure % (diff * factor) % units % (100.0 * diff / base) % FINISH);
	exe->Print (1, String ("Average Absolute Difference = %.2lf %s (%.2lf%%)") % 
		(sum_ptr->abs_diff * factor) % units % (100.0 * sum_ptr->abs_diff / base) % FINISH);
	exe->Print (1, String ("Minimum %s Difference = %.2lf %s") % measure % (sum_ptr->min_diff / units_factor) % units);
	exe->Print (1, String ("Maximum %s Difference = %.2lf %s") % measure % (sum_ptr->max_diff / units_factor) % units);
	exe->Print (1, String ("85th Percentile Range = %.2lf to %.2lf %s") % low % high % units);
	exe->Print (1, String ("Total Absolute Difference = %g %s") % (sum_ptr->abs_diff / fac) % total);
	exe->Print (1, String ("Total User Benefit = %g %s") % (diff / fac) % total);
}

//---------------------------------------------------------
//	Write_Distribution
//---------------------------------------------------------

void Difference_Data::Write_Distribution (void)
{
	if (!output_flag) return;

	int i, j, k, start, end, num_out;
	String buffer;

	exe->Show_Message (String ("Writing %s -- Record") % distrib_file.File_Type ());
	exe->Set_Progress ();

	fstream &file = distrib_file.File ();

	//---- get the data range ----

	num_out = end = 0;
	start = num_distribution;
	
	for (i=0; i < num_distribution; i++) {
		k = diff_distrib [num_periods] [i];
		if (k > 0) {
			num_out += k;
			if (start > i) start = i;
			if (end < i) end = i;
		}
	}
	if (num_out == 0) {
		exe->End_Progress ();
		return;
	}

	//---- print the header ----

	file << ((cost_flag) ? "DOLLARS" : "MINUTES");

	if (num_periods > 1) {
		file << "\tTOTAL";
	}
	for (i=0; i < num_periods; i++) {
		file << (String ("\tP%s") % periods.Range_Label (i));
	}
	file << endl;

	//---- print each record ----
	
	num_out = 0;

	for (j=start; j <= end; j++) {
		exe->Show_Progress ();

		file << (j + min_difference);

		if (num_periods > 1) {
			file << "\t" << diff_distrib [num_periods] [j];
		}
		for (i=0; i < num_periods; i++) {
			file << "\t" << diff_distrib [i] [j];
		}
		file << endl;
		num_out++;
	}
	exe->End_Progress ();

	distrib_file.Close ();

	exe->Print (2, String ("Number of %s Records = %d") % distrib_file.File_Type () % num_out);
}

//---------------------------------------------------------
//	Replicate -- replicated the data structures
//---------------------------------------------------------

void Difference_Data::Replicate (Difference_Data &data)
{
	if (data.active_flag) {
		if (data.period_flag) {
			Set_Periods (data.periods);
		}
		output_flag = data.Output_Flag ();
		cost_flag = data.Cost_Flag ();

		Report_Flags (data.total_distrib, data.period_distrib, data.total_sum, data.period_sum);
	}
}

//---------------------------------------------------------
//	Merge_Data -- copy data structures
//---------------------------------------------------------

void Difference_Data::Merge_Data (Difference_Data &data)
{
	if (!active_flag || !data.active_flag || num_periods == 0) return;

	int i, j;
	Summary_Itr sum_itr;
	Summary_Data *sum_ptr;
	PInts_Itr int_itr;
	Integers *int_ptr;
	Int_Itr itr;

	for (i=0, sum_itr = summary_array.begin (); sum_itr != summary_array.end (); sum_itr++, i++) {
		sum_ptr = &data.summary_array [i];

		sum_itr->num_diff += sum_ptr->num_diff;
		sum_itr->current += sum_ptr->current;
		sum_itr->base += sum_ptr->base;
		sum_itr->abs_diff += sum_ptr->abs_diff;

		if (sum_ptr->max_diff > sum_itr->max_diff || sum_itr->num_diff == 1) sum_itr->max_diff = sum_ptr->max_diff;
		if (sum_ptr->min_diff < sum_itr->min_diff || sum_itr->num_diff == 1) sum_itr->min_diff = sum_ptr->min_diff;
	}
	for (i=0, int_itr = diff_distrib.begin (); int_itr != diff_distrib.end (); int_itr++, i++) {
		int_ptr = &data.diff_distrib [i];

		for (j=0, itr = int_itr->begin (); itr != int_itr->end (); itr++, j++) {
			*itr += int_ptr->at (j);
		}
	}
}
