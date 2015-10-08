//*********************************************************
//	Relative_Gap_Report.cpp - relative gap by time of day
//*********************************************************

#include "LinkSum.hpp"

#include <math.h>

#define VHT			0
#define VHD			1
#define VMT			2
#define VMT2		3

//---------------------------------------------------------
//	Relative_Gap_Report
//---------------------------------------------------------

void LinkSum::Relative_Gap_Report (void)
{
	int i, j, k, index, flow_index;
	double flow, flow2, loaded_time, factor, speed, speed2;
	bool connect_flag;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Array *period_ptr;
	Flow_Time_Period_Itr turn_itr;
	Flow_Time_Array *turn_ptr;
	Flow_Time_Data flow_data;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;

	Show_Message ("Creating the Relative Gap Report -- Record");
	Set_Progress ();

	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0) && (compare_turn_array.size () > 0);

	//---- clear the summary bins -----

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	
	factor = (double) Dtime (1, HOURS);

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		for (i=0; i < 2; i++) {
			if (i == 1) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];
			flow_index = dir_ptr->Flow_Index ();

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				flow = flow_data.Flow ();
				loaded_time = flow_data.Time ();

				sum_bin [j] [VHT] += flow * loaded_time;
				sum_bin [j] [VMT] += flow * link_itr->Length ();

				period_ptr = &compare_link_array [j];

				flow_data = period_ptr->Total_Flow_Time (index, flow_index);

				flow2 = flow_data.Flow ();
				loaded_time = flow_data.Time ();

				//sum_bin [j] [VHT] += flow * loaded_time;
				sum_bin [j] [VHD] += flow2 * loaded_time;

				//sum_bin [j] [VMT] += flow * link_itr->Length ();
				sum_bin [j] [VMT2] += flow2 * link_itr->Length ();
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					for (j=0, turn_itr = turn_perf_array.begin (); turn_itr != turn_perf_array.end (); turn_itr++, j++) {
						flow_data = turn_itr->at (k);

						flow = flow_data.Flow ();
						loaded_time = flow_data.Time ();

						sum_bin [j] [VHT] += flow * loaded_time;

						turn_ptr = &compare_turn_array [j];

						flow_data = turn_ptr->at (k);

						flow2 = flow_data.Flow ();
						loaded_time = flow_data.Time ();

						//sum_bin [j] [VHT] += flow * loaded_time;
						sum_bin [j] [VHD] += flow2 * loaded_time;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (RELATIVE_GAP);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Relative_Gap_Header ();
	}

	for (k=0; k <= num_inc; k++) {

		if (k == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (k));

			sum_bin [num_inc] [VHT] += sum_bin [k] [VHT];
			sum_bin [num_inc] [VHD] += sum_bin [k] [VHD];

			sum_bin [num_inc] [VMT] += sum_bin [k] [VMT];
			sum_bin [num_inc] [VMT2] += sum_bin [k] [VMT2];
		}
		flow = sum_bin [k] [VHT];
		flow2 = sum_bin [k] [VHD];

		if (flow > 0) {
			loaded_time = (flow2 - flow) / flow;
		} else {
			loaded_time = 0;
		}
		if (flow > 0) {
			speed = sum_bin [k] [VMT] / flow;
		} else {
			speed = 0.0;
		}
		if (flow2 > 0) {
			speed2 = sum_bin [k] [VMT2] / flow2;
		} else {
			speed2 = 0.0;
		}
		Print (0, String ("  %14.1lf   %14.1lf  %13.6lf") % (flow / factor) % (flow2 / factor) % loaded_time);

		flow = External_Units (sum_bin [k] [VMT], (Metric_Flag () ? KILOMETERS : MILES));
		flow2 = External_Units (sum_bin [k] [VMT2], (Metric_Flag () ? KILOMETERS : MILES));

		Print (0, String ("  %14.1lf   %14.1lf   %10.3lf   %10.3lf   %10.3lf") % flow % flow2 % speed % speed2 % (speed - speed2));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Relative_Gap_Header
//---------------------------------------------------------

void LinkSum::Relative_Gap_Header (void)
{
	Print (1, "Relative Gap Report");
	Print (2, "Time Period      Current VHT     Previous VHT    Relative Gap");
	Print (0, "     Current VMT     Previous VMT  Current Speed  Previous Speed  Difference");
	Print (1);

}

/*********************************************|***********************************************

	Relative Gap Report

	Time Period      Current VHT     Previous VHT    Relative Gap         
	
	xx:xx..xx:xx   ffffffffffff.f   ffffffffffff.f  ffffff.ffffff  

	Total          ffffffffffff.f   ffffffffffff.f  ffffff.ffffff  

**********************************************|***********************************************/ 

