//*********************************************************
//	Write_Market.cpp - write the market segment file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Market
//---------------------------------------------------------

void ModeChoice::Write_Market (void)
{
	int i, segment;
	double total, percent;
	String text;
	Doubles *mode_ptr;
	Dbl_Itr tab_itr;
	Field_Ptr fld_ptr;

	fstream &fh = market_file.File ();

	fh << "Segment\tMode";

	for (i=0; i < num_tables; i++) {
		fld_ptr = trip_file->Field (table_map [i]);

		fh << "\t" << fld_ptr->Name ();
	}
	fh << "\tTotal\tPercent" << endl;

	//---- process each market segment ----

	for (segment=1; segment <= num_market; segment++) {

		//---- get the total number of trips ----

		total = market_seg [segment] [num_modes] [num_tables]; 
		if (total == 0.0) continue;

		//---- process each mode ----

		for (i=0; i <= num_modes; i++) {
			if (i == num_modes) {
				text = "Total";
			} else {
				text = mode_names [i];
			}
			fh << segment << "\t" << text;

			mode_ptr = &market_seg [segment] [i];

			for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
				fh << (String ("\t%.1lf") % *tab_itr);
			}
			percent = mode_ptr->at (num_tables) * 100.0 / total;
			fh << (String ("\t%.2lf") % percent) << endl;
		}
	}
	market_file.Close ();
}

