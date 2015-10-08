//********************************************************* 
//	Traveler_File.cpp - Traveler File Input/Output
//*********************************************************

#include "Traveler_File.hpp"

//-----------------------------------------------------------
//	Traveler_File constructors
//-----------------------------------------------------------

Traveler_File::Traveler_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Traveler_File::Traveler_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Traveler_File::Setup (void)
{
	File_Type ("Traveler File");
	File_ID ("Traveler");

	hhold = person = tour = trip = mode = time = distance = speed = link = dir = lane = offset = route = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Traveler_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("TRIP", DB_INTEGER, 1, NO_UNITS, true);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_INTEGER, 1, MODE_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 12, MODE_CODE);
	}
	Add_Field ("TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("DISTANCE", DB_DOUBLE, 8.1, METERS);
	Add_Field ("SPEED", DB_DOUBLE, 5.1, KPH);
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

bool Traveler_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	trip = Required_Field (TRIP_FIELD_NAMES);
	time = Required_Field ("TIME", "TOD");

	if (hhold < 0 || trip < 0 || time < 0) return (false);

	//---- optional fields ----

	mode = Optional_Field (MODE_FIELD_NAMES);
	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field (TOUR_FIELD_NAMES);

	distance = Optional_Field ("DISTANCE", "LENGTH");
	speed = Optional_Field ("SPEED");

	link = Optional_Field ("LINK");
	dir = Optional_Field ("DIR", "NODE");
	lane = Optional_Field ("LANE");
	offset = Optional_Field ("OFFSET");
	route = Optional_Field ("ROUTE", "LINE", "TRANSIT");
	
	//---- set default units ----

	Set_Units (time, DAY_TIME);
	Set_Units (mode, MODE_CODE);
	Set_Units (lane, LANE_ID_CODE);
	Set_Units (distance, METERS);
	Set_Units (offset, METERS);
	Set_Units (speed, MPS);
	return (true);
}
