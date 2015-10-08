//*********************************************************
//	Trip_Time.cpp - Print the Trip Start Time Report
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Trip_Time
//---------------------------------------------------------

void PlanSum::Trip_Time (void)
{
	int i, periods;
	double start_total, mid_total, end_total, start, mid, end;

	//---- get the total number of trips ----

	start_total = mid_total = end_total = 0;
	periods = sum_periods.Num_Periods ();

	for (i=0; i < periods; i++) {
		start_total += start_time [i];
		mid_total += mid_time [i];
		end_total += end_time [i];
	}
	if (start_total == 0 && mid_total == 0 && end_total == 0) {
		Write (2, "Trip Time Data is Zero");
		return;
	}
	if (start_total == 0) {
		start = 1.0;
	} else {
		start = start_total;
	}
	if (mid_total == 0) {
		mid = 1.0;
	} else {
		mid = mid_total;
	}
	if (end_total == 0) {
		end = 1.0;
	} else {
		end = end_total;
	}

	//---- print the report ----

	Header_Number (TRIP_TIME);

	if (!Break_Check (periods + 7)) {
		Print (1);
		Trip_Time_Header ();
	}		

	for (i=0; i < periods; i++) {
		if (start_time [i] > 0 || mid_time [i] > 0 || end_time [i] > 0) {
			Print (1, String ("%-12.12s   %10.0lf   %6.2lf   %10.0lf   %6.2lf   %10.0lf   %6.2lf") % 
				sum_periods.Range_Format (i) % start_time [i] % (100.0 * start_time [i] / start) %
				mid_time [i] % (100.0 * mid_time [i] / mid) %
				end_time [i] % (100.0 * end_time [i] / end));
		}
	}
	Print (2, String ("Total          %10.0lf   100.00   %10.0lf   100.00   %10.0lf   100.00") % start_total % mid_total % end_total);
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Time_Header
//---------------------------------------------------------

void PlanSum::Trip_Time_Header (void)
{
	Print (1, "Trip Start Time Report");
	Print (2, "Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Trip Start Time Report

	Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent

	dd:dd..dd:dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

	Total          dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

**********************************************|***********************************************/ 
