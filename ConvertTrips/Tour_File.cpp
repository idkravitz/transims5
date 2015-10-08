//********************************************************* 
//	Tour_File.cpp - Tour File Input/Output
//*********************************************************

#include "Tour_File.hpp"

//-----------------------------------------------------------
//	Tour_File constructors
//-----------------------------------------------------------

Tour_File::Tour_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Tour_File::Tour_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Tour_File::Setup (void)
{
	File_Type ("Tour File");
	File_ID ("Tour");

	hhold = person = tour = purpose = mode = origin = destination = stop_out = stop_in = start = end = group = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Tour_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PURPOSE", DB_INTEGER, 2);
	Add_Field ("MODE", DB_INTEGER, 2, MODE_CODE, true);
	Add_Field ("ORIGIN", DB_INTEGER, 10);
	Add_Field ("DESTINATION", DB_INTEGER, 10);
	Add_Field ("STOP_OUT", DB_INTEGER, 10);
	Add_Field ("STOP_IN", DB_INTEGER, 10);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("RETURN", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("GROUP", DB_INTEGER, 2);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Tour_File::Set_Field_Numbers (void)
{
	hhold = Required_Field (HHOLD_FIELD_NAMES);
	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field (TOUR_FIELD_NAMES);
	purpose = Optional_Field (PURPOSE_FIELD_NAMES);
	mode = Optional_Field (MODE_FIELD_NAMES);
	origin = Optional_Field (ORIGIN_FIELD_NAMES);
	destination = Optional_Field (DESTINATION_FIELD_NAMES);
	stop_out = Optional_Field ("STOP_OUT");
	stop_in = Optional_Field ("STOP_IN");
	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field ("RETURN", END_FIELD_NAMES);
	group = Optional_Field ("GROUP", "NUMBER", "PURPOSE");	
	
	//---- set default units ----

	Set_Units (start, SECONDS);
	Set_Units (end, SECONDS);
	Set_Units (mode, MODE_CODE);
	return (true);
}
