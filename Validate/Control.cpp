//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Validate::Program_Control (void)
{
	int i, num;
	String key, record;
	Dtime low, high;
	Db_File label_file;
	Str_Map_Stat map_stat;

	//---- set equivalence flags ----

	line_flag = Report_Flag (LINE_GROUP);
	stop_flag = (Report_Flag (STOP_GROUP) || Report_Flag (BOARD_GROUP) || Report_Flag (ALIGHT_GROUP));

	Link_Equiv_Flag (Report_Flag (LINK_GROUP) || Report_Flag (GROUP_DETAILS));
	Zone_Equiv_Flag (Report_Flag (ZONE_GROUP));
	Stop_Equiv_Flag (stop_flag);
	Line_Equiv_Flag (line_flag);

	//---- open network files ----

	Data_Service::Program_Control ();
	
	Print (2, String ("%s Control Keys:") % Program ());	

	//---- check for traffic counts ----

	if (Check_Control_Key (TRAFFIC_COUNT_FILE)) {
		link_flag = true;

		if (Report_Flag (ZONE_GROUP)) {
			if (!System_File_Flag (ZONE)) {
				Error ("Zone File is Required for Zone Group Reports");
			}
			zone_flag = true;
		} else if (Report_Flag (AREA_TYPE)) {
			zone_flag = System_File_Flag (ZONE);
		}
	} else {
		if (Report_Flag (VOLUME_LEVEL) || Report_Flag (FACILITY_TYPE) ||
			Report_Flag (AREA_TYPE) || Report_Flag (ZONE_GROUP) ||
			Report_Flag (LINK_GROUP) || Report_Flag (GROUP_DETAILS)) {

			Error ("Traffic Count file is Required for Link-Based Reports");
		}
	}

	//---- check for turning movements ----

	if (Check_Control_Key (TURN_COUNT_FILE)) {
		if ((Report_Flag (TURN_MOVEMENT) || Report_Flag (TURN_LOS)) && 
			!System_File_Flag (CONNECTION)) {

			Error ("Connection File is Required for Turning Movement Reports");
		}
		if (Report_Flag (TURN_LOS) && !(System_File_Flag (SIGNAL) &&
			System_File_Flag (TIMING_PLAN) && System_File_Flag (PHASING_PLAN))) {

			Error ("Signal Data is Required for Turn Level of Service Analysis");
		}
		turn_flag = true;
	} else {
		if (Report_Flag (TURN_MOVEMENT) || Report_Flag (TURN_LOS)) {
			Error ("Turn Count File is Required for Turning Movement Reports");
		}
		System_File_False (POCKET);
		System_File_False (CONNECTION);
		System_File_False (SIGNAL);
		System_File_False (TIMING_PLAN);
		System_File_False (PHASING_PLAN);
	}

	//---- check for transit reports ----

	if (line_flag || stop_flag) {
		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE) || !System_File_Flag (TRANSIT_DRIVER) ||
			!System_File_Flag (RIDERSHIP)) {

			Error ("Transit Network and Ridership are Required for Transit Reports");
		}
		if (line_flag && !Check_Control_Key (LINE_EQUIVALENCE_FILE)) {
			Error ("Line Group Equivalance is Required for Line Group Reports");
		}
		if (line_flag && !Check_Control_Key (LINE_GROUP_COUNT_FILE)) {
			Error ("Line Group Counts are Required for Line Group Reports");
		}
		if (stop_flag && !Check_Control_Key (STOP_EQUIVALENCE_FILE)) {
			Error ("Stop Group Equivalance is Required for Stop Group Reports");
		}
		if (stop_flag && !Check_Control_Key (STOP_GROUP_COUNT_FILE)) {
			Error ("Stop Group Counts are Required for Stop Group Reports");
		}
	} else {
		System_File_False (TRANSIT_STOP);
		System_File_False (TRANSIT_ROUTE);
		System_File_False (TRANSIT_SCHEDULE);
		System_File_False (TRANSIT_DRIVER);
		System_File_False (RIDERSHIP);
	}

	//---- check for a link delay file ----

	if (!System_File_Flag (LINK_DELAY)) {

		//---- open the link volume file ----

		if (link_flag) {
			key = Get_Control_String (INPUT_VOLUME_FILE);

			if (key.empty ()) goto control_error;

			Print (1);
			volume_file.File_Type ("Input Volume File");

			volume_file.Open (Project_Filename (key));
		
			Print (1, "Number of Time Periods = ") << volume_file.Num_Periods ();
		}

		//---- open the turn movement file ----

		if (turn_flag) {
			key = Get_Control_String (TURN_VOLUME_FILE);

			if (key.empty ()) goto control_error;

			Print (1);
			turn_volume.File_Type ("Turn Volume File");

			turn_volume.Open (Project_Filename (key));
		}
	} else {
		delay_flag = true;
	}

	//---- open the traffic count file ----

	if (link_flag) {
		key = Get_Control_String (TRAFFIC_COUNT_FILE);

		Print (1);
		count_file.File_Type ("Traffic Count File");

		count_file.Open (Project_Filename (key));

		Print (1, "Number of Time Periods = ") << count_file.Num_Periods ();
	}

	//---- open the turn count file ----

	if (turn_flag) {
		key = Get_Control_String (TURN_COUNT_FILE);

		Print (1);
		turn_count.File_Type ("Turn Count File");

		turn_count.Open (Project_Filename (key));
	}

	//---- open the transit line group count file ----

	if (line_flag) {
		key = Get_Control_String (LINE_GROUP_COUNT_FILE);

		Print (1);
		line_count_file.File_Type ("Transit Line Group Count File");

		line_count_file.Open (Project_Filename (key));

		line_count_file.Required_Field ("GROUP", "LINE", "LINEGROUP", "LINEGRP", "ID");
		line_count_file.Required_Field ("COUNT", "RIDERS", "PASSENGERS", "TOTAL", "VOLUME");
	}

	//---- open the transit stop group count file ----

	if (stop_flag) {
		key = Get_Control_String (STOP_GROUP_COUNT_FILE);

		Print (1);
		stop_count_file.File_Type ("Transit Stop Group Count File");

		stop_count_file.Open (Project_Filename (key));

		stop_count_file.Required_Field ("GROUP", "STOP", "STOPGROUP", "STOPGRP", "ID");

		if (Report_Flag (STOP_GROUP)) {
			stop_count_file.Required_Field ("TOTAL",  "ONOFF", "ON_OFF", "PASSENGERS", "COUNT");
		}
		if (Report_Flag (BOARD_GROUP)) {
			stop_count_file.Required_Field ("BOARDINGS", "BOARD", "ON", "PASSENGERS", "COUNT");
		}
		if (Report_Flag (ALIGHT_GROUP)) {
			stop_count_file.Required_Field ("ALIGHTINGS", "ALIGHT", "OFF", "PASSENGERS", "COUNT");
		}
	}

	//---- create new volume file ----

	key = Get_Control_String (NEW_VOLUME_FILE);

	if (!key.empty ()) {
		if (!delay_flag && !link_flag) {
			Error ("Link Delay or Volume File is Required to Output Volume Data");
		}
		Print (1);
		output_file.File_Type ("New Volume File");

		output_file.Create (Project_Filename (key));
		output_flag = true;
	}

	//---- create new volume count file ----

	key = Get_Control_String (NEW_VOLUME_COUNT_FILE);

	if (!key.empty ()) {
		if (!delay_flag && !link_flag) {
			Error ("Volume and Count Files are Required to Output Volume Count Data");
		}
		Print (1);
		vol_cnt_file.File_Type ("New Volume Count File");

		vol_cnt_file.Create (Project_Filename (key));
		vc_flag = true;
	}

	//---- adjust the volume ranges ----

	hours = 24.0 * sum_periods.Range_Length () / Dtime (MIDNIGHT, SECONDS);

	if (hours < 12.0) {
		double adjust = (hours <= 4.0) ? 0.1 : 0.5;

		for (i=0; volume_level [i] > 0; i++) {
			volume_level [i] = (int) (volume_level [i] * adjust + 0.5);
		}
	}
	hours = hours / sum_periods.Num_Periods ();

	//---- check the data fields ----

	if (link_flag) {
		key = "Traffic Count";

		num = sum_periods.Num_Periods ();

		if (count_file.Num_Periods () < num) goto field_error;

		for (i=0; i < num; i++) {
			sum_periods.Period_Range (i, low, high);
			if (!count_file.In_Range (low) || !count_file.In_Range ((low + high) / 2)) goto field_error;
		}
		if (!delay_flag) {
			key = "Input Volume";

			if (volume_file.Num_Periods () < num) goto field_error;

			for (i=0; i < num; i++) {
				sum_periods.Period_Range (i, low, high);
				if (!volume_file.In_Range (low) || !volume_file.In_Range ((low + high) / 2)) goto field_error;
			}
		}
	}

	//---- read the analysis method ----

	key = Get_Control_Text (ANALYSIS_METHOD);

	if (!key.empty ()) {
		if (key.Equals ("VOLUME")) {
			method = false;
		} else if (key.Equals ("VMT")) {
			method = true;
		} else {
			Error (String ("Analysis Method = %s was Unrecognized") % key);
		}
	}
	header1 = String ("\n%29cNum.  ------%s------ ---Difference---  --Abs.Error--    Std.     %%     R   ----V/C----") %
				BLANK % ((method) ? "--VMT-" : "Volume") % FINISH;

	header2 = String ("    Obs.  Estimate  Observed   %6.6s      %%     Avg.     %%     Dev.   RMSE   Sq.   Avg.  Max.\n") %
				((method) ? "VMT" : "Volume") % FINISH;
	
	//---- read the adjustment factor ----

	factor = Get_Control_Double (ADJUSTMENT_FACTOR);

	//---- open the facility type labels ----

	if (Report_Flag (FACILITY_TYPE)) {
		key = Get_Control_String (FACILITY_TYPE_LABELS);

		if (!key.empty ()) {
			label_file.File_Type ("Facility Type Label File");
			Print (1);

			label_file.Open (Project_Filename (key));

			while (label_file.Read ()) {
				record = label_file.Record_String ();
				if (record.empty ()) continue;

				record.Split (key);
				i = key.Integer ();

				map_stat = facility_type.insert (Str_Map_Data (i, record));

				if (!map_stat.second) {
					Error ("Duplicate Facility Type Label");
				}
			}
			label_file.Close ();
		}
	}

	//---- open the area type labels ----

	if (Report_Flag (AREA_TYPE)) {
		key = Get_Control_String (AREA_TYPE_LABELS);

		if (!key.empty ()) {
			label_file.File_Type ("Area Type Label File");
			Print (1);

			label_file.Open (Project_Filename (key));

			while (label_file.Read ()) {
				record = label_file.Record_String ();
				if (record.empty ()) continue;

				record.Split (key);
				i = key.Integer ();

				map_stat = area_type.insert (Str_Map_Data (i, record));

				if (!map_stat.second) {
					Error ("Duplicate Area Type Label");
				}
			}
			label_file.Close ();
		}
	}

	//---- write the report names ----

	List_Reports ();

	//---- read the zone equiv ----

	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- process support data ----

	if (Link_Equiv_Flag ()) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	//---- read the line equiv ----

	if (line_flag) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	}

	//---- read the stop equiv ----

	if (stop_flag) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}
	return;

	//---- error message ----

field_error:
	Error (String ("The %s File has insufficient data for %d Time Increments") % key % num);

control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());
} 
