//*********************************************************
//	Router_Service.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Router_Service constructor
//---------------------------------------------------------

Router_Service::Router_Service (void) : Data_Service (), Flow_Time_Service ()
{
	Service_Level (ROUTER_SERVICE);

	walk_path_flag = bike_path_flag = drive_path_flag = access_link_flag = true;
	transit_path_flag = parkride_path_flag = kissride_path_flag = true;
	script_flag = hhfile_flag = select_flag = update_flag = thread_flag = false;

	System_Data_Reserve (HOUSEHOLD, 0);

#ifdef BOOST_THREADS
	Enable_Threads (true);
#endif
}

//---------------------------------------------------------
//	Router_Service_Keys
//---------------------------------------------------------

void Router_Service::Router_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ IMPEDANCE_SORT_METHOD, "IMPEDANCE_SORT_METHOD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SAVE_ONLY_SKIMS, "SAVE_ONLY_SKIMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ WALK_PATH_DETAILS, "WALK_PATH_DETAILS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_VEHICLE_ID, "IGNORE_VEHICLE_ID", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LIMIT_PARKING_ACCESS, "LIMIT_PARKING_ACCESS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ ADJUST_ACTIVITY_SCHEDULE, "ADJUST_ACTIVITY_SCHEDULE", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ IGNORE_ACTIVITY_DURATIONS, "IGNORE_ACTIVITY_DURATIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_TIME_CONSTRAINTS, "IGNORE_TIME_CONSTRAINTS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ END_TIME_CONSTRAINT, "END_TIME_CONSTRAINT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", "0..360 minutes", NO_HELP },
		{ IGNORE_ROUTING_PROBLEMS, "IGNORE_ROUTING_PROBLEMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PERCENT_RANDOM_IMPEDANCE, "PERCENT_RANDOM_IMPEDANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 percent", "0.0..100.0 percent", NO_HELP },
		{ TRAVELER_TYPE_SCRIPT, "TRAVELER_TYPE_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAVELER_PARAMETER_FILE, "TRAVELER_PARAMETER_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ WALK_SPEED, "WALK_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 mps", "0.5..4.0 mps", NO_HELP },
		{ BICYCLE_SPEED, "BICYCLE_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "4.0 mps", "1.0..10.0 mps", NO_HELP },
		{ WALK_TIME_VALUES, "WALK_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ BICYCLE_TIME_VALUES, "BICYCLE_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "15.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ FIRST_WAIT_VALUES, "FIRST_WAIT_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ TRANSFER_WAIT_VALUES, "TRANSFER_WAIT_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ PARKING_TIME_VALUES, "PARKING_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ VEHICLE_TIME_VALUES, "VEHICLE_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "10.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ DISTANCE_VALUES, "DISTANCE_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/meter", "0.0..1000.0", NO_HELP },
		{ COST_VALUES, "COST_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/cent", "0.0..1000.0", NO_HELP },
		{ FREEWAY_BIAS_FACTORS, "FREEWAY_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ EXPRESSWAY_BIAS_FACTORS, "EXPRESSWAY_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ LEFT_TURN_PENALTIES, "LEFT_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ RIGHT_TURN_PENALTIES, "RIGHT_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ U_TURN_PENALTIES, "U_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ TRANSFER_PENALTIES, "TRANSFER_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ STOP_WAITING_PENALTIES, "STOP_WAITING_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ STATION_WAITING_PENALTIES, "STATION_WAITING_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ BUS_BIAS_FACTORS, "BUS_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "1.0..3.0", NO_HELP },
		{ BUS_BIAS_CONSTANTS, "BUS_BIAS_CONSTANTS", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ RAIL_BIAS_FACTORS, "RAIL_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.1..1.0", NO_HELP },
		{ RAIL_BIAS_CONSTANTS, "RAIL_BIAS_CONSTANTS", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "-1000..0", NO_HELP },
		{ MAX_WALK_DISTANCES, "MAX_WALK_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "2000 meters", "100..20000 meters", NO_HELP },
		{ WALK_PENALTY_DISTANCES, "WALK_PENALTY_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "2000 meters", "100..10000 meters", NO_HELP },
		{ WALK_PENALTY_FACTORS, "WALK_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_BICYCLE_DISTANCES, "MAX_BICYCLE_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "10000 meters", "1000..40000 meters", NO_HELP },
		{ BIKE_PENALTY_DISTANCES, "BIKE_PENALTY_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "10000 meters", "1000..20000 meters", NO_HELP },
		{ BIKE_PENALTY_FACTORS, "BIKE_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_WAIT_TIMES, "MAX_WAIT_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "60 minutes", "5..400 minutes", NO_HELP },
		{ WAIT_PENALTY_TIMES, "WAIT_PENALTY_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "60 minutes", "5..200 minutes", NO_HELP },
		{ WAIT_PENALTY_FACTORS, "WAIT_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MIN_WAIT_TIMES, "MIN_WAIT_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "0 seconds", "0..3600 seconds", NO_HELP },
		{ MAX_NUMBER_OF_TRANSFERS, "MAX_NUMBER_OF_TRANSFERS", LEVEL1, OPT_KEY, LIST_KEY, "3", "0..10", NO_HELP },
		{ MAX_PARK_RIDE_PERCENTS, "MAX_PARK_RIDE_PERCENTS", LEVEL1, OPT_KEY, LIST_KEY, "50 percent", "1..100 percent", NO_HELP },
		{ MAX_KISS_RIDE_PERCENTS, "MAX_KISS_RIDE_PERCENTS", LEVEL1, OPT_KEY, LIST_KEY, "35 percent", "1..100 percent", NO_HELP },
		{ KISS_RIDE_TIME_FACTORS, "KISS_RIDE_TIME_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "2.5", "1.0..4.4", NO_HELP },
		{ KISS_RIDE_STOP_TYPES, "KISS_RIDE_STOP_TYPES", LEVEL0, OPT_KEY, TEXT_KEY, "EXTERNAL", "", NO_HELP },
		{ MAX_KISS_RIDE_DROPOFF_WALK, "MAX_KISS_RIDE_DROPOFF_WALK", LEVEL0, OPT_KEY, FLOAT_KEY, "100 meters", "10..500 meters", NO_HELP },
		{ TRANSIT_PENALTY_FILE, "TRANSIT_PENALTY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PARKING_PENALTY_FILE, "PARKING_PENALTY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DEFAULT_PARKING_DURATION, "DEFAULT_PARKING_DURATION", LEVEL0, OPT_KEY, TIME_KEY, "0.0 hours", "0.0..24.0 hours", NO_HELP },
		{ MAX_LEGS_PER_PATH, "MAX_LEGS_PER_PATH", LEVEL0, OPT_KEY, INT_KEY, "1000", "10..10000", NO_HELP },
		{ MAX_NUMBER_OF_PATHS, "MAX_NUMBER_OF_PATHS", LEVEL0, OPT_KEY, INT_KEY, "4", "1..10", NO_HELP },
		{ FARE_CLASS_DISTRIBUTION, "FARE_CLASS_DISTRIBUTION", LEVEL0, OPT_KEY, LIST_KEY, "0", "", NO_HELP },
		{ LOCAL_ACCESS_DISTANCE, "LOCAL_ACCESS_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "2000 meters", "100..7500 meters", NO_HELP },
		{ LOCAL_FACILITY_TYPE, "LOCAL_FACILITY_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "EXTERNAL", "MAJOR..LOCAL, EXTERNAL", NO_HELP },
		{ LOCAL_IMPEDANCE_FACTOR, "LOCAL_IMPEDANCE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_CIRCUITY_RATIO, "MAX_CIRCUITY_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..10.0", NO_HELP },
		{ MIN_CIRCUITY_DISTANCE, "MIN_CIRCUITY_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "2000 meters", "0..10000 meters", NO_HELP },
		{ MAX_CIRCUITY_DISTANCE, "MAX_CIRCUITY_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "20000 meters", "0..100000 meters", NO_HELP },
		{ MIN_DURATION_FACTORS, "MIN_DURATION_FACTORS", LEVEL0, OPT_KEY, LIST_KEY, "0.1, 0.5, 0.8, 1.0", "0.0..1.0", NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				Error (String ("Router Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int flow_time_service_keys [] = {
		UPDATE_FLOW_RATES,  UPDATE_TURNING_MOVEMENTS, CLEAR_INPUT_FLOW_RATES, 
		UPDATE_TRAVEL_TIMES, LINK_DELAY_UPDATE_RATE, LINK_DELAY_FLOW_FACTOR, EQUATION_PARAMETERS, 0
	};
	Flow_Time_Service_Keys (flow_time_service_keys);
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Router_Service::Program_Control (void)
{
	int i;
	double dvalue;
	String text, result;
	Double_List value_list;

	Data_Service::Program_Control ();

	Read_Flow_Time_Keys ();

	Print (2, "Path Building Parameters:");

	memset (&param, '\0', sizeof (param));

	//---- check the transit flags ----

	if (transit_path_flag) {
		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE)) transit_path_flag = false;
	}
	if (parkride_path_flag && !transit_path_flag) parkride_path_flag = false;
	if (kissride_path_flag && !transit_path_flag) kissride_path_flag = false;

	if (parkride_path_flag || kissride_path_flag) Location_XY_Flag (true);
	param.grade_flag = false;

	//---- impedance sort method ----

	param.sort_method = Get_Control_Flag (IMPEDANCE_SORT_METHOD);

	//---- save only skims ----

	param.skim_only = Get_Control_Flag (SAVE_ONLY_SKIMS);

	//---- write walk path ----

	if (walk_path_flag) {
		param.walk_detail = Get_Control_Flag (WALK_PATH_DETAILS);
	}

	//---- get vehicle flags ----

	if (drive_path_flag) {
		param.ignore_veh_flag = Get_Control_Flag (IGNORE_VEHICLE_ID);

		param.limit_access = Get_Control_Flag (LIMIT_PARKING_ACCESS);

		param.delay_flag = System_File_Flag (LINK_DELAY);
		if (param.delay_flag) {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
			param.turn_delay_flag = file->Turn_Flag ();
		} else {
			param.turn_delay_flag = false;
			if (Time_Updates () && update_rate > 0) param.delay_flag = true;
		}
	}

	//---- adjust activity schedule ----

	param.adjust_schedule = Get_Control_Flag (ADJUST_ACTIVITY_SCHEDULE);

	//---- ignore activity durations ----

	param.ignore_duration = Get_Control_Flag (IGNORE_ACTIVITY_DURATIONS);

	//---- ignore time constraints ----
	
	param.ignore_time = Get_Control_Flag (IGNORE_TIME_CONSTRAINTS);

	if (!param.ignore_time) {
		param.end_time = Get_Control_Time (END_TIME_CONSTRAINT); 
	}

	//---- ignore routing errors ----
	
	param.ignore_errors = Get_Control_Flag (IGNORE_ROUTING_PROBLEMS);

	//---- random impedance ----
	
	param.random_imped = Get_Control_Double (PERCENT_RANDOM_IMPEDANCE);

	//---- open traveler type script ----

	if (System_File_Flag (HOUSEHOLD)) {
		hhfile_flag = true;
		text = Get_Control_String (TRAVELER_TYPE_SCRIPT);

		if (!text.empty ()) {
			Print (1);
			script_file.File_Type ("Traveler Type Script");

			script_file.Open (Project_Filename (text));
			script_flag = true;
		}
	}

	//---- open traveler parameter file ----

	text = Get_Control_String (TRAVELER_PARAMETER_FILE);

	if (!text.empty ()) {
		Db_File file;
		file.File_Type ("Traveler Parameter File");
		if (!script_flag) Print (1);

		if (file.Open (Project_Filename (text))) {
			bool first = true;

			while (file.Read ()) {
				if (first) {
					first = false;
					text = file.Record_String ();
					if (text.Starts_With ("Key")) continue;
				}
				Process_Control_String ((char *) file.Record_String ());
			}
		}
	}
	Print (1);

	//---- walk speed ----
	
	param.walk_speed = Get_Control_Double (WALK_SPEED);

	//---- bicycle speed ----

	param.bike_speed = Get_Control_Double (BICYCLE_SPEED);

	//---- walk time value ----

	Get_Control_List_Groups (WALK_TIME_VALUES, value_walk);
	param.value_walk = value_walk [0];

	//---- bicycle time value ----

	if (bike_path_flag) {
		Get_Control_List_Groups (BICYCLE_TIME_VALUES, value_bike);
		param.value_bike = value_bike [0];
	}

	if (transit_path_flag) {
		Print (1);

		//---- first wait value ----

		Get_Control_List_Groups (FIRST_WAIT_VALUES, value_wait);
		param.value_wait = value_wait [0];

		//---- transfer wait value ----

		Get_Control_List_Groups (TRANSFER_WAIT_VALUES, value_xfer);
		param.value_xfer = value_xfer [0];
	}

	//---- parking time value ----

	Get_Control_List_Groups (PARKING_TIME_VALUES, value_park);
	param.value_park = value_park [0];

	//---- vehicle time value ----

	Get_Control_List_Groups (VEHICLE_TIME_VALUES, value_time);
	param.value_time = value_time [0];

	//---- distance value ----

	Get_Control_List_Groups (DISTANCE_VALUES, value_dist);
	param.value_dist = value_dist [0];

	//---- cost value ----

	Get_Control_List_Groups (COST_VALUES, value_cost);
	param.value_cost = value_cost [0];

	//---- check the impedance values ----

	for (i=1; i <= (int) value_time.size (); i++) {
		if (value_walk.Best (i) == 0 && value_wait.Best (i) == 0 && value_time.Best (i) == 0 &&
			value_dist.Best (i) == 0 && value_cost.Best (i) == 0) {

			Error (String ("At least One Impedance Value must be Non-Zero for Household Type %d") % i);
		}
	}

	if (drive_path_flag) {
		Print (1);
		//---- freeway bias factors ----

		Get_Control_List_Groups (FREEWAY_BIAS_FACTORS, freeway_fac);
		param.freeway_fac = freeway_fac [0];

		//---- expressway bias factors ----

		Get_Control_List_Groups (EXPRESSWAY_BIAS_FACTORS, express_fac);
		param.express_fac = express_fac [0];
		
		//---- left turn penalty ----

		Get_Control_List_Groups (LEFT_TURN_PENALTIES, left_imped);
		param.left_imped = left_imped [0];
			
		//---- right turn penalty ----
		
		Get_Control_List_Groups (RIGHT_TURN_PENALTIES, right_imped);
		param.right_imped = right_imped [0];

		//---- u-turn penalty ----
		
		Get_Control_List_Groups (U_TURN_PENALTIES, uturn_imped);
		param.uturn_imped = uturn_imped [0];
	}

	if (transit_path_flag) {
		Print (1);

		//---- transfer_penalty ----

		Get_Control_List_Groups (TRANSFER_PENALTIES, xfer_imped);
		param.xfer_imped = xfer_imped [0];	
	
		//---- stop waiting penalty ----
		
		Get_Control_List_Groups (STOP_WAITING_PENALTIES, stop_imped);
		param.stop_imped = stop_imped [0];	
		
		//---- station waiting penalty ----
		
		Get_Control_List_Groups (STATION_WAITING_PENALTIES, station_imped);
		param.station_imped = station_imped [0];	

		//---- bus bias factor ----
		
		Get_Control_List_Groups (BUS_BIAS_FACTORS, bus_bias);
		param.bus_bias = bus_bias [0];

		//---- bus bias constant ----

		Get_Control_List_Groups (BUS_BIAS_CONSTANTS, bus_const);
		param.bus_const = bus_const [0];

		//---- rail bias factor ----

		Get_Control_List_Groups (RAIL_BIAS_FACTORS, rail_bias);
		param.rail_bias = rail_bias [0];

		//---- rail bias constant ----
		
		Get_Control_List_Groups (RAIL_BIAS_CONSTANTS, rail_const);
		param.rail_const = rail_const [0];
	}

	//---- max walk distance ----

	Print (1);
	Get_Control_List_Groups (MAX_WALK_DISTANCES, max_walk);
	param.max_walk = Round (max_walk [0]);

	//---- walk penalty distance ----

	Get_Control_List_Groups (WALK_PENALTY_DISTANCES, walk_pen);
	param.walk_pen = Round (walk_pen [0]);

	//---- walk penalty factor ----

	Get_Control_List_Groups (WALK_PENALTY_FACTORS, walk_fac);
	param.walk_fac = walk_fac [0];

	if (bike_path_flag) {

		//---- max bike distance ----

		Get_Control_List_Groups (MAX_BICYCLE_DISTANCES, max_bike);
		param.max_bike = Round (max_bike [0]);

		//---- bike penalty distance ----

		Get_Control_List_Groups (BIKE_PENALTY_DISTANCES, bike_pen);
		param.bike_pen = Round (bike_pen [0]);

		//---- bike penalty factor ----

		Get_Control_List_Groups (BIKE_PENALTY_FACTORS, bike_fac);
		param.bike_fac = bike_fac [0];
	}

	if (transit_path_flag) {
		Print (1);

		//---- max wait time ----
		
		Get_Control_List_Groups (MAX_WAIT_TIMES, max_wait);
		param.max_wait = max_wait [0];

		//---- wait penalty distance ----

		Get_Control_List_Groups (WAIT_PENALTY_TIMES, wait_pen);
		param.wait_pen = wait_pen [0];

		//---- wait penalty factor ----

		Get_Control_List_Groups (WAIT_PENALTY_FACTORS, wait_fac);
		param.wait_fac = wait_fac [0];

		//---- min wait time ----
		
		Get_Control_List_Groups (MIN_WAIT_TIMES, min_wait);
		param.min_wait = min_wait [0];

		//---- max transfers ----

		Get_Control_List_Groups (MAX_NUMBER_OF_TRANSFERS, max_xfers);
		param.max_xfers = max_xfers [0];

		//---- max park ride percentage ----

		if (parkride_path_flag) {
			Print (1);
			Get_Control_List_Groups (MAX_PARK_RIDE_PERCENTS, max_parkride);
			param.max_parkride = max_parkride [0];
		}

		//---- max kiss ride percentage ----

		if (kissride_path_flag) {
			Print (1);
			Get_Control_List_Groups (MAX_KISS_RIDE_PERCENTS, max_kissride);
			param.max_kissride = max_kissride [0];

			//---- kiss ride time factor ----

			Get_Control_List_Groups (KISS_RIDE_TIME_FACTORS, kissride_fac); 
			param.kissride_fac = kissride_fac [0];

			//---- kiss ride stop types ----

			text = Get_Control_Text (KISS_RIDE_STOP_TYPES);

			if (!text.empty ()) {
				memset (param.kissride_type, '\0', sizeof (param.kissride_type));

				while (text.Split (result)) {
					i = Stop_Code (result);

					if (i < 0 || i > EXTLOAD) {
						Error (String ("Kiss-&-Ride Stop Type %s is Out of Range") % result);
					}
					param.kissride_type [i] = true;
				}
			} else {
				for (i=0; i <= EXTLOAD; i++) {
					param.kissride_type [i] = true;
				}
			}

			//---- max kiss ride dropoff walk ----

			param.kissride_walk = Get_Control_Integer (MAX_KISS_RIDE_DROPOFF_WALK);
		}

		//---- open the transit penalty file ----
		
		text = Get_Control_String (TRANSIT_PENALTY_FILE);

		if (!text.empty ()) {
			stop_pen_file.File_Type ("Transit Penalty File");
			Print (1);

			stop_pen_file.Open (Project_Filename (text));
			param.stop_pen_flag = true;

			stop_pen_file.Required_Field ("STOP", "STOP_ID", "ID");
			stop_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
		}
	}

	if (drive_path_flag || transit_path_flag) {

		//---- open the parking penalty file ----
		
		text = Get_Control_String (PARKING_PENALTY_FILE);

		if (!text.empty ()) {
			park_pen_file.File_Type ("Parking Penalty File");
			Print (1);

			park_pen_file.Open (Project_Filename (text));
			param.park_pen_flag = true;

			park_pen_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
			park_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
		}

		//---- default parking duration ----

		Print (1);
		param.parking_duration = Get_Control_Time (DEFAULT_PARKING_DURATION);
	}

	//---- max legs per path ----

	param.leg_check = Get_Control_Integer (MAX_LEGS_PER_PATH);

	if (transit_path_flag) {
		Print (1);

		//---- max paths ----
		
		param.max_paths = Get_Control_Integer (MAX_NUMBER_OF_PATHS);

		//---- fare class distribution ----

		text = Get_Control_Text (FARE_CLASS_DISTRIBUTION);

		if (!text.empty ()) {
			memset (param.fare_class, '\0', sizeof (param.fare_class));
			param.fare_class_flag = true;
			i = 0;

			while (text.Split (result) && i <= SPECIAL) {
				dvalue = result.Double ();
	
				if (dvalue < 0.0 || dvalue > 100.0) {
					Error (String ("Fare Class Distribution %.2lf is Out of Range (0.0..100.0)") % dvalue);
				}
				param.fare_class [i++] = dvalue;
			}
			dvalue = 0;
			for (--i; i >= 0; i--) {
				dvalue += param.fare_class [i];
			}
			if (dvalue == 0.0) {
				param.fare_class [0] = 1.0;
			} else {
				dvalue = 1.0 / dvalue;

				for (i=0; i <= SPECIAL; i++) {
					param.fare_class [i] *= dvalue;
				}
			}

			//---- convert to a cumulative distribution ----

			dvalue = 0.0;

			for (i=0; i <= SPECIAL; i++) {
				dvalue += param.fare_class [i];
				param.fare_class [i] = dvalue;
			}
			param.fare_class [SPECIAL] = 1.0;
		}
	}

	if (drive_path_flag) {

		//---- local facility type ----

		Print (1);
		text = Get_Control_Text (LOCAL_FACILITY_TYPE);

		if (!text.empty ()) {
			param.local_type = Facility_Code (text);

			if (param.local_type != EXTERNAL) {
				if (param.local_type < MAJOR || param.local_type > LOCAL) {
					Error (String ("Local Facility Type %s is Out of Range (MAJOR..LOCAL)") % text);
				} else {
					param.local_flag = true;

					//---- local access distance ----

					param.local_distance = Get_Control_Integer (LOCAL_ACCESS_DISTANCE);

					//---- local impedance factor ----

					param.local_factor = Get_Control_Double (LOCAL_IMPEDANCE_FACTOR);
				}
			} else {
				param.local_flag = false;
			}
		}

		//---- max circuity ratio ----

		param.max_ratio = Get_Control_Double (MAX_CIRCUITY_RATIO);

		if (param.max_ratio != 0.0) {
			param.distance_flag = true;

			//---- min circuity distance ----

			param.min_distance = Get_Control_Integer (MIN_CIRCUITY_DISTANCE);

			//---- max circuity distance ----

			param.max_distance = Get_Control_Integer (MAX_CIRCUITY_DISTANCE);

			if (param.max_distance < param.min_distance) {
				Error ("Maximum Circuity Distance < Minimum Circuity Distance");
			}
		}
	}

	//---- minimum duration factors ----

	Get_Control_List_Groups (MIN_DURATION_FACTORS, value_list);

	for (i=0; i <= CRITICAL; i++) {
		param.duration_factor [i] = value_list.Best (i+1);
	}
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router_Service::Execute (void)
{
	int anode, bnode, link_dir, index, *link_ptr, *next_ptr;
	Int2_Map_Stat ab_stat;
	Int_Map_Itr map_itr;

	//---- compile the type script ----

	if (script_flag) {
		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		Db_Base_Array files;

		files.push_back (System_File_Base (HOUSEHOLD));

		type_script.Initialize (files, random.Seed () + 1);

		if (!type_script.Compile (script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Household Type Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			type_script.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- read the data files ----

	Data_Service::Execute ();

	if (!Control_Flag ()) {
		Error ("Router_Service::Program_Control has Not Been Called");
	}

	//---- initialize the data structures ----

	if (walk_path_flag) {
		int link, *first_ptr;
		Link_Itr link_itr;
		List_Data *list_ptr;

		walk_list.resize (node_array.size (), -1);
		walk_link.resize (link_array.size ());

		for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
			if (!Use_Permission (link_itr->Use (), WALK)) continue;
			list_ptr = &walk_link [link];

			first_ptr = &walk_list [link_itr->Anode ()];
			list_ptr->From_List (*first_ptr);
			*first_ptr = link;

			first_ptr = &walk_list [link_itr->Bnode ()];
			list_ptr->To_List (*first_ptr);
			*first_ptr = link;

			if (!bike_path_flag) {
				if (link_itr->Anode () < link_itr->Bnode ()) {
					anode = link_itr->Anode ();
					bnode = link_itr->Bnode ();
					link_dir = link_itr->Link ();
				} else {
					anode = link_itr->Bnode ();
					bnode = link_itr->Anode ();
					link_dir = -link_itr->Link ();
				}
				ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link_dir));
			}
		}
	}
	if (bike_path_flag) {
		int link, *first_ptr;
		Link_Itr link_itr;
		List_Data *list_ptr;

		bike_list.resize (node_array.size (), -1);
		bike_link.resize (link_array.size ());

		for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
			if (!Use_Permission (link_itr->Use (), BIKE)) continue;
			list_ptr = &bike_link [link];

			first_ptr = &bike_list [link_itr->Anode ()];
			list_ptr->From_List (*first_ptr);
			*first_ptr = link;

			first_ptr = &bike_list [link_itr->Bnode ()];
			list_ptr->To_List (*first_ptr);
			*first_ptr = link;

			if (!walk_path_flag) {
				if (link_itr->Anode () < link_itr->Bnode ()) {
					anode = link_itr->Anode ();
					bnode = link_itr->Bnode ();
					link_dir = link_itr->Link ();
				} else {
					anode = link_itr->Bnode ();
					bnode = link_itr->Anode ();
					link_dir = -link_itr->Link ();
				}
				ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link_dir));
			}
		}
	}
	if (walk_path_flag || bike_path_flag) {
		Link_Itr link_itr;

		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			if (!Use_Permission (link_itr->Use (), WALK) && 
				!Use_Permission (link_itr->Use (), BIKE)) continue;

			if (link_itr->Anode () < link_itr->Bnode ()) {
				anode = link_itr->Anode ();
				bnode = link_itr->Bnode ();
				link_dir = link_itr->Link ();
			} else {
				anode = link_itr->Bnode ();
				bnode = link_itr->Anode ();
				link_dir = -link_itr->Link ();
			}
			ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link_dir));
		}
	}

	//---- process access links ----

	access_link_flag = System_Data_Flag (ACCESS_LINK);

	if (access_link_flag) {
		Access_Itr access_itr;
		List_Data *from_ptr, *to_ptr, *first_ptr;

		loc_access.resize (location_array.size ());
		if (drive_path_flag) park_access.resize (parking_array.size ());
		if (transit_path_flag) stop_access.resize (stop_array.size ());
		if (transit_path_flag) node_access.resize (node_array.size ());

		from_access.resize (access_array.size ());
		to_access.resize (access_array.size ());

		for (index=0, access_itr = access_array.begin (); access_itr != access_array.end (); access_itr++, index++) {

			from_ptr = &from_access [index];
			to_ptr = &to_access [index];

			//---- from list ----

			if (access_itr->From_Type () == LOCATION_ID) {
				first_ptr = &loc_access [access_itr->From_ID ()];

				from_ptr->From_List (first_ptr->From_List ());
				first_ptr->From_List (index);

				if (access_itr->Dir () != 1) {
					to_ptr->From_List (first_ptr->To_List ());
					first_ptr->To_List (index);
				}
			} else if (access_itr->From_Type () == PARKING_ID) {
				if (drive_path_flag) {
					first_ptr = &park_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			} else if (access_itr->From_Type () == STOP_ID) {
				if (transit_path_flag) {
					first_ptr = &stop_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			} else if (access_itr->From_Type () == NODE_ID) {
				if (transit_path_flag) {
					first_ptr = &node_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			}

			//---- to list ----

			if (access_itr->To_Type () == LOCATION_ID) {
				first_ptr = &loc_access [access_itr->To_ID ()];

				to_ptr->To_List (first_ptr->To_List ());
				first_ptr->To_List (index);

				if (access_itr->Dir () != 1) {
					from_ptr->To_List (first_ptr->From_List ());
					first_ptr->From_List (index);
				}
			} else if (access_itr->To_Type () == PARKING_ID) {
				if (drive_path_flag) {
					first_ptr = &park_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			} else if (access_itr->To_Type () == STOP_ID) {
				if (transit_path_flag) {
					first_ptr = &stop_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			} else if (access_itr->To_Type () == NODE_ID) {
				if (transit_path_flag) {
					first_ptr = &node_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			}
		}
	}

	//---- link to location map ----

	link_location.resize (link_array.size (), -1);
	next_location.resize (location_array.size (), -1);

	Location_Itr loc_itr;

	for (index=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, index++) {
		next_ptr = &next_location [index];
		link_ptr = &link_location [loc_itr->Link ()];

		*next_ptr = *link_ptr;
		*link_ptr = index;
	}

	//---- link to parking map ----

	if (drive_path_flag) {
		Link_Itr link_itr;

		link_parking.resize (link_array.size (), -1);
		next_parking.resize (parking_array.size (), -1);
		
		Parking_Itr park_itr;

		for (index=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, index++) {
			next_ptr = &next_parking [index];
			link_ptr = &link_parking [park_itr->Link ()];

			*next_ptr = *link_ptr;
			*link_ptr = index;
		}
		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			if (link_itr->Grade () != 0) {
				param.grade_flag = true;
				break;
			}
		}
	}

	//---- link to stop map ----

	if (transit_path_flag) {
		int route, stop;
		Line_Itr line_itr;
		Line_Stop_Itr line_stop_itr;
		Stop_Itr stop_itr;

		//---- build the list of routes serving a stop ----
		
		Route_Stop_Data route_stop, *rstop_ptr;

		stop_list.resize (stop_array.size (), -1);
		route_stop_array.resize (line_array.Route_Stops (), route_stop);

		for (index=route=0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++, route++) {
			for (stop=0, line_stop_itr = line_itr->begin (); line_stop_itr != line_itr->end (); line_stop_itr++, stop++, index++) {
				next_ptr = &stop_list [line_stop_itr->Stop ()];
				rstop_ptr = &route_stop_array [index];

				rstop_ptr->Route (route);
				rstop_ptr->Stop (stop);

				rstop_ptr->Next (*next_ptr);
				*next_ptr = index;
			}
		}

		//---- build the list of stops on a link ----

		link_stop.resize (link_array.size (), -1);
		next_stop.resize (stop_array.size (), -1);
		
		for (index=0, stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++, index++) {
			next_ptr = &next_stop [index];
			link_ptr = &link_stop [stop_itr->Link ()];

			*next_ptr = *link_ptr;
			*link_ptr = index;
		}

		//---- park ride lot locations ----

		if (parkride_path_flag) {
			Parking_Itr park_itr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Lot_XY_Data lot_rec;

			int i, ax, ay, bx, by;
			double factor;

			for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
				if (park_itr->Type () != PARKRIDE) continue;

				link_ptr = &link_array [park_itr->Link ()];

				node_ptr = &node_array [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = &node_array [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_itr->Offset () / (double) link_ptr->Length ();

				lot_rec.Lot (i);
				lot_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				lot_rec.Y (ay + (int) ((by - ay) * factor + 0.5));

				park_ride.push_back (lot_rec);
			}
			Print (2, "Number of Park-&-Ride Lots = ") << park_ride.size ();
		}

		//----- get the kiss-&-ride lots ----

		if (kissride_path_flag) {
			Parking_Itr park_itr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Stop_Itr stop_itr;
			Lot_XY_Data kiss_rec;

			int i, ax, ay, bx, by, distance;
			double factor;
			bool flag;

			for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
				link_ptr = &link_array [park_itr->Link ()];

				node_ptr = &node_array [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = &node_array [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_itr->Offset () / (double) link_ptr->Length ();

				kiss_rec.Lot (i);
				kiss_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				kiss_rec.Y (ay + (int) ((by - ay) * factor + 0.5));
				flag = false;

				for (stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++) {
					if (!param.kissride_type [stop_itr->Type ()]) continue;

					link_ptr = &link_array [stop_itr->Link ()];

					node_ptr = &node_array [link_ptr->Anode ()];

					ax = node_ptr->X ();
					ay = node_ptr->Y ();

					node_ptr = &node_array [link_ptr->Bnode ()];

					bx = node_ptr->X ();
					by = node_ptr->Y ();

					factor = (double) stop_itr->Offset () / (double) link_ptr->Length ();

					ax += (int) ((bx - ax) * factor + 0.5) - kiss_rec.X ();
					ay += (int) ((by - ay) * factor + 0.5) - kiss_rec.Y ();

					distance = abs (ax) + abs (ay);

					if (Resolve (distance) < param.kissride_walk) {
						flag = true;
						break;
					}
				}
				if (flag) kiss_ride.push_back (kiss_rec);
			}
			Print (2, "Number of Kiss-&-Ride Lots = ") << kiss_ride.size ();
		}

		//---- stop penalty file ----

		if (param.stop_pen_flag) {
			Line_Data *line_ptr;
			Line_Run_Itr run_itr;

			int penalty, run, stop_field, pen_field, route_field, run_field, num_penalty, num_stops;
			
			stop_field = stop_pen_file.Required_Field ("STOP", "STOP_ID", "ID");
			pen_field = stop_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
			route_field = stop_pen_file.Optional_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
			run_field = stop_pen_file.Optional_Field ("RUN", "TRIP");

			//---- process each stop penalty file record ----

			num_penalty = num_stops = 0;

			while (stop_pen_file.Read ()) {
				penalty = stop_pen_file.Get_Integer (pen_field);
				if (penalty <= 0) continue;
				penalty = Round (penalty);

				route = stop_pen_file.Get_Integer (route_field);
				run = stop_pen_file.Get_Integer (run_field);
				stop = stop_pen_file.Get_Integer (stop_field);

				if (stop > 0) {
					map_itr = stop_map.find (stop);

					if (map_itr == stop_map.end ()) {
						Warning (String ("Transit Penalty Stop %d was Not Found") % stop);
						continue;
					}
					stop = map_itr->second;
				} else {
					stop = -1;
				}
				if (route > 0) {
					map_itr = line_map.find (route);

					if (map_itr == line_map.end ()) {
						Warning (String ("Transit Penalty Route %d was Not Found") % route);
						continue;
					}
					line_ptr = &line_array [route];

					for (line_stop_itr = line_ptr->begin (); line_stop_itr != line_ptr->end (); line_stop_itr++) {
						if (stop < 0 || line_stop_itr->Stop () == stop) {
							if (run > 0) {
								if (run <= (int) line_stop_itr->size ()) {
									line_stop_itr->at (run - 1).Penalty (penalty);
									num_stops++;
								}
							} else {
								for (run_itr = line_stop_itr->begin (); run_itr != line_stop_itr->end (); run_itr++) {
									run_itr->Penalty (penalty);
									num_stops++;
								}
							}
						}
					}

				} else {
					for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
						for (line_stop_itr = line_itr->begin (); line_stop_itr != line_itr->end (); line_stop_itr++) {
							if (stop < 0 || line_stop_itr->Stop () == stop) {
								if (run > 0) {
									if (run <= (int) line_stop_itr->size ()) {
										line_stop_itr->at (run - 1).Penalty (penalty);
										num_stops++;
									}
								} else {
									for (run_itr = line_stop_itr->begin (); run_itr != line_stop_itr->end (); run_itr++) {
										run_itr->Penalty (penalty);
										num_stops++;
									}
								}
							}
						}
					}
				}
				num_penalty++;
			}
			Print (2, "Number of Transit Penalty Records = ") << num_penalty;
			Print (1, "Number of Route-Run-Stop Penalties = ") << num_stops;
		}
	}

	//---- parking penalty file ----

	if (param.park_pen_flag) {
		int parking, penalty, park_field, pen_field, num_penalty;

		park_field = park_pen_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
		pen_field = park_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");

		//---- initialize the penalty field ----

		park_penalty.assign (parking_array.size (), 0);

		//---- process each parking penalty file record ----

		num_penalty = 0;

		while (park_pen_file.Read ()) {
			parking = park_pen_file.Get_Integer (park_field);
			if (parking <= 0) continue;

			map_itr = parking_map.find (parking);

			if (map_itr == parking_map.end ()) {
				Warning (String ("Parking Penalty Lot %d was Not Found") % parking);
				continue;
			}
			penalty = park_pen_file.Get_Integer (pen_field);
			if (penalty <= 0) continue;

			park_penalty [map_itr->second] = Round (penalty);
			num_penalty++;
		}
		Print (2, "Number of Parking Penalty Records = ") << num_penalty;
		park_pen_file.Close ();
	}
}

//---------------------------------------------------------
//	Set_Parameters
//---------------------------------------------------------

void Router_Service::Set_Parameters (Path_Parameters &p, int type)
{
	if (type < 0) return;

	memcpy (&p, &param, sizeof (param));

	p.traveler_type = type;

	p.value_walk = value_walk.Best (type);
	p.value_bike = value_bike.Best (type);
	p.value_time = value_time.Best (type);
	p.value_wait = value_wait.Best (type);
	p.value_xfer = value_xfer.Best (type);
	p.value_park = value_park.Best (type);
	p.value_dist = value_dist.Best (type);
	p.value_cost = value_cost.Best (type);
	p.freeway_fac = freeway_fac.Best (type);
	p.express_fac = express_fac.Best (type);
	p.left_imped = left_imped.Best (type);
	p.right_imped = right_imped.Best (type);
	p.uturn_imped = uturn_imped.Best (type);
	p.xfer_imped = xfer_imped.Best (type);
	p.stop_imped = stop_imped.Best (type);
	p.station_imped = station_imped.Best (type);
	p.bus_bias = bus_bias.Best (type);
	p.bus_const = bus_const.Best (type);
	p.rail_bias = rail_bias.Best (type);
	p.rail_const = rail_const.Best (type);
	p.max_walk = Round (max_walk.Best (type));
	p.walk_pen = Round (walk_pen.Best (type));
	p.walk_fac = walk_fac.Best (type);
	p.max_bike = Round (max_bike.Best (type));
	p.bike_pen = Round (bike_pen.Best (type));
	p.bike_fac = bike_fac.Best (type);
	p.max_wait = max_wait.Best (type);
	p.wait_pen = wait_pen.Best (type);
	p.wait_fac = wait_fac.Best (type);
	p.min_wait = min_wait.Best (type);
	p.max_xfers = max_xfers.Best (type);
	p.max_parkride = max_parkride.Best (type);
	p.max_kissride = max_kissride.Best (type);
	p.kissride_fac = kissride_fac.Best (type);

	if (p.walk_pen == 0 || p.walk_fac == 0.0) p.walk_pen = p.max_walk;
	if (p.bike_pen == 0 || p.bike_fac == 0.0) p.bike_pen = p.max_bike;
	if (p.wait_pen == 0 || p.wait_fac == 0.0) p.wait_pen = p.max_wait;
}

//---------------------------------------------------------
//	Get_Household_Data
//---------------------------------------------------------

bool Router_Service::Get_Household_Data (Household_File &file, Household_Data &household_rec, int partition)
{
	int hhold, person, type;
	Int2_Map_Stat map_stat;

	type = MAX (file.Type (), 1);

	if (script_flag) {
		type = type_script.Execute ();
		if (type == 0) return (false);
	}
	hhold = file.Household ();
	person = file.Person ();

	if (!file.Nested ()) {
		map_stat = hhold_type.insert (Int2_Map_Data (Int2_Key (hhold, 1), type));
	} else {
		map_stat = hhold_type.insert (Int2_Map_Data (Int2_Key (hhold, person), type));
	}
	person = partition;
	household_rec.Household (hhold);
	return (false);
}

//---------------------------------------------------------
//	Update_Parking
//---------------------------------------------------------

bool Router_Service::Update_Parking (Trip_Data &trip)
{
	if (System_Data_Flag (VEHICLE) && trip.Vehicle () > 0) {
		int index, min_diff, diff, lot, acc, offset, near_offset;
		bool flag;

		Int_Map_Itr map_itr;
		Vehicle_Data *veh_ptr;
		Vehicle_Index veh_index;
		Vehicle_Map_Itr veh_itr;
		Link_Data *link_ptr;
		Location_Data *loc_ptr;
		Parking_Data *lot_ptr;
		List_Data *acc_ptr;
		Access_Data *access_ptr;

		//---- get the vehicle ----

		veh_index.Household (trip.Household ());
		veh_index.Vehicle (trip.Vehicle ());

		veh_itr = vehicle_map.find (veh_index);
		if (veh_itr == vehicle_map.end ()) return (false);

		veh_ptr = &vehicle_array [veh_itr->second];

		//---- get the activity location ----

		map_itr = location_map.find (trip.Origin ());
		if (map_itr == location_map.end ()) return (false);

		index = map_itr->second;

		//---- check access links to parking ----

		if (access_link_flag) {

			for (acc = loc_access [index].From_List (); acc >= 0; acc = acc_ptr->Next (flag)) {
				access_ptr = &access_array [acc];

				flag = (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == index);
				acc_ptr = &from_access [acc];

				if (flag) {
					if (access_ptr->To_Type () != PARKING_ID) continue;
					lot = access_ptr->To_ID ();
				} else {
					if (access_ptr->From_Type () != PARKING_ID) continue;
					lot = access_ptr->From_ID ();
				}
				veh_ptr->Parking (lot);
				return (true);
			}
		}

		//---- find the closest parking lot on the link ----

		near_offset = Round (Internal_Units (10.0, FEET));	
		min_diff = MAX_INTEGER;
		lot = -1;

		loc_ptr = &location_array [index];
		index = loc_ptr->Link ();

		link_ptr = &link_array [index];

		if (loc_ptr->Dir () == 1) {
			offset = link_ptr->Length () - loc_ptr->Offset ();
		} else {
			offset = loc_ptr->Offset ();
		}

		for (index = link_parking [index]; index >= 0; index = next_parking [index]) {
			lot_ptr = &parking_array [index];

			if (lot_ptr->Dir () == 1) {
				diff = abs (link_ptr->Length () - lot_ptr->Offset () - offset);
			} else {
				diff = abs (lot_ptr->Offset () - offset);
			}
			if (diff <= min_diff) {
				min_diff = diff;
				lot = index;
				if (diff <= near_offset && loc_ptr->Location () == lot_ptr->Parking ()) break;
			}
		}
		if (lot < 0) return (false);
		veh_ptr->Parking (lot);
	}
	return (true);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Router_Service::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Type Script ----
			Print (1, "Household Type Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Type Stack ----
			Print (1, "Household Type Stack");
			Print (1);
			break;
		default:
			break;
	}
}
