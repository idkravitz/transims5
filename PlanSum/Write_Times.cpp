//*********************************************************
//	Write_Times.cpp - Write the Trip Time File
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Times
//---------------------------------------------------------

void PlanSum::Write_Times (void)
{
	int i, start, mid, end;
	Dtime low, high;

	fstream &file = time_file.File ();

	Show_Message ("Writing Trip Time File");

	file << "Time1\tTime2\tStart\tMid-Trip\tEnd" << endl;

	for (i=0; i < sum_periods.Num_Periods (); i++) {
		sum_periods.Period_Range (i, low, high);

		start = DTOI (start_time [i]);
		mid = DTOI (mid_time [i]);
		end = DTOI (end_time [i]);

		if (start > 0 || mid > 0 || end > 0) {
			file << low.Time_String () << "\t" << high.Time_String () << "\t" << start << "\t" << mid << "\t" << end << endl;
		}
	}
	Show_Message (1);
	time_file.Close ();
}
