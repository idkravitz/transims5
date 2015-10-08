//*********************************************************
//	Link_Gap_Report.cpp - Create a Link Gap Report
//*********************************************************

#include "Router.hpp"

#include "math.h"

//---------------------------------------------------------
//	Link_Gap_Report
//---------------------------------------------------------

void Router::Link_Gap_Report (void)
{
	int i;
	double gap, diff, vht, std_dev, rmse, num, hour;

	Gap_Itr gap_itr;

	hour = Dtime (1.0, HOURS);

	Header_Number (LINK_GAP);

	if (!Break_Check ((int) link_gap_array.size () + 7)) {
		Print (1);
		Link_Gap_Header ();
	}

	for (i=1, gap_itr = link_gap_array.begin (); gap_itr != link_gap_array.end (); gap_itr++, i++) {
		if (gap_itr->count == 0) continue;

		num = gap_itr->count;
		vht = gap_itr->total;
		diff = gap_itr->diff;

		if (vht > 0.0) {
			gap = diff / vht;
			rmse = 100.0 * sqrt (gap_itr->diff_sq / num) * num / vht;

			if (num > 1) {		
				std_dev = (gap_itr->diff_sq - diff * diff / num) / (num - 1.0);
				std_dev = (std_dev > 0.0) ? sqrt (std_dev) : 0.0;
				std_dev = std_dev * num / vht;
			} else {
				std_dev = 0.0;
			}
			vht /= hour;
			diff /= hour;
		} else {
			gap = 1.0;
			std_dev = rmse = 0.0;
		}

		//---- print the data record ----

		Print (1, String ("%6d    %12.6lf %12.6lf %12.6lf %8.1lf  %12.0lf %12.0lf") %
			i % gap % std_dev % gap_itr->max_gap % rmse % diff % vht);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Gap_Header
//---------------------------------------------------------

void Router::Link_Gap_Header (void)
{
	Print (1, "Link Gap Report");
	Print (2, "          -------------- Link Gap --------------     %     ---------- VHT ----------");
	Print (1, "Iteration        Total      Std.Dev      Maximum    RMSE     Difference        Total");
	Print (1);
}

/*********************************************|***********************************************

	Link Gap Report

              -------------- Link Gap --------------     %     ---------- VHT ----------
	Iteration        Total      Std.Dev      Maximum    RMSE     Difference        Total

	dddddd    fffff.ffffff fffff.ffffff fffff.ffffff  fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 
