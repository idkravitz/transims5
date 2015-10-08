//*********************************************************
//	Relocate.hpp - Update activity locations
//*********************************************************

#ifndef RELOCATE_HPP
#define RELOCATE_HPP

#include "Data_Service.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"

#define MIN_DIFF	100		//---- 100 meters ----

//---------------------------------------------------------
//	Relocate - execution class definition
//---------------------------------------------------------

class Relocate : public Data_Service
{
public:
	Relocate (void);
	virtual ~Relocate (void);

	virtual void Execute (void);

protected:
	enum Relocate_Keys { 
		//ZONE_RELOCATION_FILE = 1, NEW_HOUSEHOLD_LIST;
		OLD_NODE_FILE = 1, OLD_NODE_FORMAT, OLD_SHAPE_FILE, OLD_SHAPE_FORMAT, OLD_LINK_FILE, OLD_LINK_FORMAT,
		OLD_LOCATION_FILE, OLD_LOCATION_FORMAT, OLD_PARKING_FILE, OLD_PARKING_FORMAT, OLD_ACCESS_FILE, OLD_ACCESS_FORMAT,
		NEW_LOCATION_MAP_FILE, NEW_PARKING_MAP_FILE
	};

	virtual void Program_Control (void);

private:

	bool trip_flag, plan_flag, vehicle_flag, select_flag, loc_map_flag, park_map_flag, shape_flag, access_flag;

	Node_File old_node_file;
	Link_File old_link_file;
	Location_File old_loc_file;
	Parking_File old_park_file;
	Shape_File old_shape_file;
	Access_File old_acc_file;

	Int_Map  old_node_map, old_shape_map, old_link_map, old_dir_map, old_park_map, old_loc_map, old_acc_map;
	Int2_Map old_ab_map;

	Node_Array          old_node_array;
	Shape_Array         old_shape_array;
	Link_Array          old_link_array;
	Dir_Array           old_dir_array;
	Parking_Array       old_park_array;
	Location_Array      old_loc_array;
	Access_Array        old_acc_array;

	Db_Header new_loc_file;
	Db_Header new_park_file;

	Int_Map old_new_loc, old_new_park;

	Points points;

	//---- methods ----

	void Old_Nodes (void);
	void Old_Shapes (void);
	void Old_Links (void);
	void Old_Parking_Lots (void);
	void Old_Locations (void);
	void Old_Access_Links (void);

	XYZ Link_Offset (Link_Data *link_ptr, int dir, double offset, double side);

	void Match_Locations (void);
	void Map_Network (void);
	void Read_Trip (void);
	void Read_Plan (void);
	void Read_Vehicle (void);
};

#endif
