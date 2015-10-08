//*********************************************************
//	TransimsNet.cpp - Network Conversion Utility
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	TransimsNet constructor
//---------------------------------------------------------

TransimsNet::TransimsNet (void) : Data_Service ()
{
	Program ("TransimsNet");
	Version (9);
	Title ("Network Conversion Utility");

	System_File_Type required_files [] = {
		NODE, ZONE, LINK, NEW_LINK, NEW_LOCATION, NEW_PARKING,
		NEW_POCKET, NEW_CONNECTION, NEW_SIGN, NEW_SIGNAL, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, TURN_PENALTY, LOCATION, PARKING, ACCESS_LINK, POCKET, CONNECTION, SIGN, SIGNAL, LANE_USE, 
		NEW_NODE, NEW_ZONE, NEW_SHAPE, NEW_ACCESS_LINK, NEW_TURN_PENALTY, NEW_LANE_USE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key transimsnet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DEFAULT_LINK_SETBACK, "DEFAULT_LINK_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "7.5 meters", "0..30 meters", NO_HELP },
		{ DEFAULT_LOCATION_SETBACK, "DEFAULT_LOCATION_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "30 meters", "0..100 meters", NO_HELP },
		{ MAXIMUM_CONNECTION_ANGLE, "MAXIMUM_CONNECTION_ANGLE", LEVEL0, OPT_KEY, INT_KEY, "120 degrees", "90..180 degrees", NO_HELP },
		{ ADD_UTURN_TO_DEAD_END_LINKS, "ADD_UTURN_TO_DEAD_END_LINKS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SIGNAL_ID_AS_NODE_ID, "SIGNAL_ID_AS_NODE_ID", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ EXTERNAL_ZONE_RANGE, "EXTERNAL_ZONE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "0", "0..10000", NO_HELP },
		{ EXTERNAL_STATION_OFFSET, "EXTERNAL_STATION_OFFSET", LEVEL0, OPT_KEY, INT_KEY, "30 meters", "0..100 meters", NO_HELP },
		{ ZONE_BOUNDARY_FILE, "ZONE_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_FIELD_NAME, "ZONE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "", "ZONE, TAZ, Z, ID", NO_HELP },
		{ POCKET_LANE_WARRANT, "POCKET_LANE_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "FROM, TO, AT, TYPE, LENGTH meters, LANES", NO_HELP },
		{ TRAFFIC_CONTROL_WARRANT, "TRAFFIC_CONTROL_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "MAIN, OTHER, AT, TYPE, SETBACK meters, GROUP", NO_HELP },
		{ FACILITY_ACCESS_WARRANT, "FACILITY_ACCESS_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "TYPE, AT, SETBACK meters, MIN_LEN meters, MAX_PTS", NO_HELP },
		{ PARKING_DETAILS_WARRANT, "PARKING_DETAILS_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "AT, TIME, USE, IN seconds, OUT seconds, HOURLY cents, DAILY cents", NO_HELP },
		{ STREET_PARKING_WARRANT, "STREET_PARKING_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "TYPE, AT, TIME", NO_HELP },
		{ UPDATE_NODE_RANGE, "UPDATE_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ UPDATE_LINK_RANGE, "UPDATE_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ UPDATE_NODE_FILE, "UPDATE_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ UPDATE_LINK_FILE, "UPDATE_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_NODE_RANGE, "DELETE_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_LINK_RANGE, "DELETE_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_NODE_FILE, "DELETE_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_LINK_FILE, "DELETE_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_DETAIL_FILE, "LINK_DETAIL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (transimsnet_keys);
	Report_List (reports);

	proj_service.Add_Keys ();

	zone_flag = zout_flag, turn_flag = shape_flag = name_flag = uturn_flag = signal_id_flag = false;
	update_flag = delete_flag = connect_flag = replicate_flag = boundary_flag = false;
	update_link_flag = update_node_flag = delete_link_flag = delete_node_flag = false;
	ext_zone_flag = link_detail_flag = false;

	details_flag = false;
	location_id = parking_id = 0;
	location_base = parking_base = 0;

	nnode = nlink = nparking = nactivity = nprocess = npocket = nconnect = nsign = nsignal = nuse = 0;
	mparking = mactivity = mprocess = mpocket = muse = 0;
	xlink = xnode = xparking = xactivity = xprocess = xpocket = xconnect = xsign = xsignal = xuse = 0;
	nshort = nlength = nexternal = nzone = nzout = max_splits = 0;
	nfixed1 = nfixed2 = nfixed3 = nactuated1 = nactuated2 = nactuated3 = nstop = nyield = 0;
	min_length = link_setback = 0;
	nshape = nshapes = nturn = 0;

	straight_diff = compass.Num_Points () * 9 / 360;		//---- +/- 9 degrees ----
	thru_diff = compass.Num_Points () * 50 / 360;			//---- +/- 50 degrees ----
	max_angle = compass.Num_Points () * 120 / 360;			//---- +/- 120 degrees ----
	uturn_angle = compass.Num_Points () * 180 / 360;		//---- 180 degress ----
	short_length = Round (200.0);							//---- 200 meters ----
	external_offset = Round (30.0);							//---- 30 meters ----
	loc_setback = Round (30.0);								//---- 30 meters ----
}

//---------------------------------------------------------
//	TransimsNet destructor
//---------------------------------------------------------

TransimsNet::~TransimsNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TransimsNet *exe = new TransimsNet ();

	return (exe->Start_Execution (commands, control));
}

