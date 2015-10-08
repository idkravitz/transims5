//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ModeChoice.hpp"

#include "TppMatrix.hpp"
#include "TcadMatrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ModeChoice::Program_Control (void)
{
	int i, num, id, nest, table;
	double value, size, nesting;
	String key, text;
	Strings strings;
	Str_Itr str_itr;
	Str_ID_Itr id_itr;
	Str_ID_Stat id_stat;
	Db_Mat_Ptr matrix_ptr;
	Integers access_modes, nest_modes, zero_tab;
	Format_Type type;

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);

	//---- open trip files ----

	key = Project_Filename (Get_Control_String (TRIP_FILE));

	type = Db_Header::Def_Format (key);

	if (type == UNFORMATED) {
		if (Check_Control_Key (TRIP_FORMAT)) {
			text = Get_Control_String (TRIP_FORMAT);
		} else {
			text = Get_Default_Text (TRIP_FORMAT);
		}
		type = Format_Code (text);
	}
	if (type == TPPLUS) {
		trip_file = new TPPlus_Matrix ();
	} else if (type == TRANSCAD) {
		trip_file = new TransCAD_Matrix ();
	} else {
		Error ("Unrecognized Trip Matrix Format");
		return;
	}
	trip_files.push_back (trip_file);

	trip_file->File_Type ("Trip File");
	trip_file->File_ID ("Trip");
	trip_file->Dbase_Format (type);

	trip_file->Open (key);

	zones = trip_file->Zones ();
	num_tables = trip_file->Tables ();

	Print (0, " (Zones=") << zones << " Tables=" << num_tables << ")";

	//---- select trip tables ----

	key = Get_Control_Text (SELECT_TRIP_TABLES);

	if (key.empty () || key.Equals ("ALL")) {
		for (i=0; i <= num_tables; i++) {
			table_map.push_back (i);
		}
	} else {
		key.Parse (strings);

		for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
			i = trip_file->Required_Field (*str_itr);
			table_map.push_back (i);
		}
		num_tables = (int) table_map.size ();
	}
	Print (1);

	//---- create new trip files ----

	key = Get_Control_String (NEW_TRIP_FILE);

	if (Check_Control_Key (NEW_TRIP_FORMAT)) {
		type = Format_Code (Get_Control_String (NEW_TRIP_FORMAT));
	}
	if (type == TPPLUS) {
		new_file = new TPPlus_Matrix ();
	} else if (type == TRANSCAD) {
		new_file = new TransCAD_Matrix ();
	} else {
		Error ("Unrecognized New Trip Matrix Format");
		return;
	}
	trip_files.push_back (new_file);

	new_file->File_Type ("New Trip File");
	new_file->File_ID ("NewTrip");
	new_file->Filename (Project_Filename (key));
	new_file->Dbase_Format (type);
	new_file->Zones (zones);

	size = sizeof (double) + (trip_file->Field (0)->Decimal () + 2) / 10.0;

	num = Highest_Control_Group (NEW_TABLE_MODES, 0);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (NEW_TABLE_MODES, i);
		if (key.empty ()) continue;

		text = key;
		text.Split (key, "=");
		if (text.empty ()) text = key;

		new_file->Add_Field (text, DB_DOUBLE, size, NO_UNITS, true);
	}
	new_file->Tables (new_file->Num_Fields ());

	new_file->Create ();

	if (!new_file->Allocate_Data ()) {
		Error (String ("Insufficient Memory for Matrix %s") % new_file->File_ID ());
	}
	Print (0, " (Zones=") << new_file->Zones () << " Tables=" << new_file->Tables () << ")";
	Print (1);

	//---- open skim files ----

	num = Highest_Control_Group (SKIM_FILE, 0);
	if (num == 0) Error ("No Skim Files were Found");

	skim_files.reserve (num);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (SKIM_FILE, i);
		if (key.empty ()) continue;

		key = Project_Filename (key);

		type = Db_Header::Def_Format (key);

		if (type == UNFORMATED) {
			if (Check_Control_Key (SKIM_FORMAT, i)) {
				type = Format_Code (Get_Control_String (SKIM_FORMAT, i));
			} else {
				type = trip_file->Dbase_Format ();
			}
		}
		if (type == TPPLUS) {
			matrix_ptr = new TPPlus_Matrix ();
		} else if (type == TRANSCAD) {
			matrix_ptr = new TransCAD_Matrix ();
		} else {
			Error ("Unrecognized Skim Matrix Format");
			return;
		}
		matrix_ptr->File_Type (String ("Skim File #%d") % i);
		matrix_ptr->File_ID (String ("Skim%d") % i);
		matrix_ptr->Dbase_Format (type);

		matrix_ptr->Open (key);

		Print (0, " (Zones=") << matrix_ptr->Zones () << " Tables=" << matrix_ptr->Tables () << ")";
		Print (1);
		
		skim_files.push_back (matrix_ptr);
	}

	//---- open zone file ----

	key = Get_Control_String (ZONE_FILE);

	if (!key.empty ()) {
		zone_file.File_Type ("Zone File");

		if (Check_Control_Key (ZONE_FORMAT)) {
			zone_file.Dbase_Format (Get_Control_String (ZONE_FORMAT));
		}
		zone_file.Open (Project_Filename (key));
		Print (1);

		//---- create the zone database ----

		org_db.File_ID ("Org");
		org_db.File_Type (zone_file.File_Type ());
		des_db.File_ID ("Des");
		des_db.File_Type (zone_file.File_Type ());
		
		zone_field = zone_file.Required_Field ("ZONE", "Z", "ZON", "ID");

		org_db.Replicate_Fields (&zone_file, false, false, true);
		des_db.Replicate_Fields (&zone_file, false, false, true);
	}

	//---- open mode constant file ----

	key = Get_Control_String (MODE_CONSTANT_FILE);

	if (!key.empty ()) {
		constant_file.File_Type ("Mode Constant File");

		constant_file.Open (Project_Filename (key));
		constant_flag = true;
		Print (1);
	}

	//---- open mode choice script ----

	key = Get_Control_String (MODE_CHOICE_SCRIPT);

	if (!key.empty ()) {
		script_file.File_Type ("Mode Choice Script");

		script_file.Open (Project_Filename (key));
	}

	//---- open segment map file ----

	key = Get_Control_String (SEGMENT_MAP_FILE);

	if (!key.empty ()) {
		Print (1);
		segment_file.File_Type ("Segment Map File");

		segment_file.Open (Project_Filename (key));
		segment_flag = true;

		//---- origin map field ----

		key = Get_Control_Text (ORIGIN_MAP_FIELD);

		if (!key.empty ()) {
			org_map_field = zone_file.Required_Field (key);
			Print (0, " (Zone Field = ") << (org_map_field + 1) << ")";
		}

		//---- destination map field ----

		key = Get_Control_Text (DESTINATION_MAP_FIELD);

		if (!key.empty ()) {
			des_map_field = zone_file.Required_Field (key);
			Print (0, " (Zone Field = ") << (des_map_field + 1) << ")";
		}
	}

	//---- get the trip purpose label ----

	Print (1);
	purpose_label = Get_Control_Text (TRIP_PURPOSE_LABEL);

	//---- get the trip purpose number ----

	purpose = Get_Control_Integer (TRIP_PURPOSE_NUMBER);

	//---- get the time period ----

	period = Get_Control_Integer (TRIP_TIME_PERIOD);
	
	//---- primary mode choice ----

	Print (1);
	key = Get_Control_Text (PRIMARY_MODE_CHOICE);
	key.Parse (strings);

	nesting = 1.0;

	for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
		id = (int) mode_id.size ();

		id_stat = mode_id.insert (Str_ID_Data (*str_itr, id));

		if (id_stat.second) {
			mode_names.push_back (*str_itr);
			mode_nest.push_back (-1);
			mode_nested.push_back (-1);
			nest_levels.push_back (nesting);
			nest_modes.push_back (id);
		} else {
			Warning ("Duplicate Mode = ") << *str_itr;
		}
	}
	nest_mode.push_back (-1);
	nest_coef.push_back (nesting);
	nested_modes.push_back (nest_modes);

	//---- mode choice nest ----

	num = Highest_Control_Group (MODE_CHOICE_NEST, 0);
	nesting = 0.5;

	for (i=1; i <= num; i++) {
		key = Get_Control_String (NESTING_COEFFICIENT, i);
		if (!key.empty ()) {
			nesting = key.Double ();
		}
		key = Get_Control_Text (MODE_CHOICE_NEST, i);
		if (key.empty ()) continue;

		key.Parse (strings, "=");
		text = strings [0];

		id_itr = mode_id.find (text);

		if (id_itr == mode_id.end ()) {
			Error (String ("Nested Mode %s is not defined") % text);
			nest = 0;
		} else {
			nest = id_itr->second;
		}
		nest_modes.clear ();
		key = strings [1];
		key.Parse (strings);

		for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
			id = (int) mode_id.size ();
			id_stat = mode_id.insert (Str_ID_Data (*str_itr, id));

			if (id_stat.second) {
				mode_names.push_back (*str_itr);
				mode_nest.push_back (nest);
				mode_nested.push_back (-1);
				nest_levels.push_back (nest_levels [nest] * nesting);
				nest_modes.push_back (id);
			} else {
				Warning ("Duplicate Mode = ") << *str_itr;
			}
		}
		mode_nested [nest] = (int) nest_mode.size ();
		nest_mode.push_back (nest);
		nest_coef.push_back (nesting);
		nested_modes.push_back (nest_modes);
	}
	num_nests = (int) nested_modes.size ();

	//---- print the nesting coefficient ----

	Print (1);
	for (i=1; i <= num; i++) {
		Get_Control_Text (NESTING_COEFFICIENT, i);
	}

	//---- vehicle time value ----

	Print (1);
	time_value = Get_Control_Double (VEHICLE_TIME_VALUE);

	//---- walk time value ----

	walk_value = Get_Control_Double (WALK_TIME_VALUE);

	//---- drive access value ----

	drive_value = Get_Control_Double (DRIVE_ACCESS_VALUE);

	//---- wait time value ----

	wait_value = Get_Control_Double (WAIT_TIME_VALUE);

	//---- long wait time value ----

	lwait_value = Get_Control_Double (LONG_WAIT_VALUE);

	//---- transfer time value ----

	xwait_value = Get_Control_Double (TRANSFER_TIME_VALUE);

	//---- penalty time value ----

	tpen_value = Get_Control_Double (PENALTY_TIME_VALUE);

	//---- terminal time value ----

	term_value = Get_Control_Double (TERMINAL_TIME_VALUE);	

	//---- transfer count value ----

	xfer_value = Get_Control_Double (TRANSFER_COUNT_VALUE);

	//---- cost value table ----

	num = Highest_Control_Group (COST_VALUE_TABLE, 0);

	for (i=1; i <= num; i++) {
		value = Get_Control_Double (COST_VALUE_TABLE, i);
		cost_values.push_back (value);
	}	

	//---- mode access market ----

	num = Highest_Control_Group (MODE_ACCESS_MARKET, 0);
	Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_Text (MODE_ACCESS_MARKET, i);
		if (key.empty ()) continue;

		key.Parse (strings);
		access_modes.clear ();

		for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
			id_itr = mode_id.find (*str_itr);
			if (id_itr == mode_id.end ()) {
				Warning ("Market Access Mode ") << *str_itr << " was Not Defined";
			}
			access_modes.push_back (id_itr->second);
		}
		access_markets.push_back (access_modes);

		if (i <= 2) {
			nest = 1;
		} else if (i <= 4) {
			nest = 2;
		} else {
			nest = 0;
		}
		market_group.push_back (nest);
	}

	//---- new table modes ----

	num = Highest_Control_Group (NEW_TABLE_MODES, 0);
	Print (1);

	num_modes = (int) mode_names.size ();
	zero_tab.assign (num_tables, -1), 
	output_table.assign (num_modes, zero_tab);

	for (i=1; i <= num; i++) {
		key = Get_Control_Text (NEW_TABLE_MODES, i);
		if (key.empty ()) continue;

		key.Split (text, "=");
		text.Parse (strings);

		for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
			key = *str_itr;
			key.Split (text, ".");

			id_itr = mode_id.find (text);
			if (id_itr == mode_id.end ()) {
				Warning ("New Table Mode ") << *str_itr << " was Not Defined";
			}
			if (key.empty ()) {
				for (table=0; table < num_tables; table++) {
					output_table [id_itr->second] [table] = i - 1;
				}
			} else {
				table = key.Integer () - 1;
				if (table < 0 || table >= num_tables) {
					Warning ("New Table Mode ") << *str_itr << " was Not Defined";
				} else {
					output_table [id_itr->second] [table] = i - 1;
				}
			}
		}
	}

	//---- output trip factor ----

	Print (1);
	trip_factor = Get_Control_Double (OUTPUT_TRIP_FACTOR);

	//---- create the mode summary file ----

	key = Get_Control_String (NEW_MODE_SUMMARY_FILE);

	if (!key.empty ()) {
		Print (1);
		summary_file.File_Type ("New Mode Summary File");

		summary_file.Create (Project_Filename (key));
		summary_flag = true;
	} else {
		summary_flag = Report_Flag (MODE_SUMMARY);
	}

	//---- create the market segment file ----

	key = Get_Control_String (NEW_MARKET_SEGMENT_FILE);

	if (!key.empty ()) {
		Print (1);
		market_file.File_Type ("New Market Segment File");

		market_file.Create (Project_Filename (key));
		market_flag = true;
	} else {
		market_flag = Report_Flag (MARKET_REPORT);
	}
	if (market_flag && !segment_flag) {
		Error ("A Segment Map File is required for Market Segment Processing");
	}

	//---- create the mode segment file ----

	key = Get_Control_String (NEW_MODE_SEGMENT_FILE);

	if (!key.empty ()) {
		Print (1);
		mode_seg_file.File_Type ("New Mode Segment File");

		mode_seg_file.Create (Project_Filename (key));
		mode_seg_flag = true;
	}
	if (mode_seg_flag && !segment_flag) {
		Error ("A Segment Map File is required for Mode Segment Processing");
	}
	
	//---- create the summit file ----

	key = Get_Control_String (NEW_FTA_SUMMIT_FILE);

	if (!key.empty ()) {
		Print (1);
		summit_file.File_Type ("New FTA Summit File");
		summit_file.Dbase_Format (BINARY);
		//summit_file.Dbase_Format (TAB_DELIMITED);
		summit_file.Nest (NESTED);
		summit_file.Header_Lines (2);

		summit_file.Add_Field ("ZONES", DB_INTEGER, 4, NO_UNITS, true);
		summit_file.Add_Field ("MARKETS", DB_INTEGER, 4, NO_UNITS, true);
		summit_file.Add_Field ("IVTTT", DB_DOUBLE, 4, NO_UNITS, true);
		summit_file.Add_Field ("IVTTA", DB_DOUBLE, 4, NO_UNITS, true);
		summit_file.Add_Field ("PURPOSE", DB_STRING, 6, NO_UNITS, true);
		summit_file.Add_Field ("TIME", DB_STRING, 6, NO_UNITS, true);
		summit_file.Add_Field ("NAME", DB_STRING, 60, NO_UNITS, true);
		
		summit_org = summit_file.Add_Field ("ORG", DB_INTEGER, 2, NO_UNITS, true, NESTED);
		summit_des = summit_file.Add_Field ("DES", DB_INTEGER, 2, NO_UNITS, true, NESTED);
		summit_market = summit_file.Add_Field ("MARKET", DB_INTEGER, 2, NO_UNITS, true, NESTED);
		summit_total_trips = summit_file.Add_Field ("TOTAL_TRIPS", DB_DOUBLE, 4, NO_UNITS, true, NESTED);
		summit_motor_trips = summit_file.Add_Field ("MOTOR_TRIPS", DB_DOUBLE, 4, NO_UNITS, true, NESTED);
		summit_auto_exp = summit_file.Add_Field ("AUTO_EXP", DB_DOUBLE, 4.6, NO_UNITS, true, NESTED);
		summit_walk_market = summit_file.Add_Field ("WALK_MARKET", DB_DOUBLE, 4, NO_UNITS, true, NESTED);
		summit_walk_share = summit_file.Add_Field ("WALK_SHARE", DB_DOUBLE, 4, NO_UNITS, true, NESTED);
		summit_drive_market = summit_file.Add_Field ("DRIVE_MARKET", DB_DOUBLE, 4, NO_UNITS, true, NESTED);
		summit_drive_share = summit_file.Add_Field ("DRIVE_SHARE", DB_DOUBLE, 4, NO_UNITS, true, NESTED);

		summit_file.Create (Project_Filename (key));
		summit_flag = true;

		summit_file.Put_Field (0, zones);
		summit_file.Put_Field (1, num_tables);
		summit_file.Put_Field (2, time_value);
		summit_file.Put_Field (3, time_value);
		summit_file.Put_Field (4, purpose);
		summit_file.Put_Field (5, period);
		summit_file.Put_Field (6, purpose_label);

		summit_file.Write (false);
	}
		
	//---- create the production file ----

	key = Get_Control_String (NEW_PRODUCTION_FILE);

	if (!key.empty ()) {
		Print (1);
		prod_file.File_Type ("New Production File");

		if (Check_Control_Key (NEW_PRODUCTION_FORMAT)) {
			prod_file.Dbase_Format (Get_Control_String (NEW_PRODUCTION_FORMAT));
		}
		prod_file.Add_Field ("ZONE", DB_INTEGER, 6);

		for (i=0; i <= num_modes; i++) {
			if (i == num_modes) {
				text = "TOTAL";
			} else {
				text = mode_names [i];
			}
			prod_file.Add_Field (text, DB_DOUBLE, 16.2);
		}
		prod_file.Create (Project_Filename (key));
		prod_flag = true;
	}

	//---- create the attraction file ----

	key = Get_Control_String (NEW_ATTRACTION_FILE);

	if (!key.empty ()) {
		Print (1);
		attr_file.File_Type ("New Attraction File");

		if (Check_Control_Key (NEW_ATTRACTION_FORMAT)) {
			attr_file.Dbase_Format (Get_Control_String (NEW_ATTRACTION_FORMAT));
		}
		attr_file.Add_Field ("ZONE", DB_INTEGER, 6);

		for (i=0; i <= num_modes; i++) {
			if (i == num_modes) {
				text = "TOTAL";
			} else {
				text = mode_names [i];
			}
			attr_file.Add_Field (text, DB_DOUBLE, 16.2);
		}
		attr_file.Create (Project_Filename (key));
		attr_flag = true;
	}

	//---- open calibration target file ----

	key = Get_Control_String (CALIBRATION_TARGET_FILE);

	if (!key.empty ()) {
		Print (1);
		target_file.File_Type ("Calibration Target File");

		target_file.Open (Project_Filename (key));
		calib_flag = true;

		//---- calibration scaling factor ----

		scale_fac = Get_Control_Double (CALIBRATION_SCALING_FACTOR);

		//---- max calibration iterations ----

		max_iter = Get_Control_Integer (MAX_CALIBRATION_ITERATIONS);

		//---- calibration scaling factor ----

		exit_rmse = Get_Control_Double (CALIBRATION_EXIT_RMSE);

		//---- create the new mode constant file ----

		key = Get_Control_String (NEW_MODE_CONSTANT_FILE);

		if (!key.empty ()) {
			calib_file.File_Type ("New Mode Constant File");

			calib_file.Create (Project_Filename (key));
			output_flag = true;
		}

		//---- create the new calibration data file ----

		key = Get_Control_String (NEW_CALIBRATION_DATA_FILE);

		if (!key.empty ()) {
			data_file.File_Type ("New Calibration Data File");

			data_file.Create (Project_Filename (key));
			data_flag = true;
		}
	}

	//---- process select service keys ----

	Read_Select_Keys ();

	calib_report = Report_Flag (CALIB_REPORT);

	mode_value_flag = Report_Flag (MODE_VALUES);
	seg_value_flag = Report_Flag (SEGMENT_VALUES);
}
