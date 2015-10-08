//*********************************************************
//	Rider_Group.cpp - Create a Transit Link Group Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Rider_Group
//---------------------------------------------------------

void PlanSum::Rider_Group (void)
{
	int i, j, num, dir, length, link, previous_period, period, dir_index, periods;
	String label;
	bool flag;

	Int_Set *group;
	Int_Set_Itr link_itr;
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Driver_Array *driver_ptr;
	Driver_Itr driver_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	Group_Data group_data, *group_ptr;
	Group_Array group_array;
	Group_Itr group_itr;

	memset (&group_data, '\0', sizeof (group_data));

	periods = sum_periods.Num_Periods ();
	if (periods < 1) periods= 1;

	Show_Message ("Writing Transit Link Groups -- Record");
	Set_Progress ();

	//---- find each link group ----

	num = link_equiv.Num_Groups ();

	Header_Number (RIDER_GROUP);

	if (!Break_Check (num + 7)) {
		Print (1);
		Rider_Header ();
	}
	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == 0) continue;

		group_array.assign (periods, group_data);

		label = link_equiv.Group_Label (i);

		flag = false;
		memset (&group_data, '\0', sizeof (group_data));

		//---- write data for each line ----

		for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			Show_Progress ();

			driver_ptr = &line_itr->driver_array;

			//---- process each link in the link group ----

			for (link_itr = group->begin (); link_itr != group->end (); link_itr++) {
				link = *link_itr;
				if (link < 0) {
					link = -link;
					dir = 1;
				} else {
					dir = 0;
				}
				link_ptr = &link_array [link];
				dir_index = (dir == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

				//---- search for the link on the route ----

				length = 0;

				for (driver_itr = driver_ptr->begin (); driver_itr != driver_ptr->end (); driver_itr++) {
					dir_ptr = &dir_array [*driver_itr];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (*driver_itr == dir_index) {
						length += link_ptr->Length () / 2;
						break;
					}
					length += link_ptr->Length ();
				}

				for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
					if (stop_itr->Length () > length) {
						if (stop_itr > line_itr->begin ()) {
							stop_itr--;
							previous_period = -1;

							for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
								period = sum_periods.Period (run_itr->Schedule ());
								if (period < 0) continue;

								group_ptr = &group_array [period];

								if (period != previous_period) {
									previous_period = period;
									group_ptr->lines++;
								}
								group_ptr->runs++;
								group_ptr->riders += run_itr->Load ();
							}
						}
						flag = true;
						break;
					}
				}
			}
		}
		if (!flag) continue;

		//---- print the link group data ----

		flag = true;

		for (j=0, group_itr = group_array.begin (); group_itr != group_array.end (); group_itr++, j++) {
			if (group_itr->lines == 0) continue;

			if (flag) {
				Print (1, String ("%3d %-40.40s") % i % label);
				flag = false;
			} else {
				Print (1, String ("%44c") % BLANK);
			}
			Print (0, String ("%5d %6d  %12.12s  %7d") %
				group_itr->lines % group_itr->runs %  
				sum_periods.Range_Format (j) % group_itr->riders);
		}
		if (j > 1) Print (1);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Rider_Header
//---------------------------------------------------------

void PlanSum::Rider_Header (void)
{
	Print (1, "Transit Link Group Summary");
	Print (2, String ("Link Group%35cLines  Runs   Time-of-Day   Riders") % BLANK);
	Print (1);
}

/*********************************************|***********************************************

	Transit Link Group Summary

	Link Group                                   Lines  Runs  Time-of-Day    Riders

	ddd ssssssssssssssssssssssssssssssssssssssss dddd  ddddd  dd:dd..dd:dd  ddddddd  
	                                             dddd  ddddd  dd:dd..dd:dd  ddddddd

**********************************************|***********************************************/ 
