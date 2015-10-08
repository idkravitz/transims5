//*********************************************************
//	Link_Group.cpp - Create a Link Group Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Link_Group
//---------------------------------------------------------

void PlanSum::Link_Group (double min_vc)
{
	int i, num, dir, link, cap, tod_cap, tod_cap2, lns, lanes, period, periods, index, flow_index;
	Dtime low, high;
	double flow, vc;
    string label;
	bool flag;

	Flow_Time_Period_Itr array_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Lane_Use_Period *period_ptr;
	Int_Set *list;
	Int_Set_Itr list_itr;
	Int_Map_Itr int_itr;

	Group_Array group_array;
	Group_Data zero, *data_ptr;

	memset (&zero, '\0', sizeof (zero));
	periods = sum_periods.Num_Periods ();

	Show_Message ("Writing the Link Group Report -- Record");
	Set_Progress ();

	minimum_vc = min_vc;

	//---- find V/C ratios for each link group ----

	num = link_equiv.Num_Groups ();

	Header_Number (LINK_GROUP);

	if (!Break_Check (num + 7)) {
		Print (1);
		Link_Group_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		list = link_equiv.Group_List (i);
		if (list == 0) continue;

		label = link_equiv.Group_Label (i);

		flag = false;
		group_array.assign (periods, zero);

		//---- process each link in the link group ----

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {

			link = abs (*list_itr);
			
			int_itr = link_map.find (link);
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];

			if (*list_itr < 0) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}
			if (dir < 0) continue;
			
			//---- get the directional data ----

			dir_ptr = &dir_array [dir];

			cap = (int) (dir_ptr->Capacity () * cap_factor + 0.5);
			if (cap <= 0) continue;

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;

			flow_index = dir_ptr->Flow_Index ();

			//---- process each time period ----

			for (period=0, array_itr = link_delay_array.begin (); array_itr != link_delay_array.end (); array_itr++, period++) {
				flow = array_itr->Flow (dir);

				//---- time-of-day capacity ----

				tod_cap = tod_cap2 = cap;
				index = dir_ptr->First_Lane_Use ();

				if (index >= 0) {
					sum_periods.Period_Range (period, low, high);
					low = (low + high) / 2;

					for (period_ptr = &use_period_array [index]; ; period_ptr = &use_period_array [++index]) {
						if (period_ptr->Start () <= low && low < period_ptr->End ()) {
							lns = period_ptr->Lanes (0);
							tod_cap = (tod_cap * lns + lanes / 2) / lanes;
							if (tod_cap == 0) tod_cap = cap / 2;

							lns = period_ptr->Lanes (1);
							tod_cap2 = (tod_cap2 * lns + lanes / 2) / lanes;
							if (tod_cap2 == 0) tod_cap2 = cap / 2;
							break;
						}
						if (period_ptr->Periods () == 0) break;
					}
				}
				if (tod_cap > 0) {
					data_ptr = &group_array [period];

					flag = true;
					data_ptr->links++;
					data_ptr->volume += DTOI (flow);
					data_ptr->capacity += tod_cap;

					if (flow_index >= 0 && tod_cap2 > 0) {
						flow = array_itr->Flow (flow_index);

						data_ptr->volume += DTOI (flow);
						data_ptr->capacity += tod_cap2;
					}
				}
			}
		}
		if (!flag) continue;

		//---- print the link group data ----

		flag = true;

		for (period=0; period < periods; period++) {
			data_ptr = &group_array [period];

			if (data_ptr->capacity <= 0) continue;

			vc = (double) data_ptr->volume / data_ptr->capacity;

			if (vc > min_vc) {
				Show_Progress ();

				//---- print the data record ----

				if (flag) {
					flag = false;
					Print (1, label);
				}
				Print (1, String ("%20c%5d   %7d  %12.12s  %7d   %6.2lf") %
					BLANK % data_ptr->links % data_ptr->capacity % 
					sum_periods.Range_Format (period) % data_ptr->volume % vc);
			}
		}
	}
	End_Progress ();

	if (Progress_Count () > 0) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Group_Header
//---------------------------------------------------------

void PlanSum::Link_Group_Header (void)
{
	Print (1, String ("Link Group V/C Ratios Greater Than %.2lf") % minimum_vc);
	Print (2, "Link Group          Links  Capacity   Time-of-Day   Volume  V/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Link Group V/C Ratios Greater Than ff.ff

	Link Group          Links  Capacity   Time-of-Day   Volume  V/C Ratio

	ssssssssssssssssssssssssssssssssssssssss	
	                    ddddd   ddddddd  dd:dd..dd:dd  ddddddd    ff.ff  
	                            ddddddd  dd:dd..dd:dd  ddddddd    ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
