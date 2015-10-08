//*********************************************************
//	LineSum.hpp - summarize transit ridership data
//*********************************************************

#ifndef LINESUM_HPP
#define LINESUM_HPP

#include "Execution_Service.hpp"
#include "Partition_Files.hpp"
#include "Data_Range.hpp"
#include "Arcview_File.hpp"
#include "Projection_Service.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	LineSum - execution class definition
//---------------------------------------------------------

class LineSum : public Execution_Service
{
public:
	LineSum (void);

	virtual void Execute (void);

	struct Sort_Key {
		short  mode;
		short  leg;
		String name;
		Sort_Key (void) : mode(0), leg(0) {};
	};
	
	struct On_Off_Key {
		int    stop;
		int    mode;
		String name;
		On_Off_Key (void) : stop(0), mode(0) {};
	};

	struct Access_Key {
		int  stop;
		int  mode;
		int  node;
		Access_Key (void) : stop(0), mode(0), node(0) {};
	};

protected:
	enum LineSum_Keys { 
		PEAK_RIDERSHIP_FILE = 1, PEAK_RIDERSHIP_FORMAT, OFFPEAK_RIDERSHIP_FILE, OFFPEAK_RIDERSHIP_FORMAT, 
		NEW_PEAK_RIDERSHIP_FILE, NEW_PEAK_RIDERSHIP_FORMAT, NEW_OFFPEAK_RIDERSHIP_FILE, NEW_OFFPEAK_RIDERSHIP_FORMAT,
		NEW_TOTAL_RIDERSHIP_FILE, NEW_TOTAL_RIDERSHIP_FORMAT, STOP_NAME_FILE, STOP_NAME_FORMAT,
		LINE_REPORT_TITLE, LINE_REPORT_LINES, LINE_REPORT_MODES, LINE_REPORT_ALL_NODES,
		LINK_REPORT_TITLE, LINK_REPORT_LINKS, LINK_REPORT_MODES, LINK_REPORT_LINES, LINK_REPORT_ONEWAY, 
		ON_OFF_REPORT_TITLE, ON_OFF_REPORT_STOPS, ON_OFF_REPORT_MODES, ON_OFF_REPORT_DETAILS,
		NEW_ON_OFF_REPORT_FILE, NEW_ON_OFF_REPORT_FORMAT,
		ACCESS_REPORT_TITLE, ACCESS_REPORT_STOPS, ACCESS_REPORT_MODES, ACCESS_REPORT_DETAILS,
		NEW_ACCESS_REPORT_FILE, NEW_ACCESS_REPORT_FORMAT,
		STOP_REPORT_TITLE, STOP_REPORT_STOPS, STOP_REPORT_MODES, STOP_REPORT_LINES, STOP_REPORT_TRANSFERS,
		TOTAL_REPORT_TITLE, TOTAL_REPORT_LINES, TOTAL_REPORT_MODES, NEW_TOTAL_REPORT_FILE, NEW_TOTAL_REPORT_FORMAT,
		NEW_LINK_RIDER_FILE, NEW_LINK_RIDER_FORMAT, LINK_RIDER_MODES, LINK_RIDER_LINES, 
		LINK_RIDER_PEAK_HOURS, LINK_RIDER_PEAK_FACTOR, LINK_RIDER_PEAK_CAPACITY, LINK_RIDER_OFFPEAK_HOURS, 
		LINK_RIDER_XY_FILE, LINK_RIDER_XY_FORMAT, LINK_RIDER_SIDE_OFFSET,
		LINK_SHAPE_FILE, LINK_SHAPE_ANODE, LINK_SHAPE_BNODE,
		SERVICE_FILE, SERVICE_FORMAT, SERVICE_LINE_FIELD, SERVICE_PEAK_FIELD, SERVICE_OFFPEAK_FIELD,
		BASE_ROUTE_FILE, BASE_ROUTE_FORMAT, ALTERNATIVE_ROUTE_FILE, ALTERNATIVE_ROUTE_FORMAT,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum LineSum_Reports { LINE_REPORT = 1, LINK_REPORT, ON_OFF_REPORT, ACCESS_REPORT, STOP_REPORT, TOTAL_REPORT, DIFFERENCE_REPORT };

	bool on_off_flag, access_flag, station_flag, service_flag;
	int report_code, line_fld, peak_fld, offpeak_fld;

	typedef Partition_Data <Db_Header> File_List;
	typedef File_List::iterator        File_Itr;
	
	File_List peak_list, offpeak_list, base_routes, alt_routes;
	Db_Header new_peak_file, new_offpeak_file, new_total_file, stop_name_file, service_file;
	Str_Map stop_names;
	Str_ID service_map;

	//---- line report data ----

	struct Line_Report_Data {
		int        number;
		String     title;
		Data_Range modes;
		Strings    lines;
		bool       node_flag;
		bool       all_modes;
		Line_Report_Data (void) : number(0), node_flag(0), all_modes(0) {};
	};
	typedef vector <Line_Report_Data>   Line_Report_Array;
	typedef Line_Report_Array::iterator Line_Report_Itr;

	Line_Report_Array line_report_array;

	//---- line data sort ----
		
	struct Ride_Data {
		int ride;
		int on;
		int off;
		Ride_Data (void) : ride(0), on(0), off(0) {};
	};

	struct Leg_Data {
		int    a;
		int    b;
		short  time;
		short  dist;
		int    service;
		double runs;
		Ride_Data ab;
		Ride_Data ba;
		Leg_Data (void) : a(0), b(0), time(0), dist(0), runs(0), service(0) {};
	};

	//---- line map ----

	typedef map <Sort_Key, Leg_Data>   Line_Map;
	typedef pair <Sort_Key, Leg_Data>  Line_Map_Data;
	typedef Line_Map::iterator         Line_Map_Itr;
	typedef pair <Line_Map_Itr, bool>  Line_Map_Stat;

	Line_Map peak_map, offpeak_map;

	//---- link report data ----

	struct Link_Report_Data {
		int        number;
		bool       dir_flag;
		bool       all_modes;
		bool       all_lines;
		String     title;
		String     links;
		Integers   nodes;
		Data_Range modes;
		Strings    lines;
		Link_Report_Data (void) : number(0), dir_flag(0), all_modes(0), all_lines(0) {};
	};
	typedef vector <Link_Report_Data>   Link_Report_Array;
	typedef Link_Report_Array::iterator Link_Report_Itr;

	Link_Report_Array link_report_array;

	//---- board data sort ----

	struct On_Off_Data {
		int pk_on;
		int pk_off;
		int op_on;
		int op_off;
		On_Off_Data (void) : pk_on(0), pk_off(0), op_on(0), op_off(0) {};
	};
	typedef map <On_Off_Key, On_Off_Data>   On_Off_Map;
	typedef pair <On_Off_Key, On_Off_Data>  On_Off_Map_Data;
	typedef On_Off_Map::iterator           On_Off_Map_Itr;
	typedef pair <On_Off_Map_Itr, bool>    On_Off_Map_Stat;

	On_Off_Map on_off_map;

	//---- boarding report data ----

	struct On_Off_Report_Data {
		int        number;
		int        details;
		String     title;
		bool       all_stops;
		bool       all_modes;
		Data_Range stops;
		Data_Range modes;
		Db_Header  *file;
		On_Off_Report_Data (void) : number(0), details(0), all_stops(0), all_modes(0), file(0) {};
	};
	typedef vector <On_Off_Report_Data>   On_Off_Report_Array;
	typedef On_Off_Report_Array::iterator On_Off_Report_Itr;

	On_Off_Report_Array on_off_report_array;

	//---- access data sort ----

	typedef map <Access_Key, On_Off_Data>   Access_Map;
	typedef pair <Access_Key, On_Off_Data>  Access_Map_Data;
	typedef Access_Map::iterator            Access_Map_Itr;
	typedef pair <Access_Map_Itr, bool>     Access_Map_Stat;

	Access_Map access_map;

	//---- access report data ----

	struct Access_Report_Data {
		int        number;
		int        details;
		String     title;
		bool       all_stops;
		bool       all_modes;
		Data_Range stops;
		Data_Range modes;
		Db_Header  *file;
		Access_Report_Data (void) : number(0), details(0), all_stops(0), all_modes(0), file(0) {};
	};
	typedef vector <Access_Report_Data>   Access_Report_Array;
	typedef Access_Report_Array::iterator Access_Report_Itr;

	Access_Report_Array access_report_array;

	//---- stop report data ----

	struct Stop_Report_Data {
		int        number;
		String     title;
		bool       all_modes;
		bool       all_lines;
		Data_Range stops;
		Data_Range modes;
		Strings    lines;
		Data_Range transfers;
		Stop_Report_Data (void) : number(0), all_modes(0), all_lines(0) {};
	};
	typedef vector <Stop_Report_Data>   Stop_Report_Array;
	typedef Stop_Report_Array::iterator Stop_Report_Itr;

	Stop_Report_Array stop_report_array;

	//---- total report data ----

	struct Total_Report_Data {
		int        number;
		String     title;
		bool       all_modes;
		Strings    lines;
		Data_Range modes;
		Db_Header  *file;
		Total_Report_Data (void) : number(0), all_modes(0), file(0) {};
	};
	typedef vector <Total_Report_Data>   Total_Report_Array;
	typedef Total_Report_Array::iterator Total_Report_Itr;

	Total_Report_Array total_report_array;

	//---- link ridership files ----

	struct Link_Rider_Data {
		int          number;
		int          node_fld;
		int          xcoord_fld;
		int          ycoord_fld;
		double       peak_hours;
		double       peak_fac;
		double       peak_cap;
		double       offpeak;
		double       offset;
		bool         all_modes;
		bool         all_lines;
		bool         arcview_flag;
		Data_Range   modes;
		Strings      lines;
		Db_Header    *file;
		Db_Header    *xy_file;
		Arcview_File *arc_file;

		Link_Rider_Data (void) : number(0), node_fld(-1), xcoord_fld(-1), ycoord_fld(-1), 
			peak_hours(0), peak_fac(0), peak_cap(0), offpeak(0), offset(0), all_modes(0), 
			all_lines(0), arcview_flag(0), file(0), xy_file(0), arc_file(0) {};
	};
	typedef vector <Link_Rider_Data>   Link_Rider_Array;
	typedef Link_Rider_Array::iterator Link_Rider_Itr;

	Link_Rider_Array link_rider_array;

	//---- A-B map to Points ----

	bool shape_flag;
	int anode_field, bnode_field;
	Arcview_File link_shape;

	typedef map <Int2_Key, Points>         I2_Points_Map;
	typedef pair <Int2_Key, Points>        I2_Points_Map_Data;
	typedef I2_Points_Map::iterator        I2_Points_Map_Itr;
	typedef pair <I2_Points_Map_Itr, bool> I2_Points_Map_Stat;

	I2_Points_Map points_map;
	Projection_Service projection;

	//---- methods ----

	void Read_Shapes (void);
	void Read_Service (void);

	void Stop_Names (void);
	void Combine (int period, File_List &list, Db_Header &output);

	void Compare (void);
	void Compare_Header (void);

	void Line_Report (Line_Report_Data &report);
	void Line_Header (void);
	
	void Link_Report (Link_Report_Data &report);
	void Link_Header (void);
	
	void On_Off_Report (On_Off_Report_Data &report);
	void On_Off_Header (void);

	void Access_Report (Access_Report_Data &report);
	void Access_Header (void);

	void Stop_Report (Stop_Report_Data &report);
	void Stop_Header (void);

	void Total_Report (Total_Report_Data &report);
	void Total_Header (void);

	void Link_Rider (Link_Rider_Data &data);

	//---- route data ----

	struct Route_Data {
		String notes;
		int headway1;
		int ttime1;
		int headway2;
		int ttime2;
		int nodes;
		int match;

		Route_Data (void) : headway1(0), ttime1(0), headway2(0), ttime2(0), nodes(0), match(0) {};
	};
	typedef map <Sort_Key, Route_Data>   Route_Map;
	typedef pair <Sort_Key, Route_Data>  Route_Map_Data;
	typedef Route_Map::iterator          Route_Map_Itr;
	typedef pair <Route_Map_Itr, bool>   Route_Map_Stat;

	//---- TP+ Line Data ----

	struct TPPlus_Link {
		int  ride;
		int  on_a;
		int  off_a;
		int  on_b;
		int  off_b;
		TPPlus_Link (void) : ride(0), on_a(0), off_a(0), on_b(0), off_b(0) {};
	};
	struct TPPlus_Data {
		int    a;
		int    b;
		int    mode;
		int    service;
		String name;
		double runs;
		TPPlus_Link ab, ba;
		TPPlus_Data (void) : a(0), b(0), mode(0), runs(0) {};
	};
	typedef vector <TPPlus_Data>   TPPlus_Array;

	//---- line data ----

	struct Line_Data {
		int   a;
		int   b;
		short time;
		short dist;
		struct {
			Ride_Data pk;
			Ride_Data op;
		} ab, ba;

		Line_Data (void) : a(0), b(0), time(0), dist(0) {};
	};
	typedef vector <Line_Data>   Line_Array;
	typedef Line_Array::iterator Line_Itr;

	//---- link map ----

	struct Link_Data {
		int   ab_pk_ride;
		int   ab_op_ride;
		int   ba_pk_ride;
		int   ba_op_ride;
		Link_Data (void) : ab_pk_ride(0), ab_op_ride(0), ba_pk_ride(0), ba_op_ride(0) {};
	};
	typedef map <Sort_Key, Link_Data>   Link_Map;
	typedef pair <Sort_Key, Link_Data>  Link_Map_Data;
	typedef Link_Map::iterator          Link_Map_Itr;
	typedef pair <Link_Map_Itr, bool>   Link_Map_Stat;

	//---- rider map data ----

	struct Rider_Data {
		int    pk_time;
		int    pk_dist;
		int    op_time;
		int    op_dist;
		int    ab_pk_ride;
		int    ab_op_ride;
		int    ba_pk_ride;
		int    ba_op_ride;
		int    ab_pk_service;
		int    ab_op_service;
		int    ba_pk_service;
		int    ba_op_service;
		double ab_pk_runs;
		double ab_op_runs;
		double ba_pk_runs;
		double ba_op_runs;
		double max_load_fac;
		String max_line;
		Rider_Data (void) : pk_time(0), pk_dist(0), op_time(0), op_dist(0), ab_pk_ride(0), ab_op_ride(0),
			ba_pk_ride(0), ba_op_ride(0), ab_pk_service(0), ab_op_service(0), ba_pk_service(0), ba_op_service(0), 
			ab_pk_runs(0), ab_op_runs(0), ba_pk_runs(0), ba_op_runs(0), max_load_fac(0) {};
	};
	typedef map <Int2_Key, Rider_Data>   Rider_Map;
	typedef pair <Int2_Key, Rider_Data>  Rider_Map_Data;
	typedef Rider_Map::iterator          Rider_Map_Itr;
	typedef pair <Rider_Map_Itr, bool>   Rider_Map_Stat;

	//---- stop data sort ----

	struct Stop_Period {
		int arrive;
		int on;
		int off;
		int depart;
		Stop_Period (void) : arrive(0), on(0), off(0), depart(0) {};
	};
	struct Stop_Data {
		Stop_Period pk, op;
		Stop_Data (void) {};
	};
	typedef map <Sort_Key, Stop_Data>   Stop_Map;
	typedef pair <Sort_Key, Stop_Data>  Stop_Map_Data;
	typedef Stop_Map::iterator          Stop_Map_Itr;
	typedef pair <Stop_Map_Itr, bool>   Stop_Map_Stat;

	//---- total data array ----

	struct Total_Period {
		int dist;
		int time;
		int ab_on;
		int ba_on;
		int ab_max;
		int ba_max;
		Total_Period (void) : dist(0), time(0), ab_on(0), ba_on(0), ab_max(0), ba_max(0) {};
	};
	struct Total_Data {
		int    mode;
		String line;
		Total_Period pk, op;
		double  pmt;
		double  pht;
		Total_Data (void) : mode(0), pmt(0), pht(0) {};
	};
	typedef vector <Total_Data>   Total_Array;
	typedef Total_Array::iterator Total_Itr;
};

SYSLIB_API bool operator < (LineSum::Sort_Key left, LineSum::Sort_Key right);
SYSLIB_API bool operator < (LineSum::On_Off_Key left, LineSum::On_Off_Key right);
SYSLIB_API bool operator < (LineSum::Access_Key left, LineSum::Access_Key right);

#endif
