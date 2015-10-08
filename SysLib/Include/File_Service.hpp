//*********************************************************
//	File_Service.hpp - system file management service
//*********************************************************

#ifndef FILE_SERVICE_HPP
#define FILE_SERVICE_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Db_Header.hpp"

#include "Node_File.hpp"
#include "Zone_File.hpp"
#include "Shape_File.hpp"
#include "Link_File.hpp"
#include "Pocket_File.hpp"
#include "Lane_Use_File.hpp"
#include "Connect_File.hpp"
#include "Turn_Pen_File.hpp"
#include "Parking_File.hpp"
#include "Location_File.hpp"
#include "Access_File.hpp"
#include "Sign_File.hpp"
#include "Signal_File.hpp"
#include "Timing_File.hpp"
#include "Phasing_File.hpp"
#include "Detector_File.hpp"
#include "Stop_File.hpp"
#include "Fare_File.hpp"
#include "Line_File.hpp"
#include "Schedule_File.hpp"
#include "Driver_File.hpp"
#include "Route_Nodes_File.hpp"
#include "Selection_File.hpp"
#include "Household_File.hpp"
#include "Link_Delay_File.hpp"
#include "Performance_File.hpp"
#include "Ridership_File.hpp"
#include "Veh_Type_File.hpp"
#include "Vehicle_File.hpp"
#include "Trip_File.hpp"
#include "Problem_File.hpp"
#include "Plan_File.hpp"
#include "Skim_File.hpp"
#include "Event_File.hpp"
#include "Traveler_File.hpp"
#include "Equiv_Data.hpp"
#include "Zone_Location.hpp"

//---------------------------------------------------------
//	File_Service - system file management class
//---------------------------------------------------------

class SYSLIB_API File_Service : public Execution_Service
{
public:
	File_Service (void);
	virtual ~File_Service (void);

	bool   System_File_Flag (System_File_Type type)    { return ((type < END_FILE) ? system_file [type].flag : false); }
	void   System_File_False (System_File_Type type)   { if (type < END_FILE) { system_file [type].flag = false; system_file [type].option = false; }}
	void   System_File_True (System_File_Type type)    { if (type < END_FILE) system_file [type].flag = true; }

	bool   System_Option_Flag (System_File_Type type)  { return ((type < END_FILE) ? system_file [type].option : false); }
	void   System_Option_False (System_File_Type type) { if (type < END_FILE) system_file [type].option = false; }
	void   System_Option_True (System_File_Type type)  { if (type < END_FILE) system_file [type].option = true; }

	void   System_Read_False (System_File_Type type)   { if (type < END_FILE) system_file [type].read = false; }
	
	int    System_Data_Reserve (System_File_Type type) { return ((type < END_FILE) ? system_file [type].reserve : 0); }
	void   System_Data_Reserve (System_File_Type type, int percent) { if (type < END_FILE) system_file [type].reserve = (short) percent; }

	int    System_File_ID (System_File_Type type)      { return (type + SYSTEM_FILE_OFFSET); }
	bool   Network_File_Flag (System_File_Type type)   { return ((type < END_FILE) ? system_file [type].net : false); }

	Access_Type System_File_Access (System_File_Type type) { return ((type < END_FILE) ? system_file [type].access : READ); }
	void   System_File_Access (System_File_Type type, Access_Type access) { if (type < END_FILE) system_file [type].access = access; }

	bool   System_Data_Flag (System_File_Type type)    { return ((type < END_FILE) ? system_file [type].data : false); }
	void   System_Data_False (System_File_Type type)   { if (type < END_FILE) system_file [type].data = false; }
	void   System_Data_True (System_File_Type type)    { if (type < END_FILE) system_file [type].data = true; }

	Db_File * System_File_Handle (System_File_Type type)    { return ((type < END_FILE) ? system_file [type].file : 0); }
	Db_Base * System_File_Base (System_File_Type type)      { return ((Db_Base *) System_File_Handle (type)); }
	Db_Header * System_File_Header (System_File_Type type)  { return ((Db_Header *) System_File_Handle (type)); }
	
	void   Control_Flag (bool flag)           { control_flag = flag; }
	bool   Control_Flag (void)                { return (control_flag); }

	Units_Type Flow_Units (void)              { return (flow_units); }
	void   Flow_Units (Units_Type type)       { flow_units = type; }

	bool   Lane_Use_Flows (void)              { return (lane_use_flows); }
	void   Lane_Use_Flows (bool flag)         { lane_use_flows = flag; }

	void   AB_Map_Flag (bool value)           { ab_flag = value; }
	bool   AB_Map_Flag (void)                 { return (ab_flag); }

	bool Copy_File_Header (System_File_Type from, System_File_Type to);
	bool Required_File_Check (Db_File &file, System_File_Type type);
	
	enum File_Service_Keys { 
		NOTES_AND_NAME_FIELDS = FILE_SERVICE_OFFSET, FLOW_UNITS, SAVE_LANE_USE_FLOWS, 
		SKIM_OD_UNITS, SKIM_TIME_PERIODS, SKIM_TIME_INCREMENT, SKIM_TOTAL_TIME_FLAG, 
		SKIM_TRAVEL_TIME_FORMAT, SKIM_TRIP_LENGTH_FORMAT, NEAREST_NEIGHBOR_FACTOR, 
		MERGE_TIME_PERIODS, SKIM_FILE_HEADERS, ZONE_EQUIVALENCE_FILE, LINK_EQUIVALENCE_FILE, 
		STOP_EQUIVALENCE_FILE, LINE_EQUIVALENCE_FILE, TIME_EQUIVALENCE_FILE, ZONE_LOCATION_MAP_FILE, 
	};
	void File_Service_Keys (int *keys = 0);
	
	void   Zone_Equiv_Flag (bool flag)        { zone_flag = Required_Control_Key (ZONE_EQUIVALENCE_FILE, flag); }
	bool   Zone_Equiv_Flag (void)             { return (zone_flag); }
	
	void   Link_Equiv_Flag (bool flag)        { link_flag = Required_Control_Key (LINK_EQUIVALENCE_FILE, flag); }
	bool   Link_Equiv_Flag (void)             { return (link_flag); }
	
	void   Stop_Equiv_Flag (bool flag)        { stop_flag = Required_Control_Key (STOP_EQUIVALENCE_FILE, flag); }
	bool   Stop_Equiv_Flag (void)             { return (stop_flag); }
	
	void   Line_Equiv_Flag (bool flag)        { line_flag = Required_Control_Key (LINE_EQUIVALENCE_FILE, flag); }
	bool   Line_Equiv_Flag (void)             { return (line_flag); }
	
	void   Time_Equiv_Flag (bool flag)        { time_flag = Required_Control_Key (TIME_EQUIVALENCE_FILE, flag); }
	bool   Time_Equiv_Flag (void)             { return (time_flag); }

	void   Zone_Loc_Flag (bool flag)          { zone_loc_flag = Required_Control_Key (ZONE_LOCATION_MAP_FILE, flag); }
	bool   Zone_Loc_Flag (void)               { return (zone_loc_flag); }

	Zone_Equiv zone_equiv;
	Link_Equiv link_equiv;
	Stop_Equiv stop_equiv;
	Line_Equiv line_equiv;
	Time_Equiv time_equiv;

	Zone_Location_Map zone_loc_map;

protected:
	virtual void Program_Control (void);

	void Required_System_Files (System_File_Type files []);
	void Optional_System_Files (System_File_Type files []);

	typedef struct {
		Db_File *file;
		bool     flag;
		bool     option;
		bool     data;
		bool     read;
		bool     net;
		bool     parts;
		short    reserve;
		Access_Type  access;
	} File_Data;

	File_Data * System_File (System_File_Type type)  { return ((type < END_FILE) ? system_file + type : 0); }
	
private:

	bool control_flag, ab_flag, zone_flag, link_flag, stop_flag, line_flag, time_flag, zone_loc_flag;
	bool lane_use_flows;
	Units_Type flow_units;

	File_Data system_file [END_FILE];

	typedef struct {
		System_File_Type  type;
		const char       *key;
		const char       *format;
	} File_Key;

	static File_Key file_keys [END_FILE];
};
#endif
