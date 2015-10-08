//*********************************************************
//	LinkData.hpp - Convert Link Data to Link ID
//*********************************************************

#ifndef LINKDATA_HPP
#define LINKDATA_HPP

#include "Data_Service.hpp"
#include "Link_Data_File.hpp"
#include "Best_List.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	LinkData - execution class definition
//---------------------------------------------------------

class LinkData : public Data_Service
{
public:
	LinkData (void);
	virtual ~LinkData (void);

	virtual void Execute (void);

protected:
	enum LinkData_Keys { 
		LINK_NODE_LIST_FILE = 1, DIRECTIONAL_DATA_FILE, DIRECTIONAL_DATA_FORMAT, 
		FROM_NODE_FIELD_NAME, TO_NODE_FIELD_NAME, VOLUME_DATA_FIELD_NAME, SPEED_DATA_FIELD_NAME,
		AB_VOLUME_FIELD_NAME, BA_VOLUME_FIELD_NAME, AB_SPEED_FIELD_NAME, BA_SPEED_FIELD_NAME,
		NEW_LINK_DATA_FILE, NEW_LINK_DATA_FORMAT, 
		NEW_AB_VOLUME_FIELD_NAME, NEW_BA_VOLUME_FIELD_NAME, NEW_AB_SPEED_FIELD_NAME, NEW_BA_SPEED_FIELD_NAME, 
		NEW_DIRECTIONAL_DATA_FILE, NEW_DIRECTIONAL_DATA_FORMAT
	};
	virtual void Program_Control (void);

private:

	bool volume_flag, speed_flag, two_way_flag, data_flag, custom_flag, output_flag; 
	int nequiv, nab, ndir, nlink;
	int from_field, to_field, num_fields;
	
	Integer_List vol_fields, spd_fields;
	Integer_List vol_ab_fields, vol_ba_fields, spd_ab_fields, spd_ba_fields;
	Integer_List ab_vol_fields, ba_vol_fields, ab_spd_fields, ba_spd_fields;

	Int2_Map data_map;

	typedef struct {
		int    link;
		int    anode;
		int    bnode;
		short  num_ab;
		short  num_ba;
		Doubles volume_ab;
		Doubles volume_ba;
		Doubles speed_ab;
		Doubles speed_ba;
	} Data;

	typedef vector <Data>         Data_Array;
	typedef Data_Array::iterator  Data_Itr;

	Data_Array data_array;

	Db_File link_node;
	Db_Header dir_file, new_dir_file;
	Link_Data_File data_file;

	void Read_Link_Nodes (void);
	void Read_Dir_Data (void);
	void Write_Dir_Data (void);
};
#endif
