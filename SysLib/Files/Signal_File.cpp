//********************************************************* 
//	Signal_File.cpp - Signal File Input/Output
//*********************************************************

#include "Signal_File.hpp"

//-----------------------------------------------------------
//	Signal_File constructors
//-----------------------------------------------------------

Signal_File::Signal_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Signal_File::Signal_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Signal_File::Setup (void)
{
	File_Type ("Signal File");
	File_ID ("Signal");

	Nest (NESTED);
	Header_Lines (2);

	signal = group = times = start = end = timing = phasing = type = offset = nodes = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Signal_File::Create_Fields (void) 
{
	Add_Field ("SIGNAL", DB_INTEGER, 10);
	Add_Field ("GROUP", DB_INTEGER, 4);
	Add_Field ("TIMES", DB_INTEGER, 2, NEST_COUNT);
	Add_Field ("NODES", DB_STRING, STRING_FIELD_SIZE);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format (), false, NESTED);
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format (), false, NESTED);
	Add_Field ("TIMING", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	Add_Field ("PHASING", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE, NO_UNITS, false, NESTED);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Signal_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	if (Version () <= 40) {
		signal = Required_Field ("NODE");
	} else {
		signal = Required_Field (SIGNAL_FIELD_NAMES);
	}
	if (signal < 0) return (false);

	//---- optional fields ----

	group = Optional_Field ("GROUP", "WARRANT", "AGENCY", "STYLE");
	timing = Optional_Field ("TIMING", "TIMING_PLAN", "PLAN", "TIMING_ID");
	nodes = Optional_Field ("NODES", "NODE");
	times = Optional_Field ("TIMES", "NUM_TIMES", "NUM_NEST", "RECORDS", "PERIODS");
	Num_Nest_Field (times);

	//---- optional fields ----

	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	if (Version () <= 40) {
		phasing = timing;
	} else {
		phasing = Optional_Field ("PHASING", "PHASING_PLAN", "PHASING_ID");
	}
	Notes_Field (Optional_Field ("NOTES"));

	//---- version40 compatibility ----

	type = Optional_Field ("TYPE", "CONTROL_TYPE");
	offset = Optional_Field ("OFFSET", "TIME_OFFSET");	

	//---- set default units ----

	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (type, SIGNAL_CODE);	
	Set_Units (offset, SECONDS);

	return (true);
}
