//*********************************************************
//	Validate.hpp - Network Validation Utility
//*********************************************************

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include "Data_Service.hpp"
#include "Db_Header.hpp"
#include "Flow_Time_Data.hpp"
#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Turn_Vol_File.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Validate - execution class definition
//---------------------------------------------------------

class Validate : public Data_Service
{
public:
	Validate (void);
	virtual ~Validate (void);

	virtual void Execute (void);

protected:
	enum Validate_Keys { 
		INPUT_VOLUME_FILE = 1, TRAFFIC_COUNT_FILE, TURN_VOLUME_FILE, TURN_COUNT_FILE, LINE_GROUP_COUNT_FILE, 
		STOP_GROUP_COUNT_FILE, NEW_VOLUME_FILE, NEW_VOLUME_COUNT_FILE, ANALYSIS_METHOD,
		ADJUSTMENT_FACTOR, FACILITY_TYPE_LABELS, AREA_TYPE_LABELS
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Validate_Reports {VOLUME_LEVEL = 1, FACILITY_TYPE, AREA_TYPE, ZONE_GROUP, 
		LINK_GROUP, GROUP_DETAILS, TURN_MOVEMENT, TURN_LOS, ZONE_EQUIV, LINK_EQUIV,
		LINE_GROUP, STOP_GROUP, BOARD_GROUP, ALIGHT_GROUP, LINE_EQUIV, STOP_EQUIV};

	bool delay_flag, link_flag, turn_flag, output_flag, zone_flag, method, line_flag, stop_flag, vc_flag;
	double factor, hours;

	Db_Header zone_file, line_count_file, stop_count_file;
	Link_Data_File count_file, volume_file, output_file;
	Link_Direction_File vol_cnt_file;
	Turn_Vol_File turn_count, turn_volume;
	
	Flow_Time_Period_Array  link_count_array;
	Flow_Time_Period_Array  turn_count_array;

	String stats_format;
	String header1, header2;

	static int volume_level [];
	String group_label;

	//---- facility type and area type labels ----

	Str_Map facility_type, area_type;

	//---- evaluation statistics ----

	typedef struct {
		double per_diff;
		double avg_error;
		double per_error;
		double std_dev;
		double r_sq;
		double rmse;
		//double correl;
		double avg_vc;
	} Statistics; 

	typedef struct {
		int    index;
		int    number;
		double count;
		double volume;
		double count_sq;
		double volume_sq;
		double count_volume;
		double error;
		double error_sq;
		double capacity;
		double max_vc;
	} Group_Data; 

	typedef map <int, Group_Data>        Group_Map;
	typedef pair <int, Group_Data>       Group_Map_Data;
	typedef Group_Map::iterator          Group_Map_Itr;
	typedef pair <Group_Map_Itr, bool>   Group_Map_Stat;

	Group_Data total;
	Group_Map group_data;
	Integers node_zone;

	//---- methods ----

	void Read_Flow (Link_Data_File *file, bool type);
	void Read_Turn (Turn_Vol_File *file, bool type);

	void Write_Volume (void);
	void Write_Volume_Count (void);
    int  Closest_Zone (int x, int y);

	void Volume_Stats (void);
	void Volume_Header (void);

	void Facility_Stats (void);
	void Facility_Header (void);

	void Area_Stats (void);
	void Area_Header (void);

	void Zone_Group_Stats (void);
	void Zone_Group_Header (void);

	void Link_Group_Stats (void);
	void Link_Group_Header (void);

	void Link_Group_Details (void);
	void Group_Detail_Header (void);

	void Turn_Stats (void);
	void Turn_Header (void);

	void Turn_LOS (void);
	void Turn_LOS_Header (void);

	void Calc_Stats (Group_Data *data, Statistics &stats);
	void Sum_Group (int group, Link_Data *link, int dir);

	void Line_Group_Stats (void);
	void Line_Group_Header (void);

	void Stop_Group_Stats (int type);
	void Stop_Group_Header (int type);

};

#endif
