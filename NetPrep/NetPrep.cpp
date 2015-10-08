//*********************************************************
//	NetPrep.cpp - Network Preparation Utility
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	NetPrep constructor
//---------------------------------------------------------

NetPrep::NetPrep (void) : Data_Service ()
{
	Program ("NetPrep");
	Version (11);
	Title ("Network Preparation Utility");

	System_File_Type optional_files [] = {
		LINK, NODE, ZONE, SHAPE, ROUTE_NODES, NEW_NODE, NEW_LINK, NEW_ZONE, NEW_SHAPE, NEW_ROUTE_NODES, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};

	Control_Key netprep_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ INPUT_LINK_FILE, "INPUT_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_LINK_FORMAT, "INPUT_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ INPUT_NODE_FILE, "INPUT_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_NODE_FORMAT, "INPUT_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ INPUT_ZONE_FILE, "INPUT_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_ZONE_FORMAT, "INPUT_ZONE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ INPUT_SPDCAP_FILE, "INPUT_SPDCAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_SPDCAP_FORMAT, "INPUT_SPDCAP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "FIXED_COLUMN", FORMAT_RANGE, FORMAT_HELP },
		{ INPUT_UNITS_OF_MEASURE, "INPUT_UNITS_OF_MEASURE", LEVEL0, OPT_KEY, TEXT_KEY, "METRIC", "METRIC, ENGLISH", NO_HELP },
		{ FACILITY_INDEX_FIELD, "FACILITY_INDEX_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ AREA_TYPE_INDEX_FIELD, "AREA_TYPE_INDEX_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INTERNAL_ZONE_RANGE, "INTERNAL_ZONE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "0", "0..10000", NO_HELP },
		{ KEEP_ZONE_CONNECTORS_AS_LOCALS, "KEEP_ZONE_CONNECTORS_AS_LOCALS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ FIRST_NODE_NUMBER, "FIRST_NODE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000000000", NO_HELP },
		{ FIRST_LINK_NUMBER, "FIRST_LINK_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000000000", NO_HELP },
		{ SPEED_FACTOR_BY_FACILITY, "SPEED_FACTOR_BY_FACILITY", LEVEL0, OPT_KEY, LIST_KEY, "1.0", "1.0..2.0", NO_HELP },
		{ SPEED_ROUNDING_INCREMENT, "SPEED_ROUNDING_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 mps", "0.0..10.0 mps", NO_HELP },
		{ COORDINATE_RESOLUTION, "COORDINATE_RESOLUTION", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 meters", "0..30 meters", NO_HELP },
		{ MAXIMUM_LENGTH_TO_XY_RATIO, "MAXIMUM_LENGTH_TO_XY_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, 1.0..3.0", NO_HELP },
		{ MAXIMUM_SHAPE_ANGLE, "MAXIMUM_SHAPE_ANGLE", LEVEL0, OPT_KEY, INT_KEY, "90 degrees", "10..160 degrees", NO_HELP },
		{ MINIMUM_SHAPE_LENGTH, "MINIMUM_SHAPE_LENGTH", LEVEL0, OPT_KEY, INT_KEY, "10 meters", "0..200 meters", NO_HELP },
		{ DROP_DEAD_END_LINKS, "DROP_DEAD_END_LINKS", LEVEL0, OPT_KEY, INT_KEY, "0 meters", "0..2000 meters", NO_HELP },
		{ DROP_SHORT_LINKS, "DROP_SHORT_LINKS", LEVEL0, OPT_KEY, FLOAT_KEY, "0 meters", "0..200 meters", NO_HELP },
		{ SPLIT_LARGE_LOOPS, "SPLIT_LARGE_LOOPS", LEVEL0, OPT_KEY, INT_KEY, "0 meters", "0, 100..4000 meters", NO_HELP },
		{ COLLAPSE_SHAPE_NODES, "COLLAPSE_SHAPE_NODES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ COLLAPSE_DIVIDED_ARTERIALS, "COLLAPSE_DIVIDED_ARTERIALS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LOCAL_THRU_SEGMENT_LENGTHS, "LOCAL_THRU_SEGMENT_LENGTHS", LEVEL1, OPT_KEY, LIST_KEY, "0 meters", "0, 500..10000 meters", NO_HELP },
		{ LOCAL_SELECTION_SPACING, "LOCAL_SELECTION_SPACING", LEVEL1, OPT_KEY, LIST_KEY, "0 meters", "0, 50..10000 meters", NO_HELP },
		{ KEEP_NODE_RANGE, "KEEP_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ KEEP_LINK_RANGE, "KEEP_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ KEEP_NODE_FILE, "KEEP_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP},
		{ KEEP_LINK_FILE, "KEEP_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_NODE_RANGE, "DELETE_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_LINK_RANGE, "DELETE_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_NODE_FILE, "DELETE_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_LINK_FILE, "DELETE_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DETAIL_FILE, "NEW_LINK_DETAIL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_NODE_LIST_FILE, "NEW_LINK_NODE_LIST_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ PERIOD_TRAVEL_TIMES, "PERIOD_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ FIRST_ROUTE_NUMBER, "FIRST_ROUTE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..100000", NO_HELP },
		{ INPUT_ROUTE_FORMAT, "INPUT_ROUTE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", FORMAT_RANGE, FORMAT_HELP },
		{ ROUTE_MODE_MAP, "ROUTE_MODE_MAP", LEVEL1, OPT_KEY, LIST_KEY, "BUS", "BUS, EXPRESS, TROLLEY, STREETCAR, LIGHTRAIL, RAPIDRAIL, REGIONRAIL", NO_HELP },
		{ INPUT_ROUTE_FILE, "INPUT_ROUTE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ROUTE_PERIOD_MAP, "ROUTE_PERIOD_MAP", LEVEL1, OPT_KEY, LIST_KEY, "1", "-24..24", NO_HELP },
		{ FLIP_ROUTE_FLAG, "FLIP_ROUTE_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"CHECK_INTERSECTIONS",
		"INTERSECTION_TYPES",
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (netprep_keys);
	Report_List (reports);

	proj_service.Add_Keys ();

	new_link = new_node = 1;
	fac_fld = at_fld = -1;
	drop_length = split_length = num_loops = max_angle = min_length = next_loop = num_ratio = 0;
	convert_flag = spdcap_flag = link_flag = node_flag = zone_flag = route_flag = new_zone_flag = connector_flag = false;
	link_shape_flag = node_shape_flag = zone_shape_flag = route_shape_flag = int_zone_flag = centroid_flag = false;
	units_flag = keep_node_flag = keep_link_flag = drop_node_flag = drop_link_flag = shape_flag = false;
	length_flag = split_flag = collapse_flag = drop_flag = loop_flag = spacing_flag = false;
	divided_flag = segment_flag = speed_flag = detail_flag = link_node_flag = time_flag = false;
	length_ratio = 0.0;

	straight_diff = compass.Num_Points () * 9 / 360;		//---- 9 degrees ----
	thru_diff = 2 * straight_diff;							//---- +/- 18 degrees ----
	cross_min = thru_diff;
	cross_max = 180 - cross_min;
	forward_diff = compass.Num_Points () * 90 / 360;
	spd_inc = num_periods = num_match = 0;
	route_format = TPPLUS;
}

//---------------------------------------------------------
//	NetPrep destructor
//---------------------------------------------------------

NetPrep::~NetPrep (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	NetPrep *exe = new NetPrep ();

	return (exe->Start_Execution (commands, control));
}

