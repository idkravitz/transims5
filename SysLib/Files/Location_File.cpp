//********************************************************* 
//	Location_File.cpp - Activity Location File Input/Output
//*********************************************************

#include "Location_File.hpp"

//-----------------------------------------------------------
//	Location_File constructors
//-----------------------------------------------------------

Location_File::Location_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Location_File::Location_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Location_File::Setup (void)
{
	File_Type ("Location File");
	File_ID ("Location");

	location = link = dir = offset = setback = zone = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Location_File::Create_Fields (void) 
{
	Add_Field ("LOCATION", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	Add_Field ("SETBACK", DB_DOUBLE, 8.1, METERS);
	Add_Field ("ZONE", DB_INTEGER, 10);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Location_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	location = Required_Field ("LOCATION", "ID");
	link = Required_Field ("LINK");
	offset = Required_Field ("OFFSET");

	if (location < 0 || link < 0 || offset < 0) return (false);

	//---- optional fields ----

	setback = Optional_Field ("SETBACK");
	zone = Optional_Field ("ZONE", "TAZ");

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));
	
	//---- set default units ----

	Set_Units (offset, METERS);
	Set_Units (setback, METERS);

	return (true);
}

//-----------------------------------------------------------
//	Add_User_Fields
//-----------------------------------------------------------

void Location_File::Add_User_Fields (Location_File *file)
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

		if (fld->Name ().Equals ("LOCATION") || fld->Name ().Equals ("ID") || fld->Name ().Equals ("LINK") ||
			fld->Name ().Equals ("OFFSET") || fld->Name ().Equals ("SETBACK") || fld->Name ().Equals ("ZONE") ||
			fld->Name ().Equals ("TAZ") || fld->Name ().Equals ("DIR") || fld->Name ().Equals ("NODE") ||
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

