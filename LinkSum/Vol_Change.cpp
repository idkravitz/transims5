//*********************************************************
//	Vol_Change.cpp - Create Volume Change Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Volume_Change
//---------------------------------------------------------

void LinkSum::Volume_Change (void)
{
	int i, j, k, k1, bin, percent, max_lanes, lanes, tod_list, index, flow_index;
	int percentile [NUM_PERCENTILES];
	double total, sum, lane_miles, len, flow, old_flow;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Array *period_ptr;
	Flow_Time_Data flow_data;
	Lane_Use_Period *use_ptr;
	Doubles_Itr itr;

	Show_Message ("Summarize Volume Changes -- Record");
	Set_Progress ();
	
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

			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;
			max_lanes = lanes;

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);
				flow = flow_data.Flow ();

				period_ptr = &compare_link_array [j];

				flow_data = period_ptr->Total_Flow_Time (index, flow_index);
				old_flow = flow_data.Flow ();

				if (old_flow < minimum_flow) continue;
				if (old_flow == 0.0) old_flow = 0.1;

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
				bin = DTOI (abs (DTOI (flow - old_flow)) * 10 * RESOLUTION / old_flow);

				if (bin < 0 || bin >= NUM_SUM_BINS) bin = NUM_SUM_BINS - 1;
				
				lane_miles = lanes * len;

				sum_bin [j] [bin] += lane_miles;
				sum_bin [num_inc] [bin] += lane_miles;
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (VOLUME_CHANGE);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Volume_Change_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			total += sum_bin [i] [j];
		}
		if (total == 0.0) continue;

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));
		}

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			sum += sum_bin [i] [j];

			percent = (int) (sum * 100.0 / total + 0.5);

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
			Print (0, String (" %6.1lf") % ((double) percentile [k] / 10.0));
		}
		if (Metric_Flag ()) {
			total /= 1000.0;
		} else {
			total /= MILETOFEET;
		}
		Print (0, String ("  %7.0lf") % total);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Volume_Change_Header
//---------------------------------------------------------

void LinkSum::Volume_Change_Header (void)
{
	int i;

	Print (1, "Volume Change Distribution");
	Print (2, String ("%30cVolume Change * 100 / Previous Volume") % BLANK);
	Print (1, String ("%15c-------------Percentile Distribution by Time Period----------     Lane") % BLANK);
	Print (1, "Time Period   ");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, String ("  %3d%% ") % percent_break [i] % FINISH);
	}
	if (Metric_Flag ()) {
		Print (0, "     KM");
	} else {
		Print (0, "     MI");
	}
	Print (1);
}

/*********************************************|***********************************************

	Volume Change Distribution

                                 Volume Change * 100 / Previous Volume
	               -------------Percentile Distribution by Time Period----------    Lane
 	Time Period     50%    60%    65%    70%    75%    80%    85%    95%    99%      KM
	
	xx:xx..xx:xx   dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  ddddddd

	Total 

**********************************************|***********************************************/ 
