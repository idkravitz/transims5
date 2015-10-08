//*********************************************************
//	ArcSnapshot.cpp - ArcView Snapshot Files
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	ArcSnapShot constructor
//---------------------------------------------------------

ArcSnapshot::ArcSnapshot (void) : Data_Service (), Select_Service (), Draw_Service ()
{
	Program ("ArcSnapshot");
	Version (4);
	Title ("Generate ArcView Shapefiles from TRANSIMS Snapshots");

	System_File_Type required_files [] = {
		NODE, LINK, POCKET, VEHICLE_TYPE, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, CONNECTION, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_SUBAREA_POLYGON, SELECT_VEHICLE_TYPES, 0
	};
	int draw_service_keys [] = {
		LANE_WIDTH, CENTER_ONEWAY_LINKS, DRAW_VEHICLE_SHAPES, 0
	};
	Control_Key arcsnapshot_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ SNAPSHOT_FILE, "SNAPSHOT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SNAPSHOT_FORMAT, "SNAPSHOT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ OCCUPANCY_FILE, "OCCUPANCY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ OCCUPANCY_FORMAT, "OCCUPANCY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_ARC_SNAPSHOT_FILE, "NEW_ARC_SNAPSHOT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ CELL_SIZE, "CELL_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0, 3..10 meters", NO_HELP },
		{ ADD_PASSENGER_CIRCLE_SIZE, "ADD_PASSENGER_CIRCLE_SIZE", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ADD_PASSENGER_SQUARE_SIZE, "ADD_PASSENGER_SQUARE_SIZE", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PAD_FILE_TIME_LABEL, "PAD_FILE_TIME_LABEL", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ TIME_PROCESSING_METHOD, "TIME_PROCESSING_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "AT_INCREMENT", "AT_INCREMENT, TOTAL, MAXIMUM", NO_HELP },
		{ NEW_SNAPSHOT_FILE, "NEW_SNAPSHOT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_FORMAT, "NEW_SNAPSHOT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_LINK_SUMMARY_FILE, "NEW_LINK_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_SUMMARY_FORMAT, "NEW_LINK_SUMMARY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);
	Draw_Service_Keys (draw_service_keys);

	Key_List (arcsnapshot_keys);
	Report_List (reports);

	projection.Add_Keys ();

	snapshot_flag = occupancy_flag = output_flag = project_flag = false;
	pad_flag = sum_flag = circle_flag = square_flag = false;

	link_field = dir_field = offset_field = lane_field = type_field = cell_field = occ_field = start_field = end_field = -1;
	link_fld = start_fld = end_fld = circle_field = square_field = -1;
	num_links = num_sum = max_type = 0;
}

//---------------------------------------------------------
//	ArcSnapshot destructor
//---------------------------------------------------------

ArcSnapshot::~ArcSnapshot (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcSnapshot *exe = new ArcSnapshot ();

	return (exe->Start_Execution (commands, control));
}
