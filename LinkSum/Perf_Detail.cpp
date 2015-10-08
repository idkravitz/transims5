//*********************************************************
//	Perf_Detail.cpp - network performance details
//*********************************************************

#include "LinkSum.hpp"

#define LINKS		0
#define LENGTH		1
#define LANES		2
#define MAX_DEN		3
#define MAX_QUEUE	4
#define VMT			5
#define VHT			6
#define VHD			7
#define TIME_RATIO	8
#define DENSITY		9
#define QUEUE		10
#define FAILURE		11
#define TURNS		12
#define CONG_VMT	13
#define CONG_VHT	14
#define CONG_TIME	15
#define COUNT		16
#define PREV		14

//---------------------------------------------------------
//	Perf_Detail_Report
//---------------------------------------------------------

void LinkSum::Perf_Detail_Report (void)
{
	int i, j, k, k1, lanes, max_lanes, tod_list, index, flow_index;
	double base_time, loaded_time, length, len, base, diff, value, percent, flow, factor;
	Dtime low, high, tod;
	bool connect_flag;
	String units, vmt, lane_mi;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Array *period_ptr;
	Link_Perf_Data perf_data;
	Flow_Time_Period_Itr turn_itr;
	Flow_Time_Array *compare_ptr;
	Flow_Time_Data *turn_ptr;
	Lane_Use_Period *use_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;

	Show_Message ("Creating the Network Performance Details Report -- Record");
	Set_Progress ();

	//---- clear the summary bins -----

	for (j=0, itr = sum_bin.begin (); itr != sum_bin.end (); itr++, j++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	
	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0) && (compare_turn_array.size () > 0);

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		length = UnRound (link_itr->Length ());
		len = 0;

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
			flow_index = dir_ptr->Flow_Index ();

			base_time = dir_ptr->Time0 ();
			if (base_time <= 0.0) continue;

			tod_list = dir_ptr->First_Lane_Use ();

			max_lanes = dir_ptr->Lanes ();
			if (max_lanes < 1) max_lanes = 1;
			lanes = max_lanes;

			if (len == 0) {
				len = length;
			} else {
				len = 0;
			}

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
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
				perf_data = period_itr->Total_Link_Perf (index, flow_index);

				loaded_time = perf_data.Time ();
				flow = perf_data.Flow ();

				sum_bin [j] [LINKS] += 1;
				sum_bin [j] [LENGTH] += len;
				sum_bin [j] [LANES] += lanes * length;
				sum_bin [j] [VMT] += flow * length;
				sum_bin [j] [VHT] += flow * loaded_time;
				sum_bin [j] [VHD] += flow * (loaded_time - base_time);
				sum_bin [j] [TIME_RATIO] += (double) loaded_time / base_time;
				sum_bin [j] [DENSITY] += perf_data.Density ();
				sum_bin [j] [MAX_DEN] = MAX (sum_bin [j] [MAX_DEN], perf_data.Max_Density ());
				sum_bin [j] [QUEUE] += perf_data.Queue ();
				sum_bin [j] [MAX_QUEUE] = MAX (sum_bin [j] [MAX_QUEUE], perf_data.Max_Queue ());
				sum_bin [j] [FAILURE] += perf_data.Failure ();

				if (Ratio_Flag ()) {
					sum_bin [j] [CONG_VMT] += perf_data.Ratio_VMT ();
					sum_bin [j] [CONG_VHT] += perf_data.Ratio_VHT ();
					sum_bin [j] [CONG_TIME] += perf_data.Ratio_Count ();
					sum_bin [j] [COUNT] += perf_data.Occupancy ();
				}

				if (compare_flag) {
					period_ptr = &compare_link_array [j];
					perf_data = period_ptr->Total_Link_Perf (index, flow_index);

					loaded_time = perf_data.Time ();
					flow = perf_data.Flow ();

					sum_bin [j] [VMT+PREV] += flow * length;
					sum_bin [j] [VHT+PREV] += flow * loaded_time;
					sum_bin [j] [VHD+PREV] += flow * (loaded_time - base_time);
					sum_bin [j] [TIME_RATIO+PREV] += (double) loaded_time / base_time;
					sum_bin [j] [DENSITY+PREV] += perf_data.Density ();
					sum_bin [j] [MAX_DEN+PREV] = MAX (sum_bin [j] [MAX_DEN+PREV], perf_data.Max_Density ());
					sum_bin [j] [QUEUE+PREV] += perf_data.Queue ();
					sum_bin [j] [MAX_QUEUE+PREV] = MAX (sum_bin [j] [MAX_QUEUE+PREV], perf_data.Max_Queue ());
					sum_bin [j] [FAILURE+PREV] += perf_data.Failure ();

					if (Ratio_Flag ()) {
						sum_bin [j] [CONG_VMT+PREV] += perf_data.Ratio_VMT ();
						sum_bin [j] [CONG_VHT+PREV] += perf_data.Ratio_VHT ();
						sum_bin [j] [CONG_TIME+PREV] += perf_data.Ratio_Count ();
						sum_bin [j] [COUNT+PREV] += perf_data.Occupancy ();
					}
				}
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

						if (compare_flag) {
							compare_ptr = &compare_turn_array [j];
							turn_ptr = &compare_ptr->at (k);

							sum_bin [j] [TURNS+PREV] += turn_ptr->Flow ();
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (PERF_DETAIL);

	if (!Break_Check (num_inc * 16)) {
		Print (1);
		Perf_Detail_Header ();
	}
	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
		units = "Kilometers";
		vmt = "VKT";
		lane_mi = "km";
	} else {
		factor = 1.0 / MILETOFEET;
		units = "Miles";
		vmt = "VMT";
		lane_mi = "mi";
	}
	tod.Hours (1);

	for (i=0; i <= num_inc; i++) {
		len = sum_bin [i] [LINKS];
		if (len == 0.0) continue;

		if (i < num_inc) {
			for (k = 0; k <= COUNT; k++) {
				k1 = k + PREV;
				if (k < VMT) {
					sum_bin [num_inc] [k] = MAX (sum_bin [i] [k], sum_bin [num_inc] [k]);

					if (compare_flag) {
						sum_bin [num_inc] [k1] = MAX (sum_bin [i] [k1], sum_bin [num_inc] [k1]);
					}
				} else {
					sum_bin [num_inc] [k] += sum_bin [i] [k];

					if (compare_flag) {
						sum_bin [num_inc] [k1] += sum_bin [i] [k1];
					}
				}
			}
		}
		if (i < num_inc && sum_bin [i] [VHT] == 0.0 && (!compare_flag || sum_bin [i] [VHT+PREV] == 0.0)) continue;
		if (i) {
			if (!Break_Check ((Ratio_Flag () ? 19 : 16))) {
				Print (1);
			}
		}
		Print (1, String ("Time Period%22c") % BLANK);
		if (i == num_inc) {
			Print (0, "       Total");
			len *= num_inc;
		} else {
			Print (0, String ("%12.12s") % sum_periods.Range_Format (i));
		}
		Print (1, String ("Number of Links                 %13.2lf") % sum_bin [i] [LINKS]);
		Print (1, String ("Number of Roadway %-10.10s    %13.2lf") % units % (sum_bin [i] [LENGTH] * factor));
		Print (1, String ("Number of Lane %-10.10s       %13.2lf") % units % (sum_bin [i] [LANES] * factor));
		Print (1, String ("Vehicle %-20.20s    %13.2lf") % (units + " of Travel") % (sum_bin [i] [VMT] * factor));
		if (compare_flag) {
			base = sum_bin [i] [VMT+PREV] * factor;
			diff = sum_bin [i] [VMT] * factor - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Vehicle Hours of Travel         %13.2lf") % (sum_bin [i] [VHT] / tod));
		if (compare_flag) {
			base = sum_bin [i] [VHT+PREV] / tod;
			diff = sum_bin [i] [VHT] / tod - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}	
		Print (1, String ("Vehicle Hours of Delay          %13.2lf") % (sum_bin [i] [VHD] / tod));
		if (compare_flag) {
			base = sum_bin [i] [VHD+PREV] / tod;
			diff = sum_bin [i] [VHD] / tod - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Number of Queued Vehicles       %13.2lf") % UnRound (sum_bin [i] [QUEUE]));
		if (compare_flag) {
			base = sum_bin [i] [QUEUE+PREV];
			diff = sum_bin [i] [QUEUE] - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Maximum Queued Vehicles         %13.2lf") % sum_bin [i] [MAX_QUEUE]);
		if (compare_flag) {
			base = sum_bin [i] [MAX_QUEUE+PREV];
			diff = sum_bin [i] [MAX_QUEUE] - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Number of Cycle Failures        %13.2lf") % sum_bin [i] [FAILURE]);
		if (compare_flag) {
			base = sum_bin [i] [FAILURE+PREV];
			diff = sum_bin [i] [FAILURE] - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Number of Turning Movements     %13.2lf") % sum_bin [i] [TURNS]);
		if (compare_flag) {
			base = sum_bin [i] [TURNS+PREV];
			diff = sum_bin [i] [TURNS] - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Average Link Time Ratio         %13.2lf") % (sum_bin [i] [TIME_RATIO] / len));
		if (compare_flag) {
			base = sum_bin [i] [TIME_RATIO+PREV] / len;
			diff = sum_bin [i] [TIME_RATIO] / len - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Average Link Density (/ln-%s)   %13.2lf") % lane_mi % UnRound (sum_bin [i] [DENSITY] / len));
		if (compare_flag) {
			base = UnRound (sum_bin [i] [DENSITY+PREV] / len);
			diff = UnRound (sum_bin [i] [DENSITY] / len) - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		Print (1, String ("Maximum Link Density (/ln-%s)   %13.2lf") % lane_mi % UnRound (sum_bin [i] [MAX_DEN]));
		if (compare_flag) {
			base = UnRound (sum_bin [i] [MAX_DEN+PREV]);
			diff = UnRound (sum_bin [i] [MAX_DEN]) - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}
		length = sum_bin [i] [VMT] * factor;
		loaded_time = sum_bin [i] [VHT] / tod;
		if (loaded_time == 0.0) {
			loaded_time = length;
		} else {
			loaded_time = length / loaded_time;
		}
		Print (1, String ("Average %-19.19s     %13.2lf") % (units + " per Hour") % loaded_time);
		if (compare_flag) {
			length = sum_bin [i] [VMT+PREV] * factor;
			base = sum_bin [i] [VHT+PREV] / tod;
			if (base == 0.0) {
				base = length;
			} else {
				base = length / base;
			}
			diff = loaded_time - base;

			Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
		}

		if (Ratio_Flag ()) {
			value = sum_bin [i] [VMT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VMT] / value;

			Print (1, String ("Percent %s Congested           %13.2lf") % vmt % percent);
			if (compare_flag) {
				value = sum_bin [i] [VMT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_VMT+PREV] / value;
				diff = percent - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			value = sum_bin [i] [VHT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VHT] / value;

			Print (1, String ("Percent VHT Congested           %13.2lf") % percent);
			if (compare_flag) {
				value = sum_bin [i] [VHT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_VHT+PREV] / value;
				diff = percent - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			value = sum_bin [i] [COUNT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_TIME] / value;

			Print (1, String ("Percent Time Congested          %13.2lf") % percent);
			if (compare_flag) {
				value = sum_bin [i] [COUNT+PREV];
				if (value == 0.0) value = 1.0;
				base = 100.0 * sum_bin [i] [CONG_TIME+PREV] / value;
				diff = percent - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Perf_Detail_Header
//---------------------------------------------------------

void LinkSum::Perf_Detail_Header (void)
{
	Print (1, "Network Performance Details");
	Print (2, String ("%36cCurrent") % BLANK);
	if (compare_flag) {
		Print (0, "      Previous      Difference  Percent");
	}
	Print (1);
}

/*********************************************|***********************************************

	Network Performance Details
	
	                                    Current      Previous      Difference  Percent

	Time Period                      xx:xx..xx:xx
	Number of Links                  fffffffff.ff
	Number of Roadway Miles          fffffffff.ff
	Number of Lane Miles             fffffffff.ff
	Vehicle Miles of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Delay           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Maximum Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Cycle Failures         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Turning Movements      fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Time Ratio          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Density             fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Max Density         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Miles Per Hour           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VMT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VHT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of Time Congested        fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)

**********************************************|***********************************************/ 
