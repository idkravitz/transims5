//*********************************************************
//	Static_Service.cpp - static program service
//*********************************************************

#include "Static_Service.hpp"

#include "Execution_Service.hpp"

#include <iostream>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_EXTENSIONS
#include <io.h>
#else
#include <sys/stat.h>
#endif

#ifdef BOOST_THREADS
boost::mutex  main_mutex;
#endif

bool Static_Service::metric_flag = true;
Units_Type Static_Service::time_format = DAY_TIME;
Format_Type Static_Service::default_format = TAB_DELIMITED;

const char * SPACES           = " ";
const char * COMMA_DELIMITERS = ",";
const char * SPACE_DELIMITERS = " \t";
const char * TAB_DELIMITERS   = "\t";
const char * CSV_DELIMITERS   = ", \t";

//---- time type codes ----

Static_Service::Code_Text Static_Service::time_codes [] = {
	{ SECONDS, "SECONDS" }, { MINUTES, "MINUTES" }, { HOURS, "HOURS" }, { HOUR_CLOCK, "HOUR_CLOCK" },
	{ DAY_TIME, "DAY_TIME" }, { TIME_CODE, "TIME_CODE" }, { DAY_TIME, "24_HOUR_CLOCK" }, { 0, 0 }
};

//---- length type codes ----

Static_Service::Code_Text Static_Service::length_codes [] = {
	{ FEET, "FEET" }, { METERS, "METERS" }, { MILES, "MILES" }, { KILOMETERS, "KILOMETERS" },
	{ FEET, "FT" }, { METERS, "M" }, { MILES, "MI" }, { KILOMETERS, "KM" }, { 0, 0 }
};

//---- units of measure codes ----

Static_Service::Code_Text Static_Service::measure_codes [] = {
	{ METRIC, "METRIC" }, { ENGLISH, "ENGLISH" }, { 0, 0 }
};

Static_Service::Code_Text  Static_Service::units_codes [] = {
	//---- converted units ----
	{ NO_UNITS, "NO" }, { SECONDS, "SECONDS" }, { MINUTES, "MINUTES" }, { HOURS, "HOURS" }, 
	{ HOUR_CLOCK, "HOUR_CLOCK" }, { DAY_TIME, "DAY_TIME" }, { DAY_TIME, "24_HOUR_CLOCK" },
	{ TIME_CODE, "TIME_CODE" }, { FPS, "FPS" }, { MPS, "MPS" }, { MPH, "MPH" }, { KPH, "KPH" },
	{ FEET, "FEET" }, { METERS, "METERS" }, { MILES, "MILES" }, { KILOMETERS, "KILOMETERS" },  
	{ DEGREES, "DEGREES" },  { MILLION_DEG, "MILLION_DEGREES" }, { MILLION_DEG, "MILLION DEGREES" }, 
	{ FPS2, "FPS/S" }, { MPS2, "MPS/S" }, { MPHPS, "MPH/S" }, { KPHPS, "KPH/S" }, 
	{ SPP, "SECONDS/PERSON" }, { RATIO, "RATIO" }, { STOPPED, "STOPPED_FLOW" }, 
	{ LANE_KM, "FLOW/LANE-KM" }, { LANE_MILE, "FLOW/LANE-MILE" }, 
	{ IMP_METER, "IMPEDANCE/METER" }, { IMP_METER, "UNITS/METER" },
	{ IMP_FOOT, "IMPEDANCE/FOOT" }, { IMP_FOOT, "UNITS/FOOT" },
	{ CENT_KM, "CENTS/KILOMETER" }, { CENT_KM, "CENTS/KM" },
	{ CENT_MI, "CENTS/MILE" }, { CENT_MI, "CENTS/MI" },
	{ VHT, "VEH_HOURS" }, { VMT, "VEH_MILES" }, { VKT, "VEH_KILOMETERS" },
	{ EHT, "PCE_HOURS" }, { EMT, "PCE_MILES" }, { EKT, "PCE_KILOMETERS" },
	{ PHT, "PER_HOURS" }, { PMT, "PER_MILES" }, { PKT, "PER_KILOMETERS" },
	//---- unconverted units ----	
	{ NEST_COUNT, "NEST_COUNT" }, { CENTS, "CENTS" }, { DOLLARS_HR, "DOLLARS/HOUR" }, 
	{ IMPEDANCE, "IMPEDANCE" }, { IMPEDANCE, "UNITS" },	{ IMP_SECOND, "IMPEDANCE/SECOND" }, 
	{ IMP_SECOND, "UNITS/SECOND" },	{ IMP_CENT, "IMPEDANCE/CENT" }, { IMP_CENT, "UNITS/CENT" },
	{ VPH, "VPH" }, { VPHPL, "VPHPL" }, { PPV, "PERSONS/VEHICLE" }, { VEH_TYPE, "VEHICLE_TYPE" }, 
	{ FARE_ZONE, "FARE_ZONE" }, { TIME_PERIOD, "TIME_PERIOD" }, { YEARS, "YEARS" }, { PERCENT, "PERCENT" },
	{ VEHICLES, "VEHICLES" }, { PCE, "CAR_EQUIV" }, { PCE, "PCE" }, { PERSONS, "PERSONS" },	{ TRIPS, "TRIPS" }, 
	{ DISTRICT_OD, "ZONE_GROUPS" }, { DISTRICT_OD, "DISTRICTS" }, { ZONE_OD, "ZONES" }, { LOCATION_OD, "LOCATIONS" },
	//---- code strings ----	
	{ FACILITY_CODE, "FACILITY_TYPE" }, { USE_CODE, "USE_TYPE" }, { POCKET_CODE, "POCKET_TYPE" }, 
	{ LANE_RANGE_CODE, "LANE_RANGE_TYPE" }, { LANE_ID_CODE, "LANE_ID_TYPE" }, { RESTRICT_CODE, "RESTRICTION_TYPE"}, 
	{ CONNECT_CODE, "CONNECTION_TYPE" }, { PARKING_CODE, "PARKING_TYPE" }, 
	{ STOP_CODE, "STOP_TYPE" }, { ID_CODE, "ID_TYPE" }, { CONTROL_CODE, "CONTROL_TYPE" }, 
	{ SIGNAL_CODE, "SIGNAL_TYPE" }, { MOVEMENT_CODE, "MOVEMENT_TYPE" }, { DETECTOR_CODE, "DETECTOR_TYPE" },
	{ PROTECTION_CODE, "PROTECTION_TYPE" }, { TRANSIT_CODE, "TRANSIT_TYPE" }, 
	{ CLASS_CODE, "FARE_CLASS" }, { FARE_ZONE_RANGE, "FARE_ZONE_RANGE" }, 
	{ TRANSIT_CODE_RANGE, "TRANSIT_TYPE_RANGE" }, { TIME_PERIOD_RANGE, "TIME_PERIOD_RANGE" },
	{ CLASS_CODE_RANGE, "FARE_CLASS_RANGE" }, { VEH_USE_CODE, "VEH_USE_TYPE" }, 
	{ LOADING_CODE, "PROCESSING_RATE" }, { RELATE_CODE, "RELATE_TYPE" }, { GENDER_CODE, "GENDER_TYPE" }, 
	{ BOOL_CODE, "TRUE/FALSE" }, { MODE_CODE, "MODE_TYPE" }, { CONSTRAINT_CODE, "CONSTRAINT_TYPE" }, 
	{ PRIORITY_CODE, "PRIORITY_TYPE" }, { PROBLEM_CODE, "PROBLEM_TYPE" }, { EVENT_CODE, "EVENT_TYPE" },
	{ 0, 0 }
};

//---- format type codes ----

Static_Service::Code_Text Static_Service::format_codes [] = {
	{ UNFORMATED, "TEXT" }, { UNFORMATED, "UNFORMATED" }, { BINARY, "BINARY" },
	{ FIXED_COLUMN, "FIXED_COLUMN" }, { COMMA_DELIMITED, "COMMA_DELIMITED" },
	{ SPACE_DELIMITED, "SPACE_DELIMITED"}, { TAB_DELIMITED,  "TAB_DELIMITED" },
	{ CSV_DELIMITED, "CSV_DELIMITED" }, { DBASE, "DBASE" }, { ARCVIEW, "ARCVIEW" },
	{ VERSION3, "VERSION3" }, { SQLITE3, "SQLITE3" }, 
	{ TPPLUS, "TPPLUS" }, { TRANPLAN, "TRANPLAN" }, { TRANSCAD, "TRANSCAD"}, { 0, 0 }
};

//---- field type codes ----

Static_Service::Code_Text Static_Service::field_codes [] = {
	{ DB_INTEGER, "INTEGER" }, { DB_UNSIGNED, "UNSIGNED" }, { DB_DOUBLE, "DOUBLE" }, 
	{ DB_FIXED, "FIXED" }, { DB_STRING, "STRING" }, { DB_CHAR, "CHAR" }, { DB_TIME, "TIME" }, 
	{ 0, 0 }
};

//---- nest type codes ----

Static_Service::Code_Text Static_Service::nest_codes [] = {
	{ NO_NEST, "" }, { NESTED, "NESTED" }, { FLATTEN, "FLATTEN" }, 
	{ 0, 0 }
};

//---- facility type codes ----

Static_Service::Code_Text Static_Service::facility_codes [] = {
	{ FREEWAY, "FREEWAY" }, { EXPRESSWAY, "EXPRESSWAY" }, { PRINCIPAL, "PRINCIPAL" },
	{ MAJOR, "MAJOR" }, { MINOR, "MINOR" }, { COLLECTOR, "COLLECTOR" }, 
	{ LOCAL_THRU, "LOCAL_THRU" }, { LOCAL, "LOCAL" }, { FRONTAGE, "FRONTAGE" }, { RAMP, "RAMP" }, 
	{ BRIDGE, "BRIDGE" }, { TUNNEL, "TUNNEL" },	{ OTHER, "OTHER" }, { EXTERNAL, "EXTERNAL" }, 
	{ EXTERNAL, "ZONECONN" }, { LOCAL_THRU, "THRU" }, { LOCAL_THRU, "LOCAL_ACCESS" }, 
	{ EXPRESSWAY, "XPRESSWAY" }, { MAJOR, "PRIARTER" }, { MINOR, "SECARTER" }, 
	{ WALKWAY, "WALKWAY" }, { BIKEWAY, "BIKEWAY" },	{ BUSWAY, "BUSWAY" }, 
	{ LIGHTRAIL, "LIGHTRAIL" }, { HEAVYRAIL, "HEAVYRAIL" }, { FERRY, "FERRY" },
	{ 0, 0 }
};

//---- travel mode codes ----

Static_Service::Code_Text Static_Service::mode_codes [] = {
	{ WAIT_MODE, "WAIT" }, { WALK_MODE, "WALK" }, { BIKE_MODE, "BIKE" }, { DRIVE_MODE, "DRIVE" }, 
	{ RIDE_MODE, "RIDE" }, { TRANSIT_MODE, "TRANSIT" }, { PNR_OUT_MODE,"PNR_OUT" },
	{ PNR_IN_MODE, "PNR_IN" }, { KNR_OUT_MODE, "KNR_OUT" }, { KNR_IN_MODE,"KNR_IN" }, 
	{ TAXI_MODE, "TAXI" }, { OTHER_MODE, "OTHER" }, { HOV2_MODE, "HOV2" }, { HOV3_MODE, "HOV3" }, 
	{ HOV4_MODE, "HOV4" }, { WAIT_MODE, "NONE" }, { WALK_MODE, "ALL_WALK" }, { BIKE_MODE, "BICYCLE" }, 
	{ DRIVE_MODE, "DRIVE_ALONE" }, { RIDE_MODE, "PASSENGER" }, { TRANSIT_MODE, "BUS" }, 
	{ PNR_OUT_MODE, "PARKRIDE" }, { PNR_IN_MODE, "RIDEPARK" }, { KNR_OUT_MODE, "KISSRIDE" }, 
	{ KNR_IN_MODE, "RIDEKISS" }, { TAXI_MODE, "PICKUP" }, { OTHER_MODE, "MAGIC_MOVE" }, 
	{ HOV2_MODE, "CARPOOL2" }, { HOV3_MODE, "CARPOOL3" }, { HOV4_MODE,"CARPOOL4" }, 
	{ DRIVE_MODE, "AUTO_DRIVER" }, { RIDE_MODE, "AUTO_PASS" }, { TRANSIT_MODE, "RAIL" }, 
	{ OTHER_MODE, "SCHOOL_BUS" }, { HOV4_MODE, "HOV4+" }, { 0, 0 }
};

//---- parking type codes ----

Static_Service::Code_Text Static_Service::parking_codes [] = {
	{ STREET, "STREET" }, { LOT, "LOT", }, { BOUNDARY, "BOUNDARY" }, { PARKRIDE, "PARKRIDE" }, 
	{ STREET, "PRSTR" }, { STREET, "HISTR" }, { LOT, "DRVWY" }, { BOUNDARY, "BNDRY" },
	{ PARKRIDE,"PARKR" }, { 0, 0 }
};

//---- lane use restriction codes ----

Static_Service::Code_Text Static_Service::restrict_codes [] = {
	{ APPLY, "APPLY" }, { LIMIT, "LIMIT" }, { PROHIBIT, "PROHIBIT" }, { REQUIRE, "REQUIRE" }, 
	{ APPLY, "A" }, { LIMIT, "L" }, { PROHIBIT, "P" }, { REQUIRE, "R" }, 
	{ APPLY, "" }, { LIMIT, "O" }, { PROHIBIT, "N" }, { 0, 0 }
};

//---- pocket lane codes ----

Static_Service::Code_Text Static_Service::pocket_codes [] = {
	{ LEFT_TURN, "LEFT_TURN" }, { RIGHT_TURN, "RIGHT_TURN" }, 
	{ LEFT_MERGE, "LEFT_MERGE" }, { RIGHT_MERGE, "RIGHT_MERGE" }, 
	{ POCKET_LANE, "POCKET" }, { AUX_LANE, "AUXILIARY" },
	{ LEFT_TURN, "LEFT" }, { RIGHT_TURN, "RIGHT" }, 
	{ LEFT_MERGE, "L_MERGE" }, { RIGHT_MERGE, "R_MERGE" }, 
	{ LEFT_TURN, "LT" }, { RIGHT_TURN, "RT" }, { LEFT_MERGE, "LM" }, { RIGHT_MERGE, "RM" },
	{ POCKET_LANE, "PL" }, { AUX_LANE, "AX" }, { LEFT_TURN, "T" }, { RIGHT_MERGE, "M" }, 
	{ POCKET_LANE, "P" },  { AUX_LANE, "A" }, { LEFT_TURN, "TURN" }, { RIGHT_MERGE, "MERGE" }, 
	{ LEFT_TURN, "" }, { 0, 0 }
};

//---- link connection codes ----

Static_Service::Code_Text Static_Service::connect_codes [] = {
	{ NO_TYPE, "" }, { THRU, "THRU" }, { LEFT, "LEFT" }, { RIGHT, "RIGHT" }, { UTURN, "UTURN" },
	{ R_MERGE, "R_MERGE" }, { R_SPLIT, "R_SPLIT" }, { L_MERGE, "L_MERGE" }, { L_SPLIT, "L_SPLIT" },
	{ R_MERGE, "MERGE" }, { R_SPLIT, "DIVERGE" }, { NO_TYPE, "0" }, { THRU, "T" }, { LEFT, "L" },
	{ RIGHT, "R" }, { UTURN, "U" }, { R_MERGE, "RM" }, { R_SPLIT, "RS" }, { L_MERGE, "LM" },
	{ L_SPLIT, "LS" }, { R_MERGE, "M" }, { R_SPLIT, "D" }, { 0, 0 }
};


//---- transit mode codes ----

Static_Service::Code_Text Static_Service::transit_codes [] = {
	{ NO_TRANSIT, "NONE" }, { NO_TRANSIT, "NO_MODE" }, { NO_TRANSIT, "N/A" }, { NO_TRANSIT, "" },
	{ LOCAL_BUS, "BUS" }, { LOCAL_BUS, "LOCAL_BUS" }, { EXPRESS_BUS, "EXPRESS" }, 
	{ EXPRESS_BUS, "EXPRESS_BUS" }, { TROLLEY, "TROLLEY" }, { STREETCAR, "STREETCAR" }, 
	{ LRT, "LIGHTRAIL" }, { RAPIDRAIL, "RAPIDRAIL" }, { REGIONRAIL, "REGIONRAIL" }, 
	{ ANY_TRANSIT, "ANY_MODE" }, { ANY_TRANSIT, "ANY" }, { 0, 0 }
};

//---- transit class codes ----

Static_Service::Code_Text Static_Service::class_codes [] = {
	{ CASH, "CASH" }, { CARD, "CARD" }, { SPECIAL, "SPECIAL" }, { 0, 0 }
};

//---- transit stop codes ----

Static_Service::Code_Text Static_Service::stop_codes [] = {
	{ NO_STOP, "NO_STOP" }, { STOP, "STOP" }, { STATION, "STATION" }, { EXTLOAD, "EXTERNAL" }, 
	{ NEARSIDE, "NEARSIDE" }, { FARSIDE, "FARSIDE" }, { MIDBLOCK, "MIDBLOCK" }, 
	{ ON_ONLY, "ON_ONLY"}, { OFF_ONLY, "OFF_ONLY" }, { ON_ONLY, "BOARD" }, { OFF_ONLY, "ALIGHT" }, 
	{ NO_STOP, "" }, { NO_STOP, "NON_STOP" }, { NO_STOP, "NO" },	{ 0, 0 }
};

//---- ID type codes ----

Static_Service::Code_Text Static_Service::id_codes [] = {
	{ NO_ID, "" }, { LOCATION_ID, "LOCATION" }, { PARKING_ID, "PARKING" }, { STOP_ID, "STOP" }, 
	{ NODE_ID, "NODE" }, { ACCESS_ID, "ACCESS" }, { DIR_ID, "LINK_DIR" }, { LINK_ID, "LINK" }, 
	{ ROUTE_ID, "ROUTE" }, { ACCESS_AB, "ACCESS_AB" }, { ACCESS_BA, "ACCESS_BA" },
	{ LINK_AB, "LINK_AB" }, { LINK_BA, "LINK_BA" }, { USE_AB, "USE_AB" }, { USE_BA, "USE_BA" },
	{ USE_ID, "LANE_USE"}, { FROM_ID, "ORIGIN" } , { TO_ID, "DESTINATION" },
	{ ACCESS_AB, "ACC_AB" }, { ACCESS_BA, "ACC_BA" },
	{ LINK_AB, "AB" }, { LINK_BA, "BA" }, { USE_AB, "LANE_AB" }, { USE_BA, "LANE_BA" },
	{ LINK_AB, "LINK0" }, { LINK_BA, "LINK1" }, { USE_AB, "USE0" }, { USE_BA, "USE1" },
	{ LOCATION_ID, "ACTIVITY" }, { STOP_ID, "TRANSIT" }, { 0, 0 }
};

//---- traffic control codes ----

Static_Service::Code_Text  Static_Service::control_codes [] = {
	{ NO_CONTROL, "NONE" }, { STOP_SIGN, "STOP" }, { ALL_STOP, "ALL_STOP" }, { YIELD_SIGN, "YIELD" }, { SIGNAL_GROUP, "SIGNAL" }, 
	{ NO_CONTROL, "N" }, { STOP_SIGN, "S" }, { ALL_STOP, "A" }, { YIELD_SIGN, "Y" }, { SIGNAL_GROUP, "GROUP" },
	{ STOP_SIGN, "TWO_WAY" }, { ALL_STOP, "FOUR_WAY" }, { ALL_STOP, "ALL_WAY" }, { SIGNAL_GROUP, "SIGNAL_GROUP" },
	{ STOP_SIGN, "LOW_STOP" }, { STOP_SIGN, "LOW" }, { ALL_STOP, "ALL" }, { BLANK_CONTROL, "" }, { 0, 0 }
};

//---- signal codes ----

Static_Service::Code_Text Static_Service::signal_codes [] = {
	{ TIMED, "TIMED" }, { ACTUATED, "ACTUATED" }, { TIMED, "T" }, { ACTUATED, "A" }, 
	{ TIMED, "FIXED" }, { ACTUATED, "" }, { 0, 0 }
};

//---- movement codes ----

Static_Service::Code_Text  Static_Service::movement_codes [] = {
	{ UNDEFINED, "" }, 
	{ NB_TH, "NB_THRU" }, { NB_RT, "NB_RIGHT" }, { NB_LT, "NB_LEFT" }, { NB_UT, "NB_UTURN" },
	{ NB_TH, "NB_TH" }, { NB_RT, "NB_RT" }, { NB_LT, "NB_LT" }, { NB_UT, "NB_UT" },
	{ SB_TH, "SB_THRU" }, { SB_RT, "SB_RIGHT" }, { SB_LT, "SB_LEFT" }, { SB_UT, "SB_UTURN" },
	{ SB_TH, "SB_TH" }, { SB_RT, "SB_RT" }, { SB_LT, "SB_LT" }, { SB_UT, "SB_UT" },
	{ EB_TH, "EB_THRU" }, { EB_RT, "EB_RIGHT" }, { EB_LT, "EB_LEFT" }, { EB_UT, "EB_UTURN" },
	{ EB_TH, "EB_TH" }, { EB_RT, "EB_RT" }, { EB_LT, "EB_LT" }, { EB_UT, "EB_UT" },
	{ WB_TH, "WB_THRU" }, { WB_RT, "WB_RIGHT" }, { WB_LT, "WB_LEFT" }, { WB_UT, "WB_UTURN" },
	{ WB_TH, "WB_TH" }, { WB_RT, "WB_RT" }, { WB_LT, "WB_LT" }, { WB_UT, "WB_UT" },
	{ 0, 0 }
};

//---- protection codes ----

Static_Service::Code_Text  Static_Service::protection_codes [] = {
	{ PROTECTED, "PROTECTED" }, { PERMITTED, "PERMITTED" }, { STOP_PERMIT, "STOP_PERMIT" }, 
	{ PROTECTED, "P" }, { PERMITTED, "U" }, { STOP_PERMIT, "S" },{ PERMITTED, "" }, { 0, 0 }
};

//---- detector codes ----

Static_Service::Code_Text  Static_Service::detector_codes [] = {
	{ PRESENCE, "PRESENCE" }, { PASSAGE, "PASSAGE" }, { 0, 0 }
};

//---- use restriction codes ----

Static_Service::Code_Text  Static_Service::use_codes [] = {
	{ ANY, "ANY" }, { WALK, "WALK" }, { BIKE, "BICYCLE" }, { CAR, "AUTO" }, { TRUCK, "TRUCK" }, 
	{ BUS, "BUS" }, { RAIL, "RAIL" }, { SOV, "SOV" }, { HOV2, "HOV2" }, { HOV3, "HOV3" }, 
	{ HOV4, "HOV4" }, { LIGHTTRUCK, "LIGHTTRUCK" }, { HEAVYTRUCK, "HEAVYTRUCK" }, { RESTRICTED, "RESTRICTED" }, 
	{ NONE, "NONE" }, { CAR, "CAR" }, { BIKE, "BIKE" }, { TAXI, "TAXI" }, { RAIL, "TROLLEY" }, 
	{ RAIL, "STREETCAR" }, { RAIL, "LIGHTRAIL" }, { RAIL, "RAPIDRAIL" }, { RAIL, "REGIONRAIL" }, { 0, 0 }
};

//---- system file codes ----

Static_Service::Code_Text  Static_Service::system_file_codes [] = {
	{ NODE, "Node" }, { ZONE, "Zone" }, { SHAPE, "Shape" }, { LINK, "Link" }, { POCKET, "Pocket" }, 
	{ LANE_USE, "Lane Use" },  { CONNECTION, "Connection" }, { TURN_PENALTY,"Turn" }, 
	{ PARKING, "Parking" }, { LOCATION, "Location" }, { ACCESS_LINK, "Access Link" }, { SIGN,"Sign" }, 
	{ SIGNAL, "Signal" }, { PHASING_PLAN,  "Phasing Plan" }, { TIMING_PLAN, "Timing Plan" }, 
	{ DETECTOR, "Detector" }, { TRANSIT_STOP, "Transit Stop" }, { TRANSIT_FARE, "Transit Fare" }, 
	{ TRANSIT_ROUTE, "Transit Route" }, { TRANSIT_SCHEDULE, "Transit Schedule" }, 
	{ TRANSIT_DRIVER,"Transit Driver" }, { ROUTE_NODES,"Route Nodes" }, 
	{ SELECTION, "Selection" }, { HOUSEHOLD, "Household" }, { LINK_DELAY, "Link Delay" },
	{ PERFORMANCE, "Performance" }, { RIDERSHIP,  "Ridership" }, { VEHICLE_TYPE, "Vehicle Type" }, 
	{ VEHICLE, "Vehicle" }, { TRIP, "Trip" }, { PROBLEM, "Problem" }, { PLAN, "Plan" }, { SKIM, "Skim" }, 
	{ EVENT, "Event" }, { 0, 0 }
};

//---- problem type codes ----

Static_Service::Code_Text  Static_Service::problem_codes [] = {
	{ TOTAL_PROBLEM, "Total" }, { PATH_PROBLEM, "Path Building" }, { TIME_PROBLEM, "Time Schedule", }, 
	{ ZERO_PROBLEM, "Zero Length" }, { TYPE_PROBLEM, "Vehicle Type" }, { DIST_PROBLEM, "Path Circuity" }, 
	{ MODE_PROBLEM, "Travel Mode" }, { ACCESS_PROBLEM, "Vehicle Access" }, { WALK_PROBLEM, "Walk Distance" }, 
	{ WAIT_PROBLEM, "Wait Time" }, { LINK_PROBLEM, "Walk Access" }, { LOAD_PROBLEM, "Load Time" }, 
	{ PARK_PROBLEM, "Park-&-Ride Lot" }, { BIKE_PROBLEM, "Bike Distance" }, 
	{ DEPARTURE_PROBLEM, "Departure Time" }, { ARRIVAL_PROBLEM, "Arrival Time" }, 
	{ LINK_ACCESS_PROBLEM, "Link Access" }, { CONNECT_PROBLEM, "Link Connection" }, 
	{ PARKING_PROBLEM, "Parking Access" }, { MERGE_PROBLEM, "Lane Merging" }, 
	{ LANE_PROBLEM, "Lane Changing" }, { TURN_PROBLEM, "Turning Speed" }, { POCKET_PROBLEM, "Pocket Merge" }, 
	{ SPACING_PROBLEM, "Vehicle Spacing" }, { CONTROL_PROBLEM, "Traffic Control" }, 
	{ USE_PROBLEM, "Access Restriction" }, { STOP_PROBLEM, "Transit Stop" }, 
	{ LOCATION_PROBLEM,  "Activity Location" }, { PASSENGER_PROBLEM, "Vehicle Passenger" }, 
	{ DURATION_PROBLEM, "Activity Duration" }, { KISS_PROBLEM, "Kiss-&-Ride Lot", }, 
	{ VEHICLE_PROBLEM, "Vehicle ID" }, { SORT_PROBLEM, "Data Sort" }, { WALK_LOC_PROBLEM, "Walk Location" }, 
	{ BIKE_LOC_PROBLEM, "Bike Location" }, { TRANSIT_LOC_PROBLEM, "Transit Location" }, 
	{ MATCH_PROBLEM,  "Person Match" }, { CONSTRAINT_PROBLEM, "Schedule Constraint" }, 
	{ BOARDING_PROBLEM, "Transit Capacity" }, { DWELL_PROBLEM, "Transit Dwell" }, 
	{ TRANSFER_PROBLEM, "Number of Transfers" }, { LOCAL_PROBLEM, "Local Facility" }, 
	{ TRACE_PROBLEM, "Path Tracing" }, { PARK_USE_PROBLEM, "Parking Restriction" }, { 0, 0 }
};

//---- event type codes ----

Static_Service::Code_Text  Static_Service::event_codes [] = {
	{ TRIP_START_EVENT, "TRIP_START_TIME" }, { TRIP_END_EVENT, "TRIP_END_TIME" }, { DURATION_EVENT, "TRIP_DURATION" },
	{ TRIP_START_EVENT, "START_TIME" }, { TRIP_END_EVENT, "END_TIME" }, { DURATION_EVENT, "RUN_TIME" }, 
	{ VEH_START_EVENT, "VEH_START_TIME" }, { VEH_END_EVENT, "VEH_END_TIME" }, { VEH_LOST_EVENT, "VEH_LOST_TIME" },
	{ VEH_START_EVENT, "DEPARTURE_TIME" }, { VEH_END_EVENT, "ARRIVAL_TIME" }, { VEH_LOST_EVENT, "LOST_TIME" },
	{ VEH_START_EVENT, "VEHICLE_START" }, { VEH_END_EVENT, "VEHICLE_END" }, { VEH_LOST_EVENT, "VEHICLE_LOST" },	
	{ TRANSIT_WAIT_EVENT, "TRANSIT_WAIT" }, { TRANSIT_ON_EVENT, "TRANSIT_BOARDING" }, { TRANSIT_OFF_EVENT, "TRANSIT_ALIGHTING" },
	{ TRANSIT_WAIT_EVENT, "START_WAITING" }, { TRANSIT_ON_EVENT, "TRANSIT_ON" }, { TRANSIT_OFF_EVENT, "TRANSIT_OFF" },
	{ TRANSIT_WAIT_EVENT, "WAITING" }, { TRANSIT_ON_EVENT, "BOARDING" }, { TRANSIT_OFF_EVENT, "ALIGHTING" },
	{ VEH_LOST_EVENT, "LOST_VEHICLE" }, { 0, 0 } 
};

//---- constraint type codes ----

Static_Service::Code_Text  Static_Service::constraint_codes [] = {
	{ NO_CONSTRAINT, "NONE" }, { FIXED_TIME, "FIXED" }, { START_TIME, "START" }, { END_TIME, "ARRIVE" }, 
	{ DURATION, "DURATION" }, { PASSENGER, "PASSENGER" }, { NO_CONSTRAINT, "NO_CONSTRAINT" }, 
	{ FIXED_TIME, "FIXED_TIME" }, { START_TIME, "START_TIME" }, { END_TIME, "ARRIVAL_TIME" },
	{ END_TIME, "END_TIME" }, { END_TIME, "END" }, { END_TIME, "ARRIVAL" }, { 0, 0 }
};

//---- priority type codes ----

Static_Service::Code_Text  Static_Service::priority_codes [] = {
	{ LOW, "LOW" }, { MEDIUM, "MEDIUM" }, { HIGH, "HIGH" }, { CRITICAL, "CRITICAL" }, { 0, 0 }
};

//---- time point type codes ----

Static_Service::Code_Text  Static_Service::time_point_codes [] = {
	{ TRIP_START, "TRIP_START" }, { TRIP_END, "TRIP_END" }, { MID_TRIP, "MID_TRIP" }, 
	{ TRIP_START, "START" }, { TRIP_END, "END" }, { MID_TRIP, "MID-TRIP" },
	{ TRIP_START, "START_TIME" }, { TRIP_END, "END_TIME" }, { MID_TRIP, "MID_TIME" }, { 0, 0 }
};

//---- distance type codes ----

Static_Service::Code_Text  Static_Service::distance_codes [] = {
	{ STRAIGHT, "STRAIGHT_LINE" }, { RIGHT_ANGLE, "RIGHT_ANGLE" }, { SIMPLE_AVERAGE, "SIMPLE_AVERAGE" }, 
	{ WEIGHTED_AVERAGE, "WEIGHTED_AVERAGE" }, { 0, 0 }
};

//---- matrix type codes ----

Static_Service::Code_Text  Static_Service::matrix_codes [] = {
	{ TRIP_TABLE, "TRIP_TABLE" }, { DATA_TABLE, "DATA_TABLE" }, { SKIM_TABLE, "SKIM_TABLE" }, { TIME_TABLE, "TIME_SKIM" }, 
	{ TRIP_TABLE, "TRIP" }, { DATA_TABLE, "DATA" }, { SKIM_TABLE, "SKIM" }, { TIME_TABLE, "TIME" }, 
	{ TRIP_TABLE, "TABLE" }, { DATA_TABLE, "FACTOR" }, { TIME_TABLE, "TIME_TABLE" }, 
	{ 0, 0 }
};

//---- flow type codes ----

Static_Service::Code_Text  Static_Service::flow_codes [] = {
	{ VEHICLES, "VEHICLES" }, { PCE, "CAR_EQUIV" }, { PERSONS, "PERSONS" }, 
	{ PCE, "PCE" }, { 0, 0 }
};

//---- performance type codes ----

Static_Service::Code_Text  Static_Service::performance_codes [] = {
	{ FLOW_DATA, "FLOW" }, { TTIME_DATA, "TRAVEL_TIME" }, { VC_DATA, "VC_RATIO" }, { RATIO_DATA,"TIME_RATIO" }, 
	{ SPEED_DATA, "SPEED" }, { DELAY_DATA, "DELAY" }, { DENSITY_DATA, "DENSITY" }, { MAX_DENSITY_DATA, "MAX_DENSITY" }, 
	{ QUEUE_DATA, "QUEUE" }, { MAX_QUEUE_DATA, "MAX_QUEUE" }, { FAILURE_DATA, "CYCLE_FAILURE" }, 
	{ VMT_DATA, "VEH_METERS" }, { VHT_DATA, "VEH_SECONDS" }, { CONG_TIME_DATA, "CONGESTED_TIME" },
	{ CONG_VMT_DATA, "CONGESTED_VMT" }, { CONG_VHT_DATA, "CONGESTED_VHT" }, { FLOW_DATA, "VOLUME" },
	{ DENSITY_DATA, "AVG_DENSITY" }, { DENSITY_DATA, "AVERAGE_DENSITY" }, { MAX_DENSITY_DATA, "MAXIMUM_DENSITY" }, 
	{ QUEUE_DATA, "AVG_QUEUE" }, { QUEUE_DATA, "AVERAGE_QUEUE" }, { MAX_QUEUE_DATA, "MAXIMUM_QUEUE" }, 
	{ VMT_DATA, "VEHICLE_METERS" }, { VHT_DATA, "VEHICLE_SECONDS" }, { VMT_DATA, "VMT" }, { VHT_DATA, "VHT" }, 
	{ VMT_DATA, "VEH_MILES" }, { VHT_DATA, "VEH_HOURS" }, { 0, 0 }
};

//---- emission rate type codes ----

Static_Service::Code_Text  Static_Service::rate_codes [] = {
	{ GRAMS_PER_KILOMETER, "GRAMS_PER_KILOMETER" }, { GRAMS_PER_MILE, "GRAMS_PER_MILE" }, 
	{ GRAMS_PER_HOUR, "GRAMS_PER_HOUR" }, { POUNDS_PER_KILOMETER, "POUNDS_PER_KILOMETER" }, 
	{ POUNDS_PER_MILE, "POUNDS_PER_MILE" }, { POUNDS_PER_HOUR, "POUNDS_PER_HOUR" }, 
	{ JOULES_PER_KILOMETER, "JOULES_PER_KILOMETER" }, { JOULES_PER_MILE,  "JOULES_PER_MILE" }, 
	{ JOULES_PER_HOUR, "JOULES_PER_HOUR" }, { BTUS_PER_KILOMETER, "BTUS_PER_KILOMETER" }, 
	{ BTUS_PER_MILE, "BTUS_PER_MILE" }, { BTUS_PER_HOUR,"BTUS_PER_HOUR" }, { 0 , 0 }
};

//---- equation type codes ----

Static_Service::Code_Text  Static_Service::equation_codes [] = {
	{ BPR, "BPR" }, { BPR_PLUS, "BPR_PLUS" }, { EXP, "EXPONENTIAL" }, { CONICAL, "CONICAL" },
	{ AKCELIK, "AKCELIK" }, { BPR_PLUS, "BPR+" }, { EXP, "EXP" }, { CONICAL, "CON" }, { 0 , 0 }
};

//---- projection type codes ----

Static_Service::Code_Text  Static_Service::projection_codes [] = {
	{ NO_PROJECTION, "NO_PROJECTION" }, { LATLONG, "LATLONG" }, { STATEPLANE, "STATEPLANE" }, 
	{ UTM, "UTM" }, { 0, 0 }
};

//---- coordinate type codes ----

Static_Service::Code_Text  Static_Service::coordinate_codes [] = {
	{ NO_UNITS, "NO_UNITS" }, { FEET, "FEET" }, { METERS, "METERS" }, 
	{ MILES, "MILES" }, { KILOMETERS, "KILOMETERS" }, { DEGREES, "DEGREES" }, 
	{ MILLION_DEG, "MILLION DEGREES" }, { MILLION_DEG, "MILLION_DEGREES" }, { 0, 0 }
};

//---- loading type codes ----

Static_Service::Code_Text  Static_Service::loading_codes [] = {
	{ SERIAL, "SERIAL" }, { PARALLEL, "PARALLEL" }, { SERIAL, "" }, { 0, 0 }
};

//---- relate type codes ----

Static_Service::Code_Text  Static_Service::relate_codes [] = {
	{ NO_RELATE, "NO" }, { NO_RELATE, "" }, { NO_RELATE, "0" }, { NO_RELATE, "N" }, 
	{ HEAD_HHOLD, "HEAD_HHOLD" }, { HEAD_HHOLD, "HEAD" }, { HEAD_HHOLD, "HHOLDER" }, 
	{ HEAD_HHOLD, "1" }, { HEAD_HHOLD, "H" }, { SPOUSE, "SPOUSE" }, { SPOUSE, "PARTNER" }, 
	{ SPOUSE, "2" }, { SPOUSE, "S" }, { CHILD, "CHILD" }, { CHILD, "3" }, { CHILD, "C" }, 
	{ FAMILY, "FAMILY" }, { FAMILY, "RELATIVE" }, { FAMILY, "ADULT" }, { FAMILY, "4" }, 
	{ FAMILY, "F" }, { 0, 0 }
};

//---- gender type codes ----

Static_Service::Code_Text  Static_Service::gender_codes [] = {
	{ MALE, "MALE" }, { FEMALE, "FEMALE" }, { MALE, "1" }, { FEMALE, "2" }, 
	{ MALE, "M" }, { FEMALE, "F" }, { NO_SEX, "0" }, { NO_SEX, "" }, { 0, 0 }
};

//---- combine method type codes ----

Static_Service::Code_Text  Static_Service::combine_codes [] = {
	{ REPLACE_LINKS, "REPLACE_LINKS" }, { SIMPLE_LINK_AVG, "SIMPLE_AVERAGE" }, { WEIGHTED_LINK_AVG, "WEIGHTED_AVERAGE" }, 
	{ REPLACE_AVERAGE, "REPLACE_OR_AVERAGE" }, { REPLACE_LINKS, "REPLACE" }, { SIMPLE_LINK_AVG, "AVERAGE" }, 
	{ WEIGHTED_LINK_AVG, "WEIGHTED" }, { SIMPLE_LINK_AVG, "SIMPLE" }, { REPLACE_AVERAGE, "REPLACE_AVERAGE" }, { 0, 0 }
};

//---- selection method type codes ----

Static_Service::Code_Text  Static_Service::select_method_codes [] = {
	{ RANDOM_METHOD, "RANDOM" }, { PERCENT_METHOD, "PERCENT_DIFFERENCE" }, { GAP_METHOD, "RELATIVE_GAP" }, 
	{ RANDOM_METHOD, "RANDOM_METHOD" }, { PERCENT_METHOD, "PERCENT_METHOD" }, { GAP_METHOD, "GAP_METHOD" }, 
	{ PERCENT_METHOD, "DIFFERENCE" }, { PERCENT_METHOD, "DIFFERENCE_METHOD" }, { GAP_METHOD, "GAP" }, { 0, 0 }
};

//---- location method type codes ----

Static_Service::Code_Text  Static_Service::loc_method_codes [] = {
	{ USER_LOC, "USER" }, { RANDOM_LOC, "RANDOM" }, { CENTROID_LOC, "CENTROID" }, { DISTRIB_LOC, "DISTRIBUTED" }, 
	{ USER_LOC, "USER_LOCATION" }, { RANDOM_LOC, "RANDOM_LOCATION" }, { CENTROID_LOC, "NEAR_CENTROID" }, 
	{ DISTRIB_LOC, "DISTRIBUTE" }, { CENTROID_LOC, "ZONE_CENTROID" }, { CENTROID_LOC, "NEAR_ZONE_CENTROID" }, 
	{ DISTRIB_LOC, "SPATIAL" }, { DISTRIB_LOC, "SPATIALLY_DISTRIBUTED" }, { USER_LOC, "NONE" }, { USER_LOC, ""}, { 0, 0 }
};

//---- signal split codes ----

Static_Service::Code_Text  Static_Service::signal_split_codes [] = {
	{ LANE_SPLITS, "LANE_SPLITS" }, { CAPACITY_SPLITS, "CAPACITY_SPLITS" }, { VOLUME_SPLITS, "VOLUME_SPLITS" }, 
	{ UPDATE_SPLITS, "UPDATE_SPLITS" }, { LANE_SPLITS, "LANES" }, { CAPACITY_SPLITS, "CAPACITY" }, 
	{ VOLUME_SPLITS, "VOLUME" }, { UPDATE_SPLITS, "UPDATE" }, { 0, 0 }
};

//---- bool codes ----

Static_Service::Code_Text  Static_Service::bool_codes [] = {
	{ 1, "TRUE" }, { 0, "FALSE" }, { 1, "T" }, { 0, "F" }, 
	{ 1, "YES" }, { 0, "NO" }, { 1, "Y" }, { 0, "N" }, 
	{ 1, "1" }, { 0, "0" }, {0, "" }, { 0, 0 }
};

//---- matrix od codes ----

Static_Service::Code_Text  Static_Service::matrix_od_codes [] = {
	{ DISTRICT_OD, "ZONE_GROUPS" }, { DISTRICT_OD, "DISTRICTS" }, { ZONE_OD, "ZONES" }, 
	{ LOCATION_OD, "LOCATIONS" }, { 0, 0 }
};

//---- trip sort type codes ----

Static_Service::Code_Text  Static_Service::trip_sort_codes [] = {
	{ UNKNOWN_SORT, "NO_SORT" }, { TRAVELER_SORT, "TRAVELER_SORT" }, { TIME_SORT, "TIME_SORT" }, 
	{ UNKNOWN_SORT, "UNKNOWN" }, { TRAVELER_SORT, "TRAVELER" }, { TIME_SORT, "TIME" }, 
	{ UNKNOWN_SORT, "UNKNOWN_SORT" }, { UNKNOWN_SORT, "NO" }, { UNKNOWN_SORT, "DO_NOT_SORT" }, 
	{ 0, 0 }
};

//---- Db_file status codes ----

Static_Service::Code_Text  Static_Service::db_file_codes [] = {
	{ OK, "OK" }, { DB_ERROR, "Software Error" }, { MEMORY, "Insufficient Memory" }, 
	{ RECORD_SIZE, "Insufficient Record Size" }, { NOT_OPEN, "Not Open" }, 
	{ NULL_POINTER, "Null Data Pointer" }, { FILE_IO, "I/O Error" }, { CLOSING, "Closing" },
	{ POSITIONING, "Positioning" }, { FIELD_LIST, "Field List" }, { NO_FIELD, "Field Not Found" },
	{ FIELD_BYTES, "Incompatible Field Size" }, { NULL_NAME, "Field Name is NULL" }, 
	{ FIELD_UNITS, "Field Type and Units are Incompatible" },
	{ EXTENSION, "Extension Number" }, { PLAN_FIELDS, "Token Fields don't Match" },
	{ FILE_HEADER, "Header" }, { RECORD_SYNTAX, "Record Syntax" }, { FIELD_TYPE, "Field Type" },
	{ OPEN_DEF, "Opening Field Definition File" }, { DEF_FORMAT, "Field Definition Format" }, 
	{ DEF_NEST, "Nested Definition Fields" }, { HEADER_MEMORY, "Insufficient Memory for Header Record" }, 
	{ INDEX_OPEN, "Opening" }, { HEADER_READ, "Reading Header" }, 
	{ HEADER_VERSION, "Incompatible Index Version" }, { NODE_MEMORY, "Insufficient Node Memory" },
	{ NODE_READ, "Reading Node Records" }, { PATH_NAME, "Original Data Path Name was Not Found" },
	{ HEADER_WRITE, "Writing Header" }, { TOO_MANY_FILES, "Too Many Data File Names" },
	{ NODE_WRITE, "Writing Node Records" }, { NO_TPPLUS, "TPPlus DLL Not Installed" }, 
	{ TPPLUS_LICENSE, "TPPlus License" }, { NO_TRANSCAD, "TransCAD DLL Not Installed" }, 
	{ TRANSCAD_LICENSE, "TransCAD License" }, { 0, 0 }
};

//---- trip mode map ----

Mode_Type  Static_Service::trip_mode_map [] = {
	WAIT_MODE, WALK_MODE, DRIVE_MODE, TRANSIT_MODE, TRANSIT_MODE,
	PNR_OUT_MODE, PNR_IN_MODE, BIKE_MODE, OTHER_MODE, OTHER_MODE,
	HOV2_MODE, HOV3_MODE, HOV4_MODE, KNR_OUT_MODE, KNR_IN_MODE
};

//---- use restriction codes ----

int Static_Service::use_mask [NONE+1] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

//---- vehicle type map ----

Use_Type Static_Service::veh_type_map [] = {
	ANY, CAR, TRUCK, CAR, BUS, BUS, RAIL, RAIL, RAIL, RAIL, RAIL,
	HOV2, HOV3, HOV4, LIGHTTRUCK, HEAVYTRUCK, RESTRICTED, NONE
}; 

//---- transit use map ----

Use_Type Static_Service::transit_use_map [] = {
	ANY, BUS, BUS, RAIL, RAIL, RAIL, RAIL, RAIL, ANY
}; 

//---------------------------------------------------------
//	Static_Service constructor
//---------------------------------------------------------

Static_Service::Static_Service (void)
{
	if (use_mask [0] == -1) {
		static const char *use_map [] = {
		//---WB1234LHBRXXN---
			"1111111111100",		//---- any ----
			"1000000000000",		//---- walk ----
			"0100000000000",		//---- bike ----
			"0011110000000",		//---- car ----
			"0000001100000",		//---- truck ----
			"0000000010000",		//---- bus ----
			"0000000001000",		//---- rail ----
			"0010000000000",		//---- sov ----
			"0001000000000",		//---- hov2 ----
			"0000100000000",		//---- hov3 ----
			"0000010000000",		//---- hov4 ----
			"0000001000000",		//---- lighttruck ----
			"0000000100000",		//---- heavytruck ----
			"0000000000100",		//---- taxi ----
			"0000000000010",		//---- restricted ----
			"0000000000001"			//---- none ----
		};

		//---- calculate the access mask values ----

		for (int i=0; i <= NONE; i++) {
			const char *str_ptr = use_map [i];

			use_mask [i] = 0;

			for (int j=0; *str_ptr != '\0'; j++, str_ptr++) {
				if (*str_ptr == '1') {
					use_mask [i] |= (1 << j);
				}
			}
		}
	}
}

//---------------------------------------------------------
//	local_time - safe local time function
//---------------------------------------------------------

struct tm * Static_Service::local_time (time_t *tim)
{
#ifdef _MSC_EXTENSIONS
	static struct tm dt;

	localtime_s (&dt, tim);
	return (&dt);
#else
	return (localtime (tim));
#endif
}

//---------------------------------------------------------
//	c_time - safe time string function
//---------------------------------------------------------

char * Static_Service::c_time (time_t *tim)
{
	static char buffer [40];
#ifdef _MSC_EXTENSIONS
	ctime_s (buffer, sizeof (buffer), tim);
	buffer [24] = '\0';
#else
	sprintf (buffer, "%.24s", ctime (tim));
#endif
	return (buffer);
}

//---------------------------------------------------------
//	Clean - remove comments and extra characters
//---------------------------------------------------------

char * Static_Service::Clean (char *text)
{
	if (text) {
		char *ch1, *ch2;
		bool quote, tick;

		ch1 = ch2 = text;
		quote = tick = false;

		//--- remove leading blanks ----

		while (*ch2 == ' ') ch2++;

		//---- remove comment statements ----

		while (*ch2 != '\0') {
			if (!tick && *ch2 == '"') {
				quote = !quote;
			} else if (!quote && *ch2 == '\'') {
				tick = !tick;
			} else if (!quote && !tick && 
				((*ch2 == '#' && (*(ch2+1) == '#' || *(ch2+1) == '-' || *(ch2+1) == '*')) ||
				(*ch2 == '/' && (*(ch2+1) == '/' || *(ch2+1) == '-' || *(ch2+1) == '*')) ||
				(*ch2 == ';' && (*(ch2+1) == ';' || *(ch2+1) == '-' || *(ch2+1) == '*')))) {
				*ch1 = '\0';
				break;
			}
			*ch1++ = *ch2++;
		}
		*ch1 = '\0';

		//---- remove white space ----

		while (--ch1 >= text) {
			if (*ch1 != ' ' && *ch1 != '\t' && *ch1 != '\n' && *ch1 != '\r' && *ch1 != '\f') break;
			*ch1 = '\0';
		}
	}
	return (text);
}

//---------------------------------------------------------
//	f_open - safe file open
//---------------------------------------------------------

FILE * Static_Service::f_open (string filename, const char *control)
{
#ifdef _MSC_EXTENSIONS
	FILE *file;

	if (fopen_s (&file, filename.c_str (), control) != 0) {
		return (0);
	} else {
		return (file);
	}
#else
	return (fopen (filename.c_str (), control));
#endif
}

//---------------------------------------------------------
//	get_env - safe environment variables
//---------------------------------------------------------

char * Static_Service::get_env (const char *var)
{
#ifdef _MSC_EXTENSIONS
   char * value;
   size_t size;

   getenv_s (&size, 0, 0, var);
   if (size <= 1) return (0);

   value = (char *) malloc (size * sizeof (char));

   getenv_s (&size, value, size, var);

   return (value);
#else
	return (getenv (var));
#endif
}

//---------------------------------------------------------
//	f_exist - safe file existence check
//---------------------------------------------------------

bool Static_Service::f_exist (string filename)
{
#ifdef _MSC_EXTENSIONS
	return (_access_s (filename.c_str (), 0) == 0);
#else
	struct stat fileinfo;
	return (stat (filename.c_str (), &fileinfo) == 0);
#endif
}

//---------------------------------------------------------
//	Type_Code
//---------------------------------------------------------

int Static_Service::Type_Code (String text, Code_Text *codes, const char *label, int error_code)
{
	if (text.empty ()) return (error_code);

	if (text [0] >= '0' && text [0] <= '9' && !text.Equals ("24_HOUR_CLOCK")) {
		int code = text.Integer ();

		for (int i=0; codes [i].text != 0; i++) {
			if (codes [i].code == code) {
				return (code);
			}
		}
	} else {
		for (int i=0; codes [i].text != 0; i++) {
			if (text.Equals (codes [i].text)) {
				return (codes [i].code);
			}
		}
	}
	if (exe->Send_Messages ()) {
		exe->Error (String ("Converting %s Type %s") % label % text);
	}
	return (error_code);
}

const char * Static_Service::Type_Code (int code, Code_Text *codes)
{
	for (int i=0; codes [i].text != 0; i++) {
		if (code == codes [i].code) {
			return (codes [i].text);
		}
	}
	return (0);
}

Mode_Type Static_Service::Trip_Mode_Map (int code)
{
	if (code > 0 && code <= 14) {
		return (trip_mode_map [code]);
	}
	if (exe->Send_Messages ()) {
		exe->Error (String ("Converting Trip Mode Type %d") % code);
	}
	return (WAIT_MODE);
}

//---------------------------------------------------------
//	Check_Nest_Code
//---------------------------------------------------------

bool Static_Service::Check_Nest_Code (String text)
{
	if (text.empty ()) return (false);

	for (int i=0; nest_codes [i].text != 0; i++) {
		if (text.Equals (nest_codes [i].text)) {
			return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Use_Code
//---------------------------------------------------------

int Static_Service::Use_Code (String text)
{
	int use, code;
	Strings types;
	Str_Itr itr;

	text.Parse (types, "|+/");
	use = 0;

	for (itr = types.begin (); itr != types.end (); itr++) {
		code = Veh_Use_Code (*itr);

		//---- check the return code -----

		if (code == 0 && !itr->Equals (use_codes [0].text)) {
			return (0);
		}
		use |= use_mask [code];
	}
	return (use);
}

//---------------------------------------------------------
//	Use_Code
//---------------------------------------------------------

string Static_Service::Use_Code (int code)
{
	bool first = true;
	string text;

	if (code == 0) {
		text = "ANY";
		return (text);
	}

	for (int i=0; i <= NONE; i++) {
		if ((code & use_mask [i]) == use_mask [i]) {
			code -= use_mask [i];

			if (!first) {
				text += "|";
			} else {
				first = false;
			}
			text += use_codes [i].text;
		}
	}
	if (text.empty () || code != 0) {
		if (exe->Send_Messages ()) {
			exe->Error (String ("Converting Use Code %d") % code);
		}
	}
	return (text);
}

//---------------------------------------------------------
//	Veh_Type_Map
//---------------------------------------------------------

Use_Type Static_Service::Veh_Type_Map (int code)
{
	if (code < 0 || code > 16) {
		return (ANY);
	} else {
		return (veh_type_map [code]);
	}
}

//---------------------------------------------------------
//	Map_Veh_Type
//---------------------------------------------------------

int Static_Service::Map_Veh_Type (Use_Type code)
{
	for (int i=0; veh_type_map [i] != NONE; i++) {
		if (veh_type_map [i] == code) {
			return (i);
		}
	}
	if (exe->Send_Messages ()) {
		exe->Error (String ("Converting Vehicle Type %d") % code);
	}
	return (0);
}

//---------------------------------------------------------
//	Transit_Use_Map
//---------------------------------------------------------

Use_Type Static_Service::Transit_Use_Map (Transit_Type code)
{
	return (transit_use_map [code]);
}

//---------------------------------------------------------
//	Performance_Units_Map
//---------------------------------------------------------

Units_Type Static_Service::Performance_Units_Map (Performance_Type code, Units_Type flow_type)
{
	switch (code) {
		case FLOW_DATA:
		case FAILURE_DATA:
			return (flow_type);
		case TTIME_DATA:
		case DELAY_DATA:
			return (SECONDS);
		case VC_DATA:
		case RATIO_DATA:
			return (RATIO);
		case SPEED_DATA:
			return (KPH);
		case DENSITY_DATA:
		case MAX_DENSITY_DATA:
			return (LANE_KM);
		case QUEUE_DATA:
		case MAX_QUEUE_DATA:
			return (STOPPED);
		case VMT_DATA:
		case CONG_VMT_DATA:
			if (flow_type == VEHICLES) {
				return (VKT);
			} else if (flow_type == PERSONS) {
				return (PKT);
			} else {
				return (EKT);
			}
		case VHT_DATA:
		case CONG_VHT_DATA:
			if (flow_type == VEHICLES) {
				return (VHT);
			} else if (flow_type == PERSONS) {
				return (PHT);
			} else {
				return (EHT);
			}
		default:
			return (NO_UNITS);
			break;
	}
}

//---------------------------------------------------------
//	Movement_Code
//---------------------------------------------------------

Movement_Type Static_Service::Movement_Code (int bearing, int turn)
{
	int point;
	Movement_Type type;

	if (bearing < 0) bearing += 360;

	point = (int) (4 * bearing / 360.0 + 0.5);

	switch (point) {
		default:	//---- north bound ----
			if (turn == RIGHT) {
				type = NB_RT;
			} else if (turn == LEFT) {
				type = NB_LT;
			} else if (turn == UTURN) {
				type = NB_UT;
			} else {
				type = NB_TH;
			}
			break;
		case 1:		//---- east bound ----
			if (turn == RIGHT) {
				type = EB_RT;
			} else if (turn == LEFT) {
				type = EB_LT;
			} else if (turn == UTURN) {
				type = EB_UT;
			} else {
				type = EB_TH;
			}
			break;
		case 2:		//---- south bound ----
			if (turn == RIGHT) {
				type = SB_RT;
			} else if (turn == LEFT) {
				type = SB_LT;
			} else if (turn == UTURN) {
				type = SB_UT;
			} else {
				type = SB_TH;
			}
			break;
		case 3:		//---- west bound ----
			if (turn == RIGHT) {
				type = WB_RT;
			} else if (turn == LEFT) {
				type = WB_LT;
			} else if (turn == UTURN) {
				type = WB_UT;
			} else {
				type = WB_TH;
			}
			break;
	}
	return (type);
}

//---------------------------------------------------------
//	Problem_Code
//---------------------------------------------------------

Problem_Type  Static_Service::Problem_Code (String text)
{
	int i, len;

	len = (int) text.length ();

	for (i=1; i < len; i++) {
		if (text [i] == '_') {
			text [i] = ' ';
			i++;
		} else if (text [i] >= 'A' && text [i] <= 'Z') {
			text [i] = text [i] - 'A' + 'a';
		}
	}
	return ((Problem_Type) Type_Code (text, problem_codes, "Problem"));
}

//---------------------------------------------------------
//	Lane_Range_Code
//---------------------------------------------------------

int Static_Service::Lane_Range_Code (String text)
{
	int code;

	if (text.Equals ("All") || text.empty ()) {
		code = 0;
	} else {
		String low, high;

		if (!text.Range (low, high)) {
			code = 0;
		} else {
			char ch;
			int code1, lane1, code2, lane2;

			code1 = lane1 = code2 = lane2 = 0;
			
			ch = low [0];
			if (ch < '0' || ch > '9') {
				if (ch == 'L') {
					code1 = LEFT_POCKET;
				} else if (ch == 'R') {
					code1 = RIGHT_POCKET;
				} else {
					code1 = MAIN_LANE;
				}
				ch = low [1];
				if (ch < '0' || ch > '9') {
					low.erase (0, 2);
				} else {
					low.erase (0, 1);
				}
			}
			lane1 = low.Integer ();

			ch = high [0];
			if (ch < '0' || ch > '9') {
				if (ch == 'L') {
					code2 = LEFT_POCKET;
				} else if (ch == 'R') {
					code2 = RIGHT_POCKET;
				} else {
					code2 = MAIN_LANE;
				}
				ch = high [1];
				if (ch < '0' || ch > '9') {
					high.erase (0, 2);
				} else {
					high.erase (0, 1);
				}
			}
			lane2 = high.Integer ();

			code = lane1 + (code1 << 6);

			if (code1 != code2 || lane1 != lane2) {
				code += (lane2 << 8) + (code2 << 14);
			}
		}
	}
	return (code);
}

//---------------------------------------------------------
//	Lane_Range_Code
//---------------------------------------------------------

string Static_Service::Lane_Range_Code (int code)
{
	String text, format;

	if (code == 0) {
		text = "ALL";
	} else {
		int lane1, lane2, code1, code2;

		lane1 = code & 0x3F;
		code1 = (code & 0x00C0) >> 6;
		lane2 = (code & 0x3F00) >> 8;
		code2 = code >> 14;

		if (code1 == LEFT_POCKET) {
			text += "L";
		} else if (code1 == RIGHT_POCKET) {
			text += "R";
		}
		text += format ("%d") % lane1;

		if (lane2 > 0) {
			text += "..";
			if (code2 == LEFT_POCKET) {
				text += "L";
			} else if (code2 == RIGHT_POCKET) {
				text += "R";
			}
			text += format ("%d") % lane2;
		}
	}
	return (text);
}

//---------------------------------------------------------
//	Lane_ID_Code
//---------------------------------------------------------

int Static_Service::Lane_ID_Code (String text)
{
	int lane_id, code, lane;
	char ch;
	
	ch = text [0];
	if (ch < '0' || ch > '9') {
		if (ch == 'L') {
			code = LEFT_POCKET;
		} else if (ch == 'R') {
			code = RIGHT_POCKET;
		} else {
			code = MAIN_LANE;
		}
		ch = text [1];
		if (ch < '0' || ch > '9') {
			text.erase (0, 2);
		} else {
			text.erase (0, 1);
		}
	} else {
		code = MAIN_LANE;
	}
	lane = text.Integer ();

	lane_id = lane + (code << 6);

	return (lane_id);
}

//---------------------------------------------------------
//	Lane_ID_Code
//---------------------------------------------------------

string Static_Service::Lane_ID_Code (int lane_id)
{
	String text;
	int lane, code;

	lane = lane_id & 0x3F;
	code = (lane_id & 0xC0) >> 6;

	if (code == LEFT_POCKET) {
		text = "L%d";
	} else if (code == RIGHT_POCKET) {
		text = "R%d";
	} else {
		text = "%d";
	}
	text % lane;

	return (text);
}

//---------------------------------------------------------
//	Parse_Units
//---------------------------------------------------------

Units_Type Static_Service::Parse_Units (string &text)
{
	string units;
	String key = text;

	if (!key.Split_Last (units)) return (NO_UNITS);

	char ch = units [0];

	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
		text = key;
		return (Units_Code (units));
	} else {
		return (NO_UNITS);
	}
}

//---------------------------------------------------------
//	Type_Range
//---------------------------------------------------------

bool Static_Service::Type_Range (String &range, Units_Type type, int &low, int &high)
{
	String str_low, str_high;

	low = high = 0;

	if (!range.Range (str_low, str_high)) return (false);

	switch (type) {
		case FACILITY_CODE:
			if (str_low.Equals ("ALL")) {
				low = FREEWAY;
				high = EXTERNAL;
			} else {
				low = Facility_Code (str_low); 
				high = Facility_Code (str_high);
			}
			break;
		case MODE_CODE:
			if (str_low.Equals ("ALL")) {
				low = WAIT_MODE;
				high = MAX_MODE - 1;
			} else {
				low = Mode_Code (str_low);
				high = Mode_Code (str_high);
			}
			break;
		default:
			return (false);
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Internal Units
//---------------------------------------------------------

int Static_Service::Internal_Units (int data, Units_Type *units) 
{
	if (*units >= NEST_COUNT) return (data);

	return (DTOI (Internal_Units ((double) data, units)));
}

int Static_Service::Internal_Units (int data, Units_Type units) 
{ 
	if (units >= NEST_COUNT) return (data);

	return (DTOI (Internal_Units ((double) data, units))); 
}

double Static_Service::Internal_Units (double data, Units_Type units)
{
	return (Internal_Units (data, &units));
}

double Static_Service::Internal_Units (double data, Units_Type *units)
{
	if (*units >= NEST_COUNT) return (data);

	switch (*units) {
		case FPS:
		case FPS2:
			if (Metric_Flag ()) {
				data /= METERTOFEET;
				*units = (*units == FPS) ? MPS : MPS2;
			}
			break;
		case MPS:
		case MPS2:
			if (!Metric_Flag ()) {
				data *= METERTOFEET;
				*units = (*units == MPS) ? FPS : FPS2;
			}
			break;
		case MPH:
		case MPHPS:
			if (Metric_Flag ()) {
				data *= MPHTOMPS;
				*units = (*units == MPH) ? KPH : KPHPS;
			} else {
				data *= MILETOFEET / 3600.0;
			}
			break;
		case KPH:
		case KPHPS:
			if (Metric_Flag ()) {
				data *= 1000.0 / 3600.0;
			} else {
				data *= METERTOFEET * 1000.0 / 3600.0;
				*units = (*units == KPH) ? MPH : MPHPS;
			}
			break;
		case FEET:
			if (Metric_Flag ()) {
				data /= METERTOFEET;
				*units = METERS;
			}
			break;
		case METERS:
			if (!Metric_Flag ()) {
				data *= METERTOFEET;
				*units = FEET;
			}
			break;
		case MILES:
			if (Metric_Flag ()) {
				data *= MILETOMETER;
				*units = KILOMETERS;
			} else {
				data *= MILETOFEET;
			}
			break;
		case KILOMETERS:
			if (Metric_Flag ()) {
				data *= 1000.0;
			} else {
				data *= METERTOFEET * 1000.0;
				*units = MILES;
			}
			break;
		case LANE_MILE:
			if (Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
				*units = LANE_KM;
			}
			break;
		case LANE_KM:
			if (!Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
				*units = LANE_MILE;
			}
			break;
		case SPP:	//---- seconds per passenger ----
			data *= 10.0;
			break;
		case RATIO:
			data *= 100.0;
			break;
		case IMP_METER:
			if (!Metric_Flag ()) {
				data /= METERTOFEET;
				*units = IMP_FOOT;
			}
			break;
		case IMP_FOOT:
			if (Metric_Flag ()) {
				data *= METERTOFEET;
				*units = IMP_METER;
			}
			break;
		case CENT_KM:
			if (!Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
				*units = CENT_MI;
			}
			break;
		case CENT_MI:
			if (Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
				*units = CENT_KM;
			}
			break;
		case VHT:
		case EHT:
		case PHT:
			data *= Dtime (1.0, HOURS);
			break;
		case VMT:
			if (Metric_Flag ()) {
				data *= MILETOMETER;
				*units = VKT;
			} else {
				data *= MILETOFEET;
			}
			break;
		case VKT:
			if (Metric_Flag ()) {
				data *= 1000.0;
			} else {
				data *= METERTOFEET * 1000.0;
				*units = VMT;
			}
			break;
		case EMT:
			if (Metric_Flag ()) {
				data *= MILETOMETER;
				*units = EKT;
			} else {
				data *= MILETOFEET;
			}
			break;
		case EKT:
			if (Metric_Flag ()) {
				data *= 1000.0;
			} else {
				data *= METERTOFEET * 1000.0;
				*units = EMT;
			}
			break;
		case PMT:
			if (Metric_Flag ()) {
				data *= MILETOMETER;
				*units = PKT;
			} else {
				data *= MILETOFEET;
			}
			break;
		case PKT:
			if (Metric_Flag ()) {
				data *= 1000.0;
			} else {
				data *= METERTOFEET * 1000.0;
				*units = PMT;
			}
			break;
		default:
			break;
	}
	return (data);
}

int Static_Service::Internal_Units (string &text, Units_Type units) 
{
	int code = -2;

	switch (units) {
		case FACILITY_CODE:
			code = Facility_Code (text); 
			break;
		case USE_CODE:
			if (text.empty ()) text = "ANY";
			code = Use_Code (text);
			break;
		case POCKET_CODE:
			code = Pocket_Code (text);
			break;
		case LANE_RANGE_CODE:
			code = Lane_Range_Code (text);
			break;
		case LANE_ID_CODE:
			code = Lane_ID_Code (text);
			break;
		case CONNECT_CODE:
			code = Connect_Code (text);
			break;
		case RESTRICT_CODE:
			code = Restrict_Code (text);
			break;
		case PARKING_CODE:
			code = Parking_Code (text);
			break;
		case STOP_CODE:
			code = Stop_Code (text);
			break;
		case ID_CODE:
			code = ID_Code (text);
			break;
		case CONTROL_CODE:
			code = Control_Code (text);
			break;
		case SIGNAL_CODE:
			code = Signal_Code (text);
			break;
		case MOVEMENT_CODE:
			code = Movement_Code (text);
			break;
		case DETECTOR_CODE:
			code = Detector_Code (text);
			break;
		case PROTECTION_CODE:
			code = Protection_Code (text);
			break;
		case TRANSIT_CODE:
			code = Transit_Code (text);
			break;
		case CLASS_CODE:
			code = Class_Code (text);
			break;
		case VEH_USE_CODE:
			code = Veh_Use_Code (text);
			break;
		case LOADING_CODE:
			code = Loading_Code (text);
			break;
		case RELATE_CODE:
			code = Relate_Code (text);
			break;
		case GENDER_CODE:
			code = Gender_Code (text);
			break;
		case BOOL_CODE:
			code = Bool_Code (text);
			break;
		case MODE_CODE:
			code = Mode_Code (text);
			break;
		case CONSTRAINT_CODE:
			code = Constraint_Code (text);
			break;
		case PRIORITY_CODE:
			code = Priority_Code (text);
			break;
		case PROBLEM_CODE:
			if (!text.empty ()) {
				code = Problem_Code (text);
			} else {
				code = 0;
			}
			break;
		case EVENT_CODE:
			code = Event_Code (text);
			break;
		case FARE_ZONE_RANGE:
		case TRANSIT_CODE_RANGE:
		case TIME_PERIOD_RANGE:
		case CLASS_CODE_RANGE:
			code = -1;
			break;
		default:
			break;
	}
	return (code); 
}

//---------------------------------------------------------
//	External Units
//---------------------------------------------------------

int Static_Service::External_Units (int data, Units_Type units)
{
	if (units >= NEST_COUNT) return (data);

	return (DTOI (External_Units ((double) data, units))); 
}

double Static_Service::External_Units (double data, Units_Type units) 
{
	if (units >= NEST_COUNT) return (data);

	switch (units) {
		case FPS:
		case FPS2:
			if (Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case MPS:
		case MPS2:
			if (!Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case MPH:
		case MPHPS:
			if (Metric_Flag ()) {
				data *= MPSTOMPH;
			} else {
				data /= MILETOFEET / 3600.0;
			}
			break;
		case KPH:
		case KPHPS:
			if (Metric_Flag ()) {
				data *= 3600.0 / 1000.0; 
			} else {
				data /= METERTOFEET * 1000.0 / 3600.0;
			}
			break;
		case FEET:
			if (Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case METERS:
			if (!Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case VMT:
		case EMT:
		case PMT:
		case MILES:
			if (Metric_Flag ()) {
				data /= MILETOMETER;
			} else {
				data /= MILETOFEET;
			}
			break;
		case VKT:
		case EKT:
		case PKT:
		case KILOMETERS:
			if (Metric_Flag ()) {
				data /= 1000.0;
			} else {
				data /= METERTOFEET * 1000.0;
			}
			break;
		case LANE_MILE:
			if (Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
			}
			break;
		case LANE_KM:
			if (!Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
			}
			break;
		case SPP:	//---- seconds per passenger ----
			data /= 10.0;
			break;
		case RATIO:
			data /= 100.0;
			break;
		case IMP_METER:
			if (!Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case IMP_FOOT:
			if (Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case CENT_KM:
			if (!Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
			}
			break;
		case CENT_MI:
			if (Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
			}
			break;
		case VHT:
		case EHT:
		case PHT:
			data /= Dtime (1.0, HOURS);
			break;
		default:
			break;
	}
	return (data);
}

void Static_Service::External_Units (int code, Units_Type units, String &result) 
{
	switch (units) {
		case FACILITY_CODE:
			result = Facility_Code ((Facility_Type) code); 
			break;
		case USE_CODE:
			result = Use_Code (code);
			break;
		case POCKET_CODE:
			result = Pocket_Code ((Pocket_Type) code);
			break;
		case LANE_RANGE_CODE:
			result = Lane_Range_Code (code);
			break;
		case LANE_ID_CODE:
			result = Lane_ID_Code (code);
			break;
		case CONNECT_CODE:
			result = Connect_Code ((Connect_Type) code);
			break;
		case RESTRICT_CODE:
			result = Restrict_Code ((Restrict_Type) code);
			break;
		case PARKING_CODE:
			result = Parking_Code ((Parking_Type) code);
			break;
		case STOP_CODE:
			result = Stop_Code ((Stop_Type) code);
			break;
		case ID_CODE:
			result = ID_Code ((ID_Type) code);
			break;
		case CONTROL_CODE:
			result = Control_Code ((Control_Type) code);
			break;
		case SIGNAL_CODE:
			result = Signal_Code ((Signal_Type) code);
			break;
		case MOVEMENT_CODE:
			result = Movement_Code ((Movement_Type) code);
			break;
		case DETECTOR_CODE:
			result = Detector_Code ((Detector_Type) code);
			break;
		case PROTECTION_CODE:
			result = Protection_Code ((Protection_Type) code);
			break;
		case TRANSIT_CODE:
			result = Transit_Code ((Transit_Type) code);
			break;
		case CLASS_CODE:
			result = Class_Code ((Class_Type) code);
			break;
		case VEH_USE_CODE:
			result = Veh_Use_Code ((Use_Type) code);
			break;
		case LOADING_CODE:
			result = Loading_Code ((Loading_Type) code);
			break;
		case RELATE_CODE:
			result = Relate_Code ((Relate_Type) code);
			break;
		case GENDER_CODE:
			result = Gender_Code ((Gender_Type) code);
			break;
		case BOOL_CODE:
			result = Bool_Code (code);
			break;
		case MODE_CODE:
			result = Mode_Code ((Mode_Type) code);
			break;
		case CONSTRAINT_CODE:
			result = Constraint_Code ((Constraint_Type) code);
			break;
		case PRIORITY_CODE:
			result = Priority_Code ((Priority_Type) code);
			break;
		case PROBLEM_CODE:
			if (code > 0) {
				result = Problem_Code ((Problem_Type) code);
			} else {
				result.clear ();
			}
			break;
		case EVENT_CODE:
			result = Event_Code ((Event_Type) code);
			break;
		case FARE_ZONE_RANGE:
		case TRANSIT_CODE_RANGE:
		case TIME_PERIOD_RANGE:
		case CLASS_CODE_RANGE:
			result (code);
			break;
		default:
			result.clear ();
			break;
	}
}

double Static_Service::Convert_Units (double data, Units_Type units) 
{
	if (units >= NEST_COUNT) return (data);

	switch (units) {
		case SECONDS:
		case MINUTES:
		case HOURS:
			data /= (double) Dtime (1, units);
			break;
		case FPS:
		case FPS2:
			if (Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case MPS:
		case MPS2:
			if (!Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case MPH:
		case MPHPS:
			if (Metric_Flag ()) {
				data *= MPSTOMPH;
			} else {
				data /= MILETOFEET / 3600.0;
			}
			break;
		case KPH:
		case KPHPS:
			if (Metric_Flag ()) {
				data *= 3600.0 / 1000.0; 
			} else {
				data /= METERTOFEET * 1000.0 / 3600.0;
			}
			break;
		case FEET:
			if (Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case METERS:
			if (!Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case VMT:
		case EMT:
		case PMT:
		case MILES:
			if (Metric_Flag ()) {
				data /= MILETOMETER;
			} else {
				data /= MILETOFEET;
			}
			break;
		case VKT:
		case EKT:
		case PKT:
		case KILOMETERS:
			if (Metric_Flag ()) {
				data /= 1000.0;
			} else {
				data /= METERTOFEET * 1000.0;
			}
			break;
		case LANE_MILE:
			if (Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
			}
			break;
		case LANE_KM:
			if (!Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
			}
			break;
		case SPP:	//---- seconds per passenger ----
			data /= 10.0;
			break;
		case RATIO:
			data /= 100.0;
			break;
		case IMP_METER:
			if (!Metric_Flag ()) {
				data *= METERTOFEET;
			}
			break;
		case IMP_FOOT:
			if (Metric_Flag ()) {
				data /= METERTOFEET;
			}
			break;
		case CENT_KM:
			if (!Metric_Flag ()) {
				data *= 1000.0 / MILETOMETER;
			}
			break;
		case CENT_MI:
			if (Metric_Flag ()) {
				data *= MILETOMETER / 1000.0;
			}
			break;
		case VHT:
		case EHT:
		case PHT:
			data /= Dtime (1.0, HOURS);
			break;
		default:
			break;
	}
	return (data);
}
