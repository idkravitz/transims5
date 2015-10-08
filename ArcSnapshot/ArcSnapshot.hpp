//*********************************************************
//	ArcSnapshot.hpp - ArcView Snapshot Files
//*********************************************************

#ifndef ARCSNAPSHOT_HPP
#define ARCSNAPSHOT_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Draw_Service.hpp"
#include "Projection.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Time_Periods.hpp"
#include "Db_Array.hpp"
#include "Dtime.hpp"
#include "Shape_Tools.hpp"
#include "Snapshot_File.hpp"
#include "Occupancy_File.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	ArcSnapshot - execution class definition
//---------------------------------------------------------

class ArcSnapshot : public Data_Service, public Select_Service, public Draw_Service
{
public:
	ArcSnapshot (void);
	virtual ~ArcSnapshot (void);

	virtual void Execute (void);

protected:
	enum ArcSnapshot_Keys { 
		SNAPSHOT_FILE = 1, SNAPSHOT_FORMAT, OCCUPANCY_FILE, OCCUPANCY_FORMAT, NEW_ARC_SNAPSHOT_FILE, 
		CELL_SIZE, ADD_PASSENGER_CIRCLE_SIZE, ADD_PASSENGER_SQUARE_SIZE, PAD_FILE_TIME_LABEL, 
		TIME_PROCESSING_METHOD, NEW_SNAPSHOT_FILE, NEW_SNAPSHOT_FORMAT, NEW_LINK_SUMMARY_FILE, 
		NEW_LINK_SUMMARY_FORMAT		
	};
	virtual void Program_Control (void);

private:
	enum Method_Types { AT_INCREMENT, TOTAL, MAXIMUM };

	Snapshot_File snapshot_file, new_snapshot_file, dummy_snapshot;
	Occupancy_File occupancy_file;
	Db_Header summary_file;

	Arcview_File arcview_snapshot;

	Points points;

	Dtime max_time;
	double cell_size;
	int method, nsaved, max_type;
	int link_field, dir_field, offset_field, lane_field, type_field, cell_field, occ_field, start_field, end_field;
	int link_fld, start_fld, end_fld, num_links, num_sum, circle_field, square_field;
	bool snapshot_flag, occupancy_flag, output_flag, project_flag;
	bool pad_flag, sum_flag, circle_flag, square_flag;

	Doubles veh_length;
	Integers veh_fld, per_fld;
	Ints_Array vehicles, persons;

	Projection project_coord;
	Projection_Service projection;	

	String shapename;
	
	typedef struct {
		int lanes;
		int cells;
		int total;
		int max_total;
		Integers data;
		Integers maximum;
	} Cell_Data;

	typedef vector <Cell_Data>         Cell_Data_Array;
	typedef Cell_Data_Array::iterator  Cell_Data_Itr;	

	Cell_Data_Array cell_array;

	//---- methods ----

	void Set_Subarea (void);
	void Set_Veh_Types (void);
	void Create_Cells (void);
	void Read_Snapshot (void);
	void Sum_Snapshot (void);
	void Maximum_Check (void);
	void Write_Snapshot (int period);
	void Write_Summary (int period);
	void Draw_Vehicle (void);
};
#endif


