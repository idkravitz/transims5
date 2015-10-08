//*********************************************************
//	Group_Time.cpp - Create a Link Group Travel Time Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Group_Time_Report
//---------------------------------------------------------

void LinkSum::Group_Time_Report (void)
{
	int i, j, num, lnk, dir, link, hour, minute, second, time, time0, ttime;
	int in_link, in_link_dir, out_link_dir, base_time, connect_index, flow_index;
    String label;
	bool connect_flag;
	Dtime low, high;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Perf_Array *period_ptr;
	Flow_Time_Array *turn_period_ptr;
	Flow_Time_Data flow_data, *turn_ptr;
	Doubles group_data;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;

	Show_Message ("Creating the Travel Time Report -- Record");
	Set_Progress ();

	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0);

	//---- calculate group travel time ----

	num = link_equiv.Num_Groups ();

	Header_Number (TRAVEL_TIME);

	if (!Break_Check (num * num_inc + 7)) {
		Print (1);
		Group_Time_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == NULL) continue;

		label = link_equiv.Group_Label (i);

		in_link_dir = in_link = base_time = 0;
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
			if (dir == 0) continue;
			
			//---- get the directional data ----

			dir_ptr = &dir_array [dir];

			flow_index = dir_ptr->Flow_Index ();
			
			time0 = dir_ptr->Time0 ();
			time = 0;
			base_time += time0;

			//---- get the turn delay data ----

			connect_index = -1;

			if (connect_flag) {
				out_link_dir = dir_ptr->Link_Dir ();

				if (in_link_dir > 0) {
					map2_itr = connect_map.find (Int2_Key (in_link_dir, out_link_dir));

					if (map2_itr == connect_map.end ()) {
						Warning (String ("A Connection between %d and %d was Not Found") % in_link % link);
					} else {
						connect_index = map2_itr->second;
					}
				}
				in_link_dir = out_link_dir;
				in_link = link;
			}

			//---- process each time period ----
		
			for (j=0; j < num_inc; j++) {
				sum_periods.Period_Range (j, low, high);

				second = low + group_data [j] + 1;

				if (connect_index >= 0) {
					turn_period_ptr = turn_perf_array.Period_Ptr (second);
					turn_ptr = turn_period_ptr->Data_Ptr (connect_index);
					ttime = turn_ptr->Time ();
					group_data [j] += ttime;
					second += ttime;
				}
				period_ptr = link_perf_array.Period_Ptr (second);
				flow_data = period_ptr->Total_Flow_Time (dir, flow_index);
				time = flow_data.Time ();
				group_data [j] += ((time > 0) ? time : time0);
			}
		}

		//---- print the link group data ----

		for (j=0; j < num_inc; j++) {
			Show_Progress ();

			//---- print the start time ----

			sum_periods.Period_Range (j, low, high);

			time = DTOI (low.Seconds ());

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (1, String ("%4d  %-40.40s  %2d:%02d:%02d") % i % label % hour % minute % second);

			//---- print the base time ----

			time = Resolve (base_time);

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (0, String ("    %2d:%02d:%02d") % hour % minute % second);

			//---- print the travel time ----

			time = Resolve (group_data [j]);

			hour = time / 3600;
			second = hour * 3600;
			minute = (time - second) / 60;
			second = time - (minute * 60) - second; 

			Print (0, String ("    %2d:%02d:%02d") % hour % minute % second);
		}
		Print (1);
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Time_Header
//---------------------------------------------------------

void LinkSum::Group_Time_Header (void)
{
	Print (1, "Link Group Travel Time Report");
	Print (2, "Group Description                              Start Time  Base Time  Travel Time");
	Print (1);
}

/*********************************************|***********************************************

	Link Group Travel Time Report

	Group Description                              Start Time  Base Time  Travel Time

	dddd  ssssssssssssssssssssssssss40ssssssssssss  dd:dd:dd    dd:dd:dd    dd:dd:dd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
