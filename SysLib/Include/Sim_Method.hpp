//*********************************************************
//	Sim_Method.hpp - Network Step Simulator 
//*********************************************************

#ifndef SIM_METHOD_HPP
#define SIM_METHOD_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "TypeDefs.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "Sim_Parameters.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Traveler.hpp"
#include "Sim_Plan_Data.hpp"
#include "Sim_Vehicle.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Connection.hpp"
#include "Sim_Signal_Data.hpp"
#include "Travel_Step.hpp"

//---------------------------------------------------------
//	Sim_Method - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Method : Static_Service
{
public:
	Sim_Method (Simulator_Service &exe, Int2_Map_Data *map = 0, Sim_Parameters *param_ptr = 0);
	
	bool Step_Prep (Integers * new_trip_list);
	void Boundary (void);
	void Process_Step (void);

	Int_List * Active_Vehicles (void)        { return (&veh_list); }
	Int_List * Active_Travelers (void)       { return (&trip_list); }

	int  Num_Vehicles (void)                 { return (num_vehicles); }

	Sim_Statistics & Get_Statistics (void) { return (stats); }

private:
	bool boundary_flag;
	int subarea, part, partition, method, num_vehicles;
	Dtime one_second, use_update_time, turn_update_time;
	Dtime signal_update_time, timing_update_time, run_update_time;

	Int_List trip_list, veh_list, priority_list;

	void Network_Prep (void);
	void Update_Network (void);
	void Update_Transit (void);
	void Traffic_Controls (bool initialize = false);
	void Update_Signals (void);
	void Traveler_Events (void);
	bool Next_Travel_Plan (int traveler);
	bool Best_Lanes (Travel_Step &step);
	bool Priority_Prep (Travel_Step &travel_step);
		
	bool Move_Vehicle (Travel_Step &step, bool leader);
	bool Load_Vehicle (Travel_Step &step);
	bool Look_Ahead (Travel_Step &step);
	bool Check_Ahead (Travel_Step &step);
	bool Check_Queue (Travel_Step &step);
	bool Output_Step (Travel_Step &step);
	bool Lane_Change (Travel_Step &step);
	int  Sum_Path (Cell_Data cell_rec, Travel_Step &step);
	bool Reserve_Cell (Cell_Data cell, Travel_Step &travel_step);
	bool Check_Behind (Cell_Data cell, Travel_Step &travel_step, int num_cells = 1, bool use_flag = false);
	bool Check_Cell (Cell_Data cell, Travel_Step &step);
	bool Cell_Use (Sim_Lane_Data *lane_ptr, int lane, int cell, Travel_Step &step, bool use_flag = false);
	bool Cell_Use (Travel_Step &step);
	bool Check_Detector (int index);	
	int  Next_Signal_Event (int dir_index);

	Sim_Parameters param;
	Sim_Statistics stats;

	Int2_Array *transfers;
	Integers boundary, first_list;

	Simulator_Service *exe;
};
#endif

