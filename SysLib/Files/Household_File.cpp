//********************************************************* 
//	Household_File.cpp - Household File Input/Output
//*********************************************************

#include "Household_File.hpp"

//-----------------------------------------------------------
//	Household_File constructors
//-----------------------------------------------------------

Household_File::Household_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Household_File::Household_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Household_File::Setup (void)
{
	File_Type ("Household File");
	File_ID ("Household");
	
	Nest (NESTED);
	Header_Lines (2);

	hhold = location = persons = workers = vehicles = type = partition = -1;
	person = age = relate = gender = drive = work = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Household_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 10);
	Add_Field ("LOCATION", DB_INTEGER, 10);
	Add_Field ("PERSONS", DB_INTEGER, 2, NEST_COUNT);
	Add_Field ("WORKERS", DB_INTEGER, 2);
	Add_Field ("VEHICLES", DB_INTEGER, 2);
	//if (exe->Notes_Name_Flag ()) {
	//	Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	//}
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true, NESTED);
	Add_Field ("AGE", DB_INTEGER, 1, YEARS, true, NESTED);
	if (Code_Flag ()) {
		Add_Field ("RELATE", DB_INTEGER, 1, RELATE_CODE, true, NESTED);
		Add_Field ("GENDER", DB_INTEGER, 1, GENDER_CODE, true, NESTED);
		Add_Field ("WORK", DB_INTEGER, 1, BOOL_CODE, true, NESTED);
		Add_Field ("DRIVE", DB_INTEGER, 1, BOOL_CODE, true, NESTED);
	} else {
		Add_Field ("RELATE", DB_STRING, 12, RELATE_CODE, false, NESTED);
		Add_Field ("GENDER", DB_STRING, 8, GENDER_CODE, false, NESTED);
		Add_Field ("WORK", DB_STRING, 6, BOOL_CODE, false, NESTED);
		Add_Field ("DRIVE", DB_STRING, 6, BOOL_CODE, false, NESTED);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Household_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	location = Required_Field ("LOCATION", "ZONE");

	if (hhold < 0 || location < 0) return (false);

	//---- optional fields ----

	persons = Optional_Field ("PERSONS", "HHSIZE");
	workers = Optional_Field ("WORKERS", "JOBS");
	vehicles = Optional_Field ("VEHICLES", "AUTOS");
	type = Optional_Field ("TYPE");
	partition = Optional_Field (PARTITION_FIELD_NAMES);

	if (Version () > 40) Num_Nest_Field (persons);
	//Notes_Field (Optional_Field ("NOTES"));

	//---- optional person fields ----

	person = Optional_Field (PERSON_FIELD_NAMES);
	age = Optional_Field ("AGE");
	relate = Optional_Field ("RELATE", "ROLE", "ORDER", "RELATIONSHIP"); 
	gender = Optional_Field ("GENDER", "SEX");
	work = Optional_Field ("WORK", "WORKER");
	drive = Optional_Field ("DRIVE", "DRIVER");

	//---- set default units ----

	Set_Units (age, YEARS);
	Set_Units (relate, RELATE_CODE);
	Set_Units (gender, GENDER_CODE);
	Set_Units (work, BOOL_CODE);
	Set_Units (drive, BOOL_CODE);

	return (true);
}
