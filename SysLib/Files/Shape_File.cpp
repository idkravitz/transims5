//********************************************************* 
//	Shape_File.cpp - Shape Point File Input/Output
//*********************************************************

#include "Shape_File.hpp"

//-----------------------------------------------------------
//	Shape_File constructors
//-----------------------------------------------------------

Shape_File::Shape_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Shape_File::Shape_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Shape_File::Setup (void)
{
	File_Type ("Shape File");
	File_ID ("Shape");

	Nest (NESTED);
	Header_Lines (2);

	z_flag = false;
	link = points = x = y = z = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Shape_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_Field ("POINTS", DB_INTEGER, 4, NEST_COUNT);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS, false, NESTED);
	Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS, false, NESTED);
	if (Z_Flag ()) {
		Add_Field ("Z_COORD", DB_DOUBLE, 14.1, METERS, false, NESTED);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Shape_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");
	points = Required_Field ("POINTS", "NPOINTS", "NUM_POINTS");
	x = Required_Field (X_FIELD_NAMES);
	y = Required_Field (Y_FIELD_NAMES);

	if (link < 0 || points < 0 || x < 0 || y < 0) return (false);

	z = Optional_Field (Z_FIELD_NAMES);
	Z_Flag (z >= 0);

	Num_Nest_Field (points);
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (x, METERS);
	Set_Units (y, METERS);
	Set_Units (z, METERS);

	return (true);
}

