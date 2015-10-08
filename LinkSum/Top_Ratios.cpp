//*********************************************************
//	Top_Ratios.cpp - Create a Top 100 Ratios Report
//*********************************************************

#include "LinkSum.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Top_100_Ratios
//---------------------------------------------------------

void LinkSum::Top_100_Ratios (int type)
{
	int i, j, k, anode, bnode, an, bn, min_ratio, ratio, tod_list, lanes, lane;
	int load, base, cap, capacity, tim, vol, time0, len, index, flow_index;
	double flow;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Array *period_ptr;
	Flow_Time_Data flow_data;
	Lane_Use_Period *use_ptr;

	//---- Top 100 Ratio Report Data ----

	typedef struct {
		int link;
		int from;
		int to;
		int period;
		int base;
		int load;
		int ratio;
	} Ratio_Data;

	Ratio_Data ratios [100], *ptr;

	Show_Message ("Creating the Top 100 Ratios Report -- Record");
	Set_Progress ();

	memset (ratios, '\0', 100 * sizeof (Ratio_Data));

	//---- find Top 100 Ratios ----

	min_ratio = base = load = ratio = 0;

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

			len = link_itr->Length ();

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;

			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			time0 = dir_ptr->Time0 ();
			if (time0 == 0) continue;

			capacity = cap = DTOI (dir_ptr->Capacity () * cap_factor);

			//---- scan each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				flow = flow_data.Flow ();
				if (flow < minimum_flow) continue;

				tim = flow_data.Time ();
				vol = DTOI (flow);

				switch (type) {
					case TOP_SPEED:
						if (tim == 0) continue;
						base = (len * 10 + time0 / 2) / time0;
						load = (len * 10 + tim / 2) / tim;
						ratio = ((base - load) * 1000 + base / 2) / base;
						break;
					case TOP_TIME_RATIO:
						base = time0;
						load = tim;
						ratio = (load * 1000 + base / 2) / base;
						break;
					case TOP_VC_RATIO:
						if (tod_list >= 0) {
							sum_periods.Period_Range (j, low, high);

							tod = (low + high + 1) / 2;
							cap = capacity;
							k = tod_list;

							for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
								if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
									lane = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
									cap = capacity * lane / lanes;
									break;
								}
								if (use_ptr->Periods () == 0) break;
							}
						}
						base = cap;
						if (base == 0) continue;
						load = vol;
						ratio = (load * 1000 + base / 2) / base;
						break;
					case TOP_TIME_CHANGE:
						period_ptr = &compare_link_array [j];

						flow_data = period_ptr->Total_Flow_Time (index, flow_index);
						if (flow_data.Flow () < minimum_flow) continue;
						base = flow_data.Time ();
						if (base == 0) continue;
						load = tim;
						ratio = ((load - base) * 1000 + base / 2) / base;
						break;
					case TOP_VOL_CHANGE:
						period_ptr = &compare_link_array [j];

						flow_data = period_ptr->Total_Flow_Time (index, flow_index);
						base = DTOI (flow_data.Flow ());
						if (base < minimum_flow) continue;
						load = vol;
						ratio = ((load - base) * 1000 + base / 2) / base;
						break;
				} 
				
				if (ratio > min_ratio) {
					ptr = ratios;

					for (k=0; k < 100; k++, ptr++) {
						if (ratio > ptr->ratio) {
							if (ptr->ratio > 0 && k < 99) {
								memmove (ptr+1, ptr, (99-k) * sizeof (Ratio_Data));							
								min_ratio = ratios [99].ratio;
							}
							ptr->link = link_itr->Link ();
							ptr->from = anode;
							ptr->to = bnode;
							ptr->period = j;
							ptr->base = base;
							ptr->load = load;
							ptr->ratio = ratio;
							break;
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (type);
	New_Page ();

	ptr = ratios;

	for (i=0; i < 100; i++, ptr++) {
		if (ptr->ratio == 0) break;

		//---- print the data record ----

		Print (1, String ("%10ld%10ld%10ld  %12.12s   ") % ptr->link % ptr->from % ptr->to % 
			sum_periods.Range_Format (ptr->period));

		if (type == TOP_VC_RATIO) {
			Print (0, String ("%7d  %7d   %5.2lf") % ptr->base % ptr->load % (ptr->ratio / 1000.0));
		} else if (type == TOP_VOL_CHANGE) {
			Print (0, String ("%7d  %7d   %5.1lf") % ptr->base % ptr->load % (ptr->ratio / 10.0));
		} else {
			Print (0, String ("%7.1lf  %7.1lf   %5.1lf") % (ptr->base / 10.0) % (ptr->load / 10.0) % (ptr->ratio / 10.0));
		}
	}
	if (i) {
		Print (2, "Number of Records in the Report = ") << i;
	}
	Header_Number (0);
}

#pragma pack (pop)

//---------------------------------------------------------
//	Top_100_Speed_Header
//---------------------------------------------------------

void LinkSum::Top_100_Speed_Header (void)
{
	Print (1, "Top 100 Speed Reductions Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Travel_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Travel_Time_Header (void)
{
	Print (1, "Top 100 Travel Time Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_VC_Header
//---------------------------------------------------------

void LinkSum::Top_100_VC_Header (void)
{
	Print (1, "Top 100 Volume Capacity Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day  Capacity   Volume   Ratio");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Change_Header (void)
{
	Print (1, "Top 100 Travel Time Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Volume_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Volume_Change_Header (void)
{
	Print (1, "Top 100 Volume Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Speed Reductions Report

	      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd   ddddddd  ddddddd   ddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
