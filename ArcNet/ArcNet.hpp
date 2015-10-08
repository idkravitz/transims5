//*********************************************************
//	ArcNet.hpp - Network Conversion Utility
//*********************************************************

#ifndef ARCNET_HPP
#define ARCNET_HPP

#include "Data_Service.hpp"
#include "Draw_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Time_Periods.hpp"
#include "Db_Array.hpp"
#include "Dtime.hpp"
#include "Shape_Tools.hpp"
//#include "Subzone_File.hpp"

//---------------------------------------------------------
//	ArcNet - execution class definition
//---------------------------------------------------------

class ArcNet : public Data_Service, public Draw_Service
{
public:
	ArcNet (void);
	virtual ~ArcNet (void);

	virtual void Execute (void);

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba);
	virtual bool Get_Pocket_Data (Pocket_File &file, Pocket_Data &data);
	virtual bool Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data);
	virtual bool Get_Connect_Data (Connect_File &file, Connect_Data &data);
	virtual bool Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &data);
	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &data);
	virtual bool Get_Sign_Data (Sign_File &file, Sign_Data &data);
	virtual bool Get_Signal_Data (Signal_File &file, Signal_Data &data);
	virtual int  Get_Timing_Data (Timing_File &file, Timing_Data &data);
	virtual int  Get_Phasing_Data (Phasing_File &file, Phasing_Data &data);
	virtual bool Get_Detector_Data (Detector_File &file, Detector_Data &data);
	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &data);
	virtual bool Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data);

	virtual void Initialize_Links (Link_File &file);
	virtual void Initialize_Pockets (Pocket_File &file);
	virtual void Initialize_Timing_Plans (Timing_File &file);

protected:
	enum ArcNet_Keys { 
		SUBZONE_DATA_FILE = 1, NEW_ARC_NODE_FILE, NEW_ARC_ZONE_FILE, NEW_ARC_LINK_FILE, 
		NEW_ARC_CENTERLINE_FILE, NEW_ARC_POCKET_FILE, NEW_ARC_LANE_USE_FILE, NEW_ARC_LOCATION_FILE, 
		NEW_ARC_PARKING_FILE, NEW_ARC_ACCESS_FILE, NEW_ARC_CONNECTION_FILE, NEW_ARC_TURN_PENALTY_FILE, 
		NEW_ARC_SIGN_FILE, NEW_ARC_SIGNAL_FILE, NEW_ARC_TIMING_PLAN_FILE, NEW_ARC_PHASING_PLAN_FILE, 
		NEW_ARC_DETECTOR_FILE, NEW_ARC_TRANSIT_STOP_FILE, NEW_ARC_TRANSIT_ROUTE_FILE, 
		NEW_ARC_TRANSIT_DRIVER_FILE, NEW_ARC_ROUTE_NODES_FILE, NEW_ARC_SUBZONE_DATA_FILE, 
		SELECT_TIME, TRANSIT_TIME_PERIODS
	};
	virtual void Program_Control (void);

private:

	Arcview_File arcview_node, arcview_zone, arcview_link, arcview_center;
	Arcview_File arcview_pocket, arcview_lane_use;
	Arcview_File arcview_location, arcview_parking, arcview_access;
	Arcview_File arcview_turn, arcview_connect, arcview_sign;
	Arcview_File arcview_signal, arcview_phasing, arcview_timing, arcview_detector; 
	Arcview_File arcview_stop, arcview_route, arcview_driver, arcview_route_nodes, arcview_subzone;

	Point_Map location_pt, parking_pt, stop_pt;

	//Subzone_File subzone_file;

	Db_Data_Array link_db, pocket_db, timing_db;

	Points points;

	bool route_flag, schedule_flag, driver_flag, route_node_flag;
	bool timing_flag, subzone_flag, time_flag;

	Dtime time;

	int run_field;
	Time_Periods sched_breaks;

	Projection_Service projection;

	//void Write_Subzone (void);

	void Draw_Links (void);
	void Draw_Pockets (void);
	void Draw_Timing_Plans (void);
	void Draw_Route (void);
	void Draw_Driver (void);

	void Add_Arrow (Points &points);
};
#endif


