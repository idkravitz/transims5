//*********************************************************
//	ZoneData.hpp - Zone Data Processing
//*********************************************************

#ifndef ZONEDATA_HPP
#define ZONEDATA_HPP

#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Projection_Service.hpp"
#include "TypeDefs.hpp"

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	ZoneData - execution class definition
//---------------------------------------------------------

class ZoneData : public Data_Service
{
public:

	ZoneData (void);
	virtual ~ZoneData (void);

	virtual void Execute (void);

protected:
	enum ZoneData_Keys { 
		COPY_EXISTING_FIELDS = 1, CLEAR_ALL_FIELDS, ZONE_FILE_HEADER, NEW_ZONE_FIELD, 
		CONVERSION_SCRIPT, DATA_FILE, DATA_FORMAT, DATA_JOIN_FIELD, ZONE_JOIN_FIELD, 
		BOUNDARY_POLYGON_FILE, SUM_ATTRIBUTE_DISTANCE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);
	
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum ZoneData_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	int num_data_files, num_polygons, num_sum_distance;
	bool copy_flag, script_flag, data_flag, polygon_flag, sum_flag;
	
	Db_Base_Array data_rec;
	Zone_File *input_file, *output_file;

	//---- data groups ----

	typedef struct {
		int group;
		int join_field;
		int zone_field;
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

	//---- sum attribute distance

	typedef struct {
		int group;
		double distance;
		Db_Sort_Array *data_db;
		Integers field_map;
	} Sum_Distance;

	typedef vector <Sum_Distance>      Sum_Array;
	typedef Sum_Array::iterator        Sum_Itr;

	Sum_Array sum_group;

	Projection_Service projection;	

	Db_File program_file;
	User_Program program;

	void Set_Files (void);
	void Read_Data (void);
	void Read_Polygons (void);
	void Sum_Zone (void);
	void Write_Zone (void);
};
#endif


