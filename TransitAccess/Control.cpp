//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TransitAccess.hpp"

#include "TppMatrix.hpp"
#include "TcadMatrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransitAccess::Program_Control (void)
{
	int i, num, value, max_value;
	double factor;
	String key;
	Strings ranges;
	String_List field_list;
	Str_Itr str_itr;

	Db_Mat_Ptr matrix_ptr;
	Skim_Data skim_data;
	Mode_Group mode_group, *mode_ptr;
	Data_Range data_range, *range_ptr;
	Data_Range_Itr range_itr;
	Format_Type type;
	
	Execution_Service::Program_Control ();
	
	Metric_Flag (false);

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);

	//---- open zone file ----

	key = Get_Control_String (ZONE_FILE);

	if (!key.empty ()) {
		zone_file.File_Type ("Zone File");

		if (Check_Control_Key (ZONE_FORMAT)) {
			zone_file.Dbase_Format (Get_Control_String (ZONE_FORMAT));
		}
		zone_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (ZONE_NUMBER_FIELD);

		zone_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_fld;

		key = Get_Control_Text (ZONE_X_COORD_FIELD);

		zone_x_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_x_fld;

		key = Get_Control_Text (ZONE_Y_COORD_FIELD);

		zone_y_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_y_fld;

		if (Check_Control_Key (ZONE_LOCATION_FIELD)) {
			key = Get_Control_Text (ZONE_LOCATION_FIELD);

			zone_loc_fld = zone_file.Required_Field (key);
			Print (0, ", Number=") << zone_loc_fld;
		}
	
		key = Get_Control_Text (ZONE_AREA_FIELD);

		zone_area_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_area_fld;

		key = Get_Control_Text (ZONE_LONG_WALK_FIELD);

		zone_long_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_long_fld;

		key = Get_Control_Text (ZONE_PEF_FIELD);

		zone_pef_fld = zone_file.Required_Field (key);
		Print (0, ", Number=") << zone_pef_fld;

		//---- processings factors ----

		Get_Control_List (PEF_DISTANCE_WEIGHTS, field_list);

		field_list.erase (field_list.begin ());

		for (str_itr = field_list.begin (); str_itr != field_list.end (); str_itr++) {
			str_itr->Parse (ranges, ":");

			if (ranges.size () < 2) {
				Error ("PEF Distance Weights should be MAX_PEF:Factor");
			}
			value = ranges [0].Integer ();
			factor = ranges [1].Double ();

			pef_values.push_back (value);
			pef_factors.push_back (factor);
		}

		//---- get the area distance factors ----

		Get_Control_List (ZONE_AREA_FACTORS, walk_factors);
		walk_factors.erase (walk_factors.begin ());

		//---- zone station mode codes ----

		Get_Control_List (ZONE_STATION_MODE_CODES, field_list);
		
		field_list.erase (field_list.begin ());

		for (str_itr = field_list.begin (); str_itr != field_list.end (); str_itr++) {
			mode_codes.push_back ((int) (*str_itr) [0]);
		}
		Print (1);
	}

	//---- open node file ----

	key = Get_Control_String (NODE_FILE);

	if (!key.empty ()) {
		node_file.File_Type ("Node File");

		if (Check_Control_Key (NODE_FORMAT)) {
			node_file.Dbase_Format (Get_Control_String (NODE_FORMAT));
		}
		node_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (NODE_NUMBER_FIELD);

		node_fld = node_file.Required_Field (key);
		Print (0, ", Number=") << node_fld;

		key = Get_Control_Text (NODE_X_COORD_FIELD);

		node_x_fld = node_file.Required_Field (key);
		Print (0, ", Number=") << node_x_fld;

		key = Get_Control_Text (NODE_Y_COORD_FIELD);

		node_y_fld = node_file.Required_Field (key);
		Print (0, ", Number=") << node_y_fld;
		Print (1);
	}

	//---- open link data file ----

	key = Get_Control_String (WALK_LINK_FILE);

	if (!key.empty ()) {
		walk_link_flag = true;
		link_data_file.File_Type ("Walk Link File");

		if (Check_Control_Key (WALK_LINK_FORMAT)) {
			link_data_file.Dbase_Format (Get_Control_String (WALK_LINK_FORMAT));
		}
		link_data_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (WALK_LINK_ANODE_FIELD);

		link_anode_fld = link_data_file.Required_Field (key);
		Print (0, ", Number=") << link_anode_fld;
		
		key = Get_Control_Text (WALK_LINK_BNODE_FIELD);

		link_bnode_fld = link_data_file.Required_Field (key);
		Print (0, ", Number=") << link_bnode_fld;
		
		key = Get_Control_Text (WALK_LINK_LENGTH_FIELD);

		link_len_fld = link_data_file.Required_Field (key);
		Print (0, ", Number=") << link_len_fld;
		
		key = Get_Control_Text (WALK_LINK_ZONE_FIELD);

		link_zone_fld = link_data_file.Required_Field (key);
		Print (0, ", Number=") << link_zone_fld;
		
		key = Get_Control_Text (WALK_LINK_TYPE_FIELD);

		link_type_fld = link_data_file.Required_Field (key);
		Print (0, ", Number=") << link_type_fld;

		//---- get the type range ----

		key = Get_Control_Text (WALK_LINK_TYPE_RANGE);
		type_range.Add_Ranges (key);
	}

	//---- open extra link file ----

	key = Get_Control_String (EXTRA_LINK_FILE);

	if (!key.empty ()) {
		extra_flag = true;
		extra_link_file.File_Type ("Extra Link File");

		if (Check_Control_Key (EXTRA_LINK_FORMAT)) {
			extra_link_file.Dbase_Format (Get_Control_String (EXTRA_LINK_FORMAT));
		}
		extra_link_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (EXTRA_LINK_ACTION_FIELD);

		extra_action_fld = extra_link_file.Required_Field (key);
		Print (0, ", Number=") << extra_action_fld;

		key = Get_Control_Text (EXTRA_LINK_ANODE_FIELD);

		extra_anode_fld = extra_link_file.Required_Field (key);
		Print (0, ", Number=") << extra_anode_fld;

		key = Get_Control_Text (EXTRA_LINK_BNODE_FIELD);

		extra_bnode_fld = extra_link_file.Required_Field (key);
		Print (0, ", Number=") << extra_bnode_fld;
	}

	//---- open station file ----

	key = Get_Control_String (STATION_FILE);

	if (!key.empty ()) {
		station_file.File_Type ("Station File");

		if (Check_Control_Key (STATION_FORMAT)) {
			station_file.Dbase_Format (Get_Control_String (STATION_FORMAT));
		}
		station_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (STATION_MODE_FIELD);

		station_mode_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_mode_fld;

		key = Get_Control_Text (STATION_TYPE_FIELD);

		station_type_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_type_fld;

		key = Get_Control_Text (STATION_PNR_FLAG_FIELD);

		station_pnr_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_pnr_fld;

		key = Get_Control_Text (STATION_USE_FLAG_FIELD);

		station_use_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_use_fld;

		key = Get_Control_Text (STATION_SKIM_FIELD);

		station_skim_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_skim_fld;

		key = Get_Control_Text (STATION_ZONE_FIELD);

		station_zone_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_zone_fld;

		key = Get_Control_Text (STATION_STOP_FIELD);

		station_stop_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_stop_fld;

		key = Get_Control_Text (STATION_PARKING_FIELD);

		station_park_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_park_fld;

		num = Highest_Control_Group (STATION_NODE_FIELDS, 0);
		if (num > 0) {
			Get_Control_List_Groups (STATION_NODE_FIELDS, field_list);
			field_list.erase (field_list.begin ());

			for (str_itr = field_list.begin (); str_itr != field_list.end (); str_itr++) {
				num = station_file.Required_Field (*str_itr);

				if (station_node_flds.size () == 0) {
					Print (0, ", Numbers=") << num;
				} else {
					Print (0, ", ") << num;
				}
				station_node_flds.push_back (num);
			}
		}
		if (Check_Control_Key (STATION_ACCESS_FIELD)) {
			key = Get_Control_Text (STATION_ACCESS_FIELD);

			station_acc_fld = station_file.Required_Field (key);
			Print (0, ", Number=") << station_acc_fld;
			station_acc_flag = true;
		}
		if (Check_Control_Key (STATION_CAPACITY_FIELD)) {
			key = Get_Control_Text (STATION_CAPACITY_FIELD);

			station_cap_fld = station_file.Required_Field (key);
			Print (0, ", Number=") << station_cap_fld;
		}
		num = Highest_Control_Group (STATION_COST_FIELDS, 0);
		if (num > 0) {
			Get_Control_List_Groups (STATION_COST_FIELDS, field_list);
			field_list.erase (field_list.begin ());

			for (str_itr = field_list.begin (); str_itr != field_list.end (); str_itr++) {
				num = station_file.Required_Field (*str_itr);

				if (station_cost_flds.size () == 0) {
					Print (0, ", Numbers=") << num;
				} else {
					Print (0, ", ") << num;
				}
				station_cost_flds.push_back (num);
				cost_flag = true;
			}
		}
		num = Highest_Control_Group (STATION_TIME_FIELDS, 0);
		if (num > 0) {
			Get_Control_List_Groups (STATION_TIME_FIELDS, field_list);
			field_list.erase (field_list.begin ());

			for (str_itr = field_list.begin (); str_itr != field_list.end (); str_itr++) {
				num = station_file.Required_Field (*str_itr);

				if (station_time_flds.size () == 0) {
					Print (0, ", Numbers=") << num;
				} else {
					Print (0, ", ") << num;
				}
				station_time_flds.push_back (num);
				cost_flag = true;
			}
		}
		key = Get_Control_Text (STATION_X_COORD_FIELD);

		station_x_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_x_fld;

		key = Get_Control_Text (STATION_Y_COORD_FIELD);

		station_y_fld = station_file.Required_Field (key);
		Print (0, ", Number=") << station_y_fld;

		if (Check_Control_Key (STATION_NAME_FIELD)) {
			key = Get_Control_Text (STATION_NAME_FIELD);

			station_name_fld = station_file.Required_Field (key);
			Print (0, ", Number=") << station_name_fld;
		}
		Print (1);
	}

	//---- open skim files ----

	num = Highest_Control_Group (SKIM_FILE, 0);
	if (num == 0) Error ("No Skim Files were Found");
	
	skim_files.reserve (num);
	highest_skim = num;

	for (i=1; i <= num; i++) {
		key = Get_Control_String (SKIM_FILE, i);
		if (key.empty ()) continue;

		skim_data.number = i;

		key = Project_Filename (key);

		type = Db_Header::Def_Format (key);

		if (type == UNFORMATED) {
			if (Check_Control_Key (SKIM_FORMAT, i)) {
				type = Format_Code (Get_Control_String (SKIM_FORMAT, i));
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

		if (matrix_ptr->Zones () > ndes) ndes = matrix_ptr->Zones ();

		skim_data.index = (int) skim_files.size ();
		skim_files.push_back (matrix_ptr);

		//---- get the table numbers ----

		skim_data.time_table = Get_Control_Integer (SKIM_TIME_TABLE, i);

		skim_data.dist_table = Get_Control_Integer (SKIM_DISTANCE_TABLE, i);

		skim_array.push_back (skim_data);
		Print (1);
	}

	//---- value of time ----

	time_value = Get_Control_Double (VALUE_OF_TIME);
	if (time_value > 0.0) {
		time_value = 0.6 / time_value;		//---- minutes / cent ----
	}
	Print (1);

	//---- get the cbd zone number ----

	cbd_zone = Get_Control_Integer (CBD_ZONE_NUMBER);

	//---- get the maximum diversion data ----

	max_ratio = Get_Control_Double (MAX_DIVERSION_RATIO);
	max_dist = Get_Control_Double (MAX_DIVERSION_DISTANCE);
	Print (1);

	//---- get the barrier location map ----

	num = Highest_Control_Group (BARRIER_LOCATION_MAP, 0);
	if (num > 0) {
		max_value = 0;
		location_map.assign (num, data_range);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (BARRIER_LOCATION_MAP, i);
			if (key.empty () || key.Equals ("ALL")) continue;

			range_ptr = &location_map [i-1];

			range_ptr->Add_Ranges (key);

			value = range_ptr->Max_Value ();
			if (value > max_value) max_value = value;
		}

		//---- check for duplicates ----

		for (value=1; value <= max_value; value++) {
			num = 0;

			for (range_itr = location_map.begin (); range_itr != location_map.end (); range_itr++) {
				if (range_itr->In_Range (value)) num++;
			}
			if (num > 1) {
				Warning (String ("Location %d is Assigned to Multiple Barrier Groups") % value);
			}
		}

		//---- get the barriers ----

		key = Get_Control_Text (PROHIBITED_INTERCHANGES);
		if (key.empty ()) {
			Error ("Prohibited Interchanges are Required for Barrier Processing");
		}
		key.Parse (ranges);

		for (str_itr = ranges.begin (); str_itr != ranges.end (); str_itr++) {
			if (str_itr->Range (value, max_value)) {
				barriers.insert (Int2_Key (value, max_value));
			}
		}
		Print (1);
	}

	//---- open slug zone file ----

	key = Get_Control_String (SLUG_ZONE_FILE);

	if (!key.empty ()) {
		slug_flag = true;
		slug_file.File_Type ("Slug Zone File");

		if (Check_Control_Key (SLUG_ZONE_FORMAT)) {
			slug_file.Dbase_Format (Get_Control_String (SLUG_ZONE_FORMAT));
		}
		slug_file.Open (Project_Filename (key));
		Print (1);

		//---- get the field numbers ----

		key = Get_Control_Text (SLUG_ZONE_FIELD);

		slug_fld = slug_file.Required_Field (key);
		Print (0, ", Number=") << slug_fld;

		slug_type = Get_Control_Integer (SLUG_STATION_TYPE);
		Print (1);
	}

	//---- new sidewalk file ----

	key = Get_Control_String (NEW_SIDEWALK_FILE);
	if (!key.empty ()) {
		if (!walk_link_flag) {
			Error ("A Walk Link File is required for Sidewalk Processing");
		}
		sidewalk_file.File_Type ("New Sidewalk File");
		sidewalk_file.File_Format (UNFORMATED);
		sidewalk_file.Create (Project_Filename (key));
	} else if (walk_link_flag) {
		Error ("A Sidewalk File is required for Walk Link Processing");
	}

	//---- new walk access file ----

	key = Get_Control_String (NEW_WALK_ACCESS_FILE);

	if (!key.empty ()) {
		zone_walk_flag = true;
		walk_acc_file.File_Type ("New Walk Access File");
		walk_acc_file.File_Format (UNFORMATED);
		walk_acc_file.Create (Project_Filename (key));
	}

	//---- new walk link file ----

	key = Get_Control_String (NEW_WALK_LINK_FILE);

	if (!key.empty ()) {
		if (!zone_walk_flag) {
			Error ("A Zone Walk File is required for Walk Link Processing");
		}
		if (!walk_link_flag) {
			Error ("A Walk Link File is required for Walk Link Processing");
		}
		walk_link_file.File_Type ("New Walk Link File");
		walk_link_file.File_Format (UNFORMATED);
		walk_link_file.Create (Project_Filename (key));
	} else if (zone_walk_flag) {
		Error ("A Walk Link File is required for Zone Walk Processing");
	}

	//---- new node file ----

	key = Get_Control_String (NEW_NODE_FILE);
	if (!key.empty ()) {
		node_flag = true;
		new_node_file.File_Type ("New Node File");

		if (Check_Control_Key (NEW_NODE_FORMAT)) {
			new_node_file.Dbase_Format (Get_Control_String (NEW_NODE_FORMAT));
		}
		new_node_fld = new_node_file.Add_Field ("N", DB_INTEGER, 6);
		new_x_fld = new_node_file.Add_Field ("X", DB_INTEGER, 10);
		new_y_fld = new_node_file.Add_Field ("Y", DB_INTEGER, 10);

		new_node_file.Create (Project_Filename (key));
		Print (1);
	}

	//---- stop zone file ----

	key = Get_Control_String (NEW_STOP_ZONE_FILE);
	if (!key.empty ()) {
		stop_zone_flag = true;

		stop_zone_file.File_Type ("New Stop Zone File");
		stop_zone_file.Create (Project_Filename (key));

		//---- get the mode code ----

		key = Get_Control_Text (STOP_ZONE_MODE_CODE);
		if (!key.empty ()) {
			key.To_Upper ();
			stop_zone_mode = (int) (key [0]);
		}

		//---- get the zone offset ----

		stop_zone_offset = Get_Control_Integer (STOP_ZONE_OFFSET);
		Print (1);
	}

	//---- open mode files ----

	num = Highest_Control_Group (NEW_MODE_FILE, 0);
	if (num == 0) Error ("No Mode Files were Found");
	
	mode_array.reserve (num);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (NEW_MODE_FILE, i);
		if (key.empty ()) continue;

		mode_array.push_back (mode_group);
		mode_ptr = &mode_array.back ();

		mode_ptr->number = i;

		mode_ptr->mode_file = new Db_File (CREATE);
		mode_ptr->mode_file->File_Type (String ("New Mode File %d") % i);

		mode_ptr->mode_file->Open (Project_Filename (key));

		//---- get the mode code ----

		key = Get_Control_Text (MODE_CODE, i);
		if (!key.empty ()) {
			key.To_Upper ();
			mode_ptr->mode = (int) (key [0]);
		}

		//---- get the skim number ----

		mode_ptr->skim = Get_Control_Integer (MODE_SKIM, i);
		if (mode_ptr->skim < 0 || mode_ptr->skim > (int) skim_array.size ()) {
			Error (String ("Mode %d Skim Number %d is Out of Range (0..%d)") % i % mode_ptr->skim % skim_array.size ());
		}
		mode_ptr->skim--;

		//---- get the park-n-ride flag ----

		mode_ptr->pnr = Get_Control_Flag (MODE_PNR_FLAG, i);

		//---- get the mode connection ----

		key = Get_Control_Text (MODE_CONNECTION, i);
		if (!key.empty ()) {
			key.To_Upper ();
			if (key [0] == 'P') {
				mode_ptr->connect = PARKING_NODE;
			} else if (key [0] == 'S') {
				mode_ptr->connect = STOP_NODE;
			} else if (key [0] == 'N') {
				mode_ptr->connect = NETWORK_NODE;
			} else {
				Error (String ("Unrecognized Mode Connection = %s") % key);
			}
		}

		//---- get the max distances by station type ----

		if (Check_Control_Key (MODE_TYPE_DISTANCES, i)) {
			Get_Control_List (MODE_TYPE_DISTANCES, mode_ptr->max_dist, i);
			mode_ptr->max_dist.erase (mode_ptr->max_dist.begin ());
		}

		//---- get the walk and parking times by capacity level ----

		if (Check_Control_Key (MODE_WALK_TIMES, i)) {
			Get_Control_List (MODE_WALK_TIMES, mode_ptr->walk_time, i);
			mode_ptr->walk_time.erase (mode_ptr->walk_time.begin ());
			cost_flag = true;
		}
		Print (1);
	}

	//---- process select service keys ----

	Read_Select_Keys ();
}

