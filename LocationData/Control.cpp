//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LocationData::Program_Control (void)
{
	int i, num, field;
	double size;
	bool flag;
	String key, name, buf;
	Field_Type type;

	Subzone_Group sub_rec;
	Sub_Group_Itr sub_itr;
	Data_Group data_rec;
	Data_Itr data_itr;
	Zone_File *zone_file;
	Polygon_Group poly_rec;
	Polygon_Itr poly_itr;

	//---- check for use flag processing ----

	use_flag = (Highest_Control_Group (NEW_USE_FLAG_FIELD, 0) > 0);

	//---- check for walk access processing ----

	if (Check_Control_Key (NEW_WALK_ACCESS_FIELD)) {
		walk_access_flag = true;

		System_File_True (TRANSIT_STOP);
		System_File_True (TRANSIT_ROUTE);
		System_File_True (TRANSIT_SCHEDULE);
	} else {
		System_Option_False (ACCESS_LINK);
		System_Option_False (TRANSIT_STOP);
		System_Option_False (TRANSIT_ROUTE);
		System_Option_False (TRANSIT_SCHEDULE);
	}
	boundary_flag = (!Get_Control_String (ZONE_BOUNDARY_FILE).empty ());
	zone_loc_flag = (!Get_Control_String (NEW_ZONE_LOCATION_MAP_FILE).empty ());

	if (boundary_flag || zone_loc_flag || Check_Control_Key (SUBZONE_ZONE_FIELD) || 
		Highest_Control_Group (SUBZONE_ZONE_FIELD, 0) > 0) {
		System_File_True (ZONE);
		zone_file_flag = true;
	} else {
		System_Option_False (ZONE);
	}
	field = 0;
	zone_file = 0;

	//---- open network files ----

	Data_Service::Program_Control ();

	if (zone_file_flag) {
		zone_file = (Zone_File *) System_File_Handle (ZONE);
	}
	input_file = (Location_File *) System_File_Handle (LOCATION);

	output_file = (Location_File *) System_File_Handle (NEW_LOCATION);

	flag = exe->Notes_Name_Flag ();
	exe->Notes_Name_Flag (false);
		
	output_file->Clear_Fields ();

	Print (2, String ("%s Control Keys:") % Program ());	

	//---- copy existing fields ----

	copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);

	if (copy_flag) {
		output_file->Replicate_Fields (input_file, false);
	} else {
		output_file->Create_Fields ();
	}
	exe->Notes_Name_Flag (flag);

	//---- add the walk access field ----
	
	if (walk_access_flag) {
		key = Get_Control_Text (NEW_WALK_ACCESS_FIELD);

		walk_access_field = output_file->Field_Number (key);

		if (walk_access_field >= 0) {
			Warning (String ("New Walk Access Field %s already exists as Field %d") % key % walk_access_field);
		} else {
			output_file->Add_Field (key, DB_INTEGER, 10);

			walk_access_field = output_file->Field_Number (key);

			Print (0, ", Number = ") << (walk_access_field + 1);
		}

		//---- maximum walk distance ----

		walk_distance = Get_Control_Integer (MAX_WALK_DISTANCE);

		//---- walk access time range ----

		key = Get_Control_Text (WALK_ACCESS_TIME_RANGE);

		if (!key.empty () && !key.Equals ("ALL")) {
			walk_time_flag = true;
			if (!walk_time.Add_Ranges (key)) {
				Error ("Adding Walk Access Time Ranges");
			}
		}
	}

	//---- add the use flag fields ----
	
	if (use_flag) {
		num = Highest_Control_Group (NEW_USE_FLAG_FIELD, 0);

		//---- process each flag ----

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (NEW_USE_FLAG_FIELD, i);
			if (key.empty ()) continue;

			field = output_file->Field_Number (key);

			if (field >= 0) {
				Warning (String ("New Use Flag Field %s already exists as Field %d") % key % field);
			} else {
				output_file->Add_Field (key, DB_INTEGER, 2);

				field = output_file->Field_Number (key);

				Print (0, ", Number = ") << (field + 1);
			}
			use_field.push_back (field);

			key = Get_Control_Text (LINK_USE_FLAG_TYPES, i);
			if (key.empty ()) goto control_error;

			field = Use_Code (key);
			use_code.push_back (field); 
		}
	}

	//---- get the new subzone fields ----

	num_subzone = Highest_Control_Group (NEW_SUBZONE_FIELD, 0);

	if (num_subzone > 0) {
		subzone_flag = true;

		key = Get_Control_String (SUBZONE_ZONE_FACTOR_FILE);
		if (!key.empty ()) {
			subzone_map_flag = true;
			subzone_zone_file.File_Type ("Subzone Zone Factor File");
			subzone_zone_file.Open (Project_Filename (key));
		}
		Print (1);
	}

	//---- open each file ----

	for (i=1; i <= num_subzone; i++) {
		key = Get_Control_Text (NEW_SUBZONE_FIELD, i);
		if (key.empty ()) continue;

		sub_group.push_back (sub_rec);
		sub_itr = --sub_group.end ();
		sub_itr->group = i;

		//---- check if the field name already exists ----

		key.Split (name);
		field = output_file->Field_Number (name);

		if (field >= 0) {
			Error (String ("New Subzone Field %s already exists as Field %d") % name % field);
		} else {
			//---- create the location field ----

			key.Split (buf);
			if (buf.empty () || buf.Starts_With ("I")) {
				type = DB_INTEGER;
				sub_itr->float_flag = false;
			} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
				type = DB_DOUBLE;
				sub_itr->float_flag = true;
			} else {
				goto subzone_error;
			}
			key.Split (buf);
			if (buf.empty ()) {
				if (type == DB_DOUBLE) {
					size = 10.2;
				} else {
					size = 10.0;
				}
			} else {
				size = buf.Double ();
				if ((double) ((int) size) == size) {
					sub_itr->float_flag = false;
				}
			}
			output_file->Add_Field (name, type, size);

			field = output_file->Field_Number (name);
		
			Print (0, ", Number = ") << (field + 1);
		}
		sub_itr->loc_field = field;

		//---- get the maximum distance ----

		sub_itr->max_distance = Get_Control_Integer (MAX_SUBZONE_DISTANCE, i);

		//---- open the file ----

		key = Get_Control_String (SUBZONE_DATA_FILE, i);
		if (key.empty ()) goto control_error;
	
		sub_itr->file = new Subzone_File ();
		sub_itr->file->File_Type (String ("Subzone Data File #%d") % i);
		sub_itr->file->File_ID (String ("Subzone%d") % i);

		if (Check_Control_Key (SUBZONE_DATA_FORMAT, i)) {
			sub_itr->file->Dbase_Format (Get_Control_String (SUBZONE_DATA_FORMAT, i));
		}
		sub_itr->file->Open (Project_Filename (key));

		//---- find the data field ----

		key = Get_Control_Text (SUBZONE_DATA_FIELD, i);
		if (key.empty ()) goto control_error;

		sub_itr->data_field = sub_itr->file->Field_Number (key);

		if (sub_itr->data_field < 0) {
			Error (String ("Subzone Data Field %s was Not Found") % key);
		}
		Print (0, ", Number = ") << (sub_itr->data_field + 1);

		//---- find the zone field ----

		if (zone_file_flag) {
			key = Get_Control_Text (SUBZONE_ZONE_FIELD, i);
			if (!key.empty ()) {
				sub_itr->zone_field = zone_file->Field_Number (key);
				if (sub_itr->zone_field < 0) {
					Error (String ("Subzone Zone Field %s was Not Found") % key);
				}
				Print (0, ", Number = ") << (sub_itr->zone_field + 1);
			} else {
				sub_itr->zone_field = -1;
			}
		}
		Print (1);
	}

	//---- get the new activity location fields ----

	num = Highest_Control_Group (NEW_LOCATION_FIELD, 0);

	if (num == 0) {
		if (!walk_access_flag && !copy_flag && num_subzone == 0) {
			Warning ("No New Location Fields");
		}
	} else {
		Print (1);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (NEW_LOCATION_FIELD, i);
			if (key.empty ()) continue;

			key.Split (name);
			if (name.empty ()) goto data_error;

			field = output_file->Field_Number (name);
			if (field >= 0) goto field_error;

			key.Split (buf);
			if (buf.empty () || buf.Starts_With ("I")) {
				type = DB_INTEGER;
			} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
				type = DB_DOUBLE;
			} else if (buf.Starts_With ("S") || buf.Starts_With ("C")) {
				type = DB_STRING;
			} else {
				goto data_error;
			}

			key.Split (buf);
			if (buf.empty ()) {
				if (type == DB_DOUBLE) {
					size = 10.2;
				} else {
					size = 10.0;
				}
			} else {
				size = buf.Double ();
			}
			output_file->Add_Field (name, type, size);
		}
	}
	if (exe->Notes_Name_Flag ()) {
		if (output_file->Field_Number ("NOTES") < 0) {
			output_file->Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
		}
	}
	output_file->Write_Header ();
	
	//---- check for data files ----

	num_data_files = Highest_Control_Group (DATA_FILE, 0);

	if (num_data_files > 0) {
		data_flag = true;
	}

	//---- check for polygon files ----

	num_polygons = Highest_Control_Group (BOUNDARY_POLYGON_FILE, 0);

	if (num_polygons > 0) {
		polygon_flag = true;
	}
	
	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (key.empty ()) {
		if (data_flag || polygon_flag) {
			Error ("A Convertion Script is needed for Data Processing");
		}
	} else {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));
		Print (1);
	}

	//---- read data files ----

	if (data_flag) {

		//---- open each file ----

		for (i=1; i <= num_data_files; i++) {
			key = Get_Control_String (DATA_FILE, i);
			if (key.empty ()) continue;

			//---- create a header file and file labels ----

			Print (1);

			data_group.push_back (data_rec);
			data_itr = --data_group.end ();

			data_itr->group = i;
			data_itr->file = new Db_Header ();
			data_itr->data_db = new Db_Sort_Array ();

			data_itr->file->File_Type (String ("Data File #%d") % i);
			data_itr->file->File_ID (String ("Data%d") % i);

			if (Check_Control_Key (DATA_FORMAT, i)) {
				data_itr->file->Dbase_Format (Get_Control_String (DATA_FORMAT, i));
			}
			data_itr->file->Open (Project_Filename (key));

			//---- find the data join field ----

			key = Get_Control_Text (DATA_JOIN_FIELD, i);
			if (key.empty ()) goto control_error;

			field = data_itr->file->Field_Number (key);

			if (field < 0) {
				Error (String ("Data Join Field %s was Not Found") % key);
			}
			data_itr->join_field = field;
			Print (0, ", Number = ") << (field + 1);

			//---- find the location join field ----

			key = Get_Control_Text (LOCATION_JOIN_FIELD, i);
			if (key.empty ()) goto control_error;

			field = input_file->Field_Number (key);
			if (field < 0) {
				Error (String ("Location Join Field %s was Not Found") % key);
			}
			data_itr->loc_field = field;
			Print (0, ", Number = ") << (field + 1);
		}
	}

	//---- read the projection information ----

	if (boundary_flag || polygon_flag) {
		projection.Read_Control ();
	}

	//---- read the boundary polygons ----

	if (polygon_flag) {
		if (!script_flag) {
			Error ("Boundary Polygon processing Requires a Conversion Script");
		}

		//---- open each file ----

		for (i=1; i <= num_polygons; i++) {
			key = Get_Control_String (BOUNDARY_POLYGON_FILE, i);
			if (key.empty ()) continue;

			polygons.push_back (poly_rec);

			poly_itr = --polygons.end ();

			poly_itr->group = i;
			poly_itr->file = new Arcview_File ();
			poly_itr->data_db = new Db_Sort_Array ();
			poly_itr->file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

			Print (1);
			poly_itr->file->File_Type (String ("Boundary Polygon File #%d") % i);
			poly_itr->file->File_ID (String ("Polygon%d") % i);

			poly_itr->file->Open (Project_Filename (key));
		}
	}

	//---- new zone location file ----

	if (zone_loc_flag) {
		key = Get_Control_String (NEW_ZONE_LOCATION_MAP_FILE);
		Print (1);

		zone_loc_map.Create (Project_Filename (key));

		min_locations = Get_Control_Integer (MINIMUM_ZONE_LOCATIONS);
	}

	//---- get the zone boundary ----

	if (boundary_flag) {
		key = Get_Control_String (ZONE_BOUNDARY_FILE);
		boundary_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		Print (1);
		boundary_file.File_Type ("Zone Boundary File");

		boundary_file.Open (Project_Filename (key));

		//--- get the zone field name ----

		key = Get_Control_Text (ZONE_FIELD_NAME);

		if (key.empty ()) {
			zone_field = boundary_file.Required_Field (ZONE_FIELD_NAMES);
		} else {
			zone_field = boundary_file.Field_Number (key);

			if (zone_field < 0) {
				Error (String ("Field %s was Not Found in Zone Boundary File") % key);
			}
		}
	}
	coverage_flag = Report_Flag (ZONE_CHECK);

	//---- get the zone range ----

	if (zone_loc_flag || boundary_flag || coverage_flag) {
		key = Get_Control_Text (ZONE_UPDATE_RANGE);

		if (!key.empty () && !key.Equals ("ALL")) {
			range_flag = true;

			if (!zone_range.Add_Ranges (key)) {
				Error ("Zone Update Range");
			}
		}
	}
	Show_Message (1);
	return;

control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());

subzone_error:
	Error (String ("New Subarea Field %d is Improperly Specified") % i);

data_error:
	Error (String ("New Location Field %d is Improperly Specified") % i);

field_error:
	Error (String ("New Location Field %s already exists as Field %d") % name % field);
}
