//*********************************************************
//	ArcPlan.hpp - Plan File Display Utility
//*********************************************************

#ifndef ARCPLAN_HPP
#define ARCPLAN_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Draw_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Shape_Tools.hpp"
#include "Trip_Index.hpp"
#include "Equiv_Data.hpp"
#include "Data_Range.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	ArcPlan - execution class definition
//---------------------------------------------------------

class ArcPlan : public Data_Service, public Select_Service, public Draw_Service
{
public:
	ArcPlan (void);
	virtual ~ArcPlan (void);

	virtual void Execute (void);

	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &data);
	virtual bool Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition);

protected:
	enum ArcPlan_Keys { 
		NEW_ARC_PLAN_FILE = 1, NEW_ARC_PROBLEM_FILE, NEW_ARC_BANDWIDTH_FILE, NEW_ARC_TIME_CONTOUR_FILE, NEW_ARC_DISTANCE_CONTOUR_FILE, 
		NEW_ARC_ACCESSIBILITY_FILE, NEW_ARC_RIDERSHIP_FILE, NEW_ARC_STOP_DEMAND_FILE, NEW_ARC_STOP_GROUP_FILE, NEW_ARC_PARKING_DEMAND_FILE,
		MAXIMUM_SHAPE_ANGLE, MINIMUM_SHAPE_LENGTH, CONTOUR_TIME_INCREMENTS, CONTOUR_DISTANCE_INCREMENTS, RIDERSHIP_SCALING_FACTOR, 
		MINIMUM_RIDERSHIP_VALUE, MINIMUM_RIDERSHIP_SIZE, MAXIMUM_RIDERSHIP_SIZE
	};
	virtual void Program_Control (void);

private:
	enum ArcPlan_Reports { STOP_EQUIV = 1 };

	Arcview_File arcview_plan, arcview_problem, arcview_access;
	Arcview_File arcview_width, arcview_time, arcview_distance;
	Arcview_File arcview_rider, arcview_stop, arcview_group, arcview_parking;

	Plan_File *plan_file;

	Data_Range time_points, distance_points;

	Point_Map location_pt, parking_pt, stop_pt;

	Trip_Map problem_map;

	Points points;

	int num_out, num_problem, max_problem, num_width, num_time, num_distance, num_access;
	int num_rider, num_stop, num_group, max_angle, min_length, min_load, num_parking;

	Integers width_data, load_data, board_data, alight_data, stop_list, parking_out, parking_in;
	Bools link_flag;

	double load_factor, min_rider, max_rider, random_share;

	bool route_flag, driver_flag, stop_flag, type_flag, problem_flag, problem_out, plan_flag;
	bool path_flag, hhlist_flag, width_flag, time_flag, distance_flag, turn_flag;
	bool access_flag, rider_flag, load_flag, on_off_flag, demand_flag, group_flag, parking_flag;
	bool mode_flag, travel_modes [REGIONRAIL+1], random_flag;

	Projection_Service projection;	

	//---- methods ----

	void Read_Plan (void);
	void Write_Bandwidth (void);
	void Write_Stops (void);
	void Write_Group (void);
	void Write_Parking (void);
	void Write_Route (void);
};
#endif


