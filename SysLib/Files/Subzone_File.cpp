//********************************************************* 
//	Subzone_File.cpp - Subzone File Input/Output
//*********************************************************

#include "Subzone_File.hpp"

//-----------------------------------------------------------
//	Subzone_File constructors
//-----------------------------------------------------------

Subzone_File::Subzone_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Subzone_File::Subzone_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Subzone_File::Setup (void)
{
	File_Type ("Subzone Data File");
	File_ID ("Subzone");

	id = zone = x = y = data = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Subzone_File::Create_Fields (void) 
{
	Add_Field ("ID", DB_INTEGER, 10);
	Add_Field ("ZONE", DB_INTEGER, 10);
	Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("DATA", DB_DOUBLE, 14.1);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Subzone_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	id = Required_Field ("ID", "SUBZONE", "RECORD", "REC_ID", "SUB_ID");
	zone = Required_Field (ZONE_FIELD_NAMES);
	x = Required_Field (X_FIELD_NAMES);
	y = Required_Field (Y_FIELD_NAMES);

	if (id < 0 || zone < 0 || x < 0 || y < 0) return (false);

	//---- optional fields ----

	data = Optional_Field ("DATA", "VALUE", "TYPE", "SIZE");

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (x, METERS);
	Set_Units (y, METERS);

	return (true);
}
