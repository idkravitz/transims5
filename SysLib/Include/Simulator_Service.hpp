//*********************************************************
//	Simulator_Service.hpp - simulation service
//*********************************************************

#ifndef SIMULATOR_SERVICE_HPP
#define SIMULATOR_SERVICE_HPP

#include "Router_Service.hpp"
#include "Select_Service.hpp"
#include "Barrier.hpp"
#include "Sim_Parameters.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Traveler.hpp"
#include "Sim_Plan_Data.hpp"
#include "Sim_Vehicle.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Connection.hpp"
#include "Sim_Park_Data.hpp"
#include "Sim_Stop_Data.hpp"
#include "Sim_Signal_Data.hpp"
#include "Travel_Step.hpp"
#include "Work_Step.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Simulator_Service - simulation service class definition
//---------------------------------------------------------

class SYSLIB_API Simulator_Service : public Router_Service, public Select_Service
{
	friend class Sim_Method;
	friend class Sim_Read_Plans;
	friend class Problem_Output;
	friend class Simulator_Output;
	friend class Snapshot_Output;
	friend class Link_Delay_Output;
	friend class Performance_Output;
	friend class Ridership_Output;
	friend class Turn_Vol_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;

public:

	Simulator_Service (void);
	virtual ~Simulator_Service (void);

	void Set_Parameters (Sim_Parameters &param);

protected:
	enum Simulator_Service_Keys { 
		SIMULATION_START_TIME = SIM_SERVICE_OFFSET, SIMULATION_END_TIME, TIME_STEPS_PER_SECOND, 
		CELL_SIZE, PLAN_FOLLOWING_DISTANCE, LOOK_AHEAD_DISTANCE, LOOK_AHEAD_LANE_FACTOR, 
		LOOK_AHEAD_TIME_FACTOR, LOOK_AHEAD_VEHICLE_FACTOR, MAXIMUM_SWAPPING_SPEED, 
		MAXIMUM_SPEED_DIFFERENCE, ENFORCE_PARKING_LANES, FIX_VEHICLE_LOCATIONS, 
		DRIVER_REACTION_TIME, PERMISSION_PROBABILITY, SLOW_DOWN_PROBABILITY, SLOW_DOWN_PERCENTAGE, 
		MAX_COMFORTABLE_SPEED, TRAVELER_TYPE_FACTORS, PRIORITY_LOADING_TIME, MAXIMUM_LOADING_TIME, 
		PRIORITY_WAITING_TIME, MAXIMUM_WAITING_TIME, MAX_DEPARTURE_TIME_VARIANCE, 
		MAX_ARRIVAL_TIME_VARIANCE, RELOAD_CAPACITY_PROBLEMS, COUNT_PROBLEM_WARNINGS, 
		PRINT_PROBLEM_MESSAGES, UNSIMULATED_SUBAREAS, MACROSCOPIC_SUBAREAS, MESOSCOPIC_SUBAREAS, 
		MICROSCOPIC_SUBAREAS, TURN_POCKET_FACTOR, MERGE_POCKET_FACTOR, OTHER_POCKET_FACTOR
	};
	void Simulator_Service_Keys (int *keys = 0);

	virtual void Program_Control (void);
	virtual void Execute (void);

	void Global_Data (void);

	virtual void Output_Event (Event_Data &data)         { data.Event (0); }
	virtual bool Check_Event (Event_Type type, int mode = DRIVE_MODE, int subarea = 0) 
		{ return (type == MAX_EVENT && mode == MAX_MODE && subarea > 0); }
	
	virtual void Output_Traveler (Traveler_Data &data)   { data.Household (0); }
	virtual bool Check_Traveler (Traveler_Data &data)    { return (data.Household () > 0); }
	virtual bool Output_Traveler_Flag (void)             { return (false); }

	virtual void Output_Problem (Problem_Data &problem)  { problem.Problem (0); }
	virtual void Check_Output (Travel_Step &travel)      { travel.Status (1); }
	virtual bool Active_Vehicles (void)                  { return (!active_vehicles.empty ()); }

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);

	void Transit_Vehicles (void);
	void Transit_Plans (void);

	int  Num_Subareas (void)               { return (num_subareas); }
	void Num_Subareas (int value)          { num_subareas = value; }

	int  Num_Parts (void)                  { return (num_parts); }
	void Num_Parts (int value)             { num_parts = value; }
	
	int  First_Part (void)                 { return (first_part); }
	void First_Part (int num)              { first_part = num; }

	int  Last_Part (void)                  { return (last_part); }
	void Last_Part (int num)               { last_part = num; }

	int  Num_Simulators (void)             { return (num_sims); }
	void Num_Simulators (int value)        { num_sims = value; }

	int  Num_Vehicles (void)               { return (num_vehicles); }
	void Num_Vehicles (int value)          { num_vehicles = value; }
	void Add_Vehicles (int value = 1)      { num_vehicles += value; }

	void Add_Statistics (Sim_Statistics &_stats);
	Sim_Statistics & Get_Statistics (void) { return (stats); }
	
	Dtime step;
	Data_Range no_range, macro_range, meso_range, micro_range;

	Sim_Traveler_Array sim_traveler_array;
	Sim_Plan_Map sim_plan_map;
	Sim_Plan_Array transit_plans;
	Sim_Veh_Array  sim_veh_array;
	Veh_Type_Array sim_type_array;
	Sim_Dir_Array  sim_dir_array;
	Sim_Connect_Array sim_connection;
	Sim_Park_Array sim_park_array;
	Sim_Stop_Array sim_stop_array;
	Sim_Signal_Array sim_signal_array;

	Integers location_part;
	Int2_Map part_map;
	Int2_Array part_array;

	Barrier io_barrier, xfer_barrier, veh_barrier;

	Int2s_Array transfers;
	Work_Step work_step;

#ifdef MPI_EXE
	Ints_Array   mpi_parts;
	Int2_Array   mpi_range;
	Integers     part_rank;
	Int2_Set     mpi_exchange;
	I2_Ints_Map  mpi_boundary;
#endif

	char *veh_status;
	Integers active_vehicles;

#ifdef DEBUG_KEYS
	Time_Periods debug_time;
	Data_Range debug_list, debug_link;
	bool debug_flag, debug_time_flag;
#endif

private:
	int step_type, num_subareas, max_subarea, num_sims, num_vehicles, transit_id;
	int first_part, last_part, num_parts;
	Sim_Statistics stats;
	Sim_Parameters param;
};
#endif
