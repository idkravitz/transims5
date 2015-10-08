//********************************************************* 
//	Turn_Vol_File.cpp - Turn Volume File Input/Output
//*********************************************************

#include "Turn_Vol_File.hpp"

//-----------------------------------------------------------
//	Turn_Vol_File constructors
//-----------------------------------------------------------

Turn_Vol_File::Turn_Vol_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Turn_Vol_File::Turn_Vol_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Turn_Vol_File::Setup (void)
{
	File_Type ("Turn Volume File");
	File_ID ("Turn");

	node = link = to_link = start = end = volume = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Turn_Vol_File::Create_Fields (void) 
{
	Add_Field ("NODE", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_Field ("TO_LINK", DB_INTEGER, 10);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("VOLUME", DB_INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Turn_Vol_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE");
	link = Optional_Field (IN_LINK_FIELD_NAMES);
	to_link = Optional_Field (OUT_LINK_FIELD_NAMES);
	volume = Optional_Field ("VOLUME", "TURN_VOL", "TURN", "COUNT");

	if (node < 0 || link < 0 || to_link < 0 || volume < 0) return (false);

	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);

	//---- set default units ----

	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);

	return (true);
}
