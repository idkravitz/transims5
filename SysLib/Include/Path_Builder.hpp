//*********************************************************
//	Path_Builder.hpp - Network Path Building 
//*********************************************************

#ifndef PATH_BUILDER_HPP
#define PATH_BUILDER_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Router_Service.hpp"
#include "TypeDefs.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"

#include "Path_Data.hpp"
#include "Trip_End_Data.hpp"
#include "Transit_Sort.hpp"
#include "One_To_Many.hpp"

#ifdef BOOST_THREADS
#include "Ordered_Work.hpp"

typedef Ordered_Work <Plan_Ptr_Array, Plan_Ptr_Array> Plan_Queue;
typedef Ordered_Work <One_To_Many, One_To_Many> Skim_Queue;
#endif

//---------------------------------------------------------
//	Path_Builder - execution class definition
//---------------------------------------------------------

class SYSLIB_API Path_Builder : Static_Service
{
public:

	Path_Builder (Router_Service *exe = 0);

#ifdef BOOST_THREADS

	Path_Builder (Plan_Queue *plan_queue, Router_Service *exe);
	Path_Builder (Skim_Queue *skim_queue, Router_Service *exe);

	Plan_Queue *plan_queue;
	Skim_Queue *skim_queue;
	Flow_Time_Period_Array link_delay_array;
	Flow_Time_Period_Array turn_delay_array;

	//---- boost thread interface ----

	void operator()()
	{
		int number = 0;

		if (param.one_to_many) {
			One_To_Many *skim_ptr;

			for (;;) {
				skim_ptr = skim_queue->Get_Work (number);
				if (skim_ptr == 0) break;

				if (!Skim_Build (skim_ptr)) break;

				if (!skim_queue->Put_Result (skim_ptr, number)) break;
			}
		} else {
			Plan_Ptr_Array *array_ptr;

			for (;;) {
				array_ptr = plan_queue->Get_Work (number);
				if (array_ptr == 0) break;

				if (!Array_Processing (array_ptr)) break;

				if (!plan_queue->Put_Result (array_ptr, number)) break;
			}
		}
	}
	void Save_Flows (void);
#endif

	void Initialize (Router_Service *exe);
	bool Array_Processing (Plan_Ptr_Array *array_ptr);
	bool Skim_Build (One_To_Many *data_ptr);

private:
	unsigned max_imp, imp_diff;
	int near_offset, parking_lot;
	bool time_flag, dist_flag, length_flag, zero_flag, wait_time_flag, use_flag, local_acc_flag, access_flag;
	bool transfer_flag, reset_veh_flag, mode_path_flag [MAX_MODE], flow_flag, walk_acc_flag, park_flag;
	bool walk_flag, bike_flag, wait_flag, rail_bias_flag, bus_bias_flag, turn_flag, random_flag;

	Dtime time_limit, min_time_limit, parking_duration;
	double op_cost_rate;

	bool initialized, forward_flag, plan_flag, vehicle_flag, veh_type_flag, grade_flag;
	bool walk_path_flag, bike_path_flag, transit_path_flag, access_link_flag, reroute_flag;

	Path_Parameters param;
	Veh_Type_Data *veh_type_ptr;

	Plan_Data *plan_ptr;
	Plan_Data plan;
	Plan_Leg_Array leg_array;
	Plan_Leg_Array *leg_ptr;

	Flow_Time_Period_Array *link_flow_ptr, *turn_flow_ptr;

	One_To_Many *data_ptr;

	Trip_End_Array trip_org, trip_des, parking_lots;
	Path_End_Array from_array, to_array, from_parking, to_parking;

	bool link_to_flag, node_to_flag, stop_to_flag;
	Integers link_to, node_to, stop_to, next_to;

	Int_Map lane_use_delay;
	Path_Array link_path, node_path [MAX_PATHS+1];
	Path_Array board_path [MAX_PATHS], wait_path [MAX_PATHS], alight_path [MAX_PATHS];

	Transit_Queue next_index;
	Transit_Sort transit_sort;

	bool Plan_Build (Plan_Data *plan_ptr);
	bool Plan_Update (Plan_Data *plan_ptr);
	bool Plan_ReRoute (Plan_Data *plan_ptr);
	bool Plan_Flow (Plan_Data *plan_ptr);

	int  Build_Path (int lot = 0);

	int  Best_Destination (Path_End_Array *to_ptr, Trip_End_Array *des_ptr = 0);

	int  Set_Node_Error (void);
	int  Set_Drive_Error (void);
	int  Set_Transit_Error (void);

	int  Drive_Plan (int lot);
	int  Drive_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag = true);
	
	bool Origin_Parking (Trip_End_Array *org_ptr, Path_End_Array *from_ptr, int lot);
	bool Destination_Parking (Trip_End_Array *des_ptr, Path_End_Array *to_ptr, int lot);

	int  Node_Plan (void);
	int  Node_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag = true);
	
	bool Origin_Location (Trip_End_Array *org_ptr, Path_End_Array *from_ptr, bool stop_flag = false);
	bool Destination_Location (Trip_End_Array *des_ptr, Path_End_Array *to_ptr, bool stop_flag = false);

	int  Transit_Plan (void);
	int  Transit_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag = true);

	void Initialize_Transit (void);
	bool Transit_Access (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag = true);
	bool Walk_Access (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag = true);
	bool Stop_Alight (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag = true);
	bool Stop_Board (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag = true);
	
	int  Drive_Transit_Plan (int lot);
	int  Transit_Parking (Trip_End_Array *org_ptr, Trip_End_Array *des_ptr, int lot = -1);
	int  Parking_Update (void);

	int  Magic_Move (void);

	int  Trace_Path (Trip_End *org, Path_End_Array *from, Path_End *to);
	bool Add_Leg (int mode, int type, int index, int dir = 0, int imped = 0, int time = 0, int len = 0, int cost = 0, int path = 0);
	int  Save_Skims (void);

	bool Best_Lane_Use (int index, Dtime time, Dtime &ttime, Dtime &delay, int &cost, int &group);

	Router_Service *exe;
};
#endif
