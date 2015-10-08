//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NewFormat::Program_Control (void)
{
	bool type_flag, flat_flag, time_flag;
	String key;

	//---- open the support files ----

	Data_Service::Program_Control ();

	if (Check_Control_Key (VERSION4_PLAN_FILE) && Check_Control_Key (TRIP_SORT_TYPE)) {
		Warning ("Plan Sorting is Limited to Version 5 Plan files");
		Show_Message (1);
	}	
	Print (2, String ("%s Control Keys:") % Program ());

	//---- copy existing fields ----

	copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);

	if (copy_flag) {
		if (System_File_Flag (LOCATION) && System_File_Flag (NEW_LOCATION)) {
			Location_File *file = (Location_File *) System_File_Handle (LOCATION);
			new_loc_file = (Location_File *) System_File_Handle (NEW_LOCATION);
			new_loc_file->Add_User_Fields (file);
		}
		if (System_File_Flag (ZONE) && System_File_Flag (NEW_ZONE)) {
			Zone_File *file = (Zone_File *) System_File_Handle (ZONE);
			new_zone_file = (Zone_File *) System_File_Handle (NEW_ZONE);
			new_zone_file->Add_User_Fields (file);
		}
	}

	//---- copy existing configuration ----

	if (System_File_Flag (LINK_DELAY) && System_File_Flag (NEW_LINK_DELAY)) {
		Link_Delay_File *file = (Link_Delay_File *) System_File_Header (LINK_DELAY);
		if (file->Turn_Flag ()) {
			file = (Link_Delay_File *) System_File_Header (NEW_LINK_DELAY);
			file->Clear_Fields ();
			file->Turn_Flag (true);
			file->Set_Nesting (true);
			file->Create_Fields ();
			file->Write_Header ();
		}
	}
	if (System_File_Flag (PERFORMANCE) && System_File_Flag (NEW_PERFORMANCE)) {
		Performance_File *file = (Performance_File *) System_File_Header (PERFORMANCE);
		if (file->Turn_Flag ()) {
			file = (Performance_File *) System_File_Header (NEW_PERFORMANCE);
			file->Clear_Fields ();
			file->Turn_Flag (true);
			file->Set_Nesting (true);
			file->Create_Fields ();
			file->Write_Header ();
		}
	}
	
	//---- flatten output flag ----

	flat_flag = Get_Control_Flag (FLATTEN_OUTPUT_FLAG);

	if (flat_flag) {
		if (System_File_Flag (NEW_SHAPE)) {
			System_File_Header (NEW_SHAPE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PARKING)) {
			System_File_Header (NEW_PARKING)->Flatten_File ();
		}
		if (System_File_Flag (NEW_SIGNAL)) {
			System_File_Header (NEW_SIGNAL)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TIMING_PLAN)) {
			System_File_Header (NEW_TIMING_PLAN)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PHASING_PLAN)) {
			System_File_Header (NEW_PHASING_PLAN)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_ROUTE)) {
			System_File_Header (NEW_TRANSIT_ROUTE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_SCHEDULE)) {
			System_File_Header (NEW_TRANSIT_SCHEDULE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_DRIVER)) {
			System_File_Header (NEW_TRANSIT_DRIVER)->Flatten_File ();
		}
		if (System_File_Flag (NEW_ROUTE_NODES)) {
			System_File_Header (NEW_ROUTE_NODES)->Flatten_File ();
		}
		if (System_File_Flag (NEW_HOUSEHOLD)) {
			System_File_Header (NEW_HOUSEHOLD)->Flatten_File ();
		}
		if (System_File_Flag (NEW_LINK_DELAY)) {
			System_File_Header (NEW_LINK_DELAY)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PERFORMANCE)) {
			System_File_Header (NEW_PERFORMANCE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PLAN)) {
			System_File_Header (NEW_PLAN)->Flatten_File ();
		}
	}

	//---- toll file ----

	key = Get_Control_String (TOLL_FILE);

	if (!key.empty ()) {
		toll_file.Open (Project_Filename (key));
		toll_flag = true;
	}

	//---- activity file ----

	key = Get_Control_String (ACTIVITY_FILE);

	if (!key.empty ()) {
		activity_file.Open (Project_Filename (key));
		activity_flag = true;

		if (!System_File_Flag (NEW_TRIP)) {
			Error ("A New Trip file is required to convert an Activity File");
		}
		if (!System_File_Flag (VEHICLE)) {
			Error ("A Vehicle file is required to convert an Activity File");
		}
	}

	//---- person file ----

	key = Get_Control_String (PERSON_FILE);

	if (!key.empty ()) {
		person_file.Open (Project_Filename (key));
		person_flag = true;
	}

	//---- snapshot file ----

	key = Get_Control_String (SNAPSHOT_FILE);

	if (!key.empty ()) {
		Print (1);
		snap_file.Open (Project_Filename (key));
		snap_flag = true;
	}

	//---- new snapshot file ----

	key = Get_Control_String (NEW_SNAPSHOT_FILE);

	if (!key.empty ()) {
		new_snap_file.Compress_Flag (Set_Control_Flag (NEW_SNAPSHOT_COMPRESSION));

		if (new_snap_file.Compress_Flag ()) {
			new_snap_file.Dbase_Format (BINARY);
		} else {
			if (Check_Control_Key (NEW_SNAPSHOT_FORMAT)) {
				new_snap_file.Dbase_Format (Get_Control_String (NEW_SNAPSHOT_FORMAT));
			}
			new_snap_file.Location_Flag (snap_file.Location_Flag ());
			new_snap_file.Cell_Flag (snap_file.Cell_Flag ());
			new_snap_file.Status_Flag (snap_file.Status_Flag ());
		}
		new_snap_file.Create (Project_Filename (key));

		Get_Control_Flag (NEW_SNAPSHOT_COMPRESSION);
	} else if (snap_flag) {
		Error ("A New Snapshot File is required for Output");
	}

	//---- version 4 time format ----

	time_units = Units_Code (Get_Control_Text (VERSION4_TIME_FORMAT));
	time_flag = Check_Control_Key (VERSION4_TIME_FORMAT);

	if (time_flag) {
		int num;
		Field_Ptr fld_ptr;

		if (System_File_Flag (TRIP)) {
			Trip_File *file = (Trip_File *) System_File_Base (TRIP);
			if (file->Version () <= 40) {
				num = file->Optional_Field (START_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
				num = file->Optional_Field (END_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
				num = file->Optional_Field (DURATION_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
			}
		}
		if (activity_flag) {
			num = activity_file.Optional_Field ("START_MIN", "START", "START_TIME", "STARTTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("START_MAX", "START", "START_TIME", "STARTTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("END_MIN", "END", "END_TIME", "ENDTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("END_MAX", "END", "END_TIME", "ENDTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("TIME_MIN", "DURATION");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("TIME_MAX", "DURATION");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
		}
	}

	//---- plan file ----

	if (Check_Control_Key (VERSION4_PLAN_FILE)) {
		if (System_File_Flag (PLAN)) {
			Error ("A plan file and Version 4 plan file must be processed separateley");
		}
		if (!System_File_Flag (NEW_PLAN)) {
			Error ("A new plan file is required to convert a Version 4 plan file");
		}
		Print (1);
		plan_flag = true;
		old_plan.File_Type ("Version4 Plan File");

		//---- get the list type ----

		key = Get_Control_String (NODE_LIST_PATHS);

		if (!key.empty ()) {
			type_flag = key.Bool ();
		} else {
			type_flag = true;
		}
		old_plan.Node_Based_Flag (type_flag);

		//---- get the traveler scale ----

		key = Get_Control_String (TRAVELER_SCALING_FACTOR);

		if (!key.empty ()) {
			scale = key.Integer ();

			if (scale < 2 || scale > 100) {
				Control_Key_Error (TRAVELER_SCALING_FACTOR, "(2..100)");
			}
			old_plan.Traveler_Scale (scale);
		}

		//---- get the file format ----

		if (Check_Control_Key (VERSION4_PLAN_FORMAT)) {
			old_plan.File_Format (Get_Control_String (VERSION4_PLAN_FORMAT));
		}

		//---- open the file and print the parameters ----

		if (!old_plan.Open (Project_Filename (Get_Control_String (VERSION4_PLAN_FILE)))) {
			File_Error ("Opening Version4 Plan File", old_plan.Filename ());
		}
		Get_Control_Text (VERSION4_PLAN_FORMAT);

		type_flag = old_plan.Node_Based_Flag ();

		if (type_flag) {
			Print (1, "Plan File contains Node List Paths");
			if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || !System_File_Flag (PARKING)) {
				Error ("A Node, Link and Parking Files are Required for Node-Based Plan Files");
			}
			AB_Map_Flag (true);
		} else {
			Print (1, "Plan File contains Link List Paths");
		}
		Get_Control_Integer (TRAVELER_SCALING_FACTOR);

	} else if (snap_flag && snap_file.Version () <= 40) {

		//---- get the traveler scale ----

		scale= Get_Control_Integer (TRAVELER_SCALING_FACTOR);

		if (scale < 2 || scale > 100) {
			Control_Key_Error (TRAVELER_SCALING_FACTOR, "(2..100)");
		}
	}

	//---- Version 4 Route Header and Node files ----

	if (Check_Control_Key (VERSION4_ROUTE_HEADER)) {
		if (!System_File_Flag (NEW_ROUTE_NODES)) {
			Error ("A New Route Nodes File is Required");
		}
		int i, num;
		String field;
		Field_Ptr fld_ptr;

		if (!time_flag) time_units = MINUTES;

		Print (1);
		route_flag = true;

		Route_Nodes_File *file = (Route_Nodes_File *) System_File_Base (NEW_ROUTE_NODES);

		key = Get_Control_String (VERSION4_ROUTE_HEADER);

		route_header.File_Type ("Version4 Route Header");
		route_header.File_ID ("Header");

		route_header.Open (Project_Filename (key));

		for (i=1; i <= 24; i++) {
			field ("HEADWAY_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num < 0) break;

			fld_ptr = route_header.Field (num);
			fld_ptr->Units (time_units);

			field ("OFFSET_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num >= 0) {
				fld_ptr = route_header.Field (num);
				fld_ptr->Units (time_units);
			}
			field ("TTIME_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num >= 0) {
				fld_ptr = route_header.Field (num);
				fld_ptr->Units (time_units);
			}
		}
		route_periods = i - 1;
		if (route_periods != file->Num_Periods ()) {
			file->Num_Periods (route_periods);
			file->Clear_Fields ();
			file->Create_Fields ();
			file->Write_Header ();
		}
		key = Get_Control_String (VERSION4_ROUTE_NODES);

		if (key.empty ()) {
			Error ("A Version4 Route Nodes file is Required");
		}
		route_nodes.File_Type ("Version4 Route Nodes");
		route_nodes.File_ID ("Nodes");

		route_nodes.Open (Project_Filename (key));
	}
}
