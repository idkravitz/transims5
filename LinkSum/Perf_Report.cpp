//*********************************************************
//	Perf_Report.cpp - network performance report
//*********************************************************

#include "LinkSum.hpp"

#define LANE_MILES	0
#define VMT			1
#define VHT			2
#define VHD			3
#define TURNS		4

//---------------------------------------------------------
//	Performance_Report
//---------------------------------------------------------

void LinkSum::Performance_Report (void)
{
	int i, j, k, lanes, max_lanes, tod_list, index, flow_index;
	double base_time, loaded_time, len, flow, factor;
	Dtime low, high, tod;
	bool connect_flag;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Period_Itr turn_itr;
	Flow_Time_Data flow_data, *turn_ptr;
	Lane_Use_Period *use_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;

	Show_Message ("Creating the Network Performance Summary Report -- Record");
	Set_Progress ();

	//---- clear the summary bins -----

	for (j=0, itr = sum_bin.begin (); itr != sum_bin.end (); itr++, j++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	
	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0);

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		len = UnRound (link_itr->Length ());

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];

			base_time = dir_ptr->Time0 ();
			if (base_time <= 0.0) continue;

			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;
			max_lanes = lanes;

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				loaded_time = flow_data.Time ();
				flow = flow_data.Flow ();

				if (tod_list >= 0) {

					//---- get the time period ----

					sum_periods.Period_Range (j, low, high);
					tod = (low + high + 1) / 2;

					lanes = max_lanes;
					k = tod_list;

					for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
							break;
						}
						if (use_ptr->Periods () == 0) break;
					}
				}
				sum_bin [j] [LANE_MILES] += lanes * len;
				sum_bin [j] [VMT] += flow * len;
				sum_bin [j] [VHT] += flow * loaded_time;
				sum_bin [j] [VHD] += flow * (loaded_time - base_time);
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					for (j=0, turn_itr = turn_perf_array.begin (); turn_itr != turn_perf_array.end (); turn_itr++, j++) {
						turn_ptr = &turn_itr->at (k);

						sum_bin [j] [TURNS] += turn_ptr->Flow ();
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (PERF_REPORT);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Performance_Header ();
	}
	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
	} else {
		factor = 1.0 / MILETOFEET;
	}
	tod.Hours (1);

	for (i=0; i <= num_inc; i++) {
		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			if (sum_bin [i] [VHT] == 0.0) continue;

			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));

			sum_bin [num_inc] [LANE_MILES] = MAX (sum_bin [i] [LANE_MILES], sum_bin [num_inc] [LANE_MILES]);
			sum_bin [num_inc] [VMT] += sum_bin [i] [VMT];
			sum_bin [num_inc] [VHT] += sum_bin [i] [VHT];
			sum_bin [num_inc] [VHD] += sum_bin [i] [VHD];
			sum_bin [num_inc] [TURNS] += sum_bin [i] [TURNS];
		}
		len = (sum_bin [i] [VMT] * factor);
		base_time = sum_bin [i] [VHT] / tod;
		if (base_time == 0.0) {
			loaded_time = len;
		} else {
			loaded_time = len / base_time;
		}
		Print (0, String ("%12.2lf %13.2lf %13.2lf %9.2lf %13.2lf %10d") % 
			(sum_bin [i] [LANE_MILES] * factor) % len % base_time % 
			loaded_time % (sum_bin [i] [VHD] / tod) % ((int) (sum_bin [i] [TURNS])));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Performance_Header
//---------------------------------------------------------

void LinkSum::Performance_Header (void)
{
	String units;

	if (Metric_Flag ()) {
		units = "Kilometers";
	} else {
		units = "Miles";
	}
	Print (1, "Network Performance Summary");
	Print (2, String ("                   Lane        Vehicle       Vehicle %10.10s     Hours of      Number") % units);
	Print (1, String ("Time Period   %10.10s    %10.10s        Hours       /Hour       Delay      of Turns") % units % units);
	Print (1);
}

/*********************************************|***********************************************

	Network Performance Summary

	                  Lane        Vehicle       Vehicle  KilomMiles    Hours of      Number
	Time Period  KilomMiles    KilomMiles        Hours        /Hour      Delay      of Turns
	
	xx:xx..xx:xx fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

	Total        fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

**********************************************|***********************************************/ 
