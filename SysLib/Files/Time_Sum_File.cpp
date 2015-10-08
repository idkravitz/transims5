//********************************************************* 
//	Time_Sum_File.cpp - Event Time Summary Input/Output
//*********************************************************

#include "Time_Sum_File.hpp"

//-----------------------------------------------------------
//	Time_Sum_File constructors
//-----------------------------------------------------------

Time_Sum_File::Time_Sum_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Time_Sum_File::Time_Sum_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Time_Sum_File::Setup (void)
{
	File_Type ("Time Summary File");
	File_ID ("TimeSum");

	from_time = to_time = trip_start = started = start_diff = start_abs = sum_trip = trip_end = ended = -1;
	end_diff = end_abs = mid_trip = ttime = ttime_diff = ttime_abs = sum_error = avg_error = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Time_Sum_File::Create_Fields (void) 
{
	Add_Field ("FROM_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TO_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TRIP_START", DB_INTEGER, 10);
	Add_Field ("STARTED", DB_INTEGER, 10);
	Add_Field ("START_DIFF", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("START_ERROR", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TRIP_END", DB_INTEGER, 10);
	Add_Field ("ENDED", DB_INTEGER, 10);
	Add_Field ("END_DIFF", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END_ERROR", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("MID_TRIP", DB_INTEGER, 10);
	Add_Field ("TTIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TTIME_DIFF", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TTIME_ERROR", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("SUM_TRIPS", DB_INTEGER, 10);
	Add_Field ("SUM_ERROR", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("AVG_ERROR", DB_TIME, TIME_FIELD_SIZE, Time_Format ());

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Time_Sum_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	to_time = Required_Field ("TO_TIME", "TIME", "PERIOD", "END");
	ttime = Required_Field ("TTIME", "DURATION", "TRAVELTIME");

	if (to_time < 0 || ttime < 0) return (false);

	//---- optional fields ----

	from_time = Optional_Field ("FROM_TIME", "START");
	trip_start = Optional_Field ("TRIP_START", "TRIPSTART", "SCHEDULE1");
	started = Optional_Field ("STARTED", "START", "ACTUAL1");
	start_diff = Optional_Field ("START_DIFF", "STARTDIFF", "DIFF1");
	start_abs = Optional_Field ("START_ERROR", "STARTERROR", "DIFF1", "START_ABS");
	trip_end = Optional_Field ("TRIP_END", "TRIPEND", "SCHEDULE2");
	ended = Optional_Field ("ENDED", "END", "ACTUAL2");
	end_diff = Optional_Field ("END_DIFF", "ENDDIFF", "DIFF2");
	end_abs = Optional_Field ("END_ERROR", "ENDERROR", "DIFF2", "END_ABS");
	mid_trip = Optional_Field ("MID_TRIP", "TRIPS");
	ttime_diff = Optional_Field ("TTIME_DIFF", "DUR_DIFF");
	ttime_abs = Optional_Field ("TTIME_ERROR", "DUR_ERROR");
	sum_trip = Optional_Field ("SUM_TRIPS", "TRIPS");
	sum_error = Optional_Field ("SUM_ERROR", "ERROR");
	avg_error = Optional_Field ("AVG_ERROR", "NORMAL");

	Set_Units (from_time, SECONDS);
	Set_Units (to_time, SECONDS);
	Set_Units (start_diff, SECONDS);
	Set_Units (start_abs, SECONDS);
	Set_Units (end_diff, SECONDS);
	Set_Units (end_abs, SECONDS);
	Set_Units (ttime, SECONDS);
	Set_Units (ttime_diff, SECONDS);
	Set_Units (ttime_abs, SECONDS);
	Set_Units (sum_error, SECONDS);
	Set_Units (avg_error, SECONDS);
	return (true);
}

//-----------------------------------------------------------
//	Link_Time_Sum_File constructors
//-----------------------------------------------------------

Link_Time_Sum_File::Link_Time_Sum_File (Access_Type access, Format_Type format) : 
	Time_Sum_File (access, format)
{
	Setup ();
}

Link_Time_Sum_File::Link_Time_Sum_File (string filename, Access_Type access, Format_Type format) : 
	Time_Sum_File (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Time_Sum_File::Setup (void)
{
	File_Type ("Link Time Summary File");
	File_ID ("LinkTime");

	link = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Time_Sum_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);

	return (Time_Sum_File::Create_Fields ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Time_Sum_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK", "ID");
	if (link < 0) return (false);

	return (Time_Sum_File::Set_Field_Numbers ());
}
