//********************************************************* 
//	Turn_Pen_File.cpp - Turn Penalty File Input/Output
//*********************************************************

#include "Turn_Pen_File.hpp"

//-----------------------------------------------------------
//	Turn_Pen_File constructors
//-----------------------------------------------------------

Turn_Pen_File::Turn_Pen_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Turn_Pen_File::Turn_Pen_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Turn_Pen_File::Setup (void)
{
	File_Type ("Turn Penalty File");
	File_ID ("Turn");

	link = dir = to_link = start = end = use = min_type = max_type = penalty = in_node = out_node = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Turn_Pen_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("TO_LINK", DB_INTEGER, 10);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
	}
	Add_Field ("MIN_TYPE", DB_UNSIGNED, 1, VEH_TYPE, true);
	Add_Field ("MAX_TYPE", DB_UNSIGNED, 1, VEH_TYPE, true);
	Add_Field ("PENALTY", DB_UNSIGNED, 2, IMPEDANCE, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Turn_Pen_File::Set_Field_Numbers (void)
{
	in_node = Optional_Field ("FROM", "FROM_NODE", "FROMNODE");
	out_node = Optional_Field ("TO", "TO_NODE", "TONODE");

	if (in_node >= 0 && out_node >= 0) {
		link = Optional_Field (IN_LINK_FIELD_NAMES);
		to_link = Optional_Field (OUT_LINK_FIELD_NAMES);
	} else {
		link = Required_Field (IN_LINK_FIELD_NAMES);
		to_link = Required_Field (OUT_LINK_FIELD_NAMES);

		if (link < 0 || to_link < 0) return (false);
	}
	if (link >= 0 && to_link >= 0) {
		dir = LinkDir_Type_Field ();
	} else {
		dir = Required_Field ("DIR", "NODE", "AT", "THRU");
		
		if (dir < 0) return (false);
	}
	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	use = Optional_Field ("USE", "VEHICLE");
	min_type = Optional_Field ("MIN_TYPE", "MIN_VEHTYPE", "MIN_VEH_TYPE");
	max_type = Optional_Field ("MAX_TYPE", "MAX_VEHTYPE", "MAX_VEH_TYPE");
	penalty = Optional_Field ("PENALTY", "DELAY");

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (use, USE_CODE);
	Set_Units (min_type, VEH_TYPE);
	Set_Units (max_type, VEH_TYPE);
	Set_Units (penalty, IMPEDANCE);

	return (true);
}
