//********************************************************* 
//	Access_File.cpp - Process Link File Input/Output
//*********************************************************

#include "Access_File.hpp"

//-----------------------------------------------------------
//	Access_File constructors
//-----------------------------------------------------------

Access_File::Access_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Access_File::Access_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Access_File::Setup (void)
{
	File_Type ("Access File");
	File_ID ("Access");

	link = from_id = to_id = from_type = to_type = dir = time = cost = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Access_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_Field ("FROM_ID", DB_INTEGER, 10);
	Add_Field ("TO_ID", DB_INTEGER, 10);
	if (Code_Flag ()) {
		Add_Field ("FROM_TYPE", DB_UNSIGNED, 1, ID_CODE, true);
		Add_Field ("TO_TYPE", DB_UNSIGNED, 1, ID_CODE, true);
	} else {
		Add_Field ("FROM_TYPE", DB_STRING, 8, ID_CODE);
		Add_Field ("TO_TYPE", DB_STRING, 8, ID_CODE);
	}
	Add_Field ("DIR", DB_INTEGER, 1);
	Add_Field ("TIME", DB_TIME, 2, SECONDS, true);
	Add_Field ("COST", DB_UNSIGNED, 2, CENTS, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Access_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	from_id = Required_Field ("FROMID", "FROM_ID");
	to_id = Required_Field ("TOID", "TO_ID");
	from_type = Required_Field ("FROMTYPE", "FROM_TYPE");
	to_type = Required_Field ("TOTYPE", "TO_TYPE");

	if (from_id < 0 || to_id < 0 || from_type < 0 || to_type < 0) return (false);

	//---- optional fields ----

	link = Optional_Field ("LINK", "ACCESS", "ID", "RECORD");
	dir = Optional_Field ("DIR", "DIRECTION");
	time = Optional_Field ("DELAY", "TIME");
	cost = Optional_Field ("COST");
	Notes_Field (Optional_Field ("NOTES"));
	
	//---- set default units ----

	Set_Units (from_type, ID_CODE);	
	Set_Units (to_type, ID_CODE);
	Set_Units (time, SECONDS);
	Set_Units (cost, CENTS);

	return (true);
}
