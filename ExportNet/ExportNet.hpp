//*********************************************************
//	ExportNet.hpp - export network data in various formats
//*********************************************************

#ifndef EXPORTNET_HPP
#define EXPORTNET_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Time_Periods.hpp"
#include "Data_Range.hpp"
#include "List_Data.hpp"
#include "Db_File.hpp"
#include "Shape_Tools.hpp"
#include "Split_Data.hpp"

#include "fstream"

//---------------------------------------------------------
//	ExportNet - execution class definition
//---------------------------------------------------------

class ExportNet : public Data_Service, public Select_Service
{
public:
	ExportNet (void);
	virtual ~ExportNet (void);

	virtual void Execute (void);

protected:
	enum ExportNet_Keys { 
		NEW_UTDF_NETWORK_FILE = 1, NEW_NODE_MAP_FILE, NEW_VISSIM_XML_FILE,
		INSERT_PARKING_LINKS, WRITE_DWELL_FIELD, WRITE_TIME_FIELD, WRITE_SPEED_FIELD, 
		MINIMUM_DWELL_TIME, MAXIMUM_DWELL_TIME, TRANSIT_TIME_PERIODS, PERIOD_TRAVEL_TIMES, 
		NEW_ZONE_LOCATION_FILE
	};
	virtual void Program_Control (void);

private:

	int nroute, nnodes, num_periods, min_dwell, max_dwell;
	bool utdf_flag, map_flag, vissim_flag, parking_flag, zone_loc_flag;
	bool route_flag, time_flag, dwell_flag, ttime_flag, speed_flag;

	Time_Periods schedule_periods;
	
	Db_File utdf_file, map_file, xml_file, zone_loc_file;
	
	List_Array node_list, dir_list;

	Link_Split_Array link_splits;

	//---- methods ----

	void Write_UTDF (void);

	void Write_VISSIM (void);
	void Parking_Links (void);
	void New_Nodes (void);
	void New_Links (void);
	void New_Locations (void);
	void New_Connections (void);
	void XML_Vehicle_Types (void);
	void XML_Nodes (void);
	void XML_Zones (void);
	void XML_Links (void);
	void XML_Signals (void);
	void XML_Stops (void);
	void XML_Routes (void);

	void Write_Route (void);
};
#endif
