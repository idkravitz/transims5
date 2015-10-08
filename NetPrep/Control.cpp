//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NetPrep::Program_Control (void)
{
	String key;
	int i, j, num;

	//---- create the network files ----

	Data_Service::Program_Control ();

	//---- get the projection data ----

	proj_service.Read_Control ();

	projection.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

	Projection_Data output = proj_service.Output_Projection ();

	if (output.Type () != NO_PROJECTION && 
		((output.Units () != METERS && Metric_Flag ()) || 
		(output.Units () != FEET && !Metric_Flag ()))) {
	
		Error ("Projection Output and Units of Measure are Incompatible");
	}
	shape_flag = System_File_Flag (NEW_SHAPE);
	new_zone_flag = System_File_Flag (NEW_ZONE);

	Print (2, String ("%s Control Keys:") % Program ());	

	//---- open the input link file ----

	key = Get_Control_String (INPUT_LINK_FILE);

	if (key.empty ()) {
		if (!System_File_Flag (LINK)) goto control_error;
	} else {
		link_shape_flag = key.Ends_With (".shp");

		if (link_shape_flag) {

			//---- set the projection data ----

			link_shape_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

			link_shape_file.File_Type ("Input Link Shapefile");
			link_shape_file.File_ID ("Link");

			link_shape_file.Open (Project_Filename (key));
		} else {
			link_file.File_Type ("Input Link File");
			link_file.File_ID ("Link");
			link_flag = true;

			//---- get the file format ----

			if (Check_Control_Key (INPUT_LINK_FORMAT)) {
				link_file.Dbase_Format (Get_Control_String (INPUT_LINK_FORMAT));
			}
			link_file.Open (Project_Filename (key));
		}
	}

	//---- open the input node file ----

	key = Get_Control_String (INPUT_NODE_FILE);

	if (key.empty ()) {
		if (!System_File_Flag (NODE) && link_flag) goto control_error;
	} else {
		node_shape_flag = key.Ends_With (".shp");

		if (node_shape_flag) {

			//---- set the projection data ----

			node_shape_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

			node_shape_file.File_Type ("Input Node Shapefile");
			node_shape_file.File_ID ("Node");

			node_shape_file.Open (Project_Filename (key));
		} else {
			node_file.File_Type ("Input Node File");
			node_file.File_ID ("Node");
			node_flag = true;

			//---- get the file format ----

			if (Check_Control_Key (INPUT_NODE_FORMAT)) {
				node_file.Dbase_Format (Get_Control_String (INPUT_NODE_FORMAT));
			}
			node_file.Open (Project_Filename (key));
		}
		centroid_flag = (new_zone_flag && !System_File_Flag (ZONE));
	}

	//---- open the input zone file ----

	key = Get_Control_String (INPUT_ZONE_FILE);

	if (key.empty ()) {
		if (new_zone_flag && !System_File_Flag (ZONE) && !System_File_Flag (NODE) &&
			!node_flag && !node_shape_flag) {

			Error ("A Node or Zone file is required to created a New Zone File");
		}
	} else {
		zone_shape_flag = key.Ends_With (".shp");

		if (zone_shape_flag) {

			//---- set the projection data ----

			zone_shape_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

			zone_shape_file.File_Type ("Input Zone Shapefile");
			zone_shape_file.File_ID ("Zone");

			zone_shape_file.Open (Project_Filename (key));
		} else {
			zone_file.File_Type ("Input Zone File");
			zone_file.File_ID ("Zone");
			zone_flag = true;

			//---- get the file format ----

			if (Check_Control_Key (INPUT_ZONE_FORMAT)) {
				zone_file.Dbase_Format (Get_Control_String (INPUT_ZONE_FORMAT));
			}
			zone_file.Open (Project_Filename (key));
		}
	}

	//---- open the speed capacity table ----

	if (link_flag || link_shape_flag) {
		
		key = Get_Control_String (INPUT_SPDCAP_FILE);

		if (!key.empty ()) {
			spdcap_file.File_Type ("Input Speed-Capacity File");
			spdcap_file.File_ID ("SPDCAP");

			//---- get the file format ----

			if (Check_Control_Key (INPUT_SPDCAP_FORMAT)) {
				spdcap_file.Dbase_Format (Get_Control_String (INPUT_SPDCAP_FORMAT));
			} else {
				spdcap_file.Dbase_Format (FIXED_COLUMN);
			}
			spdcap_file.Open (Project_Filename (key));
			spdcap_flag = true;

			//---- input units of measure ----

			units_flag = (Measure_Code (Get_Control_Text (INPUT_UNITS_OF_MEASURE)) == METRIC);

			//---- facility index field ----

			if (Check_Control_Key (FACILITY_INDEX_FIELD)) {
				key = Get_Control_Text (FACILITY_INDEX_FIELD);

				if (link_shape_flag) {
					fac_fld = link_shape_file.Required_Field (key);
				} else {
					fac_fld = link_file.Required_Field (key);
				}
				Print (0, String (" (Number = %d)") % (fac_fld + 1));
			}

			//---- area type index field ----

			if (Check_Control_Key (AREA_TYPE_INDEX_FIELD)) {
				key = Get_Control_Text (AREA_TYPE_INDEX_FIELD);

				if (link_shape_flag) {
					at_fld = link_shape_file.Required_Field (key);
				} else {
					at_fld = link_file.Required_Field (key);
				}
				Print (0, String (" (Number = %d)") % (at_fld + 1));
			}
			if ((fac_fld < 0 && at_fld >= 0) || (fac_fld >= 0 && at_fld < 0)) {
				Error ("Facility and Area Types Index Fields are Required");
			}
		}
		Print (1);
	}

	//---- open the conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		script_file.File_Type ("Conversion Script");

		script_file.Open (Project_Filename (key));
		convert_flag = true;
	}

	//---- internal zone range ----

	key = Get_Control_Text (INTERNAL_ZONE_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		int_zone_range.Add_Ranges (key);
		int_zone_flag = true;
	}

	//---- keep zone connectors as locals ----

	connector_flag = Get_Control_Flag (KEEP_ZONE_CONNECTORS_AS_LOCALS);

	//---- first node number ----

	new_node = Get_Control_Integer (FIRST_NODE_NUMBER);

	//---- first link number ----

	new_link = Get_Control_Integer (FIRST_LINK_NUMBER);

	//---- speed factor by facility type ----
	
	Get_Control_List (SPEED_FACTOR_BY_FACILITY, spd_fac);

	for (i=1; i <= EXTERNAL; i++) {
		if (spd_fac.Best (i) > 1.0) {
			speed_flag = true;
			break;
		}
	}

	//---- speed rounding increment ----

	spd_inc = Round (Get_Control_Double (SPEED_ROUNDING_INCREMENT));

	speed_flag = (speed_flag || spd_inc > 0);

	//---- coordinate resolution ----

	resolution = Scale (Get_Control_Double (COORDINATE_RESOLUTION));

	//---- maximum length to xy ratio ----

	length_ratio = Get_Control_Double (MAXIMUM_LENGTH_TO_XY_RATIO);
	length_flag = (length_ratio > 0.0);

	//---- maximum shape angle ----

	max_angle = Get_Control_Integer (MAXIMUM_SHAPE_ANGLE);

	//---- minimum shape length ----

	min_length = Get_Control_Integer (MINIMUM_SHAPE_LENGTH);

	//---- drop dead end links ----

	drop_length = Get_Control_Integer (DROP_DEAD_END_LINKS);
	drop_flag = (drop_length > 0);
	if (drop_flag) {
		drop_length = Round (drop_length);
	}

	//---- drop short links ----

	short_links = Round (Get_Control_Double (DROP_SHORT_LINKS));
	short_flag = (short_links > 0);

	//---- split large loops ----

	split_length = Get_Control_Integer (SPLIT_LARGE_LOOPS);
	split_flag = (split_length > 0);
	if (split_flag) {
		if (split_flag && !shape_flag) {
			Warning ("A Shape File is needed to Split Large Loops");
		}
		split_length = Round (split_length);
	}

	//---- collape shape nodes ----

	collapse_flag = Get_Control_Flag (COLLAPSE_SHAPE_NODES);

	//---- collape divided arterials ----

	divided_flag = Get_Control_Flag (COLLAPSE_DIVIDED_ARTERIALS);

	//---- local thru segment lengths ----

	segment_flag = Get_Control_List_Groups (LOCAL_THRU_SEGMENT_LENGTHS, thru_length, true);

	//---- local selection spacing ----

	spacing_flag = Get_Control_List_Groups (LOCAL_SELECTION_SPACING, area_spacing, true);

	//---- keep node range ----

	key = Get_Control_Text (KEEP_NODE_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		keep_node_range.Add_Ranges (key);
		keep_node_flag = true;
	}

	//---- keep link range ----

	key = Get_Control_Text (KEEP_LINK_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		keep_link_range.Add_Ranges (key);
		keep_link_flag = true;
	}

	//---- keep node file ----

	key = Get_Control_String (KEEP_NODE_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Keep Node File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			keep_node_range.Add_Ranges (key);
		}
		file.Close ();
		keep_node_flag = true;
	}

	//---- keep link file ----

	key = Get_Control_String (KEEP_LINK_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Keep Link File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			keep_link_range.Add_Ranges (key);
		}
		file.Close ();
		keep_link_flag = true;
	}

	//---- delete node range ----

	key = Get_Control_Text (DELETE_NODE_RANGE);

	if (!key.empty () && !key.Equals ("NONE")) {
		drop_node_range.Add_Ranges (key);
		drop_node_flag = true;
	}

	//---- delete link range ----

	key = Get_Control_Text (DELETE_LINK_RANGE);

	if (!key.empty () && !key.Equals ("NONE")) {
		drop_link_range.Add_Ranges (key);
		drop_link_flag = true;
	}

	//---- delete node file ----

	key = Get_Control_String (DELETE_NODE_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Delete Node File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			drop_node_range.Add_Ranges (key);
		}
		file.Close ();
		drop_node_flag = true;
	}

	//---- delete link file ----

	key = Get_Control_String (DELETE_LINK_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Delete Link File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			drop_link_range.Add_Ranges (key);
		}
		file.Close ();
		drop_link_flag = true;
	}

	//----- new link detail file ----

	key = Get_Control_String (NEW_LINK_DETAIL_FILE);

	if (!key.empty ()) {
		if (!convert_flag) {
			Error ("Link Detail File requires a Conversion Script");
		}
		Print (1);
		detail_file.Create (Project_Filename (key));
		detail_flag = true;
	}

	//----- new link node list file ----

	key = Get_Control_String (NEW_LINK_NODE_LIST_FILE);

	if (!key.empty ()) {
		Print (1);
		link_node_file.File_Type ("New Link Node List File");
		link_node_file.File_ID ("LinkNode");

		link_node_file.Create (Project_Filename (key));
		link_node_flag = true;
	}

	//---- set the coordinate type ----

	if (shape_flag && link_shape_flag && link_shape_file.Z_Flag ()) {
		Shape_File *shape_file = (Shape_File *) System_File_Base (NEW_SHAPE);

		shape_file->Z_Flag (true);

		shape_file->Clear_Fields ();
		shape_file->Create_Fields ();
		shape_file->Write_Header ();
	}

	//---- transit route data ----

	if (System_File_Flag (NEW_ROUTE_NODES)) {
		route_flag = true;
		File_Group group_rec;
		Integer_List list;
		Int_Itr itr;
		String_List str_list;
		Str_Itr str_itr;

		Route_Nodes_File *route_nodes = (Route_Nodes_File *) System_File_Handle (NEW_ROUTE_NODES);

		Print (1);

		if (System_File_Flag (ROUTE_NODES)) {
			Copy_File_Header (ROUTE_NODES, NEW_ROUTE_NODES);

			num_periods = route_nodes->Num_Periods ();
			time_flag = route_nodes->Time_Flag ();
		} else {

			//---- get the time periods ----

			key = Get_Control_Text (TRANSIT_TIME_PERIODS);

			if (!key.empty ()) {
				schedule_periods.Add_Breaks (key);
			}
			num_periods = schedule_periods.Num_Periods ();
			route_nodes->Num_Periods (num_periods);

			//---- get the period travel time flag ----

			time_flag = Get_Control_Flag (PERIOD_TRAVEL_TIMES);
			route_nodes->Time_Flag (time_flag);

			//---- update the file header ----

			route_nodes->Dwell_Flag (false);
			route_nodes->Time_Flag (false);
			route_nodes->Speed_Flag (false);

			route_nodes->Clear_Fields ();
			route_nodes->Create_Fields ();
			route_nodes->Write_Header ();
		}

		//---- first route number ----

		new_route = Get_Control_Integer (FIRST_ROUTE_NUMBER);

		//---- input route format ----
		
		route_format = Format_Code (Get_Control_Text (INPUT_ROUTE_FORMAT));

		if (route_format != TPPLUS) {
			Error (String ("Input Route Format %s is Not Currently Supported") % Format_Code (route_format));
		}

		//---- process the mode map ----

		if (!Get_Control_List_Groups (ROUTE_MODE_MAP, str_list)) {
			Error ("Route Mode Map was Not Found");
		}
		for (str_itr = str_list.begin (); str_itr != str_list.end (); str_itr++) {
			j = Transit_Code (*str_itr);
			if (j == ANY_TRANSIT) {
				Error ("Route Mode Map is Out of Range");
			}
			mode_map.push_back (j);
		}

		//---- determine the number of file groups ----

		num = Highest_Control_Group (INPUT_ROUTE_FILE, 0);

		if (num == 0) {
			Error ("Missing Input Route Files");
		}

		for (i=1; i <= num; i++) {

			//---- open the input route file ----
			
			key = Get_Control_String (INPUT_ROUTE_FILE, i);
			if (key.empty ()) continue;

			group_rec.group = i;
			group_rec.line_file = new Db_File ();
			group_rec.period_map.assign ((num_periods + 1), 0);
			group_rec.flip = false;

			if (group_rec.line_file == NULL) {
				Error ("Creating a Route Group");
			}
			Print (1);
			group_rec.line_file->File_Type (String ("Input Route File #%d") % i);

			group_rec.line_file->Open (Project_Filename (key));

			//---- process the period map ----

			if (!Get_Control_List (ROUTE_PERIOD_MAP, list, i)) {
				Error ("Route Period Map was Not Found");
			}
			itr = list.begin ();

			for (j=0, ++itr;  itr != list.end (); itr++, j++) {
				if (j < num_periods) {
					group_rec.period_map [j] = *itr;
				}
			}

			//---- check the flip flag ----

			group_rec.flip = Get_Control_Flag (FLIP_ROUTE_FLAG, i);

			//---- save the record ----

			file_groups.push_back (group_rec);
		}
	}
	return;

control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());
}

