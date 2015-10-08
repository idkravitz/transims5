//*********************************************************
//	NetPrep.hpp - Network Preparation Utility
//*********************************************************

#ifndef NETPREP_HPP
#define NETPREP_HPP

#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Data_Range.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "TypeDefs.hpp"
#include "Link_Detail_File.hpp"
#include "Db_File.hpp"
#include "Best_List.hpp"

#define MAX_AT		100
#define MAX_FUNCL	100
#define MAX_LANE	100

//---------------------------------------------------------
//	NetPrep - execution class definition
//---------------------------------------------------------

class NetPrep : public Data_Service
{
public:

	NetPrep (void);
	virtual ~NetPrep (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum NetPrep_Keys { 
		INPUT_LINK_FILE = 1, INPUT_LINK_FORMAT, INPUT_NODE_FILE, INPUT_NODE_FORMAT,
		INPUT_ZONE_FILE, INPUT_ZONE_FORMAT, INPUT_SPDCAP_FILE, INPUT_SPDCAP_FORMAT, INPUT_UNITS_OF_MEASURE,	
		FACILITY_INDEX_FIELD, AREA_TYPE_INDEX_FIELD, 
		CONVERSION_SCRIPT, INTERNAL_ZONE_RANGE, KEEP_ZONE_CONNECTORS_AS_LOCALS, 
		FIRST_NODE_NUMBER, FIRST_LINK_NUMBER, SPEED_FACTOR_BY_FACILITY, 
		SPEED_ROUNDING_INCREMENT, COORDINATE_RESOLUTION, MAXIMUM_LENGTH_TO_XY_RATIO, 
		MAXIMUM_SHAPE_ANGLE, MINIMUM_SHAPE_LENGTH, DROP_DEAD_END_LINKS, DROP_SHORT_LINKS, 
		SPLIT_LARGE_LOOPS, COLLAPSE_SHAPE_NODES, COLLAPSE_DIVIDED_ARTERIALS, 
		LOCAL_THRU_SEGMENT_LENGTHS, LOCAL_SELECTION_SPACING, 
		KEEP_NODE_RANGE, KEEP_NODE_FILE, KEEP_LINK_RANGE, KEEP_LINK_FILE,
		DELETE_NODE_RANGE, DELETE_NODE_FILE, DELETE_LINK_RANGE, DELETE_LINK_FILE,
		NEW_LINK_DETAIL_FILE, NEW_LINK_NODE_LIST_FILE, 
		TRANSIT_TIME_PERIODS, PERIOD_TRAVEL_TIMES, FIRST_ROUTE_NUMBER, INPUT_ROUTE_FORMAT,
		ROUTE_MODE_MAP, INPUT_ROUTE_FILE, ROUTE_PERIOD_MAP, FLIP_ROUTE_FLAG, 
	};
	virtual void Program_Control (void);

	virtual bool Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba);
	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual int  Put_Link_Data (Link_File &file, Link_Data &data);
	virtual int  Put_Node_Data (Node_File &file, Node_Data &data);
	virtual int  Put_Shape_Data (Shape_File &file, Shape_Data &data);

private:
	enum NetPrep_Reports { SCRIPT_REPORT = 1, STACK_REPORT, CHECK_TYPES, NODE_TYPES };

	int cap_map [MAX_AT] [MAX_FUNCL] [MAX_LANE];
	int spd_map [MAX_AT] [MAX_FUNCL] [MAX_LANE];
	Double_List spd_fac;
	int spd_inc;

	int fac_fld, at_fld, drop_length, split_length, num_loops, new_link, new_node, max_angle, min_length;
	int straight_diff, thru_diff, forward_diff, short_links, cross_min, cross_max, next_loop, num_ratio;
	int num_periods, new_route, num_match;
	bool convert_flag, spdcap_flag, link_flag, node_flag, zone_flag, route_flag, new_zone_flag, connector_flag;
	bool link_shape_flag, node_shape_flag, zone_shape_flag, route_shape_flag, int_zone_flag, centroid_flag;
	bool units_flag, keep_node_flag, keep_link_flag, drop_node_flag, drop_link_flag, shape_flag;
	bool length_flag, split_flag, collapse_flag, drop_flag, short_flag, loop_flag, spacing_flag;
	bool divided_flag, segment_flag, speed_flag, detail_flag, link_node_flag, time_flag;
	double length_ratio, resolution;

	List_Array node_list;
	Integer_List area_spacing, thru_length;
	Integers select_links;

	Db_Header link_file, node_file, zone_file;
	Db_Base spdcap_file;
	Db_File script_file;

	Link_Detail_File detail_file;
	Db_File link_node_file;

	Arcview_File link_shape_file, node_shape_file, zone_shape_file;

	User_Program convert;
	Data_Range int_zone_range, keep_node_range, keep_link_range, drop_node_range, drop_link_range;
	
	Projection projection;
	Projection_Service proj_service;

	//---- link node list ----

	typedef struct {
		int      link;
		Int_List nodes;
	} Link_Nodes;

	typedef vector <Link_Nodes>        Link_Node_Array;
	typedef Link_Node_Array::iterator  Link_Node_Itr;

	Link_Node_Array link_node_array;

	//---- network selection data ----

	typedef struct {
		Integers links;
		int      node;
		int      length;
		bool     flag;
		bool     end_flag;

		void Clear (void) {
			links.clear (); node = length = 0; flag = end_flag = false;
		}
	} Crossing_Data;

	typedef struct {
		Crossing_Data *left1, *left2, *left3, *right1, *right2, *right3;
		int left_len12, left_len23, right_len12, right_len23, left_at12, left_at23, right_at12, right_at23;

		void Clear () {
			left1 = left2 = left3 = right1 = right2 = right3 = 0;
			left_len12 = left_len23 = right_len12 = right_len23 = 0;
			left_at12 = left_at23 = right_at12 = right_at23 = 0;
		}
	} Spacing_Data;

	//---- transit line files ----
	
	Format_Type route_format;
	Integers mode_map;
	Time_Periods schedule_periods;

	typedef struct {
		bool oneway, flip;
		int mode;
		Dtime ttime, offset;
		String name, notes;
		Dtimes headway;
		Integers nodes;
	} Route_Data; 

	typedef struct {
		int group;
		Db_File *line_file;
		Integer_List period_map;
		bool flip;
	} File_Group;

	typedef vector <File_Group>        File_Group_Array;
	typedef File_Group_Array::iterator File_Group_Itr;

	File_Group_Array file_groups;

	void Input_SpdCap (void);
	void Input_Nodes (void);
	void Input_Zones (void);
	void Input_Links (void);
	void Input_Routes (void);
	void Save_Route (Route_Data &data);
	void Node_List (void);
	int  Closest_Zone (int node);
	bool Compare_Links (Link_Data *link_ptr, Link_Data *link2_ptr, int node = -1, bool near_flag = false);
	void Collapse_Nodes (void);
	void Drop_Links (void);
	void Short_Links (void);
	void Split_Loops (void);
	void Bearing_Update (void);
	void Thru_Links (void);
	void Local_Thru_Links (void);
	void Local_Spacing (void);
	int  Trace_Links (int dir_index, bool from_flag, Integers &links, bool &end_flag);
	int  Space_Processing (Spacing_Data &spacing, bool flag = false);
	bool Select_Best (Crossing_Data *s1, Crossing_Data *s2, Crossing_Data *o1 = 0, Crossing_Data *o2 = 0, bool set_flag = false);
	void Delete_Link (int link);
	void Write_Link_Nodes (void);

	void Check_Intersections (void);
	void Intersection_Header (void);

	void Node_Types (void);
	void Node_Header (void);
};
#endif
