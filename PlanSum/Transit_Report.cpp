//*********************************************************
//	Transit_Report.cpp - Report Transit Ridership
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Transit_Report
//---------------------------------------------------------

void PlanSum::Transit_Report (void)
{
	int nrun, nstop, run, board, alight, load;
	int sum_board, tot_board, max_board, min_board, avg_board;
	int high_load, tot_load, max_load, min_load, avg_load;
	int tot_routes, tot_runs, tot_stops;
	int tot_tot_board, max_max_board, min_min_board;
	int tot_tot_load, max_max_load, min_min_load;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run *run_ptr;

	Show_Message ("Transit Ridership Report -- Route");
	Set_Progress ();

	Header_Number (SUM_RIDERS);

	tot_routes = (int) line_array.size ();

	if (!Break_Check (tot_routes + 8)) {
		Print (1);
		Transit_Header ();
	}
	tot_runs = tot_stops = avg_board = avg_load = 0;
	tot_tot_board = max_max_board = tot_tot_load = max_max_load = 0;
	min_min_load = min_min_board = -1;

	//---- process each transit route ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		line_ptr = &line_array [map_itr->second];

		tot_board = max_board = min_board = 0;
		tot_load = max_load = min_load = 0;
		
		nstop = (int) line_ptr->size ();
		stop_itr = line_ptr->begin ();
		nrun = (int) stop_itr->size ();

		for (run=0; run < nrun; run++) {
			load = sum_board = high_load = 0;

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				run_ptr = &stop_itr->at (run);

				board = run_ptr->Board ();
				alight = run_ptr->Alight ();

				load += board - alight;

				if (load == 0 && board == 0 && alight == 0) continue;

				if (load > high_load) high_load = load;
				sum_board += board;
			}
			if (high_load > max_load) max_load = high_load;
			if (high_load < min_load || min_load == 0) min_load = high_load;
			tot_load += high_load;

			if (sum_board > max_board) max_board = sum_board;
			if (sum_board < min_board || min_board == 0) min_board = sum_board;
			tot_board += sum_board;
		}

		//---- print report ----

		if (nrun > 0) {
			avg_board = (tot_board + nrun / 2) / nrun;
			avg_load = (tot_load + nrun / 2) / nrun;
		}

		Print (1, String ("%-12.12s%7d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d") % 
			Transit_Code ((Transit_Type) line_ptr->Mode ()) %
			line_ptr->Route () % nrun % nstop % 
			tot_board % max_board % min_board % avg_board %
			max_load % min_load % avg_load);

		if (Notes_Name_Flag () && line_ptr->Name () != 0) {
			Print (0, String ("  %s") % line_ptr->Name ());
		}

		tot_runs += nrun;
		tot_stops += nstop;
		tot_tot_board += tot_board;
		tot_tot_load += tot_load;

		if (max_load > max_max_load) max_max_load = max_load;
		if (min_load < min_min_load || min_min_load < 0) min_min_load = min_load;
		if (max_board > max_max_board) max_max_board = max_board;
		if (min_board < min_min_board || min_min_board < 0) min_min_board = min_board;
	}
	End_Progress ();

	//---- system total ----

	if (tot_runs > 0) {
		avg_board = (tot_tot_board + tot_runs / 2) / tot_runs;
		avg_load = (tot_tot_load + tot_runs / 2) / tot_runs;
	}

	Print (2, String ("%-12.12s%7d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d") % 
		"Total" % tot_routes % tot_runs % tot_stops % 
		tot_tot_board % max_max_board % min_min_board % avg_board %
		max_max_load % min_min_load % avg_load);

	Header_Number (0);
}

//---------------------------------------------------------
//	Transit_Header
//---------------------------------------------------------

void PlanSum::Transit_Header (void)
{
	Print (1, "Transit Ridership Summary");
	Print (2, String ("%22cNum.  Num.  -----------Boardings-----------  -----Max.Load Point----") % BLANK);
	Print (1, "Mode          Route   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average");
	Print (1);
}

/*********************************************|***********************************************

	Transit Ridership Summary

                          Num.  Num.  -----------Boardings-----------  -----Max.Load Point----
	Mode          Route   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average

	ssss12ssssssddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd

	Total       ddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd

**********************************************|***********************************************/ 
