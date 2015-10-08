//*********************************************************
//	ArcPlan.cpp - Plan File Display Utility
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	ArcPlan constructor
//---------------------------------------------------------

ArcPlan::ArcPlan (void) : Data_Service (), Select_Service (), Draw_Service ()
{
	Program ("ArcPlan");
	Version (9);
	Title ("Plan File Display Utility");

	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, PARKING, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, CONNECTION, POCKET, ACCESS_LINK,
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_DRIVER, VEHICLE_TYPE, 
		PLAN, PROBLEM, SELECTION, LINK_DELAY, PERFORMANCE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, STOP_EQUIVALENCE_FILE, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_FACILITY_TYPES, 
		SELECT_PROBLEM_TYPES, SELECT_LINKS, SELECT_NODES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 
		SELECT_SUBAREA_POLYGON, SELECTION_PERCENTAGE, 0

		//SELECT_PARKING_LOTS,
		//SELECT_TRANSIT_STOPS,
		//SELECT_TRANSIT_ROUTES,
		//SELECT_TRANSIT_MODES,
	};
	int draw_service_keys [] = {
		DRAW_NETWORK_LANES, LANE_WIDTH, CENTER_ONEWAY_LINKS, LINK_DIRECTION_OFFSET, 
		PARKING_SIDE_OFFSET, LOCATION_SIDE_OFFSET, TRANSIT_STOP_SIDE_OFFSET, TRANSIT_DIRECTION_OFFSET, 
		BANDWIDTH_SCALING_FACTOR, MINIMUM_BANDWIDTH_VALUE, MINIMUM_BANDWIDTH_SIZE, MAXIMUM_BANDWIDTH_SIZE, 0
	};
	Control_Key arcplan_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ NEW_ARC_PLAN_FILE, "NEW_ARC_PLAN_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_PROBLEM_FILE, "NEW_ARC_PROBLEM_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_BANDWIDTH_FILE, "NEW_ARC_BANDWIDTH_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TIME_CONTOUR_FILE, "NEW_ARC_TIME_CONTOUR_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_DISTANCE_CONTOUR_FILE, "NEW_ARC_DISTANCE_CONTOUR_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_ACCESSIBILITY_FILE, "NEW_ARC_ACCESSIBILITY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_RIDERSHIP_FILE, "NEW_ARC_RIDERSHIP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_STOP_DEMAND_FILE, "NEW_ARC_STOP_DEMAND_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_STOP_GROUP_FILE, "NEW_ARC_STOP_GROUP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_PARKING_DEMAND_FILE, "NEW_ARC_PARKING_DEMAND_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ MAXIMUM_SHAPE_ANGLE, "MAXIMUM_SHAPE_ANGLE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ MINIMUM_SHAPE_LENGTH, "MINIMUM_SHAPE_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ CONTOUR_TIME_INCREMENTS, "CONTOUR_TIME_INCREMENTS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ CONTOUR_DISTANCE_INCREMENTS, "CONTOUR_DISTANCE_INCREMENTS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ RIDERSHIP_SCALING_FACTOR, "RIDERSHIP_SCALING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ MINIMUM_RIDERSHIP_VALUE, "MINIMUM_RIDERSHIP_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ MINIMUM_RIDERSHIP_SIZE, "MINIMUM_RIDERSHIP_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ MAXIMUM_RIDERSHIP_SIZE, "MAXIMUM_RIDERSHIP_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"STOP_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Select_Service_Keys (select_service_keys);
	Draw_Service_Keys (draw_service_keys);

	Key_List (arcplan_keys);
	Report_List (reports);

	Enable_Partitions (true);

	projection.Add_Keys ();

	AB_Map_Flag (true);

	num_out = num_problem = max_problem = num_width = num_time = num_distance = num_access = 0;
	num_rider = num_stop = num_group = num_parking = 0;

	max_angle = 45;
	min_length = 5;
	load_factor = 0.0;
	min_load = 0;
	min_rider = 0.01;
	max_rider = 1000.0;
	random_share = 0.1;

	type_flag = true;
	route_flag = driver_flag = stop_flag = problem_flag = problem_out = access_flag = false;
	path_flag = hhlist_flag = width_flag = time_flag = distance_flag = turn_flag = false;
	rider_flag = load_flag = on_off_flag = demand_flag = group_flag = parking_flag = false;
	mode_flag = random_flag = false;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
	System_Data_Reserve (PROBLEM, 0);
}

//---------------------------------------------------------
//	ArcPlan destructor
//---------------------------------------------------------

ArcPlan::~ArcPlan (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcPlan *exe = new ArcPlan ();

	return (exe->Start_Execution (commands, control));
}

