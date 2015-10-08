//*********************************************************
//	VC_Report.cpp - Create Volume Capacity Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Volume_Capacity_Report
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Report (void)
{
	int i, j, k, k1, vc, percent, max_lanes, lanes, tod_list, index, flow_index;
	int percentile [NUM_PERCENTILES];
	double capacity, cap, total, sum, lane_miles, len, flow;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Data flow_data;
	Lane_Use_Period *use_ptr;
	Doubles_Itr itr;

	Show_Message ("Creating the V/C Ratio Distribution -- Record");
	Set_Progress ();

	//---- clear the summary bins -----

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		len = UnRound (link_itr->Length ());

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

			capacity = dir_ptr->Capacity ();

			if (capacity < 20) capacity = 1000.0;
			cap = capacity *= cap_factor;

			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;
			max_lanes = lanes;

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				flow = flow_data.Flow ();
				if (flow < minimum_flow) continue;

				if (tod_list >= 0) {

					//---- get the time period ----

					sum_periods.Period_Range (j, low, high);
					tod = (low + high + 1) / 2;

					cap = capacity;
					lanes = max_lanes;
					k = tod_list;

					for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
							cap = capacity * lanes / max_lanes;
							break;
						}
						if (use_ptr->Periods () == 0) break;
					}
				}

				//---- save the vc ratio ----

				vc = DTOI (flow * RESOLUTION / cap);
				if (vc < 0 || vc >= NUM_SUM_BINS) vc = NUM_SUM_BINS - 1;

				lane_miles = lanes * len;

				sum_bin [j] [vc] += lane_miles;
				sum_bin [num_inc] [vc] += lane_miles;
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (VC_RATIOS);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Volume_Capacity_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));
		}

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			total += sum_bin [i] [j];
		}
		if (total == 0.0) continue;

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			sum += sum_bin [i] [j];

			percent = (int) (sum * 100 / total + 0.5);

			for (k=k1; k < NUM_PERCENTILES; k++) {
				if (percent_break [k] <= percent) {
					percentile [k] = j;
					k1 = k + 1;
				} else {
					break;
				}
			}
		}

		//---- print the percentiles ----

		for (k=0; k < NUM_PERCENTILES; k++) {
			Print (0, String (" %6.2lf") % ((double) percentile [k] / RESOLUTION));
		}
		Print (0, String ("  %7.0lf") % (total / 1000.0));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Volume_Capacity_Header
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Header (void)
{
	int i;

	Print (1, "Volume Capacity Ratio Distribution");
	Print (2, String ("%31cVolume / Capacity Ratio") % BLANK);
	Print (1, String ("%15c-------------Percentile Distribution by Time Period----------     Lane") % BLANK);
	Print (1, "Time Period   ");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, String ("  %3d%% ") % percent_break [i] % FINISH);
	}
	Print (0, "     KM");
	Print (1);
}

/*********************************************|***********************************************

	Volume Capacity Ratio Distribution

                                   Volume / Capacity Ratio
	               -------------Percentile Distribution by Time Period----------    Lane
 	Time Period     50%    60%    65%    70%    75%    80%    85%    95%    99%      KM
	
	xx:xx..xx:xx   dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  ddddddd

	Total       
**********************************************|***********************************************/ 
