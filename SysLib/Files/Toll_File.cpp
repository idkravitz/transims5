//********************************************************* 
//	Toll_File.cpp - Toll File Input/Output
//*********************************************************

#include "Toll_File.hpp"

//-----------------------------------------------------------
//	Toll_File constructors
//-----------------------------------------------------------

Toll_File::Toll_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Toll_File::Toll_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Toll_File::Setup (void)
{
	File_Type ("Toll File");
	File_ID ("Toll");

	link = dir = start = end = use = toll = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Toll_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
	}
	Add_Field ("TOLL", DB_UNSIGNED, 2, CENTS, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Toll_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	link = Required_Field ("LINK");
	toll = Required_Field ("TOLL", "COST", "CENTS");

	if (link < 0 || toll < 0) return (false);

	//---- optional fields ----

	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	use = Optional_Field ("USE", "VEHICLE");

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (start, DAY_TIME);
	Set_Units (end, DAY_TIME);
	Set_Units (use, USE_CODE);	
	Set_Units (toll, CENTS);

	return (true);
}
