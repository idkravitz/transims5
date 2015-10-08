//*********************************************************
//	TransitDiff.hpp - Compare two transit networks
//*********************************************************

#ifndef TransitDiff_HPP
#define TransitDiff_HPP

#include "Data_Service.hpp"

//---------------------------------------------------------
//	TransitDiff - execution class definition
//---------------------------------------------------------

class TransitDiff : public Data_Service
{
public:

	TransitDiff (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum TransitDiff_Keys { 
		COMPARE_NODE_FILE = 1, COMPARE_NODE_FORMAT,	COMPARE_LINK_FILE, COMPARE_LINK_FORMAT,
		COMPARE_TRANSIT_STOP_FILE, COMPARE_TRANSIT_STOP_FORMAT,
		COMPARE_TRANSIT_ROUTE_FILE, COMPARE_TRANSIT_ROUTE_FORMAT,
		COMPARE_TRANSIT_SCHEDULE_FILE, COMPARE_TRANSIT_SCHEDULE_FORMAT,
		COMPARE_TRANSIT_DRIVER_FILE, COMPARE_TRANSIT_DRIVER_FORMAT,
		NEW_TRANSIT_DIFFERENCE_FILE, NEW_TRANSIT_DIFFERENCE_FORMAT
	};
	virtual void Program_Control (void);

private:
	enum TransitDiff_Reports { FIRST_REPORT = 1, SECOND_REPORT };

	bool driver_flag;

	Node_File     compare_node_file;
	Link_File     compare_link_file;
	Stop_File     compare_stop_file;
	Line_File     compare_line_file;
	Schedule_File compare_schedule_file;
	Driver_File   compare_driver_file;

	Node_Array    compare_node_array;
	Link_Array    compare_link_array;
	Dir_Array     compare_dir_array;
	Stop_Array    compare_stop_array;
	Line_Array    compare_line_array;

	Int_Map       compare_node_map;
	Int_Map       compare_link_map;
	Int_Map       compare_dir_map;
	Int_Map       compare_stop_map;
	Int_Map       compare_line_map;

	Db_Header     diff_file;

	void Read_Node (void);
	void Read_Link (void);
	void Read_Stop (void);
	void Read_Line (void);
	void Read_Schedule (void);
	void Read_Driver (void);

	void Compare_Routes (void);
	void First_Header (void);

	Link_Data * Set_Link_Direction (Db_Header *file, int &link, int &dir, int &offset, bool flag = false);
};
#endif
