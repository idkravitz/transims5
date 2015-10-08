//********************************************************* 
//	Travel_Time_File.cpp - Travel Time Input/Output
//*********************************************************

#include "Travel_Time_File.hpp"

//-----------------------------------------------------------
//	Travel_Time_File constructors
//-----------------------------------------------------------

Travel_Time_File::Travel_Time_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Travel_Time_File::Travel_Time_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Travel_Time_File::Setup (void)
{
	File_Type ("Travel Time File");
	File_ID ("TravelTime");

	hhold = person = tour = trip = mode = base_start = started = start_diff = -1;
	base_end = ended = end_diff = mid_trip = ttime = -1;
	purpose = constraint = start_link = end_link = trip_start =  trip_end = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Travel_Time_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("TRIP", DB_INTEGER, 1, NO_UNITS, true);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_INTEGER, 1, MODE_CODE, true);
		Add_Field ("PURPOSE", DB_INTEGER, 1, NO_UNITS, true);
		Add_Field ("CONSTRAINT", DB_INTEGER, 1, CONSTRAINT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 12, MODE_CODE);
		Add_Field ("PURPOSE", DB_INTEGER, 2);
		Add_Field ("CONSTRAINT", DB_STRING, 14, CONSTRAINT_CODE);
	}
	Add_Field ("START_LINK", DB_INTEGER, 10);
	Add_Field ("END_LINK", DB_INTEGER, 10);

	Add_Field ("TRIP_START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("BASE_START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("STARTED", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("START_DIFF", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TRIP_END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("BASE_END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("ENDED", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END_DIFF", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("MID_TRIP", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TTIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Travel_Time_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	trip = Required_Field (TRIP_FIELD_NAMES);

	if (hhold < 0 || trip < 0) return (false);

	//---- optional fields ----

	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field (TOUR_FIELD_NAMES);
	mode = Optional_Field (MODE_FIELD_NAMES);
	purpose = Optional_Field (PURPOSE_FIELD_NAMES);
	constraint = Optional_Field (CONSTRAINT_FIELD_NAMES);

	//---- optional fields ----

	start_link = Optional_Field ("START_LINK", "STARTLINK", "ORIGIN");
	end_link = Optional_Field ("END_LINK", "ENDLINK", "DESTINATION");
	trip_start = Optional_Field ("TRIP_START", "TRIPSTART");
	base_start = Optional_Field ("BASE_START", "BASESTART", "SCHEDULE1");
	started = Optional_Field ("STARTED", "START", "ACTUAL1");
	start_diff = Optional_Field ("START_DIFF", "STARTDIFF", "DIFF1");
	trip_end = Optional_Field ("TRIP_END", "TRIPEND");
	base_end = Optional_Field ("BASE_END", "BASEEND", "SCHEDULE2");
	ended = Optional_Field ("ENDED", "END", "ACTUAL2");
	end_diff = Optional_Field ("END_DIFF", "ENDDIFF", "DIFF2");
	mid_trip = Optional_Field ("MID_TRIP", "MIDTRIP", "TIME", "TOD");
	ttime = Optional_Field ("TTIME", "DURATION", "TRAVELTIME");

	Set_Units (mode, MODE_CODE);
	Set_Units (constraint, CONSTRAINT_CODE);	
	Set_Units (trip_start, SECONDS);
	Set_Units (base_start, SECONDS);
	Set_Units (started, SECONDS);
	Set_Units (start_diff, SECONDS);
	Set_Units (trip_end, SECONDS);
	Set_Units (base_end, SECONDS);
	Set_Units (ended, SECONDS);
	Set_Units (end_diff, SECONDS);
	Set_Units (mid_trip, SECONDS);
	Set_Units (ttime, SECONDS);
	return (true);
}
