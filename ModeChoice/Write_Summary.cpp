//*********************************************************
//	Write_Summary.cpp - write the mode summary file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void ModeChoice::Write_Summary (void)
{
	int i;
	double total, percent;
	String text;
	Doubles *mode_ptr;
	Dbl_Itr tab_itr;
	Field_Ptr fld_ptr;

	fstream &fh = summary_file.File ();

	fh << "Mode";

	for (i=0; i < num_tables; i++) {
		fld_ptr = trip_file->Field (table_map [i]);

		fh << "\t" << fld_ptr->Name ();
	}
	fh << "\tTotal\tPercent" << endl;

	//---- get the total number of trips ----

	total = market_seg [0] [num_modes] [num_tables]; 
	if (total == 0.0) return;

	//---- process each mode ----

	for (i=0; i <= num_modes; i++) {
		if (i == num_modes) {
			text = "Total";
		} else {
			text = mode_names [i];
		}
		fh << text;

		mode_ptr = &market_seg [0] [i];

		for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
			fh << (String ("\t%.1lf") % *tab_itr);
		}
		percent = mode_ptr->at (num_tables) * 100.0 / total;
		fh << (String ("\t%.2lf") % percent) << endl;
	}
	summary_file.Close ();
}

