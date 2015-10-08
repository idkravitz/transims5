//********************************************************* 
//	Selection_File.cpp - household selection file input/output
//*********************************************************

#include "Selection_File.hpp"

//-----------------------------------------------------------
//	Selection_File constructors
//-----------------------------------------------------------

Selection_File::Selection_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Selection_File::Selection_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Selection_File::Setup (void)
{
	File_Type ("Selection File");
	File_ID ("Select");

	hhold = person = tour = trip = type = partition = -1;
	type_flag = part_flag = false;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Selection_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("TRIP", DB_INTEGER, 1, NO_UNITS, true);
	if (type_flag) {
		Add_Field ("TYPE", DB_INTEGER, 2, NO_UNITS, true);
	}
	if (part_flag) {
		Add_Field ("PARTITION", DB_INTEGER, 2, NO_UNITS, true);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Selection_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);

	if (hhold < 0) return (false);

	//---- optional fields ----

	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field ("TOUR", "GROUP");
	trip = Optional_Field ("TRIP", "ACTIVITY", "REC", "NUM", "RECORD");
	type = Optional_Field ("TYPE", "METHOD");
	partition = Optional_Field (PARTITION_FIELD_NAMES);
	Notes_Field (Optional_Field ("NOTES"));

	type_flag = (type >= 0);
	part_flag = (partition >= 0);

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Selection_File::Default_Definition (void)
{
	Header_Lines (0);
	First_Offset (0);

	Add_Field ("HHOLD", DB_INTEGER, 10);

	return (Set_Field_Numbers ());
}
