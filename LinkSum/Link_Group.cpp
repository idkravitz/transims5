//*********************************************************
//	Link_Group.cpp - Create a Link Group Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Link_Group
//---------------------------------------------------------

void LinkSum::Link_Group (double min_volume)
{
	int i, j, num, lnk, dir, link, flow_index, group_num;
	String label;

	Link_Data *link_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Data flow_data;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Doubles group_data;

	Show_Message ("Creating the Link Group Report -- Record");
	Set_Progress ();

	header_value = DTOI (min_volume);

	//---- find events for each link group ----

	num = link_equiv.Num_Groups ();

	Header_Number (LINK_GROUP);

	if (!Break_Check (num + 7)) {
		Print (1);
		Link_Group_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == 0) continue;

		label = link_equiv.Group_Label (i);

		group_num = 0;
		group_data.assign (num_inc, 0);

		//---- process each link in the link group ----

		for (itr = group->begin (); itr != group->end (); itr++) {

			link = *itr;
			lnk = abs (link);

			map_itr = link_map.find (lnk);
			if (map_itr == link_map.end ()) continue;

			link_ptr = &link_array [map_itr->second];

			if (link < 0) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}
			if (dir < 0) continue;
			flow_index = dir_array [dir].Flow_Index ();

			group_num++;

			//---- get the directional data ----
		
			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (dir, flow_index);

				group_data [j] += flow_data.Flow ();
			}
		}
		if (group_num == 0) continue;

		//---- print the link group data ----

		for (j=0; j < num_inc; j++) {
			if (group_data [j] < min_volume) continue;

			Show_Progress ();

			//---- print the data record ----

			Print (1, String ("%4d  %-40.40s  %5d  %12.12s%9d") % 
				i % label % group_num % sum_periods.Range_Format (j) % group_data [j]);
		}
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Group_Header
//---------------------------------------------------------

void LinkSum::Link_Group_Header (void)
{
	Print (1, String ("Link Group Volume Greater Than %.0lf") % header_value);
	Print (2, "Group Description                               Links   Time-of-Day   Volume");
	Print (1);
}

/*********************************************|***********************************************

	Link Group Volume Greater Than dddddddd

	Group Description                               Links   Time-of-Day   Volume

	dddd  ssssssssssssssssssssssssss40ssssssssssss  ddddd  dd:dd..dd:dd  ddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
