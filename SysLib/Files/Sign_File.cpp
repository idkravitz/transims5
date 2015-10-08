//********************************************************* 
//	Sign_File.cpp - Sign File Input/Output
//*********************************************************

#include "Sign_File.hpp"

//-----------------------------------------------------------
//	Sign_File constructors
//-----------------------------------------------------------

Sign_File::Sign_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Sign_File::Sign_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Sign_File::Setup (void)
{
	File_Type ("Sign File");
	File_ID ("Sign");

	link = dir = sign = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Sign_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (Code_Flag ()) {
		Add_Field ("SIGN", DB_UNSIGNED, 1, CONTROL_CODE, true);
	} else {
		Add_Field ("SIGN", DB_STRING, 10, CONTROL_CODE);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Sign_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK", "INLINK", "LINK_IN");
	sign = Required_Field ("SIGN");

	if (link < 0 || sign < 0) return (false);

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----
	
	Set_Units (sign, CONTROL_CODE);

	return (true);
}
