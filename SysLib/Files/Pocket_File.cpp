//********************************************************* 
//	Pocket_File.cpp - Pocket Lane File Input/Output
//*********************************************************

#include "Pocket_File.hpp"

//-----------------------------------------------------------
//	Pocket_File constructors
//-----------------------------------------------------------

Pocket_File::Pocket_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Pocket_File::Pocket_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Pocket_File::Setup (void)
{
	File_Type ("Pocket File");
	File_ID ("Pocket");

	link = dir = type = lanes = length = offset = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Pocket_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("TYPE", DB_UNSIGNED, 1, POCKET_CODE, true);
	} else {
		Add_Field ("TYPE", DB_STRING, 12, POCKET_CODE);
	}
	Add_Field ("LANES", DB_UNSIGNED, 2);
	Add_Field ("LENGTH", DB_DOUBLE, 8.1, METERS);
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Pocket_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	type = Required_Field ("TYPE", "STYLE");
	lanes = Required_Field ("LANES", "LANE");
	length = Required_Field ("LENGTH");

	if (link < 0 || type < 0 || lanes < 0 || length < 0) return (false);
	
	//---- optional fields ----

	offset = Optional_Field ("OFFSET");	
	dir = LinkDir_Type_Field ();

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (length, METERS);
	Set_Units (offset, METERS);
	Set_Units (type, POCKET_CODE);

	return (true);
}
