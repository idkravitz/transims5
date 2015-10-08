//*********************************************************
//	System_Defines.hpp - global type definitions
//*********************************************************

#ifndef SYSTEM_DEFINES_HPP
#define SYSTEM_DEFINES_HPP

#include "APIDefs.hpp"

#define VERSION_CODE    50

#define EXECUTION_OFFSET             200
#define SYSTEM_FILE_OFFSET           300
#define SYSTEM_FORMAT_OFFSET         400
#define FILE_SERVICE_OFFSET		     500
#define DATA_SERVICE_OFFSET          600
#define SELECT_SERVICE_OFFSET        700
#define FLOW_TIME_SERVICE_OFFSET     800
#define DRAW_SERVICE_OFFSET          900
#define ROUTER_SERVICE_OFFSET       1000
#define SIM_SERVICE_OFFSET          1100

#define SNAPSHOT_OUTPUT_OFFSET      1200
#define LINK_DELAY_OUTPUT_OFFSET    1220
#define PERFORMANCE_OUTPUT_OFFSET   1240
#define TURN_VOL_OUTPUT_OFFSET      1260
#define RIDERSHIP_OUTPUT_OFFSET     1280
#define OCCUPANCY_OUTPUT_OFFSET     1300
#define EVENT_OUTPUT_OFFSET         1320
#define TRAVELER_OUTPUT_OFFSET      1340
#define SPEED_BIN_OUTPUT_OFFSET     1360
#define SYSTEM_EVENT_OUTPUT_OFFSET  1380
#define PROBLEM_LINK_OUTPUT_OFFSET  1400

#define PROJECTION_OFFSET           1500
#define SMOOTH_DATA_OFFSET          1520
#define DIFFERENCE_DATA_OFFSET      1540

#define END_CONTROL    { 0, "", LEVEL0, OPT_KEY, END_KEY, "", "", NO_HELP }

#define BLANK           ' '
#define FINISH          '\0'

SYSLIB_API extern const char * SPACES;
SYSLIB_API extern const char * COMMA_DELIMITERS;
SYSLIB_API extern const char * SPACE_DELIMITERS;
SYSLIB_API extern const char * TAB_DELIMITERS;
SYSLIB_API extern const char * CSV_DELIMITERS;

#define MAX(a,b)        ((a >= b) ? a : b)
#define MIN(a,b)        ((a <= b) ? a : b)

#define MIDNIGHT        86400
#define MAX_INTEGER     0x7FFFFFFF
#define MAX_IMPEDANCE   0xFFFFFFFF
#define MAX_USHORT      0xFFFF

#define PI              3.1415926535897932384626433832795
#define DEG2RAD         (PI / 180.0)
#define RAD2DEG         (180.0 / PI)

#define METERTOFEET     (39.37/12)              // US Survey Foot
#define	MILETOFEET      (5280.009842519685)     // US Survey Foot
#define MILETOMETER     (MILETOFEET / METERTOFEET)
#define MPSTOMPH        (3600.0 / MILETOMETER)
#define MPHTOMPS        (MILETOMETER / 3600.0)
#define MPHTOFPS        (MILETOFEET / 3600.0)

#ifndef NAN
#define NAN                       (-1.79769e+308)
#endif
#define END_OF_RECORD             -1
#define SCAN_ALL_RECORDS          -1

#define STRING_FIELD_SIZE         128
#define TIME_FIELD_SIZE           16
#define NUM_SCHEDULE_COLUMNS      8
#define TURN_SHAPE_SETBACK        25		//---- feet ----
#define NUM_CURVE_POINTS          15
#define ANY_USE_CODE              2047

//---- standard field names ----

#define START_FIELD_NAMES         "START", "STARTTIME", "START_TIME", "DEPART"
#define END_FIELD_NAMES           "END", "ENDTIME", "END_TIME", "ARRIVE"
#define DURATION_FIELD_NAMES      "DURATION", "ACTIVITY", "ACTTIME", "ACT_TIME"
#define DEPART_FIELD_NAMES        "DEPART", "START", "STARTTIME", "START_TIME"
#define ARRIVE_FIELD_NAMES        "ARRIVE", "END", "ENDTIME", "END_TIME"
#define ROUTE_FIELD_NAMES         "ROUTE", "LINE", "ROUTE_ID", "LINE_ID"
#define X_FIELD_NAMES             "X_COORD", "EASTING", "X"
#define Y_FIELD_NAMES             "Y_COORD", "NORTHING", "Y"
#define Z_FIELD_NAMES             "Z_COORD", "ELEVATION", "ALTITUDE"
#define IN_LINK_FIELD_NAMES       "LINK", "IN_LINK", "INLINK", "LINK_IN", "LINKIN"
#define OUT_LINK_FIELD_NAMES      "TO_LINK", "OUT_LINK", "OUTLINK", "LINK_OUT", "LINKOUT"
#define NUM_NEST_FIELD_NAMES      "NUM_NEST", "RECORDS", "NUM_REC", "COUNT", "MEMBERS"
#define SIGNAL_FIELD_NAMES        "SIGNAL", "CONTROLLER", "ID", "SIGNAL_ID"
#define HHOLD_FIELD_NAMES         "HHOLD", "HOUSEHOLD", "HH_ID", "HHID", "HH"
#define PERSON_FIELD_NAMES        "PERSON", "MEMBER", "PER_ID", "PERID", "PER"
#define TOUR_FIELD_NAMES          "TOUR", "GROUP"
#define TRIP_FIELD_NAMES          "TRIP", "TRP", "ACTIVITY"
#define PURPOSE_FIELD_NAMES       "PURPOSE", "TYPE", "PURP"
#define MODE_FIELD_NAMES          "MODE", "MOD", "M"
#define VEHICLE_FIELD_NAMES       "VEHICLE", "VEH", "VEH_ID", "VEHID", "V"
#define NUM_PASS_FIELD_NAMES      "NUM_PASS", "NPASS", "PASSENGERS"
#define CONSTRAINT_FIELD_NAMES    "CONSTRAINT", "LIMIT"
#define PRIORITY_FIELD_NAMES      "PRIORITY", "LEVEL"
#define ORIGIN_FIELD_NAMES        "ORIGIN", "ORG", "O", "I", "LOCATION"
#define DESTINATION_FIELD_NAMES   "DESTINATION", "DES", "D", "J"
#define PARKING_FIELD_NAMES       "PARKING", "LOT", "ID", "NUMBER", "LOT_ID"
#define PENALTY_FIELD_NAMES       "PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN"
#define PROBLEM_FIELD_NAMES       "PROBLEM", "STATUS"
#define PARTITION_FIELD_NAMES     "PARTITION", "PART", "GROUP"
#define TIME_SKIM_FIELD_NAMES     "TIME", "TOTALTIME", "TTIME"
#define WALK_SKIM_FIELD_NAMES     "WALK", "WALKTIME", "WALK_TIME"
#define DRIVE_SKIM_FIELD_NAMES    "DRIVE", "DRIVETIME", "DRIVE_TIME", "AUTO", "AUTO_TIME"
#define TRANSIT_SKIM_FIELD_NAMES  "TRANSIT", "TRANSITTIME", "TRANSIT_TIME", "BUS", "BUS_TIME"
#define WAIT_SKIM_FIELD_NAMES     "WAIT", "WAITTIME", "WAIT_TIME"
#define OTHER_SKIM_FIELD_NAMES    "OTHER", "OTHERTIME", "OTHER_TIME", "BIKE", "MAGIC_MOVE"
#define COST_SKIM_FIELD_NAMES     "COST", "TOLL", "FARE", "CENTS"
#define LENGTH_SKIM_FIELD_NAMES   "LENGTH", "DISTANCE", "LEN", "DIST"
#define IMP_SKIM_FIELD_NAMES      "IMPEDANCE", "IMPED", "IMP", "GCF"
#define FLOW_RATE_FIELD_NAMES     "FLOW_RATE", "FLOW", "AVG_VOLUME", "VOLUME", "COUNT"
#define TRAVEL_TIME_FIELD_NAMES   "TIME", "TTIME", "AVG_TIME", "SUM"
#define CONNECTION_FIELD_NAMES    "NCONNECT", "NUM_CONNECT", "NUM_TURNS", "NUM_MOVES", "NUM_NEST"
#define FLOW_OUT_FIELD_NAMES      "OUT_FLOW", "OUT_TURN", "OUTTURN", "TURN", "MOVEMENT"
#define TIME_OUT_FIELD_NAMES      "OUT_TIME", "OUTTIME", "OUT_DELAY", "PENALTY"
#define ZONE_FIELD_NAMES          "ZONE", "TAZ", "ZONENUM", "ZONE_ID", "Z"
#define AREA_TYPE_FIELD_NAMES     "AREA_TYPE", "AREATYPE", "AT", "AREA", "ATYPE"
#define NODE_FIELD_NAMES          "NODE", "N", "ID"
#define ANODE_FIELD_NAMES         "NODE_A", "A_NODE", "A", "NODEA", "ANODE"
#define BNODE_FIELD_NAMES         "NODE_B", "B_NODE", "B", "NODEB", "BNODE"

#define FILE_RANGE                "[project_directory]filename"
#define PARTITION_RANGE           "[project_directory]filename.*"
#define FORMAT_RANGE              "TEXT, BINARY, FIXED_COLUMN, COMMA_DELIMITED, SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, ARCVIEW, SQLITE3, VERSION3"
#define MATRIX_RANGE              "TPPLUS, TRANPLAN, TRANSCAD, BINARY, FIXED_COLUMN, COMMA_DELIMITED, SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, SQLITE3"
#define LENGTH_FORMAT_RANGE       "FEET, MILES, METERS, KILOMETERS"
#define TIME_FORMAT_RANGE         "SECONDS, MINUTES, HOURS, HOUR_CLOCK, DAY_TIME, TIME_CODE"
#define TIME_RANGE                "e.g., ALL, 0..97200 seconds, 0.0..27.0 hours, 0:00..27:00"
#define TIME_BREAK_RANGE          "e.g., NONE, 6:00, 10:00, 16:00"
#define MINUTE_RANGE              "0, 2..240 minutes"
#define SECOND_RANGE              "0.1..1800 seconds"
#define BOOL_RANGE                "TRUE/FALSE, YES/NO, 1/0, T/F, Y/N"
#define RANGE_RANGE               "e.g., 1, 2, 4..10, 100..200, 300"
#define NAME_RANGE                "e.g., LINE1, LINE2, LINE1..LINE10, AB..AB|"
#define SEED_RANGE                "0 = computer clock, > 0 = fixed"
#define EQUATION_RANGE            "BPR, 0.15, 4.0, 0.75"
#define COORDINATE_RANGE          "Min X, Min Y, Max X, Max Y"
#define FLOW_RANGE                "VEHICLES, CAR_EQUIV/PCE, PERSONS"
#define CONSTRAINT_RANGE          "NONE, START, ARRIVE, FIXED, DURATION, PASSENGER"
#define MODE_RANGE                "WALK, BIKE, DRIVE, RIDE, TRANSIT, PNR_OUT, PNR_IN, KNR_OUT, KNR_IN, TAXI, OTHER, HOV2, HOV3, HOV4"
#define USE_RANGE                 "ANY, WALK, BIKE, CAR, TRUCK, BUS, RAIL, SOV, HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, TAXI, RESTRICTED, NONE"
#define COMBINE_RANGE             "REPLACE_LINKS, SIMPLE_AVERAGE, WEIGHTED_AVERAGE, REPLACE_OR_AVERAGE"
#define SELECT_METHOD_RANGE       "RANDOM, PERCENT_DIFFERENCE, RELATIVE_GAP" 
#define LOCATION_METHOD_RANGE     "USER, RANDOM, CENTROID, DISTRIBUTED"
#define MATRIX_OD_RANGE           "DISTRICTS, ZONES, LOCATIONS"
#define PERF_FIELD_RANGE          "FLOW, TRAVEL_TIME, VC_RATIO, TIME_RATIO, SPEED, DELAY, DENSITY, MAX_DENSITY, QUEUE, MAX_QUEUE, CYCLE_FAILURE, VMT, VHT, CONGESTED_TIME, CONGESTED_VMT, CONGESTED_VHT"
#define ANODE_FIELD_RANGE         "NODE_A, A_NODE, A, NODEA, ANODE"
#define BNODE_FIELD_RANGE         "NODE_B, B_NODE, B, NODEB, BNODE"
#define EVENT_RANGE               "TRIP_START_TIME, TRIP_END_TIME, TRIP_DURATION, VEH_START_TIME, VEH_END_TIME, VEH_LOST_TIME, TRANSIT_WAIT, TRANSIT_BOARDING, TRANSIT_ALIGHTING"

enum Execution_Type {   //----- execution type ----
	EXECUTION_SERVICE, FILE_SERVICE, DATA_SERVICE, ROUTER_SERVICE, ROUTER_BASE, SIMULATOR_SERVICE, SIMULATOR_BASE
};

//---- Units ----

enum Measure_Type {     //---- units of measure options ----
	METRIC, ENGLISH
};

enum Units_Type {       //---- data units types ----
	//---- converted units ----
	NO_UNITS, SECONDS, MINUTES, HOURS, HOUR_CLOCK, DAY_TIME, TIME_CODE,			
	FPS, MPS, MPH, KPH, FEET, METERS, MILES, KILOMETERS, DEGREES, MILLION_DEG,
	FPS2, MPS2, MPHPS, KPHPS, SPP, RATIO, STOPPED, LANE_KM, LANE_MILE,
	IMP_METER, IMP_FOOT, CENT_KM, CENT_MI, VHT, VMT, VKT, EHT, EMT, EKT, PHT, PMT, PKT,
	//---- unconverted units ----
	NEST_COUNT, CENTS, DOLLARS_HR, IMPEDANCE, IMP_SECOND, IMP_CENT, VPH, VPHPL, PPV, 
	VEH_TYPE, FARE_ZONE, TIME_PERIOD, YEARS, PERCENT, VEHICLES, PCE, PERSONS, TRIPS,
	DISTRICT_OD, ZONE_OD, LOCATION_OD,
	//---- code strings ----
	FACILITY_CODE, USE_CODE, POCKET_CODE, LANE_RANGE_CODE, LANE_ID_CODE, RESTRICT_CODE, 
	CONNECT_CODE, PARKING_CODE, STOP_CODE, ID_CODE, CONTROL_CODE, SIGNAL_CODE, MOVEMENT_CODE, 
	DETECTOR_CODE, PROTECTION_CODE, TRANSIT_CODE, CLASS_CODE, FARE_ZONE_RANGE, 
	TRANSIT_CODE_RANGE, TIME_PERIOD_RANGE, CLASS_CODE_RANGE, VEH_USE_CODE, LOADING_CODE, 
	RELATE_CODE, GENDER_CODE, BOOL_CODE, MODE_CODE, CONSTRAINT_CODE, PRIORITY_CODE,
	PROBLEM_CODE, EVENT_CODE
};

//---- simulation scope ----

enum Simulation_Scope {       //----- simulation scope ----
	NO_SIMULATION, MACROSCOPIC, MESOSCOPIC, MICROSCOPIC 
};

//---- Control Key Codes ----

enum Level_Type {       //----- level types ----
	LEVEL0, LEVEL1, LEVEL2, LEVEL3, LEVEL4 
};
enum Option_Type {      //---- option types ----
	OPT_KEY, REQ_KEY, IGNORE_KEY 
};
enum Key_Type {         //---- key types ----
	END_KEY, INT_KEY, FLOAT_KEY, TEXT_KEY, BOOL_KEY, IN_KEY, OUT_KEY, PATH_KEY, TIME_KEY, LIST_KEY 
};
enum Help_Type {		//---- help types ----
	NO_HELP, REPORT_HELP, FORMAT_HELP
};

//---- Db_File -----

enum Format_Type {      //---- file format options ----
	DEFAULT_FORMAT, UNFORMATED, BINARY, FIXED_COLUMN, COMMA_DELIMITED, 
	SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, ARCVIEW, VERSION3, SQLITE3, 
	TPPLUS, TRANPLAN, TRANSCAD 
};
enum Access_Type {      //---- file access options ----
	READ, MODIFY, APPEND, CREATE 
};
enum Field_Type {       //---- field data type options ----
	DB_INTEGER, DB_UNSIGNED, DB_DOUBLE, DB_FIXED, DB_STRING, DB_CHAR, DB_TIME 
};
enum Nest_Type {        //---- field nest type options ----
	NO_NEST, NESTED, FLATTEN 
};
enum Status_Code {      //---- file error messages ----
	OK, DB_ERROR, MEMORY, RECORD_SIZE,                         //---- Db_Record ----
	NOT_OPEN, NULL_POINTER, FILE_IO, CLOSING, POSITIONING,     //---- Db_File ----
	FIELD_LIST, NO_FIELD, FIELD_BYTES, NULL_NAME, FIELD_UNITS, //---- Db_Base ----
	EXTENSION,                                                 //---- Ext_File ----
	PLAN_FIELDS,                                               //---- Plan_IO ----
	FILE_HEADER, RECORD_SYNTAX, FIELD_TYPE,                    //---- Csv_File ----
	OPEN_DEF, DEF_FORMAT, DEF_NEST,                            //---- Db_Header ----
	HEADER_MEMORY, INDEX_OPEN, HEADER_READ, HEADER_VERSION,    //---- Btree_Index ----
	NODE_MEMORY, NODE_READ, PATH_NAME, HEADER_WRITE, 
	TOO_MANY_FILES, NODE_WRITE,
	NO_TPPLUS, TPPLUS_LICENSE,                                   //---- TPPlus ----
	NO_TRANSCAD, TRANSCAD_LICENSE                               //---- TransCAD ----
};

//---- System File Service ----

enum System_File_Type {     //---- TRANSIMS standard file types ----
	//---- read network file types ----
	NODE, ZONE, SHAPE, LINK,
	POCKET, LANE_USE, CONNECTION, TURN_PENALTY,
	PARKING, LOCATION, ACCESS_LINK,
	SIGN, SIGNAL,
	PHASING_PLAN, TIMING_PLAN, DETECTOR,
	TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE,
	TRANSIT_SCHEDULE, TRANSIT_DRIVER, ROUTE_NODES,
	//---- read demand file types ----
	SELECTION, HOUSEHOLD, LINK_DELAY, PERFORMANCE, RIDERSHIP, 
	VEHICLE_TYPE, VEHICLE, TRIP, PROBLEM, PLAN, SKIM, EVENT, TRAVELER,
	//---- create network file types ----
	NEW_NODE, NEW_ZONE, NEW_SHAPE, NEW_LINK,
	NEW_POCKET, NEW_LANE_USE, NEW_CONNECTION, NEW_TURN_PENALTY,
	NEW_PARKING, NEW_LOCATION, NEW_ACCESS_LINK,
	NEW_SIGN, NEW_SIGNAL,
	NEW_PHASING_PLAN, NEW_TIMING_PLAN, NEW_DETECTOR,
	NEW_TRANSIT_STOP, NEW_TRANSIT_FARE, NEW_TRANSIT_ROUTE,
	NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER, NEW_ROUTE_NODES,
	//---- create demand file types ----
	NEW_SELECTION, NEW_HOUSEHOLD, NEW_LINK_DELAY, NEW_PERFORMANCE, NEW_RIDERSHIP, 
	NEW_VEHICLE_TYPE, NEW_VEHICLE, NEW_TRIP, NEW_PROBLEM, NEW_PLAN, NEW_SKIM, NEW_EVENT, NEW_TRAVELER,
	END_FILE
};
enum Direction_Type {   //---- link direction processing options ----
	LINK_SIGN, LINK_DIR, LINK_NODE 
};
enum Problem_Type {     //---- problem type options ----
	TOTAL_PROBLEM, PATH_PROBLEM, TIME_PROBLEM, ZERO_PROBLEM, 
	TYPE_PROBLEM, DIST_PROBLEM, MODE_PROBLEM, ACCESS_PROBLEM, WALK_PROBLEM, 
	WAIT_PROBLEM, LINK_PROBLEM, LOAD_PROBLEM, PARK_PROBLEM, BIKE_PROBLEM, 
	DEPARTURE_PROBLEM, ARRIVAL_PROBLEM, LINK_ACCESS_PROBLEM, CONNECT_PROBLEM, 
	PARKING_PROBLEM, MERGE_PROBLEM, LANE_PROBLEM, TURN_PROBLEM, POCKET_PROBLEM, 
	SPACING_PROBLEM, CONTROL_PROBLEM, USE_PROBLEM, STOP_PROBLEM, 
	LOCATION_PROBLEM, PASSENGER_PROBLEM, DURATION_PROBLEM, KISS_PROBLEM, 
	VEHICLE_PROBLEM, SORT_PROBLEM, WALK_LOC_PROBLEM, BIKE_LOC_PROBLEM,
	TRANSIT_LOC_PROBLEM, MATCH_PROBLEM, CONSTRAINT_PROBLEM, BOARDING_PROBLEM, DWELL_PROBLEM, 
	TRANSFER_PROBLEM, LOCAL_PROBLEM, TRACE_PROBLEM, PARK_USE_PROBLEM, MAX_PROBLEM
};

//---- Network Service ----

enum Facility_Type {    //---- link facility types ----
	FREEWAY = 1, EXPRESSWAY, PRINCIPAL, MAJOR, MINOR, COLLECTOR, LOCAL_THRU, LOCAL,
	FRONTAGE, RAMP, BRIDGE, TUNNEL, OTHER, 
	WALKWAY, BIKEWAY, BUSWAY, LIGHTRAIL, HEAVYRAIL, FERRY, EXTERNAL
};
enum Mode_Type {       //---- trip mode options ----
	WAIT_MODE, WALK_MODE, BIKE_MODE, DRIVE_MODE, RIDE_MODE, 
	TRANSIT_MODE, PNR_OUT_MODE, PNR_IN_MODE, KNR_OUT_MODE, KNR_IN_MODE,
	TAXI_MODE, OTHER_MODE, HOV2_MODE, HOV3_MODE, HOV4_MODE, MAX_MODE
};
//enum Mode_Type {        //---- trip mode options ----
//	NO_MODE, ALL_WALK, DRIVE_ALONE, TRANSIT, RAIL_TRANSIT, PNR_OUT, PNR_IN, 
//	BICYCLE, MAGIC_MOVE, SCHOOL_BUS, CARPOOL2, CARPOOL3, CARPOOL4, KNR_OUT, KNR_IN, MAX_MODE
//};
//enum Plan_Mode {
//	AUTO_MODE, TRANSIT_MODE, WALK_MODE, BIKE_MODE, ACTIVITY_MODE, OTHER_MODE, MAGIC_MODE, CARPOOL_MODE, DRIVER_MODE
//};
enum Use_Type {         //---- facility use restrictions ----
	ANY, WALK, BIKE, CAR, TRUCK, BUS, RAIL,
	SOV, HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, TAXI, RESTRICTED, NONE
};
enum Transit_Type {     //---- transit route/mode types ----
	NO_TRANSIT, LOCAL_BUS, EXPRESS_BUS, TROLLEY, STREETCAR, LRT, 
	RAPIDRAIL, REGIONRAIL, ANY_TRANSIT
};
enum Class_Type {       //---- transit fare types ----
	CASH, CARD, SPECIAL
};
enum Stop_Type {        //---- transit stop types ----
	NO_STOP, STOP, STATION, EXTLOAD, NEARSIDE, FARSIDE, MIDBLOCK, ON_ONLY, OFF_ONLY
};
enum Parking_Type {     //---- parking lot types ----
	STREET, LOT, BOUNDARY, PARKRIDE
};
enum Restrict_Type {    //---- use restriction overrides ----
	APPLY, LIMIT, PROHIBIT, REQUIRE
};
enum Pocket_Type {      //---- pocket lane types ----
	LEFT_TURN, RIGHT_TURN, LEFT_MERGE, RIGHT_MERGE, POCKET_LANE, AUX_LANE
};
enum Connect_Type {     //---- lane connection types ----
	NO_TYPE, THRU, R_SPLIT, L_SPLIT, R_MERGE, L_MERGE, RIGHT, LEFT, UTURN
};
enum Lane_Type {        //---- lane types ----
	MAIN_LANE, LEFT_POCKET, RIGHT_POCKET
};
enum ID_Type {         //---- trip start/end types ----
	NO_ID, LOCATION_ID, PARKING_ID, STOP_ID, NODE_ID, ACCESS_ID, DIR_ID, LINK_ID, ROUTE_ID, 
	ACCESS_AB, ACCESS_BA, LINK_AB, LINK_BA, USE_AB, USE_BA, USE_ID, FROM_ID, TO_ID
};
enum Control_Type {     //---- traffic control types ----
	NO_CONTROL, STOP_SIGN, ALL_STOP, YIELD_SIGN, SIGNAL_GROUP, BLANK_CONTROL 
};
enum Signal_Type {      //---- signal types ----
	TIMED, ACTUATED
};
enum Movement_Type {        //---- signal phasing plan types ----
	UNDEFINED, NB_TH, NB_RT, NB_LT, NB_UT, SB_TH, SB_RT, SB_LT, SB_UT,
	EB_TH, EB_RT, EB_LT, EB_UT, WB_TH, WB_RT, WB_LT, WB_UT
};
enum Protection_Type {  //---- signal phasing protection types ----
	PROTECTED, PERMITTED, STOP_PERMIT
};
enum Detector_Type {    //---- signal detector operations types ----
	PRESENCE, PASSAGE
};
enum Control_Status {	//---- control status types ----
	UNCONTROLLED, PROTECTED_GREEN, PERMITTED_GREEN, STOP_GREEN, YELLOW_LIGHT, RED_LIGHT, 
};
enum Shape_Code { 
	VIEW, LINE, ARC, CIRCLE, RECTANGLE, POLYGON, VECTOR, GTEXT, ARROW, DOT, MDOTS 
};

//---- Demand Service ----

enum Trip_Sort_Type {   //---- trip/plan file sort options ----
	UNKNOWN_SORT, TRAVELER_SORT, TIME_SORT
};
enum Plan_Method_Type {	//---- plan processing method options ----
	BUILD_PATH, RESKIM_PATH, REROUTE_PATH, COPY_PATH, PATH_FLOWS
};
enum Event_Type {       //---- trip time event types ----
	TRIP_START_EVENT, TRIP_END_EVENT, DURATION_EVENT, 
	VEH_START_EVENT, VEH_END_EVENT,  VEH_LOST_EVENT,
	TRANSIT_WAIT_EVENT, TRANSIT_ON_EVENT, TRANSIT_OFF_EVENT, MAX_EVENT
};
enum Constraint_Type {  //---- activity schedule constraint types ----
	NO_CONSTRAINT, FIXED_TIME, START_TIME, END_TIME, DURATION, PASSENGER 
};
enum Priority_Type {    //---- activity purpose priority types ----
	LOW, MEDIUM, HIGH, CRITICAL 
};
enum Time_Point_Type {  //---- diurnal time point types ----
	TRIP_START, TRIP_END, MID_TRIP
};
enum Distance_Type {    //---- distance calculation method types ----
	STRAIGHT, RIGHT_ANGLE, SIMPLE_AVERAGE, WEIGHTED_AVERAGE 
};
enum Matrix_Type {      //---- matrix types ----
	TRIP_TABLE, DATA_TABLE, SKIM_TABLE, TIME_TABLE 
};
enum Performance_Type { //---- link performance summary types ----
	FLOW_DATA, TTIME_DATA, VC_DATA, RATIO_DATA, SPEED_DATA,  
	DELAY_DATA, DENSITY_DATA, MAX_DENSITY_DATA,
	QUEUE_DATA, MAX_QUEUE_DATA, FAILURE_DATA, VMT_DATA, VHT_DATA,
	CONG_TIME_DATA, CONG_VMT_DATA, CONG_VHT_DATA
};
enum Rate_Type {       //---- emission rate types ----
	NO_RATE, 
	GRAMS_PER_KILOMETER, GRAMS_PER_MILE, GRAMS_PER_HOUR, 
	POUNDS_PER_KILOMETER, POUNDS_PER_MILE, POUNDS_PER_HOUR,
	JOULES_PER_KILOMETER, JOULES_PER_MILE, JOULES_PER_HOUR,
	BTUS_PER_KILOMETER, BTUS_PER_MILE, BTUS_PER_HOUR 
};
	
enum Equation_Type {   //---- volume-delay equation types ----
	BPR, BPR_PLUS, EXP, CONICAL, AKCELIK 
};

//---- Projection Service ----

enum Projection_Type { //---- projection types ----
	NO_PROJECTION, LATLONG, STATEPLANE, UTM 
};

//---- Vehicle Type Service ----

enum Loading_Type {    //---- vechicle loading types ----
	SERIAL, PARALLEL 
};

//---- Household Service ----

enum Relate_Type {     //---- relate types ----
	NO_RELATE, HEAD_HHOLD, SPOUSE, CHILD, FAMILY 
};

enum Gender_Type {     //---- gender types ----
	NO_SEX, MALE, FEMALE 
};

//---- Link Delay Combine ----

enum Combine_Type {		
	REPLACE_LINKS, SIMPLE_LINK_AVG, WEIGHTED_LINK_AVG, REPLACE_AVERAGE
};

//---- Selection Method ----

enum Sel_Method_Type {		
	RANDOM_METHOD, PERCENT_METHOD, GAP_METHOD
};

//---- Location Selection Method ----

enum Loc_Method_Type {
	USER_LOC, RANDOM_LOC, CENTROID_LOC, DISTRIB_LOC
};

//---- Signal Timing Method ----

enum Signal_Split_Type {
	LANE_SPLITS, CAPACITY_SPLITS, VOLUME_SPLITS, UPDATE_SPLITS 
};
#endif
