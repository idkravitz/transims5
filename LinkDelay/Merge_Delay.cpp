//*********************************************************
//	Merge_Delay.cpp - merge link delay data
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Merge_Delay
//---------------------------------------------------------

void LinkDelay::Merge_Delay (void)
{
	int period, record, index, rec;
	Dtime time, merge_time;
	double flow, merge_flow, factor1;
	bool factor_flag;

	Flow_Time_Itr record_itr;
	Flow_Time_Data *record_ptr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Array *period_ptr;
	Dir_Data *dir_ptr;
	Connect_Data *connect_ptr;

	//---- merge link delay files ----

	Show_Message ("Merging Link Delay Data -- Record");
	Set_Progress ();

	if (method == SIMPLE_LINK_AVG) {
		factor = 1.0;
		factor1 = 2.0;
		factor_flag = false;
	} else {
		factor_flag = (factor == 0.0);
		factor1 = factor + 1.0;
	}

	//---- process each time period ----

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {

		period_ptr = &merge_link_array [period];

		//---- process each link direction ----

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			Show_Progress ();

			flow = record_itr->Flow ();

			if (method == REPLACE_LINKS && flow > 0.0) continue;

			record_ptr = period_ptr->Data_Ptr (record);

			if (method == REPLACE_LINKS || (method == REPLACE_AVERAGE && flow == 0.0)) {
				flow = record_ptr->Flow ();
				time = record_ptr->Time ();
			} else {
				time = record_itr->Time ();
				merge_flow = record_ptr->Flow ();
				merge_time = record_ptr->Time ();

				if (factor_flag) {
					if (flow > 0) {
						factor = merge_flow / flow;
					} else {
						factor = merge_flow;
					}
					factor1 = factor + 1.0;
				}
				flow = (merge_flow * factor + flow) / factor1;
				time = DTOI ((merge_time * factor + (double) time) / factor1);
			}
			record_itr->Flow (flow);
			record_itr->Time (time);
		}
	}
	if (!turn_flag) return;

	//---- process each time period ----

	for (period=0, period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++, period++) {

		period_ptr = &merge_turn_array [period];

		//---- process each turning movement ----

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			Show_Progress ();

			flow = record_itr->Flow ();

			if (method == REPLACE_LINKS && flow > 0.0) continue;

			record_ptr = &period_ptr->at (record);

			if (method == REPLACE_LINKS || (method == REPLACE_AVERAGE && flow == 0.0)) {
				flow = record_ptr->Flow ();
				time = record_ptr->Time ();
			} else {
				time = record_itr->Time ();
				merge_flow = record_ptr->Flow ();
				merge_time = record_ptr->Time ();

				if (factor_flag) {
					if (flow > 0) {
						factor = merge_flow / flow;
					} else {
						factor = merge_flow;
					}
					factor1 = factor + 1.0;
				}
				flow = (merge_flow * factor + flow) / factor1;
				time = DTOI ((merge_time * factor + (double) time) / factor1);
			}
			record_itr->Flow (flow);
			record_itr->Time (time);
		}
	}
	End_Progress ();

	//---- apply max travel time ratio ----

	if (!ratio_flag) return;

	Show_Message ("Checking Travel Time Ratio -- Record");
	Set_Progress ();

	factor_flag = System_File_Flag (CONNECTION);

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {

		//---- process each link direction ----

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			Show_Progress ();

			if (record < (int) dir_array.size ()) {
				rec = record;
			} else {
				rec = lane_use_flow_index [record - (int) dir_array.size ()];
			}
			dir_ptr = &dir_array [rec];
			
			time = dir_ptr->Time0 ();
			if (time <= 0) continue;

			factor1 = (double) record_itr->Time () / time;
			if (factor1 <= time_ratio) continue;

			factor1 = time_ratio / factor1;

			record_itr->Time (DTOI (record_itr->Time () * factor1));

			if (factor_flag) {
				period_ptr = &turn_delay_array [period];

				for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [index];

					record_ptr = &period_ptr->at (index);

					if (record_ptr->Time () != 0) {
						record_ptr->Time (DTOI (record_ptr->Time () * factor1));
					}
				}
			}
		}
	}
	End_Progress ();
}

