//*********************************************************
//	Transfer_Report.cpp - Report Transit Transfers
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Transfer_Report
//---------------------------------------------------------

void PlanSum::Transfer_Report (int num, int mode)
{
	int i, j, total, num_board, board;
	bool flag;

	Ints_Array *on_ptr;
	Ints_Itr on_itr;
	Int_Itr int_itr;
	Integers bus_total;

	if (num == 0) {
		on_ptr = &total_on_array;
		xfer_label.clear ();
	} else if (mode == 0) {
		on_ptr = &walk_on_array [num - 1];
		xfer_label ("Walk Access %s") % sum_periods.Range_Format (num - 1);
	} else {
		on_ptr = &drive_on_array [num - 1];
		xfer_label ("Drive Access %s") % sum_periods.Range_Format (num - 1);
	}

	//---- get the dimensions ----

	total = max_rail = num_board = 0;

	for (i=0, on_itr = on_ptr->begin (); on_itr != on_ptr->end (); on_itr++, i++) {
		flag = false;

		for (j=0, int_itr = on_itr->begin (); int_itr != on_itr->end (); int_itr++, j++) {
			if (*int_itr > 0) {
				num_board += *int_itr * (i + j);
				if (j > max_rail) max_rail = j;
				flag = true;
			}
		}
		if (flag) total++;
	}
	if (total == 0) return;

	if (num == 0) {
		Header_Number (SUM_TRANSFERS);
	} else {
		Header_Number (XFER_DETAILS);
	}
	if (!Break_Check (total + 10)) {
		Print (1);
		Transfer_Header ();
	}

	//---- print each row ----

	bus_total.assign (10, 0);

	for (i=0, on_itr = on_ptr->begin (); on_itr != on_ptr->end (); on_itr++, i++) {
		for (int_itr = on_itr->begin (); int_itr != on_itr->end (); int_itr++) {
			if (*int_itr > 0) break;
		}
		if (int_itr == on_itr->end ()) continue;
		
		Print (1, String ("%3d      ") % i);

		if (max_rail > 0) {
			total = 0;

			for (j=0; j <= max_rail; j++) {
				board = on_itr->at (j);
				Print (0, String (" %7d") % board);
				total += board;
				bus_total [j] += board;
			}
		} else {
			total = on_itr->at (0);
			bus_total [0] += total;
		}
		Print (0, String (" %7d") % total);
	}
	
	//---- bus totals ----

	Print (2, "Total    ");

	if (max_rail > 0) {
		total = 0;

		for (j=0; j <= max_rail; j++) {
			Print (0, String (" %7d") % bus_total [j]);
			total += bus_total [j];
		}
	} else {
		total = bus_total [0];
	}
	Print (0, String (" %7d") % total);

	Print (2, String ("Total Boardings = %d, Boardings per Trip = %.2lf") % num_board %
		((total > 0) ? (double) num_board / total : 0.0));

	Header_Number (0);
}

//---------------------------------------------------------
//	Transfer_Header
//---------------------------------------------------------

void PlanSum::Transfer_Header (void)
{
	int i, len, len1;
	char *dash = "----------------------------------------";

	if (xfer_label.empty ()) {
		Print (1, "Transit Transfer Summary");
	} else {
		Print (1, "Transit Transfer Details -- ") << xfer_label;
	}
	Print (2, "Bus      ");

	if (max_rail > 0) {
		len = ((max_rail + 2) * 8) - 16;
		len1 = len / 2;
		len = len - len1;
		len1--;

		if (len > 2 && len1 > 2) {
			Print (0, String (" %*.*s Rail Boardings %*.*s") % len1 % len1 % dash % len % len % dash);
		} else {
			Print (0, String ("%*cRail Boardings") % (len1+1) % BLANK);
		}
	}
	Print (1, "Boardings");

	if (max_rail > 0) {
		for (i=0; i <= max_rail; i++) {
			Print (0, String (" %7d") % i);
		}
		Print (0, "   Total");
	}
	Print (1);
}

/*********************************************|***********************************************

	Transit Transfer Summary

	Bus       ------------------------------ Rail Boardings ---------------------------------
	Boardings       0       1       2       3       4       5       6       7      8+   Total

	  0       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  1       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  2       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd
	  3       ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd

	Total     ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd ddddddd


	Total Boardings = %d,  Boardings per Trip = %f.ff

**********************************************|***********************************************/ 
