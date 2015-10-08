//*********************************************************
//	Trip_Gap_Report.cpp - Create a Trip Gap Report
//*********************************************************

#include "Router.hpp"

#include "math.h"

//---------------------------------------------------------
//	Trip_Gap_Report
//---------------------------------------------------------

void Router::Trip_Gap_Report (void)
{
	int i;
	double gap, diff, total, std_dev, rmse, num;

	Gap_Itr gap_itr;

	Header_Number (TRIP_GAP);

	if (!Break_Check ((int) trip_gap_array.size () + 7)) {
		Print (1);
		Trip_Gap_Header ();
	}

	for (i=1, gap_itr = trip_gap_array.begin (); gap_itr != trip_gap_array.end (); gap_itr++, i++) {
		if (gap_itr->count == 0) continue;

		num = gap_itr->count;
		total = gap_itr->total;
		diff = gap_itr->diff;

		if (total > 0.0) {
			gap = diff / total;
			rmse = 100.0 * sqrt (gap_itr->diff_sq / num) * num / total;

			if (num > 1) {		
				std_dev = (gap_itr->diff_sq - diff * diff / num) / (num - 1.0);
				std_dev = (std_dev > 0.0) ? sqrt (std_dev) : 0.0;
				std_dev = std_dev * num / total;
			} else {
				std_dev = 0.0;
			}
			total /= 100;
			diff /= 100;
		} else {
			gap = 1.0;
			std_dev = rmse = 0.0;
		}

		//---- print the data record ----

		Print (1, String ("%6d    %12.6lf %12.6lf %12.6lf %8.1lf  %12.0lf %12.0lf") %
			i % gap % std_dev % gap_itr->max_gap % rmse % diff % total);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Gap_Header
//---------------------------------------------------------

void Router::Trip_Gap_Header (void)
{
	Print (1, "Trip Gap Report");
	Print (2, "          -------------- Trip Gap --------------     %     ----- Impedance/100 -----");
	Print (1, "Iteration        Total      Std.Dev      Maximum    RMSE     Difference        Total");
	Print (1);
}

/*********************************************|***********************************************

	Trip Gap Report

              -------------- Trip Gap --------------     %     ---- Impedance/100 ----
	Iteration        Total      Std.Dev      Maximum    RMSE    Difference       Total

	dddddd    fffff.ffffff fffff.ffffff fffff.ffffff  fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 
