//*********************************************************
//	Top_100.cpp - Create a Top 100 Flow Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void LinkSum::Top_100_Report (int type)
{
	int i, j, k, anode, bnode, an, bn, max_tim, max_an, max_bn;
	int tod_list, lanes, max_lanes, index, flow_index;
	double flow, min_flow, max_flow;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Data flow_time;
	Lane_Use_Period *use_ptr;

	//---- Top 100 Data ----

	typedef struct {
		int    link;
		int    from;
		int    to;
		int    period;
		double flow;
	} Flow_Data;

	Flow_Data flow_data [100], *flow_ptr;

	Show_Message ("Creating the Top 100 Flow Report -- Record");
	Set_Progress ();

	memset (flow_data, '\0', 100 * sizeof (Flow_Data));

	//---- find Top 100 Flows ----

	min_flow = 0;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		an = node_array [link_itr->Anode ()].Node ();
		bn = node_array [link_itr->Bnode ()].Node ();

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
				anode = bn;
				bnode = an;
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
				anode = an;
				bnode = bn;
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];

			max_flow = max_tim = max_an = max_bn = 0;

			max_lanes = lanes = dir_ptr->Lanes ();
			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			//---- scan each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_time = period_itr->Total_Flow_Time (index, flow_index);

				flow = flow_time.Flow ();
				if (flow < minimum_flow) continue;

				if (type == TOP_PERIOD) {
					if (flow <= min_flow) continue;

					flow_ptr = flow_data;

					for (k=0; k < 100; k++, flow_ptr++) {
						if (flow > flow_ptr->flow) {
							if (flow_ptr->flow > 0 && k < 99) {
								memmove (flow_ptr+1, flow_ptr, (99-k) * sizeof (Flow_Data));
								min_flow = flow_data [99].flow;
							}
							flow_ptr->link = link_itr->Link ();
							flow_ptr->from = anode;
							flow_ptr->to = bnode;
							flow_ptr->period = j;
							flow_ptr->flow = flow;
							break;
						}
					}
				} else {
					if (type == TOP_LANE_FLOW) {
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
						if (lanes > 1) flow /= lanes;
					}
					flow = DTOI (flow);

					if (flow > max_flow) {
						max_flow = flow;
						max_tim = j;
						max_an = anode;
						max_bn = bnode;
					}
				}
			}

			//---- add to the list ----

			if (max_flow > min_flow) {
				flow_ptr = flow_data;

				for (k=0; k < 100; k++, flow_ptr++) {
					if (max_flow > flow_ptr->flow) {
						if (flow_ptr->flow > 0 && k < 99) {
							memmove (flow_ptr+1, flow_ptr, (99-k) * sizeof (Flow_Data));							
							min_flow = flow_data [99].flow;
						}
						flow_ptr->link = link_itr->Link ();
						flow_ptr->from = max_an;
						flow_ptr->to = max_bn;
						flow_ptr->period = max_tim;
						flow_ptr->flow = max_flow;
						break;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	flow_ptr = flow_data;

	Header_Number (type);
	New_Page ();

	for (i=0; i < 100; i++, flow_ptr++) {
		if (flow_ptr->flow <= 0.0) break;

		//---- print the data record ----

		Print (1, String ("%10d%10d%10d  %12.12s%12.1lf") % flow_ptr->link % flow_ptr->from % flow_ptr->to % 
			sum_periods.Range_Format (flow_ptr->period) % flow_ptr->flow);
	}
	if (i) {
		Print (2, "Number of Records in the Report = ") << i;
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Link_Header
//---------------------------------------------------------

void LinkSum::Top_100_Link_Header (void)
{
	Print (1, "Top 100 Link Flow Report");
	Print (2, "      Link      From   To-Node   Time-of-Day        Flow");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Lane_Header
//---------------------------------------------------------

void LinkSum::Top_100_Lane_Header (void)
{
	Print (1, "Top 100 Lane Flow Report");
	Print (2, "      Link      From   To-Node   Time-of-Day        Flow");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Header (void)
{
	Print (1, "Top 100 Period Flow Report");
	Print (2, "      Link      From   To-Node   Time-of-Day        Flow");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Flow Report

	      Link      From   To-Node   Time-of-Day        Flow

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd  dddddddd.d

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
