//*********************************************************
//	ExportPlans.hpp - export plan data in various formats
//*********************************************************

#ifndef EXPORTPLANS_HPP
#define EXPORTPLANS_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Matrix_Data.hpp"
#include "Db_File.hpp"

#include "fstream"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	ExportPlans - execution class definition
//---------------------------------------------------------

class ExportPlans : public Data_Service, public Select_Service
{
public:
	ExportPlans (void);
	virtual ~ExportPlans (void);

	virtual void Execute (void);

protected:
	enum ExportPlans_Keys { 
		NEW_VISSIM_XML_FILE = 1, LOCATION_ZONE_MAP_FILE, SUBZONE_WEIGHT_FILE, NEW_ZONE_LOCATION_FILE
	};
	virtual void Program_Control (void);

private:
	bool vissim_flag, select_flag, map_flag, zone_loc_flag, subzone_flag;
	
	Db_File xml_file, map_file, zone_loc_file, subzone_file;
	Int_Map loc_zone_map;

	Trip_Table_Map trip_tables;

	typedef struct {
		int zone;
		double weight;
	} Subzone_Weight;

	typedef vector <Subzone_Weight>       Subzone_Array;
	typedef Subzone_Array::iterator       Subzone_Itr;

	typedef map <int, Subzone_Array>      Subzone_Map;
	typedef pair <int, Subzone_Array>     Subzone_Map_Data;
	typedef Subzone_Map::iterator         Subzone_Map_Itr;
	typedef pair <Subzone_Map_Itr, bool>  Subzone_Map_Stat;

	Subzone_Map subzone_map;

	//---- methods ----

	void Select_Links (void);
	void Read_Map (void);
	void Read_Subzone (void);
	void Read_Plans (void);
	void Write_VISSIM (void);
};
#endif
