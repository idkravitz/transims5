//*********************************************************
//	TransitNet.hpp - Transit Conversion Utility
//*********************************************************

#ifndef TRANSITNET_HPP
#define TRANSITNET_HPP

#include "Data_Service.hpp"
#include "Db_Header.hpp"
#include "Time_Periods.hpp"
#include "Best_List.hpp"
#include "TypeDefs.hpp"

#include "Node_Path_Data.hpp"

//#define MIN_LENGTH		37.5
//#define MAX_AREA_TYPE	16

//---------------------------------------------------------
//	TransitNet - execution class definition
//---------------------------------------------------------

class TransitNet : public Data_Service
{
public:
	TransitNet (void);
	virtual ~TransitNet (void);

	virtual void Execute (void);

protected:
	enum TransimsNet_Keys { 
		PARK_AND_RIDE_FILE = 1, STOP_SPACING_BY_AREA_TYPE, STOP_FACILITY_TYPE_RANGE,
		TRANSIT_TIME_PERIODS, TRANSIT_TRAVEL_TIME_FACTORS, MINIMUM_DWELL_TIME,
		INTERSECTION_STOP_TYPE, INTERSECTION_STOP_OFFSET
	};
	virtual void Program_Control (void);

private:
	enum TransitNet_Reports {ZONE_EQUIV = 1};

	bool parkride_flag, equiv_flag, dwell_flag, time_flag, speed_flag, at_flag, access_flag;
	bool facility_flag [EXTERNAL+1];

	int naccess, nlocation, nparking, line_edit, route_edit, schedule_edit, driver_edit;
	int nstop, nroute, nschedule, ndriver, end_warnings, parking_warnings;
	int max_parking, max_access, max_location, max_stop, nparkride, new_access, new_stop;
	int min_dwell, num_periods, stop_type, left_turn, bus_code, rail_code, stop_offset;

	Double_List min_stop_spacing, time_factor;
	Time_Periods schedule_periods;

	Db_Header parkride_file;

	Node_Path_Array node_path_array;
	Path_Leg_Array path_leg_array;

	Integers node_list, dir_list, local_access, fare_zone;

	vector <Int_Map> dir_stop_array;

	//---- methods ----

	void Data_Setup (void);
	void Read_ParkRide (void);
	void Node_Path (int node1, int node2, Use_Type use);
	void Build_Routes (void);
	void Station_Access (void);
};
#endif
