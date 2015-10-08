//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void EventSum::Execute (void)
{

	//---- read the network and demand files ----

	Data_Service::Execute ();

	//---- read input travel times ----

	if (time_in_flag) {
		Read_Times ();
	}

	//---- read trip file ----

	if (trip_file_flag) {
		Read_Trips ();
	}

	//---- read the event file ----

	Read_Events ();

	//---- select travelers ----

	if (select_flag) {
		Select_Travelers ();
	}

	//---- write travel time file ----

	if (time_file_flag) {
		Write_Times ();
	}

	//---- write time summary file ----

	if (time_sum_flag) {
		Write_Time_Sum ();
	}

	//---- write the link events file ----

	if (link_event_flag) {
		Write_Links ();
	}

	//---- write the distribution file ----

	if (time_diff.Output_Flag ()) {
		time_diff.Write_Distribution ();
	}

	//---- write the trip gap files ----

	if (time_gap.Output_Flag ()) {
		time_gap.Write_Trip_Gap_File ();
	}

	//---- update plans ----

	if (update_flag) {
		Update_Plans ();
	}

	//---- write the new selection file ----

	if (new_select_flag) {
		select_map.swap (selected);
		Write_Selections ();
	}

	//---- print the comparison summary ----

	time_diff.Total_Summary ();

	//---- print reports ----

	Show_Message ("Writing Summary Reports");	

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOTAL_TIME:		//---- total time distribution ----
				time_diff.Distribution_Report (TOTAL_TIME, true);
				break;
			case PERIOD_TIME:		//---- period time distributions ----
				time_diff.Distribution_Report (PERIOD_TIME, false);
				break;
			case TOT_TIME_SUM:		//---- total time summary ----
				time_diff.Total_Summary ();
				break;
			case PER_TIME_SUM:		//---- period time summaries ----
				time_diff.Period_Summary (PER_TIME_SUM);
				break;
			case TRP_TIME_GAP:		//---- trip time gap ----
				time_gap.Trip_Gap_Report (TRP_TIME_GAP);
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void EventSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOTAL_TIME:		//---- total time distribution ----
			time_diff.Distribution_Header (true);
			break;
		case PERIOD_TIME:		//---- period time distributions ----
			time_diff.Distribution_Header (false);
			break;
		case TOT_TIME_SUM:		//---- total time summary ----
			break;
		case PER_TIME_SUM:		//---- period time summary ----
			time_diff.Period_Header ();
			break;
		case TRP_TIME_GAP:		//---- trip time gap ----
			time_gap.Trip_Gap_Header ();
			break;
		default:
			break;
	}
}
