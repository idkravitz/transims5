//********************************************************* 
//	Zone_File.cpp - Zone File Input/Output
//*********************************************************

#include "Zone_File.hpp"

//-----------------------------------------------------------
//	Zone_File constructors
//-----------------------------------------------------------

Zone_File::Zone_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Zone_File::Zone_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Zone_File::Setup (void)
{
	File_Type ("Zone File");
	File_ID ("Zone");

	zone = x = y = z = area = min_x = min_y = max_x = max_y = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Zone_File::Create_Fields (void) 
{
	Add_Field ("ZONE", DB_INTEGER, 10);
	Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("Z_COORD", DB_DOUBLE, 14.1, METERS);
	Add_Field ("AREA_TYPE", DB_INTEGER, 3);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Zone_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	zone = Required_Field (ZONE_FIELD_NAMES);

	if (zone < 0) return (false);

	//---- optional fields ----

	x = Optional_Field (X_FIELD_NAMES);
	y = Optional_Field (Y_FIELD_NAMES);
	z = Optional_Field (Z_FIELD_NAMES);
	area = Optional_Field (AREA_TYPE_FIELD_NAMES);
	min_x = Optional_Field ("MIN_X", "MINIMUM_X", "MINX", "LOW_X", "LOWX");
	min_y = Optional_Field ("MIN_Y", "MINIMUM_Y", "MINY", "LOW_Y", "LOWY");
	max_x = Optional_Field ("MAX_X", "MAXIMUM_X", "MAXX", "HIGH_X", "HIGHX");
	max_y = Optional_Field ("MAX_Y", "MAXIMUM_Y", "MAXY", "HIGH_Y", "HIGHY");

	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (x, METERS);
	Set_Units (y, METERS);
	Set_Units (z, METERS);
	Set_Units (min_x, METERS);
	Set_Units (min_y, METERS);
	Set_Units (max_x, METERS);
	Set_Units (max_y, METERS);

	return (true);
}

//-----------------------------------------------------------
//	Add_User_Fields
//-----------------------------------------------------------

void Zone_File::Add_User_Fields (Zone_File *file)
{
	int i, offset, nfield;
	bool convert, binary;
	Field_Ptr fld;

	if (file == 0) return;

	if (Dbase_Format () == DEFAULT_FORMAT) {
		Dbase_Format (DEFAULT_FORMAT);
	}
	convert = (file->Dbase_Format () != Dbase_Format () || Num_Fields () > 0);
	binary = (file->Record_Format () == BINARY);

	offset = END_OF_RECORD;
	nfield = file->Num_Fields ();

	for (i=0; i < nfield; i++) {
		fld = file->Field (i);
		if (fld == 0) continue;

		//---- skip standard fields ----

		if (fld->Name ().Equals ("ZONE") || fld->Name ().Equals ("TAZ") || fld->Name ().Equals ("ZONENUM") ||
			fld->Name ().Equals ("ZONE_ID") || fld->Name ().Equals ("Z") || fld->Name ().Equals ("AREA_TYPE") ||
			fld->Name ().Equals ("AREATYPE") || fld->Name ().Equals ("AT") || fld->Name ().Equals ("ATYPE") || 
			fld->Name ().Equals ("X_COORD") || fld->Name ().Equals ("EASTING") || fld->Name ().Equals ("X") ||
			fld->Name ().Equals ("Y_COORD") || fld->Name ().Equals ("NORTHING") || fld->Name ().Equals ("Y") ||
			fld->Name ().Equals ("NOTES")) continue;

		if (!convert) {
			offset = fld->Offset ();
		}
		Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, NO_NEST, offset);
	}
	Write_Fields ();
	Write_Header ();
}
