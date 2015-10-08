//********************************************************* 
//	Ridership_File.cpp - Transit Ridership Input/Output
//*********************************************************

#include "Ridership_File.hpp"

//-----------------------------------------------------------
//	Ridership_File constructors
//-----------------------------------------------------------

Ridership_File::Ridership_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Ridership_File::Ridership_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Ridership_File::Setup (void)
{
	File_Type ("Transit Ridership File");
	File_ID ("Ridership");

	mode = route = run = stop = schedule = time = board = alight = load = factor = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Ridership_File::Create_Fields (void) 
{
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
	}
	Add_Field ("ROUTE", DB_INTEGER, 10);
	Add_Field ("RUN", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("STOP", DB_INTEGER, 10);
	Add_Field ("SCHEDULE", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("BOARD", DB_UNSIGNED, 2, NO_UNITS, true);
	Add_Field ("ALIGHT", DB_UNSIGNED, 2, NO_UNITS, true);
	Add_Field ("LOAD", DB_UNSIGNED, 2, NO_UNITS, true);
	Add_Field ("FACTOR", DB_DOUBLE, 5.2, RATIO);
	
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Ridership_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	route = Required_Field (ROUTE_FIELD_NAMES);
	run = Required_Field ("RUN", "TRIP");
	stop = Required_Field ("STOP", "STOP_ID");
	board = Required_Field ("BOARD", "ON", "GET_ON");
	alight = Required_Field ("ALIGHT", "OFF", "GET_OFF");

	if (route < 0 || run < 0 || stop < 0 || board < 0 || alight < 0) return (false);

	//---- optional fields ----

	mode = Optional_Field (MODE_FIELD_NAMES);
	schedule = Optional_Field ("SCHEDULE", "DEPART");
	time = Optional_Field ("TIME", "ARRIVE", "ACTUAL");
	load = Optional_Field ("LOAD", "RIDERS", "PASSENGERS", "IN");
	factor = Optional_Field ("FACTOR", "LOAD_FACTOR", "VC");

	//---- set default units ----

	Set_Units (mode, TRANSIT_CODE);
	Set_Units (schedule, DAY_TIME);
	Set_Units (time, DAY_TIME);
	Set_Units (factor, RATIO);

	return (true);
}
