//********************************************************* 
//	Phasing_File.cpp - Phasing Plan File Input/Output
//*********************************************************

#include "Phasing_File.hpp"

//-----------------------------------------------------------
//	Phasing_File constructors
//-----------------------------------------------------------

Phasing_File::Phasing_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Phasing_File::Phasing_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Phasing_File::Setup (void)
{
	File_Type ("Phasing Plan File");
	File_ID ("Phasing");

	Nest (NESTED);
	Header_Lines (2);

	signal = phasing = phase = detectors = movements = -1;
	movement = link = dir = to_link = protect = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Phasing_File::Create_Fields (void) 
{
	Add_Field ("SIGNAL", DB_INTEGER, 10);
	Add_Field ("PHASING", DB_INTEGER, 3);
	Add_Field ("PHASE", DB_INTEGER, 3);
	Add_Field ("MOVEMENTS", DB_INTEGER, 2, NEST_COUNT);
	Add_Field ("DETECTORS", DB_STRING, STRING_FIELD_SIZE);

	if (Code_Flag ()) {
		Add_Field ("MOVEMENT", DB_UNSIGNED, 1, MOVEMENT_CODE, true, NESTED);
	} else {
		Add_Field ("MOVEMENT", DB_STRING, 10, MOVEMENT_CODE, false, NESTED);
	}
	Add_Field ("LINK", DB_INTEGER, 10, NO_UNITS, false, NESTED);
	Add_LinkDir_Field (NESTED);
	Add_Field ("TO_LINK", DB_INTEGER, 10, NO_UNITS, false, NESTED);

	if (Code_Flag ()) {
		Add_Field ("PROTECTION", DB_UNSIGNED, 1, PROTECTION_CODE, true, NESTED);
	} else {
		Add_Field ("PROTECTION", DB_STRING, 12, PROTECTION_CODE, false, NESTED);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Phasing_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	if (Version () <= 40) {
		signal = Required_Field ("NODE");
		phasing = Required_Field ("TIMING", "TIMING_PLAN", "PLAN", "TIMING_ID");
	} else {
		signal = Required_Field (SIGNAL_FIELD_NAMES);
		phasing = Required_Field ("PHASING", "PHASING_PLAN", "PHASING_ID", "PLAN");
	}
	phase = Required_Field ("PHASE");
	link = Required_Field (IN_LINK_FIELD_NAMES);
	to_link = Required_Field (OUT_LINK_FIELD_NAMES);

	if (signal < 0 || phasing < 0 || phase < 0 || link < 0 || to_link < 0) return (false);

	//---- optional fields ----
	
	detectors = Optional_Field ("DETECTORS");
	movements = Optional_Field ("MOVEMENTS", "NUM_TURNS", "NUM_NEST", "RECORDS");
	Num_Nest_Field (movements);

	dir = LinkDir_Type_Field ();
	movement = Optional_Field ("MOVEMENT", "TURN", "TURN_CODE");
	protect = Optional_Field ("PROTECTION");

	//---- set default units ----

	Set_Units (movement, MOVEMENT_CODE);
	Set_Units (protect, PROTECTION_CODE);

	return (true);
}
