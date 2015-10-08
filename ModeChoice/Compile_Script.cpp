//*********************************************************
//	Compile_Script.cpp - set data files for the user script
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Compile_Script
//---------------------------------------------------------

void ModeChoice::Compile_Script (void)
{
	int i;
	Db_Base *file;
	Str_Itr mode_itr;
	Int_Itr int_itr;
	String text;
	Db_Mat_Itr matrix_itr;
	Dbls_Array mode_zeros;

	//---- add fields to the trip file ----

	trip_file->File_Access (MODIFY);

	trip_file->Add_Field ("PURPOSE", DB_INTEGER, sizeof (int), NO_UNITS, true);
	trip_file->Add_Field ("PERIOD", DB_INTEGER, sizeof (int), NO_UNITS, true);

	segment_field = trip_file->Num_Fields ();
	trip_file->Add_Field ("SEGMENT", DB_INTEGER, sizeof (int), NO_UNITS, true);

	num_access = (int) access_markets.size ();
	market_field = trip_file->Num_Fields ();

	for (i=1; i <= num_access; i++) {
		text ("ACCESS%d") % i;
		trip_file->Add_Field (text, DB_DOUBLE, sizeof (double), NO_UNITS, true);
	}

	//---- create mode files ----

	time_field = 0;
	walk_field = 1;
	auto_field = 2;
	wait_field = 3;
	lwait_field = 4;
	xwait_field = 5;
	tpen_field = 6;
	term_field = 7;
	cost_field = 8;
	xfer_field = 9;
	bias_field = 10;
	pef_field = 11;
	cbd_field = 12;
	const_field = 13;
	tab_field = 14;

	for (mode_itr = mode_names.begin (); mode_itr != mode_names.end (); mode_itr++) {
		file = new Db_Base (MODIFY, BINARY);
		file->File_ID (*mode_itr);

		file->Add_Field ("TIME", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("WALK", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("AUTO", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("WAIT", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("LWAIT", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("XWAIT", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("TPEN", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("TERM", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("COST", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("XFER", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("BIAS", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("PEF", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("CBD", DB_DOUBLE, sizeof (double), NO_UNITS, true);
		file->Add_Field ("CONSTANT", DB_DOUBLE, sizeof (double), NO_UNITS, true);

		for (i=0; i < trip_file->Tables (); i++) {
			text = trip_file->Field (i)->Name ();
			file->Add_Field (text, DB_DOUBLE, sizeof (double), NO_UNITS, true);
		}
		data_rec.push_back (file);
	}

	//---- initialize summary space ----
	
	if (mode_value_flag || seg_value_flag) {
		zero_values.assign (tab_field, 0.0);
		mode_zeros.assign (num_modes, zero_values);

		i = 1;
		if (seg_value_flag) i += num_market;

		value_sum.assign (i, mode_zeros);
	}

	//---- create the program data structure ----

	data_rec.push_back (trip_file);
	data_rec.push_back (&org_db);
	data_rec.push_back (&des_db);

	for (matrix_itr = skim_files.begin (); matrix_itr != skim_files.end (); matrix_itr++) {
		data_rec.push_back (*matrix_itr);
	}
	Write (1, "Compiling Mode Choice Script");

	if (Report_Flag (PRINT_SCRIPT)) {
		Header_Number (PRINT_SCRIPT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	program.Initialize (data_rec, random.Seed () + 1);

	if (!program.Compile (script_file, Report_Flag (PRINT_SCRIPT))) {
		Error ("Compiling Mode Choice Script");
	}
	if (Report_Flag (PRINT_STACK)) {
		Header_Number (PRINT_STACK);

		program.Print_Commands (false);
	}
	Header_Number (0);
	Show_Message (1);
}
