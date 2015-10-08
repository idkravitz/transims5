//*********************************************************
//	ModeChoice.hpp - mode choice processing
//*********************************************************

#ifndef MODECHOICE_HPP
#define MODECHOICE_HPP

#include "Execution_Service.hpp"
#include "Select_Service.hpp"
#include "Db_Matrix.hpp"
#include "Db_Header.hpp"
#include "Db_Array.hpp"
#include "TypeDefs.hpp"
#include "User_Program.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	ModeChoice - execution class definition
//---------------------------------------------------------

class ModeChoice : public Execution_Service, public Select_Service
{
public:
	ModeChoice (void);
	virtual ~ModeChoice (void);

	virtual void Execute (void);

protected:
	enum ModeChoice_Keys { 
		TRIP_FILE = 1, TRIP_FORMAT, SELECT_TRIP_TABLES,
		NEW_TRIP_FILE, NEW_TRIP_FORMAT, SKIM_FILE, SKIM_FORMAT, 
		ZONE_FILE, ZONE_FORMAT,	MODE_CONSTANT_FILE, MODE_CHOICE_SCRIPT, 
		SEGMENT_MAP_FILE, ORIGIN_MAP_FIELD, DESTINATION_MAP_FIELD,
		TRIP_PURPOSE_LABEL, TRIP_PURPOSE_NUMBER, TRIP_TIME_PERIOD, 
		PRIMARY_MODE_CHOICE, MODE_CHOICE_NEST, NESTING_COEFFICIENT, 
		VEHICLE_TIME_VALUE, WALK_TIME_VALUE, DRIVE_ACCESS_VALUE,
		WAIT_TIME_VALUE, LONG_WAIT_VALUE, TRANSFER_TIME_VALUE, PENALTY_TIME_VALUE, 
		TERMINAL_TIME_VALUE, TRANSFER_COUNT_VALUE, COST_VALUE_TABLE, 
		MODE_ACCESS_MARKET, NEW_TABLE_MODES, OUTPUT_TRIP_FACTOR, NEW_MODE_SUMMARY_FILE,
		NEW_MARKET_SEGMENT_FILE, NEW_MODE_SEGMENT_FILE, NEW_FTA_SUMMIT_FILE, 
		NEW_PRODUCTION_FILE, NEW_PRODUCTION_FORMAT, NEW_ATTRACTION_FILE, NEW_ATTRACTION_FORMAT,
		CALIBRATION_TARGET_FILE, CALIBRATION_SCALING_FACTOR, MAX_CALIBRATION_ITERATIONS, 
		CALIBRATION_EXIT_RMSE, NEW_MODE_CONSTANT_FILE, NEW_CALIBRATION_DATA_FILE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum ModeChoice_Reports { PRINT_SCRIPT = 1, PRINT_STACK, MODE_SUMMARY, MARKET_REPORT, 
		CALIB_REPORT, TARGET_DATA, MODE_VALUES, SEGMENT_VALUES 
	};
	int num_modes, num_tables, num_nests, num_access, num_market;
	int org_map_field, des_map_field, segment_field, zone_field, zones, purpose, period, market_field;
	int time_field, walk_field, auto_field, wait_field, lwait_field, xwait_field, tpen_field, term_field;
	int cost_field, xfer_field, bias_field, pef_field, cbd_field, const_field, tab_field, header_value, max_iter;
	double time_value, walk_value, drive_value, wait_value, lwait_value, xwait_value, tpen_value, term_value;
	double xfer_value, scale_fac, exit_rmse, trip_factor;
	bool summary_flag, market_flag, segment_flag, constant_flag, mode_value_flag, seg_value_flag;
	bool calib_flag, calib_seg_flag, calib_tab_flag, calib_report, output_flag, data_flag, mode_seg_flag;
	bool summit_flag, prod_flag, attr_flag;

	Doubles cost_values, zero_values;
	Dbls_Array min_const, max_const;
	String purpose_label;
	Strings mode_names;
	Str_ID mode_id;
	
	Int2_Map segment_map;
	Ints_Array access_markets, nested_modes, output_table;
	Integers nest_mode, mode_nest, mode_nested;
	Doubles nest_levels, nest_coef;
	Integers market_group, table_map;

	Db_Matrix *trip_file, *new_file;
	Db_Matrices trip_files, skim_files;
	Db_Header zone_file, segment_file, target_file, constant_file, calib_file, prod_file, attr_file;
	Db_File script_file, summary_file, market_file, data_file, mode_seg_file;

	Db_Header summit_file;
	int summit_org, summit_des,summit_market, summit_total_trips, summit_motor_trips, summit_auto_exp;
	int summit_walk_market, summit_walk_share, summit_drive_market, summit_drive_share;

	Db_Sort_Array org_db, des_db;
	Db_Base_Array data_rec;

	User_Program program;

	//---- market segment summaries ----

	typedef vector <Dbls_Array>    Market_Seg;
	typedef Market_Seg::iterator   Mrkt_Seg_Itr;

	Market_Seg market_seg, seg_constant, calib_target, calib_trips, calib_const, value_sum;

	//---- zone summaries ----

	Dbls_Array prod_share, attr_share;

	//---- methods ----

	void Compile_Script (void);
	void Read_Constants (void);
	void Read_Zone (void);
	void Segment_Map (void);
	void Read_Targets (void);
	void Target_Header (void);
	void Choice_Process (int iter = 1, bool save_flag = true);
	bool Adjust_Constants (int iter = 1, bool save_flag = true);
	void Calib_Header (void);
	void Mode_Summary (int segment = 0);
	void Mode_Header (void);
	void Value_Summary (int segment = 0);
	void Value_Header (void);
	void Write_Summary (void);
	void Write_Market (void);
	void Write_Mode_Segment (void);
	void Write_Zones (void);
};
#endif
