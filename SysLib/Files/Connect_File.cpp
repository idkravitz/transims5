//********************************************************* 
//	Connect_File.cpp - link connection file input/output
//*********************************************************

#include "Connect_File.hpp"

//-----------------------------------------------------------
//	Connect_File constructors
//-----------------------------------------------------------

Connect_File::Connect_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Connect_File::Connect_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Connect_File::Setup (void)
{
	File_Type ("Connection File");
	File_ID ("Connect");

	link = dir = to_link = lanes = to_lanes = in_high = out_high = -1;
	type = penalty = speed = capacity = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Connect_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("TO_LINK", DB_INTEGER, 10);
	if (Code_Flag ()) {
		Add_Field ("LANES", DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
		Add_Field ("TO_LANES", DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
		Add_Field ("TYPE", DB_UNSIGNED, 1, CONNECT_CODE, true);
	} else {
		Add_Field ("LANES", DB_STRING, 8, LANE_RANGE_CODE);
		Add_Field ("TO_LANES", DB_STRING, 8, LANE_RANGE_CODE);
		Add_Field ("TYPE", DB_STRING, 8, CONNECT_CODE);
	}
	Add_Field ("PENALTY", DB_UNSIGNED, 2, IMPEDANCE, true);
	Add_Field ("SPEED", DB_DOUBLE, 5.1, KPH);
	Add_Field ("CAPACITY", DB_UNSIGNED, 8, VPH);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Connect_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field (IN_LINK_FIELD_NAMES);
	to_link = Required_Field (OUT_LINK_FIELD_NAMES);

	if (link < 0 || to_link < 0) return (false);

	//---- optional fields ----
	
	dir = LinkDir_Type_Field ();
	lanes = Optional_Field ("LANES", "IN_LOW", "INLANE", "LANE_IN", "LANEIN");
	to_lanes = Optional_Field ("TO_LANES", "OUT_LOW", "OUTLANE", "LANE_OUT", "LANEOUT");
	type = Optional_Field ("TYPE", "MOVEMENT", "TURN", "TURNTYPE", "TURN_TYPE");
	penalty = Optional_Field ("PENALTY", "DELAY");
	speed = Optional_Field ("SPEED", "MAX_SPD");
	capacity = Optional_Field ("CAPACITY", "SAT_FLOW");
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----
	
	if (Version () > 40) {
		Set_Units (lanes, LANE_RANGE_CODE);
		Set_Units (to_lanes, LANE_RANGE_CODE);
		in_high = out_high = -1;
	} else {
		in_high = Optional_Field ("IN_HIGH", "HIGH_IN", "LANES", "LANE_IN", "LANEIN");
		out_high = Optional_Field ("OUT_HIGH", "HIGH_OUT", "TO_LANES", "LANE_OUT", "LANEOUT");
	}
	Set_Units (type, CONNECT_CODE);
	Set_Units (penalty, IMPEDANCE);
	Set_Units (speed, MPS);
	Set_Units (capacity, VPH);

	return (true);
}
