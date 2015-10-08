//*********************************************************
//	ModeChoice.cpp - mode choice processing
//*********************************************************

#include "ModeChoice.hpp"

int tc_status;

//---------------------------------------------------------
//	ModeChoice constructor
//---------------------------------------------------------

ModeChoice::ModeChoice (void) : Execution_Service (), Select_Service ()
{
	Program ("ModeChoice");
	Version (14);
	Title ("Mode Choice Processing");
	
	int select_service_keys [] = {
		SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TRIP_FILE, "TRIP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_FORMAT, "TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },

		{ NEW_TRIP_FILE, "NEW_TRIP_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_FORMAT, "NEW_TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },
		{ SELECT_TRIP_TABLES, "SELECT_TRIP_TABLES", LEVEL0, OPT_KEY, TEXT_KEY, "ALL", "", NO_HELP },
		{ SKIM_FILE, "SKIM_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SKIM_FORMAT, "SKIM_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },
		{ ZONE_FILE, "ZONE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_FORMAT, "ZONE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MODE_CONSTANT_FILE, "MODE_CONSTANT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MODE_CHOICE_SCRIPT, "MODE_CHOICE_SCRIPT", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SEGMENT_MAP_FILE, "SEGMENT_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ORIGIN_MAP_FIELD, "ORIGIN_MAP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "SEGMENT", "", NO_HELP },
		{ DESTINATION_MAP_FIELD, "DESTINATION_MAP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "SEGMENT", "", NO_HELP },
		{ TRIP_PURPOSE_LABEL, "TRIP_PURPOSE_LABEL", LEVEL0, OPT_KEY, TEXT_KEY, "Peak Home-Based Work", "", NO_HELP },
		{ TRIP_PURPOSE_NUMBER, "TRIP_PURPOSE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..100", NO_HELP },
		{ TRIP_TIME_PERIOD, "TRIP_TIME_PERIOD", LEVEL0, OPT_KEY, INT_KEY, "1", "1..100", NO_HELP },
		{ PRIMARY_MODE_CHOICE, "PRIMARY_MODE_CHOICE", LEVEL0, REQ_KEY, TEXT_KEY, "", "", NO_HELP },
		{ MODE_CHOICE_NEST, "MODE_CHOICE_NEST", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NESTING_COEFFICIENT, "NESTING_COEFFICIENT", LEVEL1, OPT_KEY, FLOAT_KEY, "0.5", "0.0..1.0", NO_HELP },
		{ VEHICLE_TIME_VALUE, "VEHICLE_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "-0.02", "0, -0.04..-0.01", NO_HELP },
		{ WALK_TIME_VALUE, "WALK_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ DRIVE_ACCESS_VALUE, "DRIVE_ACCESS_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ WAIT_TIME_VALUE, "WAIT_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ LONG_WAIT_VALUE, "LONG_WAIT_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ TRANSFER_TIME_VALUE, "TRANSFER_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP }, 
		{ PENALTY_TIME_VALUE, "PENALTY_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ TERMINAL_TIME_VALUE, "TERMINAL_TIME_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ TRANSFER_COUNT_VALUE, "TRANSFER_COUNT_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0, -1.0..-0.01", NO_HELP },
		{ COST_VALUE_TABLE, "COST_VALUE_TABLE", LEVEL1, OPT_KEY, FLOAT_KEY, "0.0", "0, -5.0..0.0", NO_HELP },
		{ MODE_ACCESS_MARKET, "MODE_ACCESS_MARKET", LEVEL1, OPT_KEY, TEXT_KEY, "", "SOV, SR2, SR3...", NO_HELP },
		{ NEW_TABLE_MODES, "NEW_TABLE_MODES", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ OUTPUT_TRIP_FACTOR, "OUTPUT_TRIP_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "1.0..1000.0", NO_HELP },
		{ NEW_MODE_SUMMARY_FILE, "NEW_MODE_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MARKET_SEGMENT_FILE, "NEW_MARKET_SEGMENT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MODE_SEGMENT_FILE, "NEW_MODE_SEGMENT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_FTA_SUMMIT_FILE, "NEW_FTA_SUMMIT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PRODUCTION_FILE, "NEW_PRODUCTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PRODUCTION_FORMAT, "NEW_PRODUCTION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_ATTRACTION_FILE, "NEW_ATTRACTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ATTRACTION_FORMAT, "NEW_ATTRACTION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ CALIBRATION_TARGET_FILE, "CALIBRATION_TARGET_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ CALIBRATION_SCALING_FACTOR, "CALIBRATION_SCALING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "1.0..5.0", NO_HELP },
		{ MAX_CALIBRATION_ITERATIONS, "MAX_CALIBRATION_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "20", "1..1000", NO_HELP },
		{ CALIBRATION_EXIT_RMSE, "CALIBRATION_EXIT_RMSE", LEVEL0, OPT_KEY, FLOAT_KEY, "5.0", "0.01..50.0", NO_HELP },
		{ NEW_MODE_CONSTANT_FILE, "NEW_MODE_CONSTANT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_CALIBRATION_DATA_FILE, "NEW_CALIBRATION_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"MODE_CHOICE_SCRIPT",
		"MODE_CHOICE_STACK",
		"MODE_SUMMARY_REPORT",
		"MARKET_SEGMENT_REPORT",
		"CALIBRATION_REPORT",
		"TARGET_DATA_REPORT",
		"MODE_VALUE_SUMMARY",
		"SEGMENT_VALUE_SUMMARY",
		""
	};
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);

	org_map_field = des_map_field = segment_field = zone_field = market_field = -1;
	num_modes = num_tables = num_access = num_market = header_value = zones = 0;
	max_iter = 1;
	time_field = walk_field = auto_field = wait_field = lwait_field = xwait_field = tpen_field = term_field = -1;
	cost_field = xfer_field = bias_field = pef_field = cbd_field = const_field = tab_field = -1;
	time_value = walk_value = drive_value = wait_value = lwait_value = xwait_value = tpen_value = term_value = 0.0;
	xfer_value = scale_fac = 0.0;
	trip_factor = 1.0;
	summary_flag = market_flag = segment_flag = constant_flag = mode_value_flag = seg_value_flag = false;
	calib_flag = calib_seg_flag = calib_tab_flag = calib_report = output_flag = data_flag = mode_seg_flag = false;
	prod_flag = attr_flag = false;
}

//---------------------------------------------------------
//	ModeChoice destructor
//---------------------------------------------------------

ModeChoice::~ModeChoice (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ModeChoice *exe = new ModeChoice ();

	return (exe->Start_Execution (commands, control));
}
