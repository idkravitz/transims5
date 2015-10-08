//*********************************************************
//	Perf_File.cpp - network performance data file
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
//	Perf_Detail_File
//---------------------------------------------------------

void LinkSum::Perf_Detail_File (void)
{
	int i, j, k, k1, lanes, max_lanes, tod_list, index, flow_index;
	int text_field, value_field;
	double base_time, loaded_time, length, len, value, percent, flow, factor;
	String buffer, units, vmt, lane_mi;
	bool connect_flag;
	Dtime low, high, tod;

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

	Show_Message ("Writing Performance Data File -- Record");
	Set_Progress ();

	text_field = detail_file.Field_Number ("MEASURE");
	value_field = detail_file.Field_Number ("VALUE");

	//---- clear the summary bins -----

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}
	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0) && (compare_turn_array.size () > 0);

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
			flow_index = dir_ptr->Flow_Index ();

			base_time = dir_ptr->Time0 ();
			if (base_time <= 0.0) continue;

			tod_list = dir_ptr->First_Lane_Use ();

			max_lanes = dir_ptr->Lanes ();
			if (max_lanes < 1) max_lanes = 1;
			lanes = max_lanes;

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
				sum_bin [j] [LANES] += lanes * len;
				sum_bin [j] [VMT] += flow * len;
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

					sum_bin [j] [VMT+PREV] += flow * len;
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

	//---- write the data ----
	
	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
		units = "Kilometers";
		vmt = "VKT";
		lane_mi = "km)";
	} else {
		factor = 1.0 / MILETOFEET;
		units = "Miles";
		vmt = "VMT";
		lane_mi = "mi)";
	}
	tod.Hours (1);

	for (i=0; i <= num_inc; i++) {
		len = sum_bin [i] [LINKS];
		if (len == 0.0) continue;

		detail_file.Put_Field (text_field, buffer);

		if (i == num_inc) {
			buffer = "Time Period Total";
			len *= num_inc;
		} else {
			buffer = String ("Time Period %12.12s") % sum_periods.Range_Format (i);

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
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, 0.0);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Number of Links"));
		detail_file.Put_Field (value_field, sum_bin [i] [LINKS]);
		detail_file.Write ();

		buffer = "Number of Roadway " + units;
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, sum_bin [i] [LENGTH] * factor);
		detail_file.Write ();

		buffer = "Number of Lane " + units;
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, sum_bin [i] [LANES] * factor);
		detail_file.Write ();

		buffer = "Vehicle " + units + " of Travel";
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, sum_bin [i] [VMT] * factor);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Vehicle Hours of Travel"));
		detail_file.Put_Field (value_field, sum_bin [i] [VHT] / tod);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Vehicle Hours of Delay"));
		detail_file.Put_Field (value_field, sum_bin [i] [VHD] / tod);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Number of Queued Vehicles"));
		detail_file.Put_Field (value_field, UnRound (sum_bin [i] [QUEUE]));
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Maximum Queued Vehicles"));
		detail_file.Put_Field (value_field, sum_bin [i] [MAX_QUEUE]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Number of Cycle Failures"));
		detail_file.Put_Field (value_field, sum_bin [i] [FAILURE]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Number of Turning Movements"));
		detail_file.Put_Field (value_field, sum_bin [i] [TURNS]);
		detail_file.Write ();

		detail_file.Put_Field (text_field, String ("Average Link Time Ratio"));
		detail_file.Put_Field (value_field, sum_bin [i] [TIME_RATIO] / len);
		detail_file.Write ();

		buffer = "Average Link Density (/ln-" + lane_mi;
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, UnRound (sum_bin [i] [DENSITY] / len));
		detail_file.Write ();

		buffer = "Maximum Link Density (/ln-" + lane_mi;
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, UnRound (sum_bin [i] [MAX_DEN]));
		detail_file.Write ();

		length = sum_bin [i] [VMT] * factor;
		loaded_time = sum_bin [i] [VHT] / tod;
		if (loaded_time == 0.0) {
			loaded_time = length;
		} else {
			loaded_time = length / loaded_time;
		}
		buffer = "Average " + units + " Per Hour";
		detail_file.Put_Field (text_field, buffer);
		detail_file.Put_Field (value_field, loaded_time);
		detail_file.Write ();

		if (Ratio_Flag ()) {
			value = sum_bin [i] [VMT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VMT] / value;

			if (Metric_Flag ()) {
				buffer = "Percent VKT Congested";
			} else {
				buffer = "Percent VMT Congested";
			}
			detail_file.Put_Field (text_field, buffer);
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();

			value = sum_bin [i] [VHT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_VHT] / value;

			detail_file.Put_Field (text_field, String ("Percent VHT Congested"));
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();

			value = sum_bin [i] [COUNT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [i] [CONG_TIME] / value;

			detail_file.Put_Field (text_field, String ("Percent Time Congested"));
			detail_file.Put_Field (value_field, percent);
			detail_file.Write ();
		}
	}
}
