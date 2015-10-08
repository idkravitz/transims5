//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcNet::Program_Control (void)
{
	int i, num_files;
	String key, text;
	bool z_flag, m_flag;

	//---- open network files ----

	Data_Service::Program_Control ();

	num_files = 0;

	//---- open subzone files ----

	key = Get_Control_String (SUBZONE_DATA_FILE);

	if (!key.empty ()) {
		subzone_flag = true;
		num_files++;

		Print (1);
		//subzone_file.Open (Project_Filename (key));
	}

	//---- set the transit options ----

	route_flag = System_File_Flag (TRANSIT_ROUTE);
	schedule_flag = System_File_Flag (TRANSIT_SCHEDULE);
	driver_flag = System_File_Flag (TRANSIT_DRIVER);

	if (!route_flag && !driver_flag) {
		route_node_flag = System_File_Flag (ROUTE_NODES);
	}

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	//---- get the arcview file ----
	
	Print (2, String ("%s Output Files:") % Program ());

	//---- open the arcview node file ----

	key = Get_Control_String (NEW_ARC_NODE_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_node.File_Type ("New Arc Node File");
		arcview_node.File_ID ("ArcNode");
		arcview_node.File_Access (CREATE);
		arcview_node.Shape_Type (DOT);
		arcview_node.Z_Flag (z_flag);
		arcview_node.M_Flag (m_flag);

		Required_File_Check (arcview_node, NODE);

		if (!arcview_node.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Node File", arcview_node.Shape_Filename ());
		}
		arcview_node.Replicate_Fields (System_File_Base (NODE));

		arcview_node.Write_Header ();

		arcview_node.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview zone file ----

	key = Get_Control_String (NEW_ARC_ZONE_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_zone.File_Type ("New Arc Zone File");
		arcview_zone.File_ID ("ArcZone");
		arcview_zone.File_Access (CREATE);
		arcview_zone.Shape_Type (DOT);
		arcview_zone.Z_Flag (z_flag);
		arcview_zone.M_Flag (m_flag);

		Required_File_Check (arcview_zone, ZONE);

		if (!arcview_zone.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Zone File", arcview_zone.Shape_Filename ());
		}
		arcview_zone.Replicate_Fields (System_File_Base (ZONE));

		arcview_zone.Write_Header ();

		arcview_zone.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview link file ----

	key = Get_Control_String (NEW_ARC_LINK_FILE);

	if (!key.empty ()) {
		num_files++;
		arcview_link.File_Type ("New Arc Link File");
		arcview_link.File_ID ("ArcLink");
		arcview_link.File_Access (CREATE);
		arcview_link.Shape_Type (VECTOR);
		arcview_link.Z_Flag (z_flag);
		arcview_link.M_Flag (m_flag);

		Required_File_Check (arcview_link, LINK);
		Required_File_Check (arcview_link, NODE);

		if (!arcview_link.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Link File", arcview_link.Shape_Filename ());
		}
		arcview_link.Replicate_Fields (System_File_Base (LINK));

		arcview_link.Write_Header ();

		arcview_link.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview centerline file ----

	key = Get_Control_String (NEW_ARC_CENTERLINE_FILE);

	if (!key.empty ()) {
		num_files++;
		arcview_center.File_Type ("New Arc Centerline File");
		arcview_center.File_ID ("ArcLine");
		arcview_center.File_Access (CREATE);
		arcview_center.Shape_Type (VECTOR);
		arcview_center.Z_Flag (z_flag);
		arcview_center.M_Flag (m_flag);

		Required_File_Check (arcview_center, LINK);
		Required_File_Check (arcview_center, NODE);

		if (!arcview_center.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Centerline File", arcview_center.Shape_Filename ());
		}
		arcview_center.Replicate_Fields (System_File_Base (LINK));

		arcview_center.Write_Header ();

		arcview_center.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- store link data ----

	if (arcview_link.Is_Open () || arcview_center.Is_Open ()) {
		link_db.Replicate_Fields (System_File_Base (LINK), true);
		link_db.File_ID ("Link");
	}

	//---- open the arcview pocket file ----

	key = Get_Control_String (NEW_ARC_POCKET_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_pocket.File_Type ("New Arc Pocket File");
		arcview_pocket.File_ID ("ArcPocket");
		arcview_pocket.File_Access (CREATE);
		arcview_pocket.Shape_Type (VECTOR);
		arcview_pocket.Z_Flag (z_flag);
		arcview_pocket.M_Flag (m_flag);

		Required_File_Check (arcview_pocket, POCKET);
		Required_File_Check (arcview_pocket, LINK);

		if (!arcview_pocket.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Pocket File", arcview_pocket.Shape_Filename ());
		}
		arcview_pocket.Replicate_Fields (System_File_Base (POCKET));

		arcview_pocket.Write_Header ();

		arcview_pocket.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		pocket_db.Replicate_Fields (System_File_Base (POCKET), true);
		pocket_db.Add_Field ("POCKET_INDEX", DB_INTEGER, 10);
		pocket_db.File_ID ("Pocket");
	}

	//---- open the arcview lane use file ----

	key = Get_Control_String (NEW_ARC_LANE_USE_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_lane_use.File_Type ("New Arc Lane Use File");
		arcview_lane_use.File_ID ("ArcLaneUse");
		arcview_lane_use.File_Access (CREATE);
		arcview_lane_use.Shape_Type (VECTOR);
		arcview_lane_use.Z_Flag (z_flag);
		arcview_lane_use.M_Flag (m_flag);

		Required_File_Check (arcview_lane_use, LANE_USE);
		Required_File_Check (arcview_lane_use, LINK);

		if (!arcview_lane_use.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Lane Use File", arcview_lane_use.Shape_Filename ());
		}
		arcview_lane_use.Replicate_Fields (System_File_Base (LANE_USE));

		arcview_lane_use.Write_Header ();

		arcview_lane_use.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview location file ----

	key = Get_Control_String (NEW_ARC_LOCATION_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_location.File_Type ("New Arc Location File");
		arcview_location.File_ID ("ArcLocation");
		arcview_location.File_Access (CREATE);
		arcview_location.Shape_Type (DOT);
		arcview_location.Z_Flag (z_flag);
		arcview_location.M_Flag (m_flag);

		Required_File_Check (arcview_location, LOCATION);
		Required_File_Check (arcview_location, LINK);

		if (!arcview_location.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Location File", arcview_location.Shape_Filename ());
		}
		arcview_location.Replicate_Fields (System_File_Base (LOCATION));

		arcview_location.Write_Header ();

		arcview_location.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview parking file ----

	key = Get_Control_String (NEW_ARC_PARKING_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_parking.File_Type ("New Arc Parking File");
		arcview_parking.File_ID ("ArcParking");
		arcview_parking.File_Access (CREATE);
		arcview_parking.Shape_Type (DOT);
		arcview_parking.Z_Flag (z_flag);
		arcview_parking.M_Flag (m_flag);

		Required_File_Check (arcview_parking, PARKING);
		Required_File_Check (arcview_parking, LINK);

		if (!arcview_parking.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Parking File", arcview_parking.Shape_Filename ());
		}
		arcview_parking.Replicate_Fields (System_File_Base (PARKING), true, false);

		arcview_parking.Write_Header ();

		arcview_parking.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview access link file ----

	key = Get_Control_String (NEW_ARC_ACCESS_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_access.File_Type ("New_Arc Access File");
		arcview_access.File_ID ("ArcAccess");
		arcview_access.File_Access (CREATE);
		arcview_access.Shape_Type (LINE);
		arcview_access.Z_Flag (z_flag);
		arcview_access.M_Flag (m_flag);

		Required_File_Check (arcview_access, ACCESS_LINK);
		Required_File_Check (arcview_access, LOCATION);
		Required_File_Check (arcview_access, PARKING);

		if (!arcview_access.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Access File", arcview_access.Shape_Filename ());
		}
		arcview_access.Replicate_Fields (System_File_Base (ACCESS_LINK));

		arcview_access.Write_Header ();

		arcview_access.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview link connection file ----

	key = Get_Control_String (NEW_ARC_CONNECTION_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_connect.File_Type ("New Arc Connection File");
		arcview_connect.File_ID ("ArcConnect");
		arcview_connect.File_Access (CREATE);
		arcview_connect.Shape_Type (VECTOR);
		arcview_connect.Z_Flag (z_flag);
		arcview_connect.M_Flag (m_flag);

		Required_File_Check (arcview_connect, CONNECTION);
		Required_File_Check (arcview_connect, LINK);
		Required_File_Check (arcview_connect, POCKET);

		if (!arcview_connect.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Connection File", arcview_connect.Shape_Filename ());
		}
		arcview_connect.Replicate_Fields (System_File_Base (CONNECTION));

		arcview_connect.Write_Header ();

		arcview_connect.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview turn penalty file ----

	key = Get_Control_String (NEW_ARC_TURN_PENALTY_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_turn.File_Type ("New Arc Turn Penalty File");
		arcview_turn.File_ID ("ArcTurnPen");
		arcview_turn.File_Access (CREATE);
		arcview_turn.Shape_Type (VECTOR);
		arcview_turn.Z_Flag (z_flag);
		arcview_turn.M_Flag (m_flag);

		Required_File_Check (arcview_turn, TURN_PENALTY);
		Required_File_Check (arcview_turn, LINK);

		if (!arcview_turn.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Turn Penalty File", arcview_turn.Shape_Filename ());
		}
		arcview_turn.Replicate_Fields (System_File_Base (TURN_PENALTY));

		arcview_turn.Write_Header ();

		arcview_turn.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview sign file ----

	key = Get_Control_String (NEW_ARC_SIGN_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_sign.File_Type ("New Arc Sign File");
		arcview_sign.File_ID ("ArcSign");
		arcview_sign.File_Access (CREATE);
		arcview_sign.Shape_Type (DOT);
		arcview_sign.Z_Flag (z_flag);
		arcview_sign.M_Flag (m_flag);

		Required_File_Check (arcview_sign, SIGN);
		Required_File_Check (arcview_sign, LINK);

		if (!arcview_sign.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Sign File", arcview_sign.Shape_Filename ());
		}
		arcview_sign.Replicate_Fields (System_File_Base (SIGN));

		arcview_sign.Write_Header ();

		arcview_sign.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview signal file ----

	key = Get_Control_String (NEW_ARC_SIGNAL_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_signal.File_Type ("New Arc Signal File");
		arcview_signal.File_ID ("ArcSignal");
		arcview_signal.File_Access (CREATE);
		arcview_signal.Shape_Type (MDOTS);
		arcview_signal.Z_Flag (z_flag);
		arcview_signal.M_Flag (m_flag);

		Required_File_Check (arcview_signal, SIGNAL);
		Required_File_Check (arcview_signal, NODE);

		if (!arcview_signal.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Signal File", arcview_signal.Shape_Filename ());
		}
		arcview_signal.Replicate_Fields (System_File_Base (SIGNAL), true, false);

		arcview_signal.Write_Header ();

		arcview_signal.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview timing plan file ----

	key = Get_Control_String (NEW_ARC_TIMING_PLAN_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_timing.File_Type ("New Arc Timing Plan File");
		arcview_timing.File_ID ("ArcTiming");
		arcview_timing.File_Access (CREATE);
		arcview_timing.Shape_Type (VECTOR);
		arcview_timing.Z_Flag (z_flag);
		arcview_timing.M_Flag (m_flag);

		Required_File_Check (arcview_timing, TIMING_PLAN);
		Required_File_Check (arcview_timing, SIGNAL);
		Required_File_Check (arcview_timing, PHASING_PLAN);

		if (!arcview_timing.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Timing Plan File", arcview_timing.Shape_Filename ());
		}
		arcview_timing.Replicate_Fields (System_File_Base (TIMING_PLAN), true, false);

		arcview_timing.Write_Header ();

		arcview_timing.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		
		timing_db.Replicate_Fields (System_File_Base (TIMING_PLAN), true, false);
		timing_db.File_ID ("Timing");

		timing_flag = true;
	}

	//---- open the arcview phasing plan file ----

	key = Get_Control_String (NEW_ARC_PHASING_PLAN_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_phasing.File_Type ("New Arc Phasing Plan File");
		arcview_phasing.File_ID ("ArcPhasing");
		arcview_phasing.File_Access (CREATE);
		arcview_phasing.Shape_Type (VECTOR);
		arcview_phasing.Z_Flag (z_flag);
		arcview_phasing.M_Flag (m_flag);
		
		Required_File_Check (arcview_phasing, PHASING_PLAN);
		Required_File_Check (arcview_phasing, LINK);

		if (!arcview_phasing.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Phasing Plan File", arcview_phasing.Shape_Filename ());
		}
		arcview_phasing.Replicate_Fields (System_File_Base (PHASING_PLAN), true, false);

		arcview_phasing.Write_Header ();

		arcview_phasing.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview detector file ----

	key = Get_Control_String (NEW_ARC_DETECTOR_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_detector.File_Type ("New Arc Detector File");
		arcview_detector.File_ID ("ArcDetector");
		arcview_detector.File_Access (CREATE);
		arcview_detector.Shape_Type (VECTOR);
		arcview_detector.Z_Flag (z_flag);
		arcview_detector.M_Flag (m_flag);

		Required_File_Check (arcview_detector, DETECTOR);
		Required_File_Check (arcview_detector, PHASING_PLAN);
		Required_File_Check (arcview_detector, LINK);

		if (!arcview_detector.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Detector File", arcview_detector.Shape_Filename ());
		}
		arcview_detector.Replicate_Fields (System_File_Base (DETECTOR));

		arcview_detector.Write_Header ();

		arcview_detector.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview transit stop file ----

	key = Get_Control_String (NEW_ARC_TRANSIT_STOP_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_stop.File_Type ("New Arc Transit Stop File");
		arcview_stop.File_ID ("ArcStop");
		arcview_stop.File_Access (CREATE);
		arcview_stop.Shape_Type (DOT);
		arcview_stop.Z_Flag (z_flag);
		arcview_stop.M_Flag (m_flag);

		Required_File_Check (arcview_stop, TRANSIT_STOP);
		Required_File_Check (arcview_stop, LINK);

		if (!arcview_stop.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Transit Stop File", arcview_stop.Shape_Filename ());
		}
		arcview_stop.Replicate_Fields (System_File_Base (TRANSIT_STOP));

		arcview_stop.Write_Header ();

		arcview_stop.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview transit route file ----

	key = Get_Control_String (NEW_ARC_TRANSIT_ROUTE_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_route.File_Type ("New Arc Transit Route File");
		arcview_route.File_ID ("ArcRoute");
		arcview_route.File_Access (CREATE);
		arcview_route.Shape_Type (VECTOR);
		arcview_route.Z_Flag (z_flag);
		arcview_route.M_Flag (m_flag);

		Required_File_Check (arcview_route, TRANSIT_ROUTE);
		Required_File_Check (arcview_route, TRANSIT_STOP);

		if (!arcview_route.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Transit Route File", arcview_route.Shape_Filename ());
		}

		//---- copy the existing header fields ----
	
		arcview_route.Replicate_Fields (System_File_Base (TRANSIT_ROUTE), true, false);

		if (schedule_flag) {
			arcview_route.Add_Field ("NUM_RUNS", DB_INTEGER, 5);
		}
		if (driver_flag) {
			arcview_route.Add_Field ("NUM_LINKS", DB_INTEGER, 5);
			arcview_route.Add_Field ("VEH_TYPE", DB_INTEGER, 3);
			arcview_route.Add_Field ("SUB_TYPE", DB_INTEGER, 2);
		}
		if (schedule_flag) {
			key = Get_Control_String (TRANSIT_TIME_PERIODS);

			if (!key.empty ()) {
				if (!sched_breaks.Add_Breaks (key)) {
					Error ("Adding Transit Time Period Breaks");
				}
				run_field = arcview_route.Num_Fields ();

				for (i=0; i < sched_breaks.Num_Ranges (); i++) {
					Dtime low, high;

					sched_breaks.Period_Range (i, low, high);

					key = "RUNS_" + low.Time_Label ();

					arcview_route.Add_Field (key, DB_INTEGER, 4);
				}
			} else {
				schedule_flag = false;
			}
		}
		arcview_route.Write_Header ();

		arcview_route.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview transit driver file ----

	key = Get_Control_String (NEW_ARC_TRANSIT_DRIVER_FILE);
	if (!key.empty ()) {
		num_files++;

		arcview_driver.File_Type ("New Arc Transit Driver File");
		arcview_driver.File_ID ("ArcDriver");
		arcview_driver.File_Access (CREATE);
		arcview_driver.Shape_Type (VECTOR);
		arcview_driver.Z_Flag (z_flag);
		arcview_driver.M_Flag (m_flag);

		Required_File_Check (arcview_driver, TRANSIT_DRIVER);
		Required_File_Check (arcview_driver, TRANSIT_ROUTE);

		if (!arcview_driver.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Transit Driver File", arcview_driver.Shape_Filename ());
		}

		//---- copy the existing header fields ----
		
		arcview_driver.Replicate_Fields (System_File_Base (TRANSIT_DRIVER), true, false);

		arcview_driver.Write_Header ();

		arcview_driver.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}
	
	//---- open the arcview route nodes file ----

	key = Get_Control_String (NEW_ARC_ROUTE_NODES_FILE);
	if (!key.empty ()) {
		num_files++;
		arcview_route_nodes.File_Type ("New Arc Route Nodes File");
		arcview_route_nodes.File_ID ("ArcRouteNodes");
		arcview_route_nodes.File_Access (CREATE);
		arcview_route_nodes.Shape_Type (VECTOR);
		arcview_route_nodes.Z_Flag (z_flag);
		arcview_route_nodes.M_Flag (m_flag);

		Required_File_Check (arcview_route_nodes, ROUTE_NODES);

		if (!arcview_route_nodes.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Route Nodes File", arcview_route_nodes.Shape_Filename ());
		}

		//---- copy the existing header fields ----

		arcview_route_nodes.Replicate_Fields (System_File_Base (ROUTE_NODES), true, false, true);

		arcview_route_nodes.Write_Header ();

		arcview_route_nodes.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- open the arcview subzone file ----

	if (subzone_flag) {
		//arcview_subzone.File_Type ("ArcView Subzone File");
		//arcview_subzone.File_Access (CREATE);
		//arcview_subzone.Shape_Type (DOT);
		//arcview_subzone.Z_Flag (z_flag);
		//arcview_subzone.M_Flag (m_flag);

		//if (!arcview_subzone.Open (Shapename (arcdir, subzone_file.Filename ()))) {
		//	File_Error ("Opening ArcView Subzone File", arcview_subzone.Shape_Filename ());
		//}
		//arcview_subzone.Replicate_Fields (&subzone_file);

		//arcview_subzone.Write_Header ();

		//arcview_subzone.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- check the file count ----

	if (num_files == 0) {
		Error ("No Input Network Files");
	}

	//---- offset parameters ----

	Print (2, String ("%s Control Keys:") % Program ());

	time = Get_Control_Time (SELECT_TIME);
	time_flag = (time != 0);

	//---- transit schedulet ----

	if (route_flag && schedule_flag) {
		Get_Control_Text (TRANSIT_TIME_PERIODS);
	}

	Read_Draw_Keys ();
}
