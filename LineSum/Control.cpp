//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LineSum::Program_Control (void)
{
	int i, num_files, anode, bnode;
	bool arcview_flag, z_flag, m_flag;
	String key, format;
	Strings ab_strings, node_pairs;
	Str_Itr str_itr, ab_itr;
	Db_Header *fh = 0;
	File_Itr file_itr;
	Line_Report_Data line_report_data;
	Link_Report_Data link_report_data;
	On_Off_Report_Data on_off_report_data;
	Access_Report_Data access_report_data;
	Stop_Report_Data stop_report_data;
	Total_Report_Data total_report_data;
	Link_Rider_Data link_rider_data;
	Dtime hour;

	//---- process the global control keys ----

	Execution_Service::Program_Control ();

	Metric_Flag (false);
	hour.Hours (1.0);

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- initialize the peak file list ----

	num_files = Highest_Control_Group (PEAK_RIDERSHIP_FILE, 0);

	if (num_files > 0) {
		peak_list.Initialize (num_files);
		Print (1);

		//---- open each file ----

		for (i=1; i <= num_files; i++) {

			//---- open the peak ridership file ----

			key = Get_Control_String (PEAK_RIDERSHIP_FILE, i);
			if (key.empty ()) continue;
	
			fh = peak_list [i-1];

			fh->File_Type (String ("Peak Ridership File #%d") % i);
			fh->File_ID (String ("Peak%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (PEAK_RIDERSHIP_FORMAT, i)) {
				fh->Dbase_Format (Get_Control_String (PEAK_RIDERSHIP_FORMAT, i));
			}
			fh->Open (Project_Filename (key));
		}
	}

	//---- initialize the offpeak file list ----

	num_files = Highest_Control_Group (OFFPEAK_RIDERSHIP_FILE, 0);

	if (num_files > 0) {
		offpeak_list.Initialize (num_files);
		Print (1);

		//---- open each file ----

		for (i=1; i <= num_files; i++) {

			//---- open the offpeak ridership file ----

			key = Get_Control_String (OFFPEAK_RIDERSHIP_FILE, i);
			if (key.empty ()) continue;
	
			fh = offpeak_list [i-1];

			fh->File_Type (String ("Offpeak Ridership File #%d") % i);
			fh->File_ID (String ("Offpeak%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (OFFPEAK_RIDERSHIP_FORMAT, i)) {
				fh->Dbase_Format (Get_Control_String (OFFPEAK_RIDERSHIP_FORMAT, i));
			}
			fh->Open (Project_Filename (key));
		}
	}
	if (peak_list.size () == 0 && offpeak_list.size () == 0) {
//		Error ("No Peak or Offpeak Ridership File Keys were Found");
	}

	//---- open the new peak ridership file ----

	Print (1);
	key = Get_Control_String (NEW_PEAK_RIDERSHIP_FILE);

	if (!key.empty ()) {
		if (peak_list.size () == 0) {
			Error ("Input Peak Ridership Files are Required");
		}
		fh = 0;

		for (file_itr = peak_list.begin (); file_itr != peak_list.end (); file_itr++) {
			if ((*file_itr)->Is_Open ()) {
				fh = *file_itr;
				break;
			}
		}
		if (fh == 0) Error ("Peak Input files are Not Open");

		new_peak_file.File_Type ("New Peak Ridership File");
		new_peak_file.File_ID ("NewPeak");

		//---- get the file format ----

		if (Check_Control_Key (NEW_PEAK_RIDERSHIP_FORMAT)) {
			new_peak_file.Dbase_Format (Get_Control_String (NEW_PEAK_RIDERSHIP_FORMAT));
		}
		new_peak_file.Header_Lines (fh->Header_Lines ());

		new_peak_file.Replicate_Fields (fh);

		new_peak_file.Create (Project_Filename (key));
	}

	//---- open the new offpeak ridership file ----

	key = Get_Control_String (NEW_OFFPEAK_RIDERSHIP_FILE);

	if (!key.empty ()) {
		if (offpeak_list.size () == 0) {
			Error ("Input Offpeak Ridership Files are Required");
		}
		fh = 0;

		for (file_itr = offpeak_list.begin (); file_itr != offpeak_list.end (); file_itr++) {
			if ((*file_itr)->Is_Open ()) {
				fh = *file_itr;
				break;
			}
		}
		if (fh == 0) Error ("Offpeak Input files are Not Open");

		new_offpeak_file.File_Type ("New Offpeak Ridership File");
		new_offpeak_file.File_ID ("NewOffpeak");

		//---- get the file format ----

		if (Check_Control_Key (NEW_OFFPEAK_RIDERSHIP_FORMAT)) {
			new_offpeak_file.Dbase_Format (Get_Control_String (NEW_OFFPEAK_RIDERSHIP_FORMAT));
		}
		new_offpeak_file.Header_Lines (fh->Header_Lines ());

		new_offpeak_file.Replicate_Fields (fh);

		new_offpeak_file.Create (Project_Filename (key));
	}

	//---- open the new total ridership file ----

	key = Get_Control_String (NEW_TOTAL_RIDERSHIP_FILE);

	if (!key.empty ()) {
		if (peak_list.size () == 0 || offpeak_list.size () == 0) {
			Error ("Input Peak and Offpeak Ridership Files are Required");
		}
		fh = 0;

		for (file_itr = peak_list.begin (); file_itr != peak_list.end (); file_itr++) {
			if ((*file_itr)->Is_Open ()) {
				fh = *file_itr;
				break;
			}
		}
		if (fh == 0) Error ("Peak Input files are Not Open");

		new_total_file.File_Type ("New Total Ridership File");
		new_total_file.File_ID ("NewTotal");

		//---- get the file format ----

		if (Check_Control_Key (NEW_TOTAL_RIDERSHIP_FORMAT)) {
			new_total_file.Dbase_Format (Get_Control_String (NEW_TOTAL_RIDERSHIP_FORMAT));
		}
		new_total_file.Header_Lines (fh->Header_Lines ());

		new_total_file.Replicate_Fields (fh);

		new_total_file.Create (Project_Filename (key));
	}

	//---- open the stop name file ----

	key = Get_Control_String (STOP_NAME_FILE);
	if (!key.empty ()) {
		Print (1);
		stop_name_file.File_Type ("Stop Name File");
		stop_name_file.File_ID ("StopName");
		station_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (STOP_NAME_FORMAT)) {
			stop_name_file.Dbase_Format (Get_Control_String (STOP_NAME_FORMAT));
		}
		stop_name_file.Open (Project_Filename (key));
	}

	//---- line report data ----

	num_files = Highest_Control_Group (LINE_REPORT_LINES);

	if (num_files > 0) {

		//---- read the line report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (LINE_REPORT_LINES, i)) continue;
			Print (1);

			//---- report number and title ----

			line_report_data.number = i;
			line_report_data.title = Get_Control_Text (LINE_REPORT_TITLE, i);

			//---- line range ----
			
			key = Get_Control_Text (LINE_REPORT_LINES, i);
			if (key.empty ()) continue;

			key.Parse (line_report_data.lines);

			//---- selected modes ----

			key = Get_Control_Text (LINE_REPORT_MODES, i);
			line_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				line_report_data.all_modes = false;
				if (!line_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				line_report_data.all_modes = true;
			}

			//---- report all nodes ----

			line_report_data.node_flag = Get_Control_Flag (LINE_REPORT_ALL_NODES, i);

			//---- store the control data ----

			line_report_array.push_back (line_report_data);
		}
	}

	//---- link report data ----

	num_files = Highest_Control_Group (LINK_REPORT_LINKS);

	if (num_files > 0) {

		//---- read the link report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (LINK_REPORT_LINKS, i)) continue;
			Print (1);

			//---- report number and title ----

			link_report_data.number = i;
			link_report_data.title = Get_Control_Text (LINK_REPORT_TITLE, i);

			//---- anode-bnode pairs ----
			
			key = Get_Control_Text (LINK_REPORT_LINKS, i);
			if (key.empty ()) continue;

			link_report_data.links = key;

			key.Parse (node_pairs);
			link_report_data.nodes.clear ();

			for (str_itr = node_pairs.begin (); str_itr != node_pairs.end (); str_itr++) {
				str_itr->Parse (ab_strings, "-");

				for (anode=0, ab_itr = ab_strings.begin (); ab_itr != ab_strings.end (); ab_itr++, anode = bnode) {
					bnode = ab_itr->Integer ();
					if (anode > 0) {
						link_report_data.nodes.push_back (anode);
						link_report_data.nodes.push_back (bnode);
					}
				}
			}

			//---- selected modes ----

			key = Get_Control_Text (LINK_REPORT_MODES, i);
			link_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				link_report_data.all_modes = false;
				if (!link_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				link_report_data.all_modes = true;
			}

			//---- selected lines ----

			key = Get_Control_Text (LINK_REPORT_LINES, i);
			link_report_data.lines.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				link_report_data.all_lines = false;
				key.Parse (link_report_data.lines);
			} else {
				link_report_data.all_lines = true;
			}

			//---- oneway/twoway flag ----

			link_report_data.dir_flag = Get_Control_Flag (LINK_REPORT_ONEWAY, i);

			//---- store the control data ----

			link_report_array.push_back (link_report_data);
		}
	}

	//---- on-off report data ----

	num_files = Highest_Control_Group (ON_OFF_REPORT_STOPS);

	if (num_files > 0) {
		on_off_flag = true;

		//---- read the boarding report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (ON_OFF_REPORT_STOPS, i)) continue;
			Print (1);

			//---- report number and title ----

			on_off_report_data.number = i;
			on_off_report_data.title = Get_Control_Text (ON_OFF_REPORT_TITLE, i);

			//---- stop range ----

			key = Get_Control_Text (ON_OFF_REPORT_STOPS, i);
			if (key.empty ()) continue;

			on_off_report_data.stops.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				on_off_report_data.all_stops = false;
				if (!on_off_report_data.stops.Add_Ranges (key)) {
					exe->Error ("Adding Stop Ranges");
				}
			} else {
				on_off_report_data.all_stops = true;
			}

			//---- select modes ----

			key = Get_Control_Text (ON_OFF_REPORT_MODES, i);

			on_off_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				on_off_report_data.all_modes = false;
				if (!on_off_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				on_off_report_data.all_modes = true;
			}

			//---- level of detail ----

			key = Get_Control_Text (ON_OFF_REPORT_DETAILS, i);

			if (!key.empty ()) {
				char ch = key [0];
				if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';

				if (ch == 'T' || ch == 'Y' || ch == '1') {
					on_off_report_data.details = 1;
				} else if (ch == 'M' || ch == '2') {
					on_off_report_data.details = 2;
				} else {
					on_off_report_data.details = 0;
				}
			} else {
				on_off_report_data.details = 0;
			}

			//---- output data file ----

			key = Get_Control_String (NEW_ON_OFF_REPORT_FILE, i);

			if (!key.empty ()) {
				on_off_report_data.file = new Db_Header ();
				on_off_report_data.file->File_Type (String ("New On-Off Report File #%d") % i);

				if (Check_Control_Key (NEW_ON_OFF_REPORT_FORMAT, i)) {
					on_off_report_data.file->Dbase_Format (Get_Control_String (NEW_ON_OFF_REPORT_FORMAT, i));
				}
				on_off_report_data.file->Create (Project_Filename (key));
			} else {
				on_off_report_data.file = 0;
			}

			//---- store the control data ----

			on_off_report_array.push_back (on_off_report_data);
		}
	}

	//---- access report data ----

	num_files = Highest_Control_Group (ACCESS_REPORT_STOPS);

	if (num_files > 0) {
		access_flag = true;

		//---- read the access report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (ACCESS_REPORT_STOPS, i)) continue;
			Print (1);

			//---- report number and title ----

			access_report_data.number = i;
			access_report_data.title = Get_Control_Text (ACCESS_REPORT_TITLE, i);

			//---- stop range ----

			key = Get_Control_Text (ACCESS_REPORT_STOPS, i);
			if (key.empty ()) continue;

			access_report_data.stops.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				access_report_data.all_stops = false;
				if (!access_report_data.stops.Add_Ranges (key)) {
					exe->Error ("Adding Stop Ranges");
				}
			} else {
				access_report_data.all_stops = true;
			}

			//---- select modes ----

			key = Get_Control_Text (ACCESS_REPORT_MODES, i);

			access_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				access_report_data.all_modes = false;
				if (!access_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				access_report_data.all_modes = true;
			}

			//---- level of detail ----

			key = Get_Control_Text (ACCESS_REPORT_DETAILS, i);

			if (!key.empty ()) {
				char ch = key [0];
				if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';

				if (ch == 'T' || ch == 'Y' || ch == '1') {
					access_report_data.details = 1;
				} else if (ch == 'M' || ch == '2') {
					access_report_data.details = 2;
				} else {
					access_report_data.details = 0;
				}
			} else {
				access_report_data.details = 0;
			}

			//---- output data file ----

			key = Get_Control_String (NEW_ACCESS_REPORT_FILE, i);

			if (!key.empty ()) {
				access_report_data.file = new Db_Header ();
				access_report_data.file->File_Type (String ("New Access Report File #%d") % i);

				if (Check_Control_Key (NEW_ACCESS_REPORT_FORMAT, i)) {
					access_report_data.file->Dbase_Format (Get_Control_String (NEW_ACCESS_REPORT_FORMAT, i));
				}
				access_report_data.file->Create (Project_Filename (key));
			} else {
				access_report_data.file = 0;
			}

			//---- store the control data ----

			access_report_array.push_back (access_report_data);
		}
	}

	//---- stop report data ----

	num_files = Highest_Control_Group (STOP_REPORT_STOPS);

	if (num_files > 0) {
		access_flag = true;

		//---- read the stop report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (STOP_REPORT_STOPS, i)) continue;
			Print (1);

			//---- report number and title ----

			stop_report_data.number = i;
			stop_report_data.title = Get_Control_Text (STOP_REPORT_TITLE, i);

			//---- stop range ----
			
			key = Get_Control_Text (STOP_REPORT_STOPS, i);
			if (key.empty ()) continue;

			stop_report_data.stops.clear ();

			if (!stop_report_data.stops.Add_Ranges (key)) {
				exe->Error ("Adding Stop Ranges");
			}

			//---- select modes ----

			key = Get_Control_Text (STOP_REPORT_MODES, i);
			stop_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				stop_report_data.all_modes = false;
				if (!stop_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				stop_report_data.all_modes = true;
			}

			//---- line range ----

			key = Get_Control_Text (STOP_REPORT_LINES, i);
			stop_report_data.lines.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				stop_report_data.all_lines = false;
				key.Parse (stop_report_data.lines);
			} else {
				stop_report_data.all_lines = true;
			}

			//---- transfer modes ----

			key = Get_Control_Text (STOP_REPORT_TRANSFERS, i);
			stop_report_data.transfers.clear ();

			if (!key.empty () && !key.Equals ("NONE")) {
				if (!stop_report_data.transfers.Add_Ranges (key)) {
					exe->Error ("Adding Transfer Mode Ranges");
				}
			}

			//---- store the control data ----

			stop_report_array.push_back (stop_report_data);
		}
	}

	//---- total report data ----

	num_files = Highest_Control_Group (TOTAL_REPORT_LINES);

	if (num_files > 0) {

		//---- read the total report keys ----

		for (i=1; i <= num_files; i++) {
			if (!Check_Control_Key (TOTAL_REPORT_LINES, i)) continue;
			Print (1);

			//---- report number and title ----

			total_report_data.number = i;
			total_report_data.title = Get_Control_Text (TOTAL_REPORT_TITLE, i);

			//---- line range ----

			key = Get_Control_Text (TOTAL_REPORT_LINES, i);
			if (key.empty ()) continue;

			key.Parse (total_report_data.lines);

			//---- selected modes ----

			key = Get_Control_Text (TOTAL_REPORT_MODES, i);
			
			total_report_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				total_report_data.all_modes = false;
				if (!total_report_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				total_report_data.all_modes = true;
			}

			//---- output data file ----

			key = Get_Control_String (NEW_TOTAL_REPORT_FILE, i);
			if (!key.empty ()) {
				total_report_data.file = new Db_Header ();
				total_report_data.file->File_Type (String ("New Total Report File #%d") % i);

				if (Check_Control_Key (NEW_TOTAL_REPORT_FORMAT, i)) {
					total_report_data.file->Dbase_Format (Get_Control_String (NEW_TOTAL_REPORT_FORMAT, i));
				}
				total_report_data.file->Create (Project_Filename (key));
			} else {
				total_report_data.file = 0;
			}

			//---- store the control data ----

			total_report_array.push_back (total_report_data);
		}
	}

	//---- link rider file data ----

	num_files = Highest_Control_Group (NEW_LINK_RIDER_FILE);

	if (num_files > 0) {
		arcview_flag = false;

		//---- read the link rider keys ----

		for (i=1; i <= num_files; i++) {

			//---- output data file ---

			key = Get_Control_String (NEW_LINK_RIDER_FILE, i);
			if (key.empty ()) continue;
			
			link_rider_data.number = i;

			if (Check_Control_Key (NEW_LINK_RIDER_FORMAT, i)) {
				format = Get_Control_String (NEW_LINK_RIDER_FORMAT, i);

				link_rider_data.arcview_flag = format.Equals ("ARCVIEW");
			} else {
				link_rider_data.arcview_flag = false;
			}
			Print (1);
			if (link_rider_data.arcview_flag) {
				link_rider_data.file = link_rider_data.arc_file = new Arcview_File ();

				link_rider_data.arc_file->File_Type (String ("New Link Rider File #%d") % i);
				link_rider_data.arc_file->Shape_Type (VECTOR);
				link_rider_data.arc_file->Z_Flag (z_flag);
				link_rider_data.arc_file->M_Flag (m_flag);

				link_rider_data.arc_file->Create (Project_Filename (key));

				link_rider_data.arc_file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
				arcview_flag = true;
			} else {
				link_rider_data.file = new Db_Header ();

				if (Check_Control_Key (NEW_LINK_RIDER_FORMAT, i)) {
					link_rider_data.file->Dbase_Format (format);
				}
				link_rider_data.file->File_Type (String ("New Link Rider File #%d") % i);
				link_rider_data.file->Create (Project_Filename (key));
			}

			//---- selected modes ----

			key = Get_Control_Text (LINK_RIDER_MODES, i);
			link_rider_data.modes.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				link_rider_data.all_modes = false;
				if (!link_rider_data.modes.Add_Ranges (key)) {
					exe->Error ("Adding Modes Ranges");
				}
			} else {
				link_rider_data.all_modes = true;
			}

			//---- selected lines ----

			key = Get_Control_Text (LINK_RIDER_LINES, i);
			link_rider_data.lines.clear ();

			if (!key.empty () && !key.Equals ("ALL")) {
				link_rider_data.all_lines = false;
				key.Parse (link_rider_data.lines);
			} else {
				link_rider_data.all_lines = true;
			}
			
			//---- peak hours ----

			link_rider_data.peak_hours = (double) Get_Control_Time (LINK_RIDER_PEAK_HOURS, i) / hour;

			//---- peak factor ----

			link_rider_data.peak_fac = Get_Control_Double (LINK_RIDER_PEAK_FACTOR, i);

			//---- peak capacity ----

			link_rider_data.peak_cap = Get_Control_Double (LINK_RIDER_PEAK_CAPACITY, i);
			
			//---- offpeak hours ----

			link_rider_data.offpeak = (double) Get_Control_Time (LINK_RIDER_OFFPEAK_HOURS, i) / hour;

			//---- open node file for arcview output ----

			if (link_rider_data.arcview_flag) {
				key = Get_Control_String (LINK_RIDER_XY_FILE, i);

				if (key.empty ()) {
					Error ("A Link Rider XY File is required for Arcview Output");
				}
				link_rider_data.xy_file = new Db_Header ();
				link_rider_data.xy_file->File_Type (String ("Link Rider XY File #%d") % i);

				if (Check_Control_Key (LINK_RIDER_XY_FORMAT, i)) {
					link_rider_data.xy_file->Dbase_Format (Get_Control_String (LINK_RIDER_XY_FORMAT, i));
				}
				link_rider_data.xy_file->Open (Project_Filename (key));

				link_rider_data.node_fld = link_rider_data.xy_file->Required_Field (NODE_FIELD_NAMES);
				link_rider_data.xcoord_fld = link_rider_data.xy_file->Required_Field (X_FIELD_NAMES);
				link_rider_data.ycoord_fld = link_rider_data.xy_file->Required_Field (Y_FIELD_NAMES);

				link_rider_data.offset = Get_Control_Double (LINK_RIDER_SIDE_OFFSET, i);
			} else {
				link_rider_data.offset = 0.0;
			}

			//---- store the control data ----

			link_rider_array.push_back (link_rider_data);
		}

		//---- open the link shape file ----

		if (arcview_flag) {
			key = Get_Control_String (LINK_SHAPE_FILE);

			if (!key.empty ()) {
				Print (1);
				shape_flag = true;

				link_shape.File_Type ("Link Shape File");
				link_shape.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

				link_shape.Open (Project_Filename (key));

				//---- get the field numbers ----

				key = Get_Control_Text (LINK_SHAPE_ANODE);

				anode_field = link_shape.Required_Field (key);
				Print (0, ", Number=") << anode_field;
				
				key = Get_Control_Text (LINK_SHAPE_BNODE);

				bnode_field = link_shape.Required_Field (key);
				Print (0, ", Number=") << bnode_field;
			}
		}

		//---- process service level files ----

		key = Get_Control_String (SERVICE_FILE);

		if (!key.empty ()) {
			Print (1);
			service_file.File_Type ("Service File");
			service_flag = true;

			//---- get the file format ----

			if (Check_Control_Key (SERVICE_FORMAT)) {
				service_file.Dbase_Format (Get_Control_String (SERVICE_FORMAT));
			}
			service_file.Open (Project_Filename (key));

			//---- get the field numbers ----

			key = Get_Control_Text (SERVICE_LINE_FIELD);

			line_fld = service_file.Required_Field (key);
			Print (0, ", Number=") << line_fld;

			if (Check_Control_Key (SERVICE_PEAK_FIELD)) {
				key = Get_Control_Text (SERVICE_PEAK_FIELD);

				peak_fld = service_file.Required_Field (key);
				Print (0, ", Number=") << peak_fld;
			}
			if (Check_Control_Key (SERVICE_OFFPEAK_FIELD)) {
				key = Get_Control_Text (SERVICE_OFFPEAK_FIELD);

				offpeak_fld = service_file.Required_Field (key);
				Print (0, ", Number=") << offpeak_fld;
			}
		}
	}

	//---- initialize the base routes ----

	num_files = Highest_Control_Group (BASE_ROUTE_FILE, 0);

	if (num_files > 0) {
		base_routes.Initialize (num_files);
		Print (1);

		//---- open each file ----

		for (i=1; i <= num_files; i++) {

			//---- open the base ridership file ----

			key = Get_Control_String (BASE_ROUTE_FILE, i);
			if (key.empty ()) continue;
	
			fh = base_routes [i-1];

			fh->File_Type (String ("Base Route File #%d") % i);
			fh->File_ID (String ("Base%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (BASE_ROUTE_FORMAT, i)) {
				fh->Dbase_Format (Get_Control_String (BASE_ROUTE_FORMAT, i));
			}
			fh->Open (Project_Filename (key));
		}
	}

	//---- initialize the alternative routes ----

	num_files = Highest_Control_Group (ALTERNATIVE_ROUTE_FILE, 0);

	if (num_files > 0) {
		alt_routes.Initialize (num_files);
		Print (1);

		//---- open each file ----

		for (i=1; i <= num_files; i++) {

			//---- open the alternative ridership file ----

			key = Get_Control_String (ALTERNATIVE_ROUTE_FILE, i);
			if (key.empty ()) continue;
	
			fh = alt_routes [i-1];

			fh->File_Type (String ("Alternative Route File #%d") % i);
			fh->File_ID (String ("Alte%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (ALTERNATIVE_ROUTE_FORMAT, i)) {
				fh->Dbase_Format (Get_Control_String (ALTERNATIVE_ROUTE_FORMAT, i));
			}
			fh->Open (Project_Filename (key));
		}
	}
}

