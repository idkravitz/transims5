//*********************************************************
//	TransitAccess.hpp - generate transit access links
//*********************************************************

#ifndef TRANSITACCESS_HPP
#define TRANSITACCESS_HPP

#include "Execution_Service.hpp"
#include "Select_Service.hpp"
#include "Db_Matrix.hpp"
#include "Db_Header.hpp"
#include "Db_Array.hpp"
#include "Data_Range.hpp"
#include "Best_List.hpp"
#include "TypeDefs.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	TransitAccess - execution class definition
//---------------------------------------------------------

class TransitAccess : public Execution_Service, public Select_Service
{
public:
	TransitAccess (void);
	virtual ~TransitAccess (void);

	virtual void Execute (void);

protected:
	enum TransitAccess_Keys { 
		ZONE_FILE = 1, ZONE_FORMAT, ZONE_NUMBER_FIELD, ZONE_X_COORD_FIELD, ZONE_Y_COORD_FIELD, ZONE_LOCATION_FIELD,
		ZONE_AREA_FIELD, ZONE_LONG_WALK_FIELD, ZONE_PEF_FIELD, PEF_DISTANCE_WEIGHTS, ZONE_AREA_FACTORS,
		ZONE_STATION_MODE_CODES, NODE_FILE, NODE_FORMAT, NODE_NUMBER_FIELD, NODE_X_COORD_FIELD, NODE_Y_COORD_FIELD,
		WALK_LINK_FILE, WALK_LINK_FORMAT, WALK_LINK_ANODE_FIELD, WALK_LINK_BNODE_FIELD,
		WALK_LINK_LENGTH_FIELD, WALK_LINK_ZONE_FIELD, WALK_LINK_TYPE_FIELD, WALK_LINK_TYPE_RANGE,
		EXTRA_LINK_FILE, EXTRA_LINK_FORMAT, EXTRA_LINK_ACTION_FIELD, EXTRA_LINK_ANODE_FIELD, EXTRA_LINK_BNODE_FIELD,
		STATION_FILE, STATION_FORMAT, STATION_MODE_FIELD, STATION_TYPE_FIELD, STATION_PNR_FLAG_FIELD, 
		STATION_USE_FLAG_FIELD,	STATION_SKIM_FIELD, STATION_ZONE_FIELD, STATION_STOP_FIELD, STATION_PARKING_FIELD, 
		STATION_NODE_FIELDS, STATION_CAPACITY_FIELD, STATION_COST_FIELDS, STATION_TIME_FIELDS, STATION_ACCESS_FIELD,
		STATION_X_COORD_FIELD, STATION_Y_COORD_FIELD, STATION_NAME_FIELD,
		SKIM_FILE, SKIM_FORMAT, SKIM_TIME_TABLE, SKIM_DISTANCE_TABLE, VALUE_OF_TIME,
		CBD_ZONE_NUMBER, MAX_DIVERSION_RATIO, MAX_DIVERSION_DISTANCE, 
		BARRIER_LOCATION_MAP, PROHIBITED_INTERCHANGES, 
		SLUG_ZONE_FILE, SLUG_ZONE_FORMAT, SLUG_ZONE_FIELD, SLUG_STATION_TYPE,
		NEW_SIDEWALK_FILE, NEW_WALK_ACCESS_FILE, NEW_WALK_LINK_FILE,
		NEW_NODE_FILE, NEW_NODE_FORMAT, NEW_STOP_ZONE_FILE, STOP_ZONE_MODE_CODE, STOP_ZONE_OFFSET,
		NEW_MODE_FILE, MODE_CODE, MODE_SKIM, MODE_PNR_FLAG, MODE_CONNECTION, MODE_TYPE_DISTANCES, MODE_WALK_TIMES,
	};
	enum Connect_Types {
		PARKING_NODE, STOP_NODE, NETWORK_NODE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum TransitAccess_Reports { ZONE_REPORT = 1, STATION_REPORT };

	bool slug_flag, node_flag, cost_flag, zone_walk_flag, walk_link_flag, extra_flag, station_acc_flag;
	int nzones, ndes, cbd_zone, cbd_x, cbd_y, slug_type, slug_fld, slug_zone, slug_stop;
	int missing_skims, num_default, num_links, highest_skim;
	double max_ratio, max_dist, time_value;

	//---- zone data ----

	int zone_fld, zone_x_fld, zone_y_fld, zone_loc_fld, zone_area_fld, zone_long_fld, zone_pef_fld;
	Integers pef_values, mode_codes;
	Doubles pef_factors;
	Double_List walk_factors;
	Db_Header zone_file;

	//---- zone walk data ----
	
	typedef struct zone_data {
		int     x_coord;
		int     y_coord;
		int     location;
		int     barrier;
		int     slug;
		int     walk;
		double  distance;
		double  weight;
		bool    del_flag;
	} Zone_Data;

	typedef map <int, Zone_Data>      Zone_Map;
	typedef pair <int, Zone_Data>     Zone_Map_Data;
	typedef Zone_Map::iterator        Zone_Map_Itr;
	typedef pair <Zone_Map_Itr, bool> Zone_Map_Stat;

	Zone_Map zone_map;

	//---- barrier data ----

	Data_Ranges location_map;
	Int2_Set barriers;

	//---- node data ----

	int node_fld, node_x_fld, node_y_fld;
	Db_Header node_file;

	typedef struct node_data {
		int x_coord;
		int y_coord;
		int use;
	} Node_Data;

	typedef map <int, Node_Data>      Node_Map;
	typedef pair <int, Node_Data>     Node_Map_Data;
	typedef Node_Map::iterator        Node_Map_Itr;
	typedef pair <Node_Map_Itr, bool> Node_Map_Stat;

	Node_Map node_map;

	//---- walk link data ----

	int link_anode_fld, link_bnode_fld, link_len_fld, link_zone_fld, link_type_fld;
	Db_Header link_data_file;
	Data_Range type_range;

	//---- extra link data ----

	int extra_action_fld, extra_anode_fld, extra_bnode_fld;
	Db_Header extra_link_file;
	Int2_Set skip_links;

	//---- walk access output files ----

	Db_File sidewalk_file, walk_acc_file, walk_link_file;

	//---- station data ----

	int new_node_fld, new_x_fld, new_y_fld;
	int station_mode_fld, station_type_fld, station_pnr_fld, station_use_fld;
	int station_skim_fld, station_zone_fld,  station_stop_fld, station_park_fld;
	int station_cap_fld, station_acc_fld, station_x_fld, station_y_fld, station_name_fld;
	Integer_List station_node_flds, station_cost_flds, station_time_flds;

	Db_Header station_file, slug_file, new_node_file;

	typedef struct station_data {
		int      mode;
		int      type;
		bool     pnr;
		bool     use;
		int      skim;
		int      zone;
		int      stop;
		int      park;
		int      node;
		int      x_coord;
		int      y_coord;
		int      cbd_dist;
		bool     missing;
		String   name;
	} Station_Data;

	typedef vector <Station_Data>      Station_Array;
	typedef Station_Array::iterator    Station_Itr;

	Station_Array station_array;
	Int_Map station_access;
	Node_Map new_node_map;

	//---- skim data ----

	class Skim_Data
	{
	public:
		Skim_Data (void)           { number = time_table = dist_table = index = 0; }

		int        number;
		int        index;
		int        time_table;
		int        dist_table;
		Dbls_Array time;
		Dbls_Array distance;     
	};
	typedef vector <Skim_Data>    Skim_Array;
	typedef Skim_Array::iterator   Skim_Itr;

	Skim_Array skim_array;
	Db_Matrices skim_files;

	//---- mode data ----

	class Mode_Group
	{
	public:
		Mode_Group (void)           { number = mode = skim = pnr = connect = count = 0; mode_file = 0; }
		virtual ~Mode_Group (void)  { if (mode_file) delete mode_file; }

		void Write_Record (void);

		int         number;
		int         mode;
		int         skim;
		int         pnr;
		int         connect;
		int         count;
		Double_List max_dist;
		Double_List walk_time;
		Db_File    *mode_file;
	};
	typedef vector <Mode_Group>    Mode_Array;
	typedef Mode_Array::iterator   Mode_Itr;

	Mode_Array mode_array;

	//---- stop zone file ----

	bool stop_zone_flag;
	int stop_zone_mode, stop_zone_offset;
	Db_File stop_zone_file;

	//---- methods ----

	void Read_Zone (void);
	void Read_Node (void);
	void Read_Extra (void);
	void Read_Link (void);
	void Walk_Access (void);
	void Read_Station (void);
	void Read_Slug (void);
	void Read_Skims (Skim_Itr &itr);
	void Process_Origin (Zone_Map_Itr &itr);
	
	void Zone_Report (void);
	void Zone_Header (void);

	void Station_Report (void);
	void Station_Header (void);
};
#endif
