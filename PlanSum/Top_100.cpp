//*********************************************************
//	Top_100.cpp - Create a Top 100 V/C Ratio Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void PlanSum::Top_100_Report (void)
{
	int i, vc, dir, index, period, cap, tod_cap, num, lanes, from, to, group, size;
	Dtime low, high;
	double flow;

	Flow_Time_Period_Itr period_itr;
	Flow_Time_Itr data_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Lane_Use_Period *period_ptr;

	typedef struct {
		int  vc;
		int  period;
		int  link;
		int  dir;
		int  volume;
		int  capacity;
	} VC_Data;

	VC_Data vc_data [100], *vc_ptr;

	Show_Message ("Creating the Top 100 V/C Ratio Report");
	Set_Progress ();

	memset (vc_data, '\0', 100 * sizeof (VC_Data));
	size = (int) dir_array.size ();

	//---- find the highest V/C ratios ----

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {
		
		link_delay_array.periods->Period_Range (period, low, high);
		low = (low + high) / 2;

		for (dir=0, data_itr = period_itr->begin (); data_itr != period_itr->end (); data_itr++, dir++) {
			Show_Progress ();

			flow = data_itr->Flow ();
			if (flow == 0.0) continue;

			if (dir < size) {
				index = dir;
				group = 0;
			} else {
				index = lane_use_flow_index [dir - size];
				group = 1;
			}
			dir_ptr = &dir_array [index];

			//---- time of day capacity ----

			cap = (int) (dir_ptr->Capacity () * cap_factor + 0.5);
			if (cap <= 0) continue;
			tod_cap = cap;

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;

			index = dir_ptr->First_Lane_Use ();
			if (index >= 0) {
				for (period_ptr = &use_period_array [index]; ; period_ptr = &use_period_array [++index]) {
					if (period_ptr->Start () <= low && low < period_ptr->End ()) {
						num = period_ptr->Lanes (group);
						tod_cap = (tod_cap * num + lanes / 2) / lanes;
						if (tod_cap == 0) tod_cap = cap / 2;
						break;
					}
					if (period_ptr->Periods () == 0) break;
				}
			}

			//---- check the V/C ratio value ----

			vc = DTOI (flow * 100.0 / tod_cap);

			vc_ptr = vc_data;

			for (i=0; i < 100; i++, vc_ptr++) {
				if (vc > vc_ptr->vc) {
					if (vc_ptr->vc > 0 && i < 99) {
						memmove (vc_ptr+1, vc_ptr, (99-i) * sizeof (VC_Data));
					}
					vc_ptr->vc = vc;
					vc_ptr->period = period;
					vc_ptr->link = dir_ptr->Link ();
					vc_ptr->dir = dir_ptr->Dir ();
					vc_ptr->volume = DTOI (flow);
					vc_ptr->capacity = tod_cap;
					break;
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	vc_ptr = vc_data;

	Header_Number (TOP_100);

	if (!Break_Check (100 + 7)) {
		Print (1);
		Top_100_Header ();
	}
	from = to = 0;

	for (i=0; i < 100; i++, vc_ptr++) {
		if (vc_ptr->vc == 0) break;

		link_ptr = &link_array [vc_ptr->link];
		node_ptr = &node_array [link_ptr->Anode ()];
		if (vc_ptr->dir == 0) {
			from = node_ptr->Node ();
		} else {
			to = node_ptr->Node ();
		}
		node_ptr = &node_array [link_ptr->Bnode ()];
		if (vc_ptr->dir == 0) {
			to = node_ptr->Node ();
		} else {
			from = node_ptr->Node ();
		}

		//---- print the data record ----

		Print (1, String ("%8d  %8d  %8d   %7d  %12.12s  %7d   %6.2lf") %
			link_ptr->Link () % from % to % vc_ptr->capacity % 
			sum_periods.Range_Format (vc_ptr->period) % 
			vc_ptr->volume % ((double) vc_ptr->vc / 100.0));
	}
	if (i) {
		Print (2, "Number of Records in the Report = ") << i;
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Header
//---------------------------------------------------------

void PlanSum::Top_100_Header (void)
{
	Print (1, "Top 100 V/C Ratios Report");
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 V/C Ratios Report

	    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
