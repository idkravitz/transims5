//********************************************************* 
//	Stop_File.cpp - Transit Stop File Input/Output
//*********************************************************

#include "Stop_File.hpp"

//-----------------------------------------------------------
//	Stop_File constructors
//-----------------------------------------------------------

Stop_File::Stop_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Stop_File::Stop_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Stop_File::Setup (void)
{
	File_Type ("Transit Stop File");
	File_ID ("Stop");

	stop = link = dir = offset = use = type = space = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Stop_File::Create_Fields (void) 
{
	Add_Field ("STOP", DB_INTEGER, 10);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 50);
	}
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
		Add_Field ("TYPE", DB_UNSIGNED, 1, STOP_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
		Add_Field ("TYPE", DB_STRING, 16, STOP_CODE);
	}
	Add_Field ("SPACE", DB_UNSIGNED, 1, NO_UNITS, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Stop_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	stop = Required_Field ("STOP", "STOP_ID", "ID");
	link = Required_Field ("LINK");
	offset = Required_Field ("OFFSET");

	if (stop < 0 || link < 0 || offset < 0) return (false);

	//---- optional fields ----

	use = Optional_Field ("USE", "VEHICLE");
	type = Optional_Field ("TYPE", "STYLE");
	space = Optional_Field ("CAPACITY", "SPACE");
	name = Optional_Field ("NAME", "STOP_NAME", "STNAME", "DESCRIPTION");

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));
	
	//---- set default units ----

	Set_Units (offset, METERS);
	Set_Units (use, USE_CODE);
	Set_Units (type, STOP_CODE);

	return (true);
}
