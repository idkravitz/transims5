//*********************************************************
//	LocationData.hpp - activity location data processing
//*********************************************************

#ifndef LOCATIONDATA_HPP
#define LOCATIONDATA_HPP

#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Subzone_File.hpp"
#include "Subzone_Data.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Zone_Location.hpp"
#include "Projection_Service.hpp"
#include "TypeDefs.hpp"

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	LocationData - execution class definition
//---------------------------------------------------------

class LocationData : public Data_Service
{
public:

	LocationData (void);
	virtual ~LocationData (void);

	virtual void Execute (void);

protected:
	enum LocationData_Keys { 
		COPY_EXISTING_FIELDS = 1, NEW_WALK_ACCESS_FIELD, MAX_WALK_DISTANCE, WALK_ACCESS_TIME_RANGE,
		NEW_USE_FLAG_FIELD, LINK_USE_FLAG_TYPES, SUBZONE_ZONE_FACTOR_FILE, NEW_SUBZONE_FIELD, 
		MAX_SUBZONE_DISTANCE, SUBZONE_DATA_FILE, SUBZONE_DATA_FORMAT, SUBZONE_DATA_FIELD, 
		SUBZONE_ZONE_FIELD, NEW_LOCATION_FIELD, CONVERSION_SCRIPT, DATA_FILE, DATA_FORMAT, 
		DATA_JOIN_FIELD, LOCATION_JOIN_FIELD, BOUNDARY_POLYGON_FILE, NEW_ZONE_LOCATION_MAP_FILE, 
		MINIMUM_ZONE_LOCATIONS, ZONE_BOUNDARY_FILE, ZONE_FIELD_NAME, ZONE_UPDATE_RANGE 
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);

private:
	enum LocationData_Reports { PRINT_SCRIPT = 1, PRINT_STACK, ZONE_CHECK };

	int num_subzone, num_data_files, walk_access_field, walk_distance;
	int zone_field, num_polygons, min_locations;
	bool copy_flag, script_flag, data_flag, polygon_flag, zone_file_flag, subzone_map_flag;
	bool walk_access_flag, walk_time_flag, boundary_flag, coverage_flag, use_flag, subzone_flag;
	bool zone_loc_flag, range_flag;

	Location_File *input_file, *output_file;

	Db_Header subzone_zone_file;
	Db_Base_Array data_rec;
	Arcview_File boundary_file;
	Zone_Location_Map zone_loc_map;

	Time_Periods walk_time;
	Integers use_code, use_field, stop_runs;

	Db_File program_file;
	User_Program program;

	//---- transit access data ----

	typedef struct {
		int weight;
		int distance;
		int count;
	} Loc_Walk_Data;

	typedef vector <Loc_Walk_Data>     Loc_Walk_Array;
	typedef Loc_Walk_Array::iterator   Loc_Walk_Itr;

	Loc_Walk_Array loc_walk_array;

	//---- subzone groups ----

	typedef struct {
		int group;
		Subzone_File *file;
		Subzone_Array data;
		Int2_Map data_map;
		Int_Dbl_Map loc_weight;
		Int_Dbl_Map zone_data;
		int loc_field;
		int max_distance;
		int data_field;
		int zone_field;
		bool float_flag;
	} Subzone_Group;
	
	typedef vector <Subzone_Group>       Sub_Group_Array;
	typedef Sub_Group_Array::iterator    Sub_Group_Itr;

	Sub_Group_Array sub_group;

	//---- data groups ----

	typedef struct {
		int group;
		int join_field;
		int loc_field;
		Db_Header *file;
		Db_Sort_Array *data_db;
	} Data_Group;

	typedef vector <Data_Group>          Data_Array;
	typedef Data_Array::iterator         Data_Itr;

	Data_Array data_group;

	//---- polygon groups ----

	typedef struct {
		int group;
		Arcview_File *file;
		Db_Sort_Array *data_db;
		Points_Map polygon;
	} Polygon_Group;

	typedef vector <Polygon_Group>     Polygon_Array;
	typedef Polygon_Array::iterator    Polygon_Itr;

	Polygon_Array polygons;

	Points_Map boundary_map;

	Int_Map loc_zone_map;
	Int_Dbl_Map loc_weight;
	I2_Dbl_Map subzone_weight;
	I2_Dbl_Map subzone_map;

	Data_Range zone_range;
	Projection_Service projection;	

	void Set_Files (void);
	void Read_Data (void);
	void Read_Subzone_Map (void);
	void Read_Subzone (void);
	void Read_Boundary (void);
	void Read_Polygons (void);
	void Walk_Access (void);
	void Subzone_Map_Weights (void);
	void Subzone_Weights (void);
	void Data_Processing (void);
	void Write_Location (void);
	void Zone_Locations (void);
	void Check_Zones (void);
};
#endif
