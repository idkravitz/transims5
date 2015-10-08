//*********************************************************
//	LineSum.cpp - summarize transit ridership data
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	LineSum constructor
//---------------------------------------------------------

LineSum::LineSum (void) : Execution_Service ()
{
	Program ("LineSum");
	Version (17);
	Title ("Transit Ridership Summary");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ PEAK_RIDERSHIP_FILE, "PEAK_RIDERSHIP_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PEAK_RIDERSHIP_FORMAT, "PEAK_RIDERSHIP_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OFFPEAK_RIDERSHIP_FILE, "OFFPEAK_RIDERSHIP_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OFFPEAK_RIDERSHIP_FORMAT, "OFFPEAK_RIDERSHIP_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_PEAK_RIDERSHIP_FILE, "NEW_PEAK_RIDERSHIP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PEAK_RIDERSHIP_FORMAT, "NEW_PEAK_RIDERSHIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_OFFPEAK_RIDERSHIP_FILE, "NEW_OFFPEAK_RIDERSHIP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_OFFPEAK_RIDERSHIP_FORMAT, "NEW_OFFPEAK_RIDERSHIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TOTAL_RIDERSHIP_FILE, "NEW_TOTAL_RIDERSHIP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TOTAL_RIDERSHIP_FORMAT, "NEW_TOTAL_RIDERSHIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ STOP_NAME_FILE, "STOP_NAME_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STOP_NAME_FORMAT, "STOP_NAME_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ LINE_REPORT_TITLE, "LINE_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Line Report", "Report Title", NO_HELP },
		{ LINE_REPORT_LINES, "LINE_REPORT_LINES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", NAME_RANGE, NO_HELP }, 
		{ LINE_REPORT_MODES, "LINE_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ LINE_REPORT_ALL_NODES, "LINE_REPORT_ALL_NODES", LEVEL1, OPT_KEY, BOOL_KEY, "False", BOOL_RANGE, NO_HELP },
		{ LINK_REPORT_TITLE, "LINK_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Link Report", "Report Title", NO_HELP },
		{ LINK_REPORT_LINKS, "LINK_REPORT_LINKS", LEVEL1, OPT_KEY, LIST_KEY, "", "e.g., 100-200, 300-400-500", NO_HELP }, 
		{ LINK_REPORT_MODES, "LINK_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ LINK_REPORT_LINES, "LINK_REPORT_LINES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", NAME_RANGE, NO_HELP }, 
		{ LINK_REPORT_ONEWAY, "LINK_REPORT_ONEWAY", LEVEL1, OPT_KEY, BOOL_KEY, "False", BOOL_RANGE, NO_HELP }, 
		{ ON_OFF_REPORT_TITLE, "ON_OFF_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Boarding Report", "Report Title", NO_HELP },
		{ ON_OFF_REPORT_STOPS, "ON_OFF_REPORT_STOPS", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ ON_OFF_REPORT_MODES, "ON_OFF_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ ON_OFF_REPORT_DETAILS, "ON_OFF_REPORT_DETAILS", LEVEL1, OPT_KEY, BOOL_KEY, "False", "TRUE/FALSE/MODE, YES/NO/MODE, 1/0/2, T/F/M, Y/N/M", NO_HELP }, 
		{ NEW_ON_OFF_REPORT_FILE, "NEW_ON_OFF_REPORT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ON_OFF_REPORT_FORMAT, "NEW_ON_OFF_REPORT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ ACCESS_REPORT_TITLE, "ACCESS_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Access Report", "Report Title", NO_HELP },
		{ ACCESS_REPORT_STOPS, "ACCESS_REPORT_STOPS", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ ACCESS_REPORT_MODES, "ACCESS_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ ACCESS_REPORT_DETAILS, "ACCESS_REPORT_DETAILS", LEVEL1, OPT_KEY, BOOL_KEY, "False", "TRUE/FALSE/MODE, YES/NO/MODE, 1/0/2, T/F/M, Y/N/M", NO_HELP }, 
		{ NEW_ACCESS_REPORT_FILE, "NEW_ACCESS_REPORT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ACCESS_REPORT_FORMAT, "NEW_ACCESS_REPORT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ STOP_REPORT_TITLE, "STOP_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Stop Report", "Report Title", NO_HELP },
		{ STOP_REPORT_STOPS, "STOP_REPORT_STOPS", LEVEL1, OPT_KEY, LIST_KEY, "", RANGE_RANGE, NO_HELP }, 
		{ STOP_REPORT_MODES, "STOP_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ STOP_REPORT_LINES, "STOP_REPORT_LINES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", NAME_RANGE, NO_HELP }, 
		{ STOP_REPORT_TRANSFERS, "STOP_REPORT_TRANSFERS", LEVEL1, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP }, 
		{ TOTAL_REPORT_TITLE, "TOTAL_REPORT_TITLE", LEVEL1, OPT_KEY, TEXT_KEY, "Total Report", "Report Title", NO_HELP },
		{ TOTAL_REPORT_LINES, "TOTAL_REPORT_LINES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", "e.g., LINE1, LINE2, LINE1..LINE10", NO_HELP }, 
		{ TOTAL_REPORT_MODES, "TOTAL_REPORT_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ NEW_TOTAL_REPORT_FILE, "NEW_TOTAL_REPORT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TOTAL_REPORT_FORMAT, "NEW_TOTAL_REPORT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_LINK_RIDER_FILE, "NEW_LINK_RIDER_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_RIDER_FORMAT, "NEW_LINK_RIDER_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ LINK_RIDER_MODES, "LINK_RIDER_MODES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP }, 
		{ LINK_RIDER_LINES, "LINK_RIDER_LINES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", NAME_RANGE, NO_HELP }, 
		{ LINK_RIDER_PEAK_HOURS, "LINK_RIDER_PEAK_HOURS", LEVEL1, OPT_KEY, TIME_KEY, "6.0 hours", "1.0..10.0 hours", NO_HELP }, 
		{ LINK_RIDER_PEAK_FACTOR, "LINK_RIDER_PEAK_FACTOR", LEVEL1, OPT_KEY, FLOAT_KEY, "1.0", "1.0..1.5", NO_HELP }, 
		{ LINK_RIDER_PEAK_CAPACITY, "LINK_RIDER_PEAK_CAPACITY", LEVEL1, OPT_KEY, FLOAT_KEY, "1.0", "1.0..1000.0", NO_HELP }, 
		{ LINK_RIDER_OFFPEAK_HOURS, "LINK_RIDER_OFFPEAK_HOURS", LEVEL1, OPT_KEY, TIME_KEY, "10.0 hours", "1.0..20.0 hours", NO_HELP }, 
		{ LINK_RIDER_XY_FILE, "LINK_RIDER_XY_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_RIDER_XY_FORMAT, "LINK_RIDER_XY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ LINK_RIDER_SIDE_OFFSET, "LINK_RIDER_SIDE_OFFSET", LEVEL1, OPT_KEY, FLOAT_KEY, "0.0 feet", "0.0..1000 feet", NO_HELP },
		{ LINK_SHAPE_FILE, "LINK_SHAPE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_SHAPE_ANODE, "LINK_SHAPE_ANODE", LEVEL0, OPT_KEY, TEXT_KEY, "ANODE", "", NO_HELP },
		{ LINK_SHAPE_BNODE, "LINK_SHAPE_BNODE", LEVEL0, OPT_KEY, TEXT_KEY, "BNODE", "", NO_HELP },
		{ SERVICE_FILE, "SERVICE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SERVICE_FORMAT, "SERVICE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SERVICE_LINE_FIELD, "SERVICE_LINE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "LINE", "", NO_HELP },
		{ SERVICE_PEAK_FIELD, "SERVICE_PEAK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "PEAK", "", NO_HELP },
		{ SERVICE_OFFPEAK_FIELD, "SERVICE_OFFPEAK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "OFFPEAK", "", NO_HELP },
		{ BASE_ROUTE_FILE, "BASE_ROUTE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ BASE_ROUTE_FORMAT, "BASE_ROUTE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DBASE", FORMAT_RANGE, FORMAT_HELP },
		{ ALTERNATIVE_ROUTE_FILE, "ALTERNATIVE_ROUTE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ALTERNATIVE_ROUTE_FORMAT, "ALTERNATIVE_ROUTE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DBASE", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"LINE_REPORT",
		"LINK_REPORT",
		"ON_OFF_REPORT",
		"ACCESS_REPORT",
		"STOP_REPORT",
		"TOTAL_REPORT",
		"DIFFERENCE_REPORT",
		""
	};
	Key_List (keys);
	Report_List (reports);

	projection.Add_Keys ();

	on_off_flag = access_flag = station_flag = shape_flag = service_flag = false;
	anode_field = bnode_field = line_fld = peak_fld = offpeak_fld = -1;
	report_code = 0;

	Page_Size (172, 65);

	int ignore_keys [] = {
		TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT, UNITS_OF_MEASURE, 
		RANDOM_NUMBER_SEED,	MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, 0
	};
	Ignore_Keys (ignore_keys);
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LineSum *exe = new LineSum ();

	return (exe->Start_Execution (commands, control));
}

//---------------------------------------------------------
//	line map less than operator
//---------------------------------------------------------

bool operator < (LineSum::Sort_Key left, LineSum::Sort_Key right)
{
	if (left.mode < right.mode) return (true);
	if (left.mode == right.mode) {
		if (left.name < right.name) return (true);
		if (left.name == right.name) {
			if (left.leg < right.leg) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	board map less than operator
//---------------------------------------------------------

bool operator < (LineSum::On_Off_Key left, LineSum::On_Off_Key right)
{
	if (left.stop < right.stop) return (true);
	if (left.stop == right.stop) {
		if (left.mode < right.mode) return (true);
		if (left.mode == right.mode) {
			if (left.name < right.name) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	access map less than operator
//---------------------------------------------------------

bool operator < (LineSum::Access_Key left, LineSum::Access_Key right)
{
	if (left.stop < right.stop) return (true);
	if (left.stop == right.stop) {
		if (left.mode < right.mode) return (true);
		if (left.mode == right.mode) {
			if (left.node < right.node) return (true);
		}
	}
	return (false); 
}
