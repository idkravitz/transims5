//********************************************************* 
//	Lane_Use_File.cpp - Lane Use File Input/Output
//*********************************************************

#include "Lane_Use_File.hpp"

//-----------------------------------------------------------
//	Lane_Use_File constructors
//-----------------------------------------------------------

Lane_Use_File::Lane_Use_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Lane_Use_File::Lane_Use_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Lane_Use_File::Setup (void)
{
	File_Type ("Lane Use File");
	File_ID ("LaneUse");

	link = dir = lanes = type = use = min_type = max_type = min_trav = max_trav = start = end = offset = length = toll = rate = min_delay = max_delay = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Lane_Use_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("LANES", DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
		Add_Field ("TYPE", DB_UNSIGNED, 1, RESTRICT_CODE, true);
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
	} else {
		Add_Field ("LANES", DB_STRING, 8, LANE_RANGE_CODE);
		Add_Field ("TYPE", DB_STRING, 10, RESTRICT_CODE);
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
	}
	Add_Field ("MIN_TYPE", DB_UNSIGNED, 1, VEH_TYPE, true);
	Add_Field ("MAX_TYPE", DB_UNSIGNED, 1, VEH_TYPE, true);
	Add_Field ("MIN_TRAV", DB_UNSIGNED, 1, NO_UNITS, true);
	Add_Field ("MAX_TRAV", DB_UNSIGNED, 1, NO_UNITS, true);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("LENGTH", DB_DOUBLE, 8.1, METERS);
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	Add_Field ("TOLL", DB_UNSIGNED, 2, CENTS, true);
	Add_Field ("TOLL_RATE", DB_DOUBLE, 4.1, CENT_KM, true);
	Add_Field ("MIN_DELAY", DB_DOUBLE, 8.1, SECONDS);
	Add_Field ("MAX_DELAY", DB_DOUBLE, 8.1, SECONDS);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Lane_Use_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	lanes = Required_Field ("LANES", "LANE");
	use = Required_Field ("USE", "VEHICLE");

	if (link < 0 || lanes < 0 || use < 0) return (false);

	//---- optional fields ----

	type = Optional_Field ("TYPE", "RESTRICT");
	min_type = Optional_Field ("MIN_TYPE", "MIN_VEHTYPE", "MIN_VEH_TYPE");
	max_type = Optional_Field ("MAX_TYPE", "MAX_VEHTYPE", "MAX_VEH_TYPE");
	min_trav = Optional_Field ("MIN_TRAV", "TRAVELER1", "MIN_TRAVELER");
	max_trav = Optional_Field ("MAX_TRAV", "TRAVELER2", "MAX_TRAVELER");
	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	offset = Optional_Field ("OFFSET");
	length = Optional_Field ("LENGTH");
	toll = Optional_Field ("TOLL", "COST", "CENTS");
	rate = Optional_Field ("TOLL_RATE", "RATE", "COST_RATE");
	min_delay = Optional_Field ("MIN_DELAY", "FIXED", "DELAY");
	max_delay = Optional_Field ("MAX_DELAY", "VARIABLE", "RANDOM");

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	if (Version () > 40) Set_Units (lanes, LANE_RANGE_CODE);
	Set_Units (use, USE_CODE);
	Set_Units (type, RESTRICT_CODE);
	Set_Units (min_type, VEH_TYPE);
	Set_Units (max_type, VEH_TYPE);
	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (length, METERS);
	Set_Units (offset, METERS);
	Set_Units (toll, CENTS);
	Set_Units (rate, CENT_KM);
	Set_Units (min_delay, SECONDS);
	Set_Units (max_delay, SECONDS);

	return (true);
}
