//*********************************************************
//	Path_Parameters.hpp - path building parameters
//*********************************************************

#ifndef PATH_PARAMETERS_HPP
#define PATH_PARAMETETS_HPP

#include "System_Defines.hpp"
#include "Dtime.hpp"
#include "Random.hpp"

typedef struct {
    Mode_Type mode;                         //---- travel mode code ----
    Use_Type  use;                          //---- use type code ----
	int       traveler_type;                //---- traveler type code ----
	int       veh_type;                     //---- vehicle type code ----

	bool      sort_method;                  //---- use the sorted impedance path building method ----
	bool      one_to_many;                  //---- save from/to one location to/from many locations ----
	bool      skim_only;                    //---- save only the path summary to the plan record ----
	bool      skim_total_time;              //---- save only total travel time in the path skim ----
	bool      walk_detail;                  //---- save detailed walk paths to the plan record ----
	bool      grade_flag;                   //---- link file includes grade data ----

	bool      adjust_schedule;              //---- adjust activity schedule -----
	bool      ignore_duration;              //---- ignore activity duration ----
	bool      ignore_time;                  //---- ignore time schedule constraints ----
	Dtime     end_time;                     //---- maximum trip end time schedule variance in rounded seconds ----

	bool      ignore_veh_flag;              //---- ignore vehicle IDs and locations ----
	bool      ignore_errors;                //---- ignore path building problems ----

	bool      limit_access;                 //---- limit parking access to access link connections ----
	double    random_imped;                 //---- maximum percent random impedance variance ----
	Random    random;                       //---- random number generator ----
	bool      delay_flag;                   //---- use time period travel times -----
	bool      turn_delay_flag;              //---- use time period turn delays ----
	bool      flow_flag;                    //---- save flow data by time period ----
	bool      turn_flow_flag;               //---- save turn flow data by time period ----

	//---- rounded impedance units ----

	double    value_walk;                    //---- per second of walking ----
	double    value_bike;                    //---- per second of biking ----
	double    value_wait;                    //---- per second of first wait time ----
	double    value_xfer;                    //---- per second of transfer wait time ----
	double    value_park;                    //---- per second of parking time ----
	double    value_time;                    //---- per second of in-vehicle time ----
	double    value_dist;                    //---- per meter|foot of vehicle driving ----
	double    value_cost;                    //---- per cent of travel cost ----
	double    freeway_fac;                   //---- factor freeway travel time impedance ----
	double    express_fac;                   //---- factor expressway travel time impedance ----

	int       left_imped;                    //---- per left turn movement ----
	int       right_imped;                   //---- per right turn movement ----
	int       uturn_imped;                   //---- per u-turn movement ----
	int       xfer_imped;                    //---- per transit transfer ----
	int       stop_imped;                    //---- per boarding at a transit stop ----
	int       station_imped;                 //---- per boarding at a transit station ----
	double    rail_bias;                     //---- perception factor for rail in-vehicle time ----
	int       rail_const;                    //---- per rail boarding ----
	double    bus_bias;                      //---- perception factor for bus in-vehicle time -----
	int       bus_const;                     //---- per bus boarding ----

	//---- path building constraints ----

	int       max_parkride;                  //---- maximum percentage of total trip time as auto driver ----
	int       max_kissride;                  //---- maximum percentage of total trip time as auto passenger ----
	double    kissride_fac;                  //---- factor kiss-&-ride auto time to account for driver impedance ----
	int       max_walk;                      //---- maximum cumulative rounded meters|feet of walking ----
	int       walk_pen;                      //---- rounded meters|feet where walk penalty starts ----
	double    walk_fac;                      //---- walk penalty factor ----
	int       max_bike;                      //---- maximum cumulative rounded meters|feet of biking ----
	int       bike_pen;                      //---- rounded meters|feet where bike penalty starts ----
	double    bike_fac;                      //---- bike penalty factor ----
	Dtime     max_wait;                      //---- maximum rounded seconds of waiting per boarding ----
	Dtime     wait_pen;                      //---- rounded seconds where wait penalty starts ----
	double    wait_fac;                      //---- wait penalty factor ----
	Dtime     min_wait;                      //---- minimum rounded seconds of waiting per boarding ----
	int       max_xfers;                     //---- maximum number of transfers per trip ----
	int       max_paths;                     //---- maximum number of path options per trip ----

	bool      stop_pen_flag;                 //---- transit penalty file flag ----
	bool      park_pen_flag;                 //---- parking penalty file flag ----

	bool      park_hours_flag;               //---- calculate parking cost based on activity duration ----
	Dtime     parking_duration;              //---- default parking duration in rounded seconds ----

	bool      tod_flag;                      //---- network has time of day changes ----
	bool      local_flag;                    //---- implement local facility type restrictions ----
	int       local_type;                    //---- facility type that start local restrictions ----
	int       local_distance;                //---- meters|feet from the trip ends for local accessibility ----
	double    local_factor;                  //---- impedance factor for local facilities in line-haul area ----
	bool      distance_flag;                 //---- circuity-based processing flag ----
	double    max_ratio;                     //---- maximum ratio of path distance to straight line distance ----
	int       min_distance;                  //---- minimum distance (meters|feet) where circuity is considered ----
	int       max_distance;                  //---- maximum distance (meters|feet) where circuity is considered ----

	double    walk_speed;                    //---- walk speed in meters|feet per second ----
	double    bike_speed;                    //---- bike speed in meters|feet per second ----

	int       leg_check;                     //---- maximum number of legs in a transit trip ----
	bool      fare_class_flag;               //---- implement fare class probability ----
	double    fare_class [SPECIAL+1];        //---- distribution of transit passengers by fare class -----

	int       kissride_walk;                 //---- maximum walk between drop-off lot and transit stop ----
	bool      kissride_type [EXTLOAD+1];     //---- transit stop types where kiss-&-ride is permitted ----

	double    duration_factor [CRITICAL+1];  //---- minimum duration factor by trip priority level ----

} Path_Parameters;

#endif
