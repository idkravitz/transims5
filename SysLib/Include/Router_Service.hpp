//*********************************************************
//	Router_Service.hpp - path building service
//*********************************************************

#ifndef ROUTER_SERVICE_HPP
#define ROUTER_SERVICE_HPP

#include "Data_Service.hpp"
#include "Flow_Time_Service.hpp"
#include "User_Program.hpp"
#include "TypeDefs.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "Path_Parameters.hpp"
#include "Db_Header.hpp"
#include "Route_Stop_Data.hpp"
#include "Lot_XY_Data.hpp"
#include "One_To_Many.hpp"

#define MAX_PATHS	10

//---------------------------------------------------------
//	Router_Service - router service class definition
//---------------------------------------------------------

class SYSLIB_API Router_Service : public Data_Service, public Flow_Time_Service
{
	friend class Path_Builder;
	friend class Router_Base;

public:

	Router_Service (void);

	void Walk_Paths (bool flag)                   { walk_path_flag = flag; }
	void Bike_Paths (bool flag)                   { bike_path_flag = flag; }
	void Drive_Paths (bool flag)                  { drive_path_flag = flag; }
	void Transit_Paths (bool flag)                { transit_path_flag = flag; }
	void ParkRide_Paths (bool flag)               { parkride_path_flag = flag; }
	void KissRide_Paths (bool flag)               { kissride_path_flag = flag; }
	void Link_Delays (bool flag)                  { param.delay_flag = flag; }
	void Link_Flows (bool flag)                   { param.flow_flag = flag; }
	void Turn_Flows (bool flag)                   { param.turn_flow_flag = flag; }
	void Skim_Total_Time (bool flag)              { param.skim_total_time = flag; }

	bool Link_Flows (void)                        { return (param.flow_flag); }
	bool Turn_Flows (void)                        { return (param.turn_flow_flag); }
	
	void Set_Parameters (Path_Parameters &param, int type = 0);
	
	virtual bool Save_Plans (Plan_Ptr_Array *ptr) { delete ptr; return (false); }
	virtual bool Save_Skims (One_To_Many *ptr)    { delete ptr; return (false); }

	//---- routing parameter control keys ----

protected:
	enum Router_Service_Keys { 
		IMPEDANCE_SORT_METHOD = ROUTER_SERVICE_OFFSET, SAVE_ONLY_SKIMS, WALK_PATH_DETAILS, 
		IGNORE_VEHICLE_ID, LIMIT_PARKING_ACCESS, ADJUST_ACTIVITY_SCHEDULE, IGNORE_ACTIVITY_DURATIONS, 
		IGNORE_TIME_CONSTRAINTS, END_TIME_CONSTRAINT, IGNORE_ROUTING_PROBLEMS, 
		PERCENT_RANDOM_IMPEDANCE, TRAVELER_TYPE_SCRIPT, TRAVELER_PARAMETER_FILE, 
		WALK_SPEED, BICYCLE_SPEED, WALK_TIME_VALUES, BICYCLE_TIME_VALUES, 
		FIRST_WAIT_VALUES, TRANSFER_WAIT_VALUES, PARKING_TIME_VALUES, VEHICLE_TIME_VALUES, 
		DISTANCE_VALUES, COST_VALUES, FREEWAY_BIAS_FACTORS, EXPRESSWAY_BIAS_FACTORS, 
		LEFT_TURN_PENALTIES, RIGHT_TURN_PENALTIES, U_TURN_PENALTIES, 
		TRANSFER_PENALTIES, STOP_WAITING_PENALTIES, STATION_WAITING_PENALTIES,
		BUS_BIAS_FACTORS, BUS_BIAS_CONSTANTS, RAIL_BIAS_FACTORS, RAIL_BIAS_CONSTANTS, 
		MAX_WALK_DISTANCES, WALK_PENALTY_DISTANCES, WALK_PENALTY_FACTORS, 
		MAX_BICYCLE_DISTANCES, BIKE_PENALTY_DISTANCES, BIKE_PENALTY_FACTORS, 
		MAX_WAIT_TIMES, WAIT_PENALTY_TIMES, WAIT_PENALTY_FACTORS, MIN_WAIT_TIMES, 
		MAX_NUMBER_OF_TRANSFERS, MAX_PARK_RIDE_PERCENTS, MAX_KISS_RIDE_PERCENTS, 
		KISS_RIDE_TIME_FACTORS, KISS_RIDE_STOP_TYPES, MAX_KISS_RIDE_DROPOFF_WALK, 
		TRANSIT_PENALTY_FILE, PARKING_PENALTY_FILE, DEFAULT_PARKING_DURATION, 
		MAX_LEGS_PER_PATH, MAX_NUMBER_OF_PATHS, FARE_CLASS_DISTRIBUTION, 
		LOCAL_ACCESS_DISTANCE, LOCAL_FACILITY_TYPE, LOCAL_IMPEDANCE_FACTOR,
		MAX_CIRCUITY_RATIO,	MIN_CIRCUITY_DISTANCE, MAX_CIRCUITY_DISTANCE, MIN_DURATION_FACTORS
	};
	enum Router_Service_Reports { SCRIPT_REPORT = 1, STACK_REPORT };

	void Router_Service_Keys (int *keys = 0);

	virtual void Program_Control (void);
	virtual void Execute (void);
	virtual void Page_Header (void);

	virtual bool Get_Household_Data (Household_File &file, Household_Data &data, int partition = 0);

	bool Update_Parking (Trip_Data &trip);

	User_Program type_script;
	Int2_Map hhold_type;
	Db_File script_file;
	bool script_flag, hhfile_flag, select_flag, update_flag, thread_flag;

private:

	Double_List value_walk, value_bike, value_wait, value_xfer, value_time, value_dist, value_cost, value_park;
	Integer_List left_imped, right_imped, uturn_imped, xfer_imped, stop_imped, station_imped;
	Integer_List max_parkride, max_kissride, max_walk, walk_pen, max_bike, bike_pen;
	Integer_List max_xfers, rail_const, bus_const;
	Dtime_List max_wait, wait_pen, min_wait;
	Double_List freeway_fac, express_fac, kissride_fac, rail_bias, bus_bias, walk_fac, bike_fac, wait_fac;

	Db_Header park_pen_file;
	Db_Header stop_pen_file;

	//---- routing specific network data ----

	bool walk_path_flag, bike_path_flag, drive_path_flag, access_link_flag;
	bool transit_path_flag, parkride_path_flag, kissride_path_flag;

	Integers walk_list, bike_list;
	List_Array walk_link, bike_link;

	Integers link_location, next_location;
	Integers link_parking, next_parking;
	Integers link_stop, next_stop, stop_list;
	Route_Stop_Array route_stop_array;
	Lot_XY_Array park_ride, kiss_ride;
	Integers park_penalty;

	List_Array loc_access, park_access, stop_access, node_access, from_access, to_access;

	List_Array & next_access (bool from_flag)  { return ((from_flag) ? from_access : to_access); }

	Int2_Map ab_link;

	Path_Parameters param;
};
#endif
