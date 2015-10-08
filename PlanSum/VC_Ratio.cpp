//*********************************************************
//	VC_Ratio.cpp - Create a V/C Ratio Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	VC_Ratio
//---------------------------------------------------------

void PlanSum::VC_Ratio (double min_vc)
{
	int dir, index, period, cap, tod_cap, num, lanes, from, to, group, size;
	Dtime low, high;
	double flow, vc;

	Flow_Time_Period_Itr period_itr;
	Flow_Time_Itr data_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Lane_Use_Period *period_ptr;

	Show_Message ("Writing a V/C Ratio Report -- Records");
	Set_Progress ();

	minimum_vc = min_vc;

	Header_Number (VC_RATIO);
	New_Page ();

	//---- find V/C ratios greater than min_vc ----

	size = (int) dir_array.size ();

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {
		
		link_delay_array.periods->Period_Range (period, low, high);
		low = (low + high) / 2;

		for (dir=0, data_itr = period_itr->begin (); data_itr != period_itr->end (); data_itr++, dir++) {
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

			vc = flow / tod_cap;
			if (vc < min_vc) continue;

			Show_Progress ();

			from = to = 0;

			link_ptr = &link_array [dir_ptr->Link ()];
			node_ptr = &node_array [link_ptr->Anode ()];
			if (dir_ptr->Dir () == 0) {
				from = node_ptr->Node ();
			} else {
				to = node_ptr->Node ();
			}
			node_ptr = &node_array [link_ptr->Bnode ()];
			if (dir_ptr->Dir () == 0) {
				to = node_ptr->Node ();
			} else {
				from = node_ptr->Node ();
			}

			//---- print the data record ----

			Print (1, String ("%8d  %8d  %8d   %7d  %12.12s  %7.0lf   %6.2lf") %
				link_ptr->Link () % from % to % tod_cap %
				sum_periods.Range_Format (period) % flow % vc);
		}
	}
	End_Progress ();

	if (Progress_Count () > 0) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	VC_Ratio_Header
//---------------------------------------------------------

void PlanSum::VC_Ratio_Header (void)
{
	Print (1, String ("All V/C Ratios Greater Than %.2lf") % minimum_vc);
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	All V/C Ratios Greater Than ff.ff

 	    Link      From   To-Node  Capacity   Time-of-Day   Volume  V/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
