//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ModeChoice::Execute (void)
{
	int i, num_seg;
	bool flag, save;
		
	Doubles zeros, zero_tab;
	Dbls_Array mode_zeros;

	//---- compile the model script ----

	Compile_Script ();

	//---- read the segment map file ----

	if (segment_flag) {
		Segment_Map ();
		num_seg = num_market + 1;
	} else {
		num_seg = 1;
	}

	//---- allocate summary space ----

	if (market_flag || summary_flag || constant_flag || calib_flag) {
		zero_tab.assign (num_tables + 1, 0.0);
		mode_zeros.assign ((num_modes + 1), zero_tab);
		market_seg.assign (num_seg, mode_zeros);

		if (constant_flag || calib_flag) {
			seg_constant.assign (num_seg, mode_zeros);
		}
	}
	if (prod_flag || attr_flag) {
		zeros.assign (zones, 0.0);

		if (prod_flag) {
			prod_share.assign ((num_modes + 1), zeros);
		}
		if (attr_flag) {
			attr_share.assign ((num_modes + 1), zeros);
		}
	}

	//---- read the mode constants file ----

	if (constant_flag) {
		Read_Constants ();
	}

	//---- read the calibration targets ----

	if (calib_flag) {
		Read_Targets ();
	}

	//---- read the zone file ----

	Read_Zone ();

	//---- perform mode choice ----

	flag = false;

	for (i=1; i <= max_iter; i++) {
		save = (flag || i == max_iter);

		Choice_Process (i, save);

		//---- calibrate the constants ----

		if (calib_flag) {
			flag = Adjust_Constants (i, save);
		}
		if (save) break;

		//---- zero the market segments ----

		market_seg.assign (num_seg, mode_zeros);
	}

	//---- write the summary file ----

	if (summary_flag && summary_file.Is_Open ()) {
		Write_Summary ();
	}

	//---- write the market segment file ----

	if (market_flag && market_file.Is_Open ()) {
		Write_Market ();
	}

	//---- write the market segment file ----

	if (mode_seg_flag) {
		Write_Mode_Segment ();
	}

	//---- write the zone data ----

	if (prod_flag || attr_flag) {
		Write_Zones ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case MODE_SUMMARY:		//---- Mode Summary Report ----
				Mode_Summary ();
				break;
			case MARKET_REPORT:		//---- Market Segment Report ----
				for (int j=1; j <= num_market; j++) {
					Mode_Summary (j);
				}
				break;
			case MODE_VALUES:		//---- Mode Value Summaryt ----
				Value_Summary ();
				break;
			case SEGMENT_VALUES:	//---- Segment Value Summary ----
				for (int j=1; j <= num_market; j++) {
					Value_Summary (j);
				}
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ModeChoice::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Mode Choice Script ----
			Print (1, "Mode Choice Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Mode Choice Stack ----
			Print (1, "Mode Choice Stack");
			Print (1);
			break;
		case MODE_SUMMARY:		//---- Mode Summary Report ----
		case MARKET_REPORT:		//---- Market Summary Report ----
			Mode_Header ();
			break;
		case MODE_VALUES:		//---- Mode Value Summary ----
		case SEGMENT_VALUES:	//---- Segment Value Summary ----
			Value_Header ();
			break;
		case CALIB_REPORT:		//---- Calibration Report ----
			Calib_Header ();
			break;
		case TARGET_DATA:		//---- Target Data Report ----
			Target_Header ();
			break;
		default:
			break;
	}
}
