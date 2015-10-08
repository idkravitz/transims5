//*********************************************************
//	Mode_Summary.cpp - mode summary report
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Mode_Summary
//---------------------------------------------------------

void ModeChoice::Mode_Summary (int segment)
{
	int i;
	double total, percent;
	String text;
	Doubles *mode_ptr;
	Dbl_Itr tab_itr;

	header_value = segment;
	Header_Number (MODE_SUMMARY);

	if (!Break_Check (num_modes + 8)) {
		Print (1);
		Mode_Header ();
	}

	//---- get the total number of trips ----

	total = market_seg [segment] [num_modes] [num_tables];
	if (total == 0.0) return;

	//---- process each mode ----

	for (i=0; i <= num_modes; i++) {
		if (i == num_modes) {
			Print (1);
			text = "Total";
		} else {
			text = mode_names [i];
		}
		Print (1, String ("%-20s") % text);

		mode_ptr = &market_seg [segment] [i];

		for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
			Print (0, String ("  %10.1lf") % *tab_itr);
		}
		percent = mode_ptr->at (num_tables) * 100.0 / total;

		Print (0, String ("  %6.2lf%%") % percent % FINISH);
	}

	//---- table percents ----

	text = "Percent";
	Print (1, String ("%-20s") % text);

	mode_ptr = &market_seg [segment] [num_modes];

	for (tab_itr = mode_ptr->begin (); tab_itr != mode_ptr->end (); tab_itr++) {
		percent = *tab_itr * 100.0 / total;
		Print (0, String ("     %6.2lf%%") % percent % FINISH);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Mode_Header
//---------------------------------------------------------

void ModeChoice::Mode_Header (void)
{
	int i;
	String text;
	Field_Ptr fld_ptr;

	if (header_value > 0) {
		Print (1, "Market Segment Report #") << header_value << " -- " << purpose_label;
	} else {
		Print (1, "Mode Summary Report -- ") << purpose_label;
	}
	Print (2, "Trips by Mode       ");

	for (i=0; i < num_tables; i++) {
		fld_ptr = trip_file->Field (table_map [i]);
		text = fld_ptr->Name ();
		if (text.length () < 3) {
			text = "Table " + text;
		}
		Print (0, String ("  %10s") % text);
	}
	Print (0, "       Total  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Mode Summary Report -- sssssssssssss
	Market Segment Report #dd -- sssssssssssss

	Trips by Mode         ssssssssss  ssssssssss  ssssssssss  ssssssssss       Total  Percent

	ssssssss20ssssssssss  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  fff.ff%
	
	Total                 ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f  ffffffff.f
	Percent                  fff.ff%     fff.ff%     fff.ff%     fff.ff%

**********************************************|***********************************************/ 
