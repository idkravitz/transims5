//********************************************************* 
//	Line_File.cpp - Transit Route File Input/Output
//*********************************************************

#include "Line_File.hpp"

//-----------------------------------------------------------
//	Line_File constructors
//-----------------------------------------------------------

Line_File::Line_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Line_File::Line_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Line_File::Setup (void)
{
	File_Type ("Transit Route File");
	File_ID ("Route");

	Nest (NESTED);
	Header_Lines (2);

	route = stops = mode = stop = zone = flag = name = type = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Line_File::Create_Fields (void) 
{
	Add_Field ("ROUTE", DB_INTEGER, 10);
	Add_Field ("STOPS", DB_INTEGER, 10, NEST_COUNT);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
	}
	Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 40);
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("STOP", DB_INTEGER, 4, NO_UNITS, true, NESTED);
	Add_Field ("ZONE", DB_INTEGER, 2, FARE_ZONE, true, NESTED);
	Add_Field ("TIMEPT", DB_INTEGER, 1, NO_UNITS, true, NESTED);
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Line_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	route = Required_Field (ROUTE_FIELD_NAMES);
	stops = Required_Field ("STOPS", "NSTOPS", "NUM_STOPS");
	mode = Required_Field (MODE_FIELD_NAMES);
	stop = Required_Field ("STOP");

	if (route < 0 || mode < 0 || stops < 0 || stop < 0) return (false);

	//---- optional fields ----

	type = Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	zone = Optional_Field ("ZONE", "FARE_ZONE", "DISTRICT");
	flag = Optional_Field ("TIMEPT", "FLAG");
	if (exe->Notes_Name_Flag ()) {
		name = Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	}
	Num_Nest_Field (stops);
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (mode, TRANSIT_CODE);
	Set_Units (type, VEH_TYPE);
	Set_Units (zone, FARE_ZONE);

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Line_File::Default_Definition (void)
{
	Nest (NESTED);
	Create_Fields ();

	return (Write_Def_Header (""));
}
