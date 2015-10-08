//********************************************************* 
//	Occupancy_File.cpp - Cell Occupancy File Input/Output
//*********************************************************

#include "Occupancy_File.hpp"

//-----------------------------------------------------------
//	Occupancy_File constructors
//-----------------------------------------------------------

Occupancy_File::Occupancy_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Occupancy_File::Occupancy_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Occupancy_File::Setup (void)
{
	File_Type ("Occupancy File");
	File_ID ("Occupancy");

	link = dir = start = end = lane = offset = occupancy = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Occupancy_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	if (Code_Flag ()) {
		Add_Field ("LANE", DB_UNSIGNED, 1, LANE_ID_CODE, true);
	} else {
		Add_Field ("LANE", DB_STRING, 4, LANE_ID_CODE);
	}
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	Add_Field ("OCCUPANCY", DB_INTEGER, 10);
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Occupancy_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	end = Required_Field (END_FIELD_NAMES);
	lane = Optional_Field ("LANE");
	offset = Optional_Field ("OFFSET", "DISTANCE");

	if (link < 0 || end < 0 || lane < 0 || offset < 0) return (false);

	//---- optional fields ----

	start = Optional_Field (START_FIELD_NAMES);

	occupancy = Optional_Field ("OCCUPANCY", "COUNT");

	dir = LinkDir_Type_Field ();

	//---- set default units ----

	Set_Units (lane, LANE_ID_CODE);
	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (offset, METERS);

	return (true);
}
