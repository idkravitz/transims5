//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcSnapshot::Program_Control (void)
{
	int i;
	String key, text;
	bool z_flag, m_flag;

	string method_text [] = { "AT_INCREMENT", "TOTAL", "MAXIMUM", "" };

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	max_time = Model_End_Time ();

	//---- get the projection data ----

	project_flag = projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	//---- open the snapshot file ----

	Print (1);	
	key = Get_Control_String (SNAPSHOT_FILE);

	if (!key.empty ()) {
		if (Check_Control_Key (SNAPSHOT_FORMAT)) {
			snapshot_file.Dbase_Format (Get_Control_String (SNAPSHOT_FORMAT));
		}
		snapshot_file.Open (Project_Filename (key));
	} else {
		key = Get_Control_String (OCCUPANCY_FILE);

		if (key.empty ()) {
			Error ("A Snapshot or Occupancy file is Required");
		}
		if (Check_Control_Key (OCCUPANCY_FORMAT)) {
			occupancy_file.Dbase_Format (Get_Control_String (OCCUPANCY_FORMAT));
		}
		occupancy_file.Open (Project_Filename (key));
		occupancy_flag = true;
	}

	//---- get the arcview snapshot file ----

	key = Get_Control_String (NEW_ARC_SNAPSHOT_FILE);
	if (!key.empty ()) {
		if (key.Ends_With (".shp")) {
			key.Split_Last (text, ".");
		}
		shapename = Project_Filename (key);
		text = shapename + "_time_of_day.shp";

		Print_Filename (2, "New Arc Snapshot File", text);
		snapshot_flag = true;
	}

	//---- new snapshot file ----

	key = Get_Control_String (NEW_SNAPSHOT_FILE);

	if (!key.empty ()) {
		Print (1);
		if (Check_Control_Key (NEW_SNAPSHOT_FORMAT)) {
			new_snapshot_file.Dbase_Format (Get_Control_String (NEW_SNAPSHOT_FORMAT));
		}
		new_snapshot_file.Location_Flag (true);

		new_snapshot_file.Create (Project_Filename (key));
		output_flag = true;

		if (project_flag) {
			project_coord.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		}
	}

	//---- new link summary file ----

	key = Get_Control_String (NEW_LINK_SUMMARY_FILE);

	if (!key.empty ()) {
		if (occupancy_flag) {
			Error ("New Link Summary File requires a Snapshot File");
		}
		Print (1);
		summary_file.File_Type ("Link Summary File");
		
		if (Check_Control_Key (NEW_LINK_SUMMARY_FORMAT)) {
			summary_file.Dbase_Format (Get_Control_String (NEW_LINK_SUMMARY_FORMAT));
		}
		sum_flag = true;

		summary_file.Create (Project_Filename (key));
	}
	if (!snapshot_flag && !output_flag && !sum_flag) {
		Error ("An Arcview Snapshot, New Snapshot, or New Link Summary File is Required");
	}

	//---- cell size ----
		
	cell_size = Get_Control_Double (CELL_SIZE);

	//---- lane width ----

	Read_Draw_Keys ();

	//---- draw vehicle shapes ----

	if (snapshot_flag && !shape_flag) {
		circle_flag = Get_Control_Flag (ADD_PASSENGER_CIRCLE_SIZE);
		square_flag = Get_Control_Flag (ADD_PASSENGER_SQUARE_SIZE);
	}

	//---- pad file time labels ----

	pad_flag = Get_Control_Flag (PAD_FILE_TIME_LABEL);

	//---- get the time processing method ----

	key = Get_Control_Text (TIME_PROCESSING_METHOD);

	if (!key.empty ()) {
		for (i=0; (int) method_text [i].size () > 0; i++) {
			if (key.Equals (method_text [i])) {
				method = i;
				break;
			}
		}
		if ((int) method_text [i].size () == 0) {
			Error (String ("Unrecognized Time Processing Method = %s") % key);
		}
		if (!occupancy_flag && method == TOTAL) {
			Error ("Snapshot Files cannot be Totaled");
		}
	}

	//---- initialize the shape file ----

	arcview_snapshot.File_Type ("New Arc Snapshot File");
	arcview_snapshot.File_Access (CREATE);
	arcview_snapshot.Shape_Type ((shape_flag) ? POLYGON : DOT);
	arcview_snapshot.Z_Flag (z_flag);
	arcview_snapshot.M_Flag (m_flag);

	if (occupancy_flag) {
		arcview_snapshot.Replicate_Fields (&occupancy_file);
		arcview_snapshot.LinkDir_Type ((Direction_Type) occupancy_file.LinkDir_Type ());

		link_field = occupancy_file.Link_Field ();
		dir_field = occupancy_file.Dir_Field ();
		start_field = occupancy_file.Start_Field ();
		end_field = occupancy_file.End_Field ();
		lane_field = occupancy_file.Lane_Field ();
		offset_field = occupancy_file.Offset_Field ();
		occ_field = occupancy_file.Occupancy_Field ();
		type_field = -1;
	} else {
		Snapshot_File *file = &snapshot_file;
		if (file->Compress_Flag ()) {
			dummy_snapshot.Status_Flag ();
			dummy_snapshot.Create_Fields ();
			file = &dummy_snapshot;
		}
		arcview_snapshot.Replicate_Fields (file);
		arcview_snapshot.LinkDir_Type ((Direction_Type) file->LinkDir_Type ());

		link_field = file->Link_Field ();
		dir_field = file->Dir_Field ();
		lane_field = file->Lane_Field ();
		offset_field = file->Offset_Field ();
		type_field = file->Type_Field ();
		cell_field = file->Cell_Field ();
		occ_field = start_field = end_field = -1;

		if (circle_flag) {
			arcview_snapshot.Add_Field ("RADIUS", DB_DOUBLE, 14.2);
			circle_field = arcview_snapshot.Field_Number ("RADIUS");
		}
		if (square_flag) {
			arcview_snapshot.Add_Field ("SQUARE", DB_DOUBLE, 14.2);
			square_field = arcview_snapshot.Field_Number ("SQUARE");
		}
	}
	arcview_snapshot.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
}
