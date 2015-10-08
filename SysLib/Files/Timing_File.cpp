//********************************************************* 
//	Timing_File.cpp - Timing Plan File Input/Output
//*********************************************************

#include "Timing_File.hpp"

//-----------------------------------------------------------
//	Timing_File constructors
//-----------------------------------------------------------

Timing_File::Timing_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Timing_File::Timing_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Timing_File::Setup (void)
{
	File_Type ("Timing Plan File");
	File_ID ("Timing");

	Nest (NESTED);
	Header_Lines (2);

	signal = timing = type = cycle = offset = phases = -1;
	phase = barrier = ring = position = minimum = maximum = extend = yellow = red = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Timing_File::Create_Fields (void) 
{
	Add_Field ("SIGNAL", DB_INTEGER, 10);
	Add_Field ("TIMING", DB_INTEGER, 10);
	if (Code_Flag ()) {
		Add_Field ("TYPE", DB_UNSIGNED, 1, SIGNAL_CODE, true);
	} else {
		Add_Field ("TYPE", DB_STRING, 10, SIGNAL_CODE);
	}
	Add_Field ("CYCLE", DB_INTEGER, 2, SECONDS, true);
	Add_Field ("OFFSET", DB_INTEGER, 2, SECONDS, true);
	Add_Field ("PHASES", DB_INTEGER, 1, NEST_COUNT, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("PHASE", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	Add_Field ("BARRIER", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	Add_Field ("RING", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	Add_Field ("POSITION", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	Add_Field ("MIN_GREEN", DB_INTEGER, 2, SECONDS, true, NESTED);
	Add_Field ("MAX_GREEN", DB_INTEGER, 2, SECONDS, true, NESTED);
	Add_Field ("EXTENSION", DB_INTEGER, 2, SECONDS, true, NESTED);
	Add_Field ("YELLOW", DB_INTEGER, 1, SECONDS, true, NESTED);
	Add_Field ("ALL_RED", DB_INTEGER, 1, SECONDS, true, NESTED);
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Timing_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	timing = Required_Field ("TIMING", "TIMING_PLAN", "PLAN", "TIMING_ID");
	phase = Required_Field ("PHASE");
	minimum = Required_Field ("MIN_GREEN", "GREENMIN");

	if (timing < 0 || phase < 0 || minimum < 0) return (false);

	//---- optional fields ----

	signal = Optional_Field (SIGNAL_FIELD_NAMES);
	type = Optional_Field ("TYPE", "CONTROL_TYPE");
	cycle = Optional_Field ("CYCLE", "CYCLE_LEN", "CYCLE_LENGTH");
	offset = Optional_Field ("OFFSET", "TIME_OFFSET");	
	phases = Optional_Field ("PHASES", "NUM_PHASES", "NUM_NEST", "RECORDS");

	Num_Nest_Field (phases);
	Notes_Field (Optional_Field ("NOTES"));

	barrier = Optional_Field ("BARRIER", "GROUP");
	ring = Optional_Field ("RING", "GROUPFIRST");
	position = Optional_Field ("POSITION", "NUMBER", "NEXT_PHASE", "NEXTPHASES");
	maximum = Optional_Field ("MAX_GREEN", "GREENMAX");
	extend = Optional_Field ("EXTENSION", "EXT_GREEN", "GREENEXT");
	yellow = Optional_Field ("YELLOW");
	red = Optional_Field ("ALL_RED", "RED_CLEAR", "REDCLEAR");
	
	//---- set default units ----

	Set_Units (type, SIGNAL_CODE);	
	Set_Units (cycle, SECONDS);
	Set_Units (offset, SECONDS);
	Set_Units (minimum, SECONDS);
	Set_Units (maximum, SECONDS);
	Set_Units (extend, SECONDS);
	Set_Units (yellow, SECONDS);
	Set_Units (red, SECONDS);

	return (true);
}
