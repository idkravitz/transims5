//*********************************************************
//	Write_Mode_Seg.cpp - write the mode segment file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Mode_Segment
//---------------------------------------------------------

void ModeChoice::Write_Mode_Segment (void)
{
	int mode, segment;
	double trips, total;
	Doubles totals;

	fstream &fh = mode_seg_file.File ();

	totals.assign ((num_market + 1), 0.0);

	fh << "Mode";

	for (segment=1; segment <= num_market; segment++) {
		fh << "\t" << segment;
	}
	fh << "\tTotal" << endl;

	//---- process each mode ----

	for (mode=0; mode < num_modes; mode++) {
		fh << mode_names [mode];
		total = 0.0;

		//---- process each market segment ----

		for (segment=1; segment <= num_market; segment++) {
			total += trips = market_seg [segment] [mode] [num_tables];

			if (mode_nest [mode] < 0) {
				totals [segment] += trips;
			}
			fh << (String ("\t%.1lf") % trips);
		}
		fh << (String ("\t%.1lf") % total) << endl;
	}

	//---- segment totals ----

	fh << "Total";
	total = 0.0;

	for (segment=1; segment <= num_market; segment++) {
		total += trips = totals [segment];
		fh << (String ("\t%.1lf") % trips);
	}
	fh << (String ("\t%.1lf") % total) << endl;

	mode_seg_file.Close ();
}

