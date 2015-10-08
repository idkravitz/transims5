//********************************************************* 
//	Event_File.cpp - Event File Input/Output
//*********************************************************

#include "Event_File.hpp"

//-----------------------------------------------------------
//	Event_File constructors
//-----------------------------------------------------------

Event_File::Event_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Event_File::Event_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Event_File::Setup (void)
{
	File_Type ("Event File");
	File_ID ("Event");

	hhold = person = tour = trip = mode = type = schedule = actual = link = dir = lane = offset = route = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Event_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("TRIP", DB_INTEGER, 1, NO_UNITS, true);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_INTEGER, 1, MODE_CODE, true);
		Add_Field ("EVENT", DB_INTEGER, 1, EVENT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 12, MODE_CODE);
		Add_Field ("EVENT", DB_STRING, 20, EVENT_CODE);
	}
	Add_Field ("SCHEDULE", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("ACTUAL", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("LANE", DB_UNSIGNED, 1, LANE_ID_CODE, true);
	} else {
		Add_Field ("LANE", DB_STRING, 3, LANE_ID_CODE);
	}
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	Add_Field ("ROUTE", DB_INTEGER, 10);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Event_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	trip = Required_Field (TRIP_FIELD_NAMES);
	type = Required_Field ("EVENT", "TYPE");
	actual = Required_Field ("ACTUAL", "TIME");

	if (hhold < 0 || trip < 0 || type < 0 || actual < 0) return (false);

	//---- optional fields ----

	mode = Optional_Field (MODE_FIELD_NAMES);
	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field (TOUR_FIELD_NAMES);
	schedule = Optional_Field ("SCHEDULE", "PLAN");

	link = Optional_Field ("LINK");
	dir = Optional_Field ("DIR", "NODE");
	lane = Optional_Field ("LANE");
	offset = Optional_Field ("OFFSET");
	route = Optional_Field ("ROUTE", "LINE", "TRANSIT");
	
	//---- set default units ----

	Set_Units (schedule, DAY_TIME);
	Set_Units (actual, DAY_TIME);
	Set_Units (mode, MODE_CODE);
	Set_Units (type, EVENT_CODE);
	Set_Units (lane, LANE_ID_CODE);
	Set_Units (offset, METERS);
	return (true);
}
