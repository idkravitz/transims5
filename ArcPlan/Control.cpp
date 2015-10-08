//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcPlan::Program_Control (void)
{
	String key, text;
	bool z_flag, m_flag;

	//---- set the equivalance flag ----

	Stop_Equiv_Flag (Check_Control_Key (NEW_ARC_STOP_GROUP_FILE));

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	problem_flag = System_File_Flag (PROBLEM);
	plan_flag = System_File_Flag (PLAN);

	if (plan_flag) {
		plan_file = (Plan_File *) System_File_Handle (PLAN);
	}

	//---- set the transit options ----

	stop_flag = System_File_Flag (TRANSIT_STOP);
	route_flag = System_File_Flag (TRANSIT_ROUTE);
	driver_flag = System_File_Flag (TRANSIT_DRIVER);

	if ((route_flag || driver_flag) && !stop_flag) {
		Error ("Transit Stops are required for Transit Route Processing");
	}

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	Print (1);

	//---- get the arcview plan file ----

	key = Get_Control_String (NEW_ARC_PLAN_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Plan Output");
		}
		arcview_plan.File_Type ("New Arc Plan File");
		arcview_plan.File_Access (CREATE);
		arcview_plan.Shape_Type (VECTOR);
		arcview_plan.Z_Flag (z_flag);
		arcview_plan.M_Flag (m_flag);

		if (!arcview_plan.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Plan File", arcview_plan.Shape_Filename ());
		}
		path_flag = true;

		arcview_plan.Replicate_Fields (plan_file, false, false, true);

		//---- add leg summary fields ----

		arcview_plan.Add_Field ("LEG_MODE", DB_STRING, 12, MODE_CODE);
		arcview_plan.Add_Field ("LEG_ID", DB_INTEGER, 10);
		arcview_plan.Add_Field ("LEG_TIME", DB_TIME, 10, SECONDS);
		arcview_plan.Add_Field ("LEG_LENGTH", DB_INTEGER, 10, METERS);
		arcview_plan.Add_Field ("LEG_COST", DB_DOUBLE, 6.1, CENTS);
		arcview_plan.Add_Field ("LEG_IMPED", DB_INTEGER, 10, IMPEDANCE);

		arcview_plan.Write_Header ();

		arcview_plan.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview problem file ----

	key = Get_Control_String (NEW_ARC_PROBLEM_FILE);
	if (!key.empty ()) {
		if (!problem_flag) {
			Error ("A Problem File is Required for Arc Problem Output");
		}
		arcview_problem.File_Type ("New Arc Problem File");
		arcview_problem.File_Access (CREATE);
		arcview_problem.Shape_Type (DOT);
		arcview_problem.Z_Flag (z_flag);
		arcview_problem.M_Flag (m_flag);

		if (!arcview_problem.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Problem File", arcview_problem.Shape_Filename ());
		}
		arcview_problem.Replicate_Fields (System_File_Base (PROBLEM));

		arcview_problem.Write_Header ();

		arcview_problem.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		problem_out = true;
	}

	//---- get the arcview bandwidth file ----

	key = Get_Control_String (NEW_ARC_BANDWIDTH_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Bandwidth Output");
		}
		arcview_width.File_Type ("New Arc Bandwidth File");
		arcview_width.File_Access (CREATE);
		arcview_width.Shape_Type (POLYGON);
		arcview_width.Z_Flag (z_flag);
		arcview_width.M_Flag (m_flag);

		if (!arcview_width.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Bandwidth File", arcview_width.Shape_Filename ());
		}
		width_flag = true;

		arcview_width.Add_Field ("LINK", DB_INTEGER, 10);
		arcview_width.Add_Field ("DIR", DB_INTEGER, 1);
		arcview_width.Add_Field ("VOLUME", DB_INTEGER, 10);

		arcview_width.Write_Header ();

		arcview_width.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview time contour file ----

	key = Get_Control_String (NEW_ARC_TIME_CONTOUR_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Time Contour Output");
		}
		arcview_time.File_Type ("New Arc Time Contour File");
		arcview_time.File_Access (CREATE);
		arcview_time.Shape_Type (VECTOR);
		arcview_time.Z_Flag (z_flag);
		arcview_time.M_Flag (m_flag);

		if (!arcview_time.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Time Contour File", arcview_time.Shape_Filename ());
		}
		time_flag = true;

		arcview_time.Add_Field ("CONTOUR", DB_INTEGER, 10);
		arcview_time.Add_Field ("TIME", DB_INTEGER, 10);

		arcview_time.Write_Header ();

		arcview_time.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview distance contour file ----

	key = Get_Control_String (NEW_ARC_DISTANCE_CONTOUR_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Distance Contour Output");
		}
		arcview_distance.File_Type ("New Arc Distance Contour File");
		arcview_distance.File_Access (CREATE);
		arcview_distance.Shape_Type (VECTOR);
		arcview_distance.Z_Flag (z_flag);
		arcview_distance.M_Flag (m_flag);

		if (!arcview_distance.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Distance Contour File", arcview_distance.Shape_Filename ());
		}
		distance_flag = true;

		arcview_distance.Add_Field ("CONTOUR", DB_INTEGER, 10);
		arcview_distance.Add_Field ("DISTANCE", DB_INTEGER, 10);

		arcview_distance.Write_Header ();

		arcview_distance.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview accessibilityr file ----

	key = Get_Control_String (NEW_ARC_ACCESSIBILITY_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Accessibility Output");
		}
		arcview_access.File_Type ("New Arc Accessibility File");
		arcview_access.File_Access (CREATE);
		arcview_access.Shape_Type (DOT);
		arcview_access.Z_Flag (z_flag);
		arcview_access.M_Flag (m_flag);

		if (!arcview_access.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Accessibility File", arcview_access.Shape_Filename ());
		}
		access_flag = true;

		arcview_access.Add_Field ("ORIGIN", DB_INTEGER, 10);
		arcview_access.Add_Field ("START", DB_STRING, 20);
		arcview_access.Add_Field ("LOCATION", DB_INTEGER, 10);
		arcview_access.Add_Field ("TTIME", DB_INTEGER, 10);
		arcview_access.Add_Field ("DISTANCE", DB_INTEGER, 10);

		arcview_access.Write_Header ();

		arcview_access.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- set the load flag ----

	key = Get_Control_String (RIDERSHIP_SCALING_FACTOR);

	if (!key.empty ()) {
		load_factor = key.Double ();

		load_flag = (load_factor != 0.0);
	}

	//---- get the arcview ridership file ----

	key = Get_Control_String (NEW_ARC_RIDERSHIP_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Ridership Output");
		}
		if (!stop_flag || !route_flag || !driver_flag) {
			Error ("Transit Data are need for Ridership Output");
		}
		arcview_rider.File_Type ("New Arc Ridership File");
		arcview_rider.File_Access (CREATE);

		if (load_flag) {
			arcview_rider.Shape_Type (POLYGON);
		} else {
			arcview_rider.Shape_Type (VECTOR);
		}
		arcview_rider.Z_Flag (z_flag);
		arcview_rider.M_Flag (m_flag);

		if (!arcview_rider.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Ridership File", arcview_rider.Shape_Filename ());
		}
		rider_flag = true;

		arcview_rider.Add_Field ("LINK", DB_INTEGER, 10);
		arcview_rider.Add_Field ("DIR", DB_INTEGER, 1);
		arcview_rider.Add_Field ("SEGMENT", DB_INTEGER, 2);
		arcview_rider.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_rider.Add_Field ("RIDERS", DB_INTEGER, 10);
		arcview_rider.Add_Field ("ALIGHT", DB_INTEGER, 10);

		arcview_rider.Write_Header ();

		arcview_rider.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop demand file ----

	key = Get_Control_String (NEW_ARC_STOP_DEMAND_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Stop Demand Output");
		}
		if (!stop_flag) {
			Error ("Transit Stops are need for Stop Demand Output");
		}
		arcview_stop.File_Type ("New Arc Stop Demand File");
		arcview_stop.File_Access (CREATE);
		arcview_stop.Shape_Type (DOT);
		arcview_stop.Z_Flag (z_flag);
		arcview_stop.M_Flag (m_flag);

		if (!arcview_stop.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Stop Demand File", arcview_stop.Shape_Filename ());
		}
		on_off_flag = demand_flag = true;

		arcview_stop.Add_Field ("STOP", DB_INTEGER, 10);
		arcview_stop.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_stop.Add_Field ("ALIGHT", DB_INTEGER, 10);
		arcview_stop.Add_Field ("TOTAL", DB_INTEGER, 10);

		arcview_stop.Write_Header ();

		arcview_stop.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop group file ----

	key = Get_Control_String (NEW_ARC_STOP_GROUP_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Stop Group Output");
		}
		if (!stop_flag) {
			Error ("Transit Stops are need for Stop Group Output");
		}
		arcview_group.File_Type ("New Arc Stop Group File");
		arcview_group.File_Access (CREATE);
		arcview_group.Shape_Type (DOT);
		arcview_group.Z_Flag (z_flag);
		arcview_group.M_Flag (m_flag);

		if (!arcview_group.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Stop Group File", arcview_group.Shape_Filename ());
		}
		on_off_flag = group_flag = true;

		arcview_group.Add_Field ("GROUP", DB_INTEGER, 10);
		arcview_group.Add_Field ("NAME", DB_STRING, 40);
		arcview_group.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_group.Add_Field ("ALIGHT", DB_INTEGER, 10);
		arcview_group.Add_Field ("TOTAL", DB_INTEGER, 10);

		arcview_group.Write_Header ();

		arcview_group.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview parking demand file ----

	key = Get_Control_String (NEW_ARC_PARKING_DEMAND_FILE);
	if (!key.empty ()) {
		if (!plan_file) {
			Error ("A Plan File is Required for Arc Parking Demand Output");
		}
		arcview_parking.File_Type ("New Arc Parking Demand File");
		arcview_parking.File_Access (CREATE);
		arcview_parking.Shape_Type (DOT);
		arcview_parking.Z_Flag (z_flag);
		arcview_parking.M_Flag (m_flag);

		if (!arcview_parking.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Parking Demand File", arcview_parking.Shape_Filename ());
		}
		parking_flag = true;

		arcview_parking.Add_Field ("PARKING", DB_INTEGER, 10);
		arcview_parking.Add_Field ("DEPART", DB_INTEGER, 10);
		arcview_parking.Add_Field ("ARRIVE", DB_INTEGER, 10);
		arcview_parking.Add_Field ("TOTAL", DB_INTEGER, 10);

		arcview_parking.Write_Header ();

		arcview_parking.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- check for output ----

	if (!path_flag && !problem_flag && !width_flag && !time_flag && !distance_flag && !access_flag &&
		!rider_flag && !on_off_flag && !parking_flag && !problem_out) {
		Error ("An Output ArcView File was Not Specified");
	}

	//---- get offset parameters ----

	Read_Draw_Keys ();

	//---- polygon smoothing keys ----

	if (width_flag || load_flag) {

		//---- maximum shape angle ----

		max_angle= Get_Control_Integer (MAXIMUM_SHAPE_ANGLE);

		//---- minimum shape length ----

		min_length = Get_Control_Integer (MINIMUM_SHAPE_LENGTH);
	}

	//---- time contour increments ----

	if (time_flag) {

		//---- get the time increments ----

		key = Get_Control_Text (CONTOUR_TIME_INCREMENTS);

		if (!key.empty ()) {
			time_points.Add_Breaks (key);
		}

		//---- check for a link delay file ----

		if (System_File_Flag (LINK_DELAY)) {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);

			if (file->Dbase_Format () == VERSION3) {
				turn_flag = false;
				System_File_False (CONNECTION);
			} else if (!System_File_Flag (CONNECTION)) {
				Warning ("Link Connection is required for Turning Delays");
				Show_Message (1);
				turn_flag = false;
			} else {
				turn_flag = true;
			}
		} else {
			System_File_False (CONNECTION);
		}
	} else {
		System_File_False (CONNECTION);
	}

	//---- distance contour increments ----

	if (distance_flag) {

		//---- get the distance increments ----

		key = Get_Control_Text (CONTOUR_DISTANCE_INCREMENTS);

		if (!key.empty ()) {
			distance_points.Add_Breaks (key);
		}
	}

	//---- ridership scaling factor ----

	if (load_flag) {
		if (load_factor < 0.01 || load_factor > 100000.0) {
			Error (String ("Ridership Scaling Factor %.2lf is Out of Range (0.01..100000.0)") % load_factor);
		}
		Print (2, String ("Ridership Scaling Factor = %.2lf riders / meter") % load_factor);

		//---- minimum ridership value ----

		min_load = Get_Control_Integer (MINIMUM_RIDERSHIP_VALUE);

		//---- minimum ridership size ----

		min_rider = Get_Control_Double (MINIMUM_RIDERSHIP_SIZE);

		//---- maximum ridership size ----

		max_rider = Get_Control_Double (MAXIMUM_RIDERSHIP_SIZE);
	}

	//---- read the stop equiv ----

	if (Stop_Equiv_Flag ()) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}
}
