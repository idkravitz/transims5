//*********************************************************
//	Read_Targets.cpp - read the calibration targets
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Read_Targets
//---------------------------------------------------------

void ModeChoice::Read_Targets (void)
{
	int i, num_targets, target, table, num_seg, seg, s1, s2, mode, nest;
	int mode_field, segment_field, target_field, min_field, max_field;
	double sum;
	bool report_flag;
	String text;
	Str_ID_Itr id_itr;
	Integers *nest_ptr;
	Int_Itr mode_itr;
	Doubles zeros;
	Dbls_Array mode_zeros;
	Ints_Array nest_zeros;

	Show_Message (String ("Reading %s -- Record") % target_file.File_Type ());
	Set_Progress ();

	//---- identify the data fields ----

	mode_field = target_file.Optional_Field ("MODE", "M", "MOD", "MODES");
	segment_field = target_file.Optional_Field ("SEGMENT", "SEG", "MARKET", "S");
	target_field = target_file.Optional_Field ("TARGET", "TRIPS", "SHARE", "SPLIT");
	min_field = target_file.Optional_Field ("MIN_CONST", "MIN", "MINIMUM", "MIN_CONSTANT");
	max_field = target_file.Optional_Field ("MAX_CONST", "MAX", "MAXIMUM", "MAX_CONSTANT");
	
	text = trip_file->Field (table_map [0])->Name ();

	calib_tab_flag = (target_file.Optional_Field (text.c_str ()) >= 0);

	if (mode_field < 0 && target_field < 0 && segment_field < 0) {
		calib_tab_flag = false;

		if (target_file.Num_Fields () == 2) {
			mode_field = 0;
			target_field = 1;
		} else if (target_file.Num_Fields () == 3) {
			segment_field = 0;
			mode_field = 1;
			target_field = 2;
		} else {
			goto field_error;
		}
	} else if (mode_field < 0 || (target_field < 0 && !calib_tab_flag)) {
		goto field_error;
	}

	//---- scan the segment fields for non-zero values ----

	if (segment_field >= 0) {
		calib_seg_flag = false;

		while (target_file.Read ()) {
			seg = target_file.Get_Integer (segment_field);
			if (seg != 0) {
				calib_seg_flag = true;
				break;
			}
		}
		target_file.Rewind ();
	} else {
		calib_seg_flag = false;
	}
	if (calib_seg_flag && !segment_flag) {
		Error ("A Segment Map File is required for Segment Calibration");
	}

	//---- initialize the calibration data ----

	num_seg = 1;
	if (calib_seg_flag) {
		num_seg += num_market;
		market_flag = true;
	} else {
		summary_flag = true;
	}
	zeros.assign (num_modes, -1000);
	min_const.assign (num_seg, zeros);

	zeros.assign (num_modes, 1000);
	max_const.assign (num_seg, zeros);

	zeros.assign ((num_tables + 1), -1.0);
	mode_zeros.assign ((num_modes + 1), zeros);
	calib_target.assign (num_seg, mode_zeros);

	zeros.assign ((num_tables + 1), 0.0);
	mode_zeros.assign ((num_modes + 1), zeros);
	calib_trips.assign (num_seg, mode_zeros);

	calib_const = seg_constant;

	//---- set the report flag ----

	report_flag = Report_Flag (TARGET_DATA);

	if (report_flag) {
		Header_Number (TARGET_DATA);

		if (!Break_Check (num_seg * num_modes + 8)) {
			Print (1);
			Target_Header ();
		}
	}

	//---- read the target file ----

	num_targets = 0;

	while (target_file.Read ()) {
		Show_Progress ();

		text = target_file.Get_String (mode_field);
		if (text.empty ()) continue;

		id_itr = mode_id.find (text);
		if (id_itr == mode_id.end ()) {
			Warning ("Target Mode ") << text << " was Not Defined";
		}
		mode = id_itr->second;

		if (calib_seg_flag) {
			seg = target_file.Get_Integer (segment_field);
			if (seg < 1 || seg > num_seg) {
				Warning ("Segment ") << seg << " is Out of Range (1.." << num_seg << ")";
				seg = 0;
			}
		} else {
			seg = 0;
		}
		target = target_file.Get_Integer (target_field);

		if (calib_tab_flag) {
			target = 0;

			for (i=0; i < num_tables; i++) {
				text = trip_file->Field (table_map [i])->Name ();

				target += table = target_file.Get_Integer (text.c_str ());

				calib_target [seg] [mode] [i] = table;
			}
		}
		calib_target [seg] [mode] [num_tables] = target;
		num_targets++;

		if (min_field >= 0) {
			min_const [seg] [mode] = target_file.Get_Double (min_field);
		}
		if (max_field >= 0) {
			max_const [seg] [mode] = target_file.Get_Double (max_field);
		}

		//---- write report record ----

		if (report_flag) {
			Print (1, "");
			if (calib_seg_flag) {
				Print (0, String ("%4d    ") % seg);
			}
			Print (0, String ("%-20s  ") % mode_names [mode]);

			if (calib_tab_flag) {
				for (i=0; i < num_tables; i++) {
					Print (0, String ("%8.0lf ") % calib_target [seg] [mode] [i]);
				}
			}
			Print (0, String ("%8.0lf  %8.2lf  %8.2lf") % target % 
				min_const [seg] [mode] % max_const [seg] [mode]);
		}
	}
	End_Progress ();
	target_file.Close ();

	Header_Number (0);

	//---- complete the target data ----

	if (calib_seg_flag) {
		s1 = 1;
		s2 = num_market;
	} else {
		s1 = s2 = 0;
	}
	for (seg=s1; seg <= s2; seg++) {

		//---- sum the nest targets ----

		for (i = num_nests - 1; i >= 0; i--) {
			nest = nest_mode [i];
			nest_ptr = &nested_modes [i];

			for (table = 0; table <= num_tables; table++) {
				sum = 0;

				for (mode_itr = nest_ptr->begin (); mode_itr != nest_ptr->end (); mode_itr++) {
					sum += calib_target [seg] [*mode_itr] [table];
				}
				if (nest < 0) {
					calib_target [seg] [num_modes] [table] += sum;
				} else {
					calib_target [seg] [nest] [table] = sum;
				}
			}
		}
	}
	Print (2, "Number of Calibration Targets = ") << num_targets;

	//---- define the output fields -----

	if (output_flag) {
		if (calib_seg_flag) {
			calib_file.Add_Field ("SEGMENT", DB_INTEGER, 4);
		}
		calib_file.Add_Field ("MODE", DB_STRING, 24);
		calib_file.Add_Field ("CONSTANT", DB_DOUBLE, 12.6);

		for (i=0; i < num_tables; i++) {
			text = trip_file->Field (table_map [i])->Name ();

			calib_file.Add_Field (text, DB_DOUBLE, 12.6);
		}
		calib_file.Write_Header ();
	}

	//---- write the data file header ----

	if (data_flag) {
		if (calib_seg_flag) {
			data_file.File () << "Segment\t";
		}
		if (calib_tab_flag) {
			data_file.File () << "Table\t";
		}
		data_file.File () << "Mode\tTarget\tTrips\tDiff\tPercent\tConstant" << endl;
	}
	return;

field_error:
	Error ("Calibration Field Names are Not Defined");
}

//---------------------------------------------------------
//	Target_Header
//---------------------------------------------------------

void ModeChoice::Target_Header (void)
{
	int i;
	Field_Ptr fld_ptr;

	Print (1, "Target Data Report -- ") << purpose_label;
	Print (2, "");
	if (calib_seg_flag) {
		Print (0, "Segment ");
	}
	Print (0, "Trips by Mode         ");

	if (calib_tab_flag) {
		for (i=0; i < num_tables; i++) {
			fld_ptr = trip_file->Field (table_map [i]);
			Print (0, String ("%8.8s ") % fld_ptr->Name ());
		}
	}
	Print (0, "  Target  Min_Const  Max_Const");
	Print (1);
}

/*********************************************|***********************************************

	Target Data Report -- sssssssssssss

	Trips by Mode           Target  Min_Const  Max_Const

	ssssssss20ssssssssss  ffffffff  ffffff.ff  ffffff.ff

or
	Segment  Trips by Mode           Target  Min_Const  Max_Const

	 dddd    ssssssss20ssssssssss  ffffffff  ffffff.ff  ffffff.ff

or
	Segment  Trips by Mode               M1       M2       M3       M4   Target  Min_Const  Max_Const

	 dddd    ssssssss20ssssssssss  ffffffff ffffffff ffffffff ffffffff ffffffff  ffffff.ff  ffffff.ff


**********************************************|***********************************************/ 
