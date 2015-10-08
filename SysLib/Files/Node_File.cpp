//********************************************************* 
//	Node_File.cpp - Node File Input/Output
//*********************************************************

#include "Node_File.hpp"

//-----------------------------------------------------------
//	Node_File constructors
//-----------------------------------------------------------

Node_File::Node_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Node_File::Node_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Node_File::Setup (void)
{
	File_Type ("Node File");
	File_ID ("Node");

	node = x = y = z = subarea = part = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Node_File::Create_Fields (void) 
{
	Add_Field ("NODE", DB_INTEGER, 10);
	Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("Z_COORD", DB_DOUBLE, 14.1, METERS);
	if (Subarea_Flag ()) {
		Add_Field ("SUBAREA", DB_INTEGER, 4);
		Add_Field ("PARTITION", DB_INTEGER, 4);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Node_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node = Required_Field ("NODE", "ID", "N");
	x = Required_Field (X_FIELD_NAMES);
	y = Required_Field (Y_FIELD_NAMES);

	if (node < 0 || x < 0 || y < 0) return (false);

	//---- optional fields ----

	z = Optional_Field (Z_FIELD_NAMES);

	subarea = Optional_Field ("SUBAREA", "AREA");
	part = Optional_Field ("PARTITION", "PART");
	Subarea_Flag ((subarea >= 0 || part >= 0));

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (x, METERS);
	Set_Units (y, METERS);
	Set_Units (z, METERS);

	return (true);
}
