//********************************************************* 
//	Parking_File.cpp - Parking File Input/Output
//*********************************************************

#include "Parking_File.hpp"

//-----------------------------------------------------------
//	Parking_File constructors
//-----------------------------------------------------------

Parking_File::Parking_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Parking_File::Parking_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Parking_File::Setup (void)
{
	File_Type ("Parking File");
	File_ID ("Parking");
	
	Nest (NESTED);
	Header_Lines (2);

	parking = link = dir = offset = type = -1;
	use = start = end = space = time_in = time_out = hourly = daily = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Parking_File::Create_Fields (void) 
{
	Add_Field ("PARKING", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	if (Code_Flag ()) {
		Add_Field ("TYPE", DB_UNSIGNED, 1, PARKING_CODE, true);
	} else {
		Add_Field ("TYPE", DB_STRING, 10, PARKING_CODE);
	}
	Add_Field ("NUM_NEST", DB_INTEGER, 2, NEST_COUNT);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true, NESTED);
	} else {
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE, false, NESTED);
	}
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format (), false, NESTED);
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format (), false, NESTED);
	Add_Field ("SPACE", DB_UNSIGNED, 2, NO_UNITS, true, NESTED);
	Add_Field ("TIME_IN", DB_TIME, 2, SECONDS, true, NESTED);
	Add_Field ("TIME_OUT", DB_TIME, 2, SECONDS, true, NESTED);
	Add_Field ("HOURLY", DB_UNSIGNED, 2, CENTS, true, NESTED);
	Add_Field ("DAILY", DB_UNSIGNED, 2, CENTS, true, NESTED);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Parking_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	parking = Required_Field (PARKING_FIELD_NAMES);
	link = Required_Field ("LINK");
	offset = Required_Field ("OFFSET");
	type = Required_Field ("TYPE", "STYLE");

	if (parking < 0 || link < 0 || offset < 0 || type < 0) return (false);

	//---- optional fields ----

	use = Optional_Field ("USE", "VEHICLE");
	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	space = Optional_Field ("CAPACITY", "SPACE");
	time_in = Optional_Field ("TIME_IN", "DELAY", "TIME", "PARK", "PARKTIME");
	time_out = Optional_Field ("TIME_OUT", "UNPARK", "UNPARKTIME");
	hourly = Optional_Field ("HOURLY", "RATE");
	daily = Optional_Field ("DAILY", "MAXIMUM");

	dir = LinkDir_Type_Field ();	
	Num_Nest_Field (Optional_Field (NUM_NEST_FIELD_NAMES));
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (offset, METERS);
	Set_Units (type, PARKING_CODE);
	Set_Units (use, USE_CODE);
	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (time_in, SECONDS);
	Set_Units (time_out, SECONDS);
	Set_Units (hourly, CENTS);
	Set_Units (daily, CENTS);

	return (true);
}
