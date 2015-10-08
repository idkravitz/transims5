//*********************************************************
//	ArcNet.cpp - Network Conversion Utility
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	ArcNet constructor
//---------------------------------------------------------

ArcNet::ArcNet (void) : Data_Service (), Draw_Service ()
{
	Program ("ArcNet");
	Version (7);
	Title ("Generate ArcView Shapefiles from a TRANSIMS Network");

	System_File_Type optional_files [] = {
		NODE, ZONE, SHAPE, LINK, POCKET, LANE_USE, LOCATION, PARKING, ACCESS_LINK, 
		CONNECTION, TURN_PENALTY, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR,
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, ROUTE_NODES,
		VEHICLE_TYPE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	int draw_service_keys [] = {
		DRAW_NETWORK_LANES, LANE_WIDTH, CENTER_ONEWAY_LINKS, 
		LINK_DIRECTION_OFFSET, DRAW_AB_DIRECTION, POCKET_SIDE_OFFSET, PARKING_SIDE_OFFSET, 
		LOCATION_SIDE_OFFSET, SIGN_SIDE_OFFSET, SIGN_SETBACK, TRANSIT_STOP_SIDE_OFFSET, 
		TRANSIT_DIRECTION_OFFSET, TRANSIT_OVERLAP_FLAG, DRAW_ONEWAY_ARROWS, ONEWAY_ARROW_LENGTH, 
		ONEWAY_ARROW_SIDE_OFFSET, CURVED_CONNECTION_FLAG, 0
	};
	Control_Key arcnet_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ SUBZONE_DATA_FILE, "SUBZONE_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_NODE_FILE, "NEW_ARC_NODE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_ZONE_FILE, "NEW_ARC_ZONE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_LINK_FILE, "NEW_ARC_LINK_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_CENTERLINE_FILE, "NEW_ARC_CENTERLINE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_POCKET_FILE, "NEW_ARC_POCKET_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_LANE_USE_FILE, "NEW_ARC_LANE_USE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_LOCATION_FILE, "NEW_ARC_LOCATION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_PARKING_FILE, "NEW_ARC_PARKING_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_ACCESS_FILE, "NEW_ARC_ACCESS_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_CONNECTION_FILE, "NEW_ARC_CONNECTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TURN_PENALTY_FILE, "NEW_ARC_TURN_PENALTY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_SIGN_FILE, "NEW_ARC_SIGN_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_SIGNAL_FILE, "NEW_ARC_SIGNAL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TIMING_PLAN_FILE, "NEW_ARC_TIMING_PLAN_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_PHASING_PLAN_FILE, "NEW_ARC_PHASING_PLAN_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_DETECTOR_FILE, "NEW_ARC_DETECTOR_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TRANSIT_STOP_FILE, "NEW_ARC_TRANSIT_STOP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TRANSIT_ROUTE_FILE, "NEW_ARC_TRANSIT_ROUTE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TRANSIT_DRIVER_FILE, "NEW_ARC_TRANSIT_DRIVER_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_ROUTE_NODES_FILE, "NEW_ARC_ROUTE_NODES_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_SUBZONE_DATA_FILE, "NEW_ARC_SUBZONE_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ SELECT_TIME, "SELECT_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "0:00..24:00", NO_HELP },
		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "0:00", "0:00..24:00", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Draw_Service_Keys (draw_service_keys);

	Key_List (arcnet_keys);
	Report_List (reports);

	projection.Add_Keys ();

	route_flag = schedule_flag = driver_flag = route_node_flag = false;
	timing_flag = subzone_flag = false;

	run_field = 0;
}

//---------------------------------------------------------
//	ArcNet destructor
//---------------------------------------------------------

ArcNet::~ArcNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcNet *exe = new ArcNet ();

	return (exe->Start_Execution (commands, control));
}

