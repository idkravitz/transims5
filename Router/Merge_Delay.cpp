//*********************************************************
//	Merge_Delay.cpp - merge link delay data
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Merge_Delay
//---------------------------------------------------------

double Router::Merge_Delay (double factor, bool zero_flag)
{
	int period, record, gap_period, last_period;
	Dtime time, merge_time;
	double flow, merge_flow, factor1, gap, vht, diff, merge_vht, diff_vht, sum_vht;
	double period_diff, period_sum;
	bool factor_flag, gap_flag, report_flag;

	Flow_Time_Itr record_itr;
	Flow_Time_Data *record_ptr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Array *period_ptr;
	Gap_Data gap_data;
	//Dir_Data *dir_ptr;
	//Connect_Data *connect_ptr;

	last_period = -1;
	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	
	report_flag = Report_Flag (LINK_GAP);
	memset (&gap_data, '\0', sizeof (gap_data));

	gap_flag = (link_gap_flag || link_gap > 0.0 || report_flag);

	//---- merge link delay files ----

	factor_flag = (factor != 0.0);
	factor1 = factor + 1.0;

	//---- process each time period ----

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {

		//---- check for a new summary time period ----

		if (gap_flag) {
			gap_period = sum_periods.Period (link_delay_array.periods->Period_Time (period));

			if (gap_period != last_period) {
				diff_vht += period_diff;
				sum_vht += period_sum;

				if (last_period >= 0) {
					if (period_sum > 0.0) {
						gap = period_diff / period_sum;
					} else if (period_diff > 0.0) {
						gap = 1.0;
					} else {
						gap = 0.0;
					}
					if (link_gap_flag) link_gap_file.File () << "\t" << gap;
					if (report_flag && gap > gap_data.max_gap) gap_data.max_gap = gap;
				}
				period_diff = period_sum = 0.0;
				last_period = gap_period;
			}
		}

		//---- process each link direction ----

		period_ptr = &old_link_array [period];

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			
			record_ptr = period_ptr->Data_Ptr (record);

			flow = record_itr->Flow ();
			time = record_itr->Time ();
		
			merge_flow = record_ptr->Flow ();
			merge_time = record_ptr->Time ();

			if (gap_flag) {
				vht = flow * time;
				merge_vht = merge_flow * merge_time;
				diff = fabs (vht - merge_vht);
				period_diff += diff;
				period_sum += vht;

				if (report_flag) {
					gap_data.count++;
					gap_data.diff += diff;
					gap_data.diff_sq += diff * diff;
					gap_data.total += vht;
				}
			}
			if (factor_flag) {
				flow = (merge_flow * factor + flow) / factor1;
				time = (int) ((merge_time * factor + (double) time) / factor1 + 0.5);

				record_itr->Flow (flow);
				record_itr->Time (time);
			}
			record_ptr->Flow (flow);
			record_ptr->Time (time);

			if (zero_flag) record_itr->Flow (0.0);
		}
	}

	//---- add the turning movement delays ----

	if (Turn_Flows ()) {

		for (period=0, period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++, period++) {

			period_ptr = &old_turn_array [period];

			//---- process each turn ----

			for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
				
				record_ptr = period_ptr->Data_Ptr (record);

				flow = record_itr->Flow ();
				time = record_itr->Time ();
				
				merge_flow = record_ptr->Flow ();
				merge_time = record_ptr->Time ();

				if (gap_flag) {
					vht = flow * time;
					merge_vht = merge_flow * merge_time;
					diff = fabs (vht - merge_vht);
					period_diff += diff;
					period_sum += vht;

					if (report_flag) {
						gap_data.count++;
						gap_data.diff += diff;
						gap_data.diff_sq += diff * diff;
						gap_data.total += vht;
					}
				}
				if (factor_flag) {
					flow = (merge_flow * factor + flow) / factor1;
					time = (int) ((merge_time * factor + (double) time) / factor1 + 0.5);

					record_itr->Flow (flow);
					record_itr->Time (time);
				}
				record_ptr->Flow (flow);
				record_ptr->Time (time);

				if (zero_flag) record_itr->Flow (0.0);
			}
		}
	}

	//---- process the last summary time period ----

	if (gap_flag) {
		diff_vht += period_diff;
		sum_vht += period_sum;

		if (last_period >= 0) {
			if (period_sum > 0.0) {
				gap = period_diff / period_sum;
			} else if (period_diff > 0.0) {
				gap = 1.0;
			} else {
				gap = 0.0;
			}
			if (link_gap_flag) link_gap_file.File () << "\t" << gap;
			if (report_flag && gap > gap_data.max_gap) gap_data.max_gap = gap;
		}
	}
	if (sum_vht > 0.0) {
		gap = diff_vht / sum_vht;
	} else if (diff_vht > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}
	if (link_gap_flag) {
		link_gap_file.File () << "\t" << gap << endl;
	}
	if (report_flag) {
		link_gap_array.push_back (gap_data);
	}
	return (gap);
}

