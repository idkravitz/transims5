//********************************************************* 
//	Fare_File.cpp - Node File Input/Output
//*********************************************************

#include "Fare_File.hpp"

//-----------------------------------------------------------
//	Fare_File constructors
//-----------------------------------------------------------

Fare_File::Fare_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Fare_File::Fare_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Fare_File::Setup (void)
{
	File_Type ("Transit Fare File");
	File_ID ("Fare");

	from_zone = to_zone = from_mode = to_mode = period = type = fare = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Fare_File::Create_Fields (void) 
{
	if (Code_Flag ()) {
		Add_Field ("FROM_ZONE", DB_UNSIGNED, 2, FARE_ZONE, true);
		Add_Field ("TO_ZONE", DB_UNSIGNED, 2, FARE_ZONE, true);
		Add_Field ("FROM_MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
		Add_Field ("TO_MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
		Add_Field ("PERIOD", DB_UNSIGNED, 1, TIME_PERIOD, true);
		Add_Field ("CLASS", DB_UNSIGNED, 1, CLASS_CODE, true);
		Add_Field ("FARE", DB_INTEGER, 2, CENTS, true);
	} else {
		Add_Field ("FROM_ZONE", DB_STRING, STRING_FIELD_SIZE, FARE_ZONE_RANGE);
		Add_Field ("TO_ZONE", DB_STRING, STRING_FIELD_SIZE, FARE_ZONE_RANGE);
		Add_Field ("FROM_MODE", DB_STRING, STRING_FIELD_SIZE, TRANSIT_CODE_RANGE);
		Add_Field ("TO_MODE", DB_STRING, STRING_FIELD_SIZE, TRANSIT_CODE_RANGE);
		Add_Field ("PERIOD", DB_STRING, STRING_FIELD_SIZE, TIME_PERIOD_RANGE);
		Add_Field ("CLASS", DB_STRING, STRING_FIELD_SIZE, CLASS_CODE_RANGE);
		Add_Field ("FARE", DB_INTEGER, 2, CENTS, true);
		if (exe->Notes_Name_Flag ()) {
			Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Fare_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	from_zone = Required_Field ("FROM_ZONE", "FROMZONE", "FROM", "BOARD");
	to_zone = Required_Field ("TO_ZONE", "TOZONE", "TO", "ALIGHT");
	to_mode = Required_Field ("MODE", "TO_MODE", "TOMODE");
	fare = Required_Field ("FARE", "COST");

	if (from_zone < 0 || to_zone < 0 || to_mode < 0 || fare < 0) return (false);

	//---- optional fields ----

	from_mode = Optional_Field ("FROM_MODE", "FROMMODE");
	period = Optional_Field ("PERIOD", "TIME", "TIME_PERIOD", "TIME_RANGE");
	type = Optional_Field ("CLASS", "TYPE");

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	if (Code_Flag ()) {
		Set_Units (from_zone, FARE_ZONE);
		Set_Units (to_zone, FARE_ZONE);
		Set_Units (from_mode, TRANSIT_CODE);
		Set_Units (to_mode, TRANSIT_CODE);
		Set_Units (period, TIME_PERIOD);
		Set_Units (type, CLASS_CODE);
	} else {
		Set_Units (from_zone, FARE_ZONE_RANGE);
		Set_Units (to_zone, FARE_ZONE_RANGE);
		Set_Units (from_mode, TRANSIT_CODE_RANGE);
		Set_Units (to_mode, TRANSIT_CODE_RANGE);
		Set_Units (period, TIME_PERIOD_RANGE);
		Set_Units (type, CLASS_CODE_RANGE);
	}
	Set_Units (fare, CENTS);

	return (true);
}
