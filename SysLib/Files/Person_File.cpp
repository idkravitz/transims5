//********************************************************* 
//	Person_File.cpp - household person file input/output
//*********************************************************

#include "Person_File.hpp"

//-----------------------------------------------------------
//	Person_File constructors
//-----------------------------------------------------------

Person_File::Person_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Person_File::Person_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Person_File::Setup (void)
{
	File_Type ("Person File");
	File_ID ("Person");

	hhold = person = age = relate = gender = drive = work = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Person_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 10);
	Add_Field ("PERSON", DB_INTEGER, 4);
	Add_Field ("AGE", DB_INTEGER, 1, YEARS, true);
	if (Code_Flag ()) {
		Add_Field ("RELATE", DB_INTEGER, 1, RELATE_CODE, true);
		Add_Field ("GENDER", DB_INTEGER, 1, GENDER_CODE, true);
		Add_Field ("WORK", DB_INTEGER, 1, BOOL_CODE, true);
		Add_Field ("DRIVE", DB_INTEGER, 1, BOOL_CODE, true);
	} else {
		Add_Field ("RELATE", DB_STRING, 12, RELATE_CODE);
		Add_Field ("GENDER", DB_STRING, 8, GENDER_CODE);
		Add_Field ("WORK", DB_STRING, 6, BOOL_CODE);
		Add_Field ("DRIVE", DB_STRING, 6, BOOL_CODE);
	}
	//if (exe->Notes_Name_Flag ()) {
	//	Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	//}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Person_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	person = Required_Field (PERSON_FIELD_NAMES);

	if (hhold < 0 || person < 0) return (false);

	//---- optional fields ----

	age = Optional_Field ("AGE");
	relate = Optional_Field ("RELATE", "ROLE", "ORDER", "RELATIONSHIP"); 
	gender = Optional_Field ("GENDER", "SEX");
	work = Optional_Field ("WORK", "WORKER");
	drive = Optional_Field ("DRIVE", "DRIVER");
	//Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (age, YEARS);
	Set_Units (relate, RELATE_CODE);
	Set_Units (gender, GENDER_CODE);
	Set_Units (work, BOOL_CODE);
	Set_Units (drive, BOOL_CODE);

	return (true);
}
