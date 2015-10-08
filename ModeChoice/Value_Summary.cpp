//*********************************************************
//	Value_Summary.cpp - value summary report
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Value_Summary
//---------------------------------------------------------

void ModeChoice::Value_Summary (int segment)
{
	int i, j;
	double value, trips;
	Doubles *mode_ptr;

	header_value = segment;
	Header_Number (MODE_VALUES);

	if (!Break_Check (num_modes + 5)) {
		Print (1);
		Value_Header ();
	}

	//---- process each mode ----

	for (i=0; i < num_modes; i++) {
		mode_ptr = &value_sum [segment] [i];

		trips = mode_ptr->at (const_field);
		if (trips <= 0) continue;

		Print (1, String ("%-20s") % mode_names [i]);

		for (j=0; j < const_field; j++) {
			value = mode_ptr->at (j) / trips;

			Print (0, String ("  %7.2lf") % value);
		}
		Print (0, String ("  %8.0lf") % trips);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Value_Header
//---------------------------------------------------------

void ModeChoice::Value_Header (void)
{
	int i;
	String text;
	Field_Ptr fld_ptr;
	Db_Base *mode_file;

	mode_file = data_rec [0];

	if (header_value > 0) {
		Print (1, "Segment Value Summary Report #") << header_value << " -- " << purpose_label;
	} else {
		Print (1, "Mode Value Summary Report -- ") << purpose_label;
	}
	Print (2, "Average Mode Values ");

	for (i=0; i < const_field; i++) {
		fld_ptr = mode_file->Field (i);

		Print (0, String ("  %7s") % fld_ptr->Name ());
	}
	Print (0, "     Trips");
	Print (1);
}

/*********************************************|***********************************************

	Mode Value Summary Report -- sssssssssssss
	Segment Value Summary Report #dd -- sssssssssssss

	Average Mode Values   sssssss  sssssss  sssssss  sssssss  sssssss  sssssss  sssssss  sssssss  sssssss  sssssss     Trips

	ssssssss20ssssssssss  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffffffff 

**********************************************|***********************************************/ 
