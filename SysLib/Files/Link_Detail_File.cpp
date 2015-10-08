//********************************************************* 
//	Link_Detail_File.cpp - Link Detail File Input/Output
//*********************************************************

#include "Link_Detail_File.hpp"

//-----------------------------------------------------------
//	Link_Detail_File constructors
//-----------------------------------------------------------

Link_Detail_File::Link_Detail_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Link_Detail_File::Link_Detail_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Detail_File::Setup (void)
{
	File_Type ("Link Detail File");
	File_ID ("Detail");

	link = dir = control = group = use = lanes = period = -1;
	left_merge = left = left_thru = thru = right_thru = right = right_merge = -1;
	lm_length = lt_length = rt_length = rm_length = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Detail_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("CONTROL", DB_INTEGER, 1, CONTROL_CODE, true);
	} else {
		Add_Field ("CONTROL", DB_STRING, 12, CONTROL_CODE);
	}
	Add_Field ("GROUP", DB_INTEGER, 2);
	Add_Field ("LM_LENGTH", DB_DOUBLE, 6.1);
	Add_Field ("L_MERGE", DB_INTEGER, 2);
	Add_Field ("LT_LENGTH", DB_DOUBLE, 6.1);
	Add_Field ("LEFT", DB_INTEGER, 2);
	Add_Field ("LEFT_THRU", DB_INTEGER, 2);
	Add_Field ("THRU", DB_INTEGER, 2);
	Add_Field ("RIGHT_THRU", DB_INTEGER, 2);
	Add_Field ("RIGHT", DB_INTEGER, 2);
	Add_Field ("RT_LENGTH", DB_DOUBLE, 6.1);
	Add_Field ("R_MERGE", DB_INTEGER, 2);
	Add_Field ("RM_LENGTH", DB_DOUBLE, 6.1);
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
		Add_Field ("LANES", DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
		Add_Field ("LANES", DB_STRING, 8, LANE_RANGE_CODE);
	}
	Add_Field ("PERIOD", DB_STRING, STRING_FIELD_SIZE);

	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Detail_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");

	if (link < 0) return (false);
	
	dir = LinkDir_Type_Field ();
	control = Optional_Field ("CONTROL", "TYPE", "INT_TYPE");
	group = Optional_Field ("GROUP", "TYPE");
	lm_length = Optional_Field ("LM_LENGTH", "LM_LEN");
	left_merge = Optional_Field ("L_MERGE", "LEFT_MERGE", "LT_MERGE");
	lt_length = Optional_Field ("LT_LENGTH", "LT_LEN", "LEFT_LEN");
	left = Optional_Field ("LEFT", "LEFT_TURN", "L");
	left_thru = Optional_Field ("LEFT_THRU", "SHARED_LEFT", "LT");
	thru = Optional_Field ("THRU", "T");
	right_thru = Optional_Field ("RIGHT_THRU", "SHARED_RIGHT", "RT");
	right = Optional_Field ("RIGHT", "R");
	rt_length = Optional_Field ("RT_LENGTH", "RT_LEN", "RIGHT_LEN");
	right_merge = Optional_Field ("R_MERGE", "RIGHT_MERGE", "RT_MERGE");
	rm_length = Optional_Field ("RM_LENGTH", "RM_LEN");
	use = Optional_Field ("USE", "USE_TYPE", "VEHICLE");
	lanes = Optional_Field ("LANES", "USE_LANES", "LANE");
	period = Optional_Field ("PERIOD", "USE_PERIOD", "USE_TIME");

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (control, CONTROL_CODE);
	Set_Units (lm_length, METERS);
	Set_Units (lt_length, METERS);
	Set_Units (rt_length, METERS);
	Set_Units (rm_length, METERS);
	Set_Units (use, USE_CODE);
	if (Version () > 40) Set_Units (lanes, LANE_RANGE_CODE);

	return (true);
}
