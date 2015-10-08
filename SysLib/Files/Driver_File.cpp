//********************************************************* 
//	Driver_File.cpp - Transit Driver File Input/Output
//*********************************************************

#include "Driver_File.hpp"

//-----------------------------------------------------------
//	Driver_File constructors
//-----------------------------------------------------------

Driver_File::Driver_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Driver_File::Driver_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Driver_File::Setup (void)
{
	File_Type ("Transit Driver File");
	File_ID ("Driver");

	Nest (NESTED);
	Header_Lines (2);

	route = links = type = subtype = link = dir = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Driver_File::Create_Fields (void) 
{
	Add_Field ("ROUTE", DB_INTEGER, 10);
	Add_Field ("NLINKS", DB_INTEGER, 4, NEST_COUNT);
	Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("LINK", DB_INTEGER, 4, NO_UNITS, true, NESTED);
	Add_LinkDir_Field (NESTED);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Driver_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	route = Required_Field ("ROUTE");
	links = Required_Field ("LINKS", "NLINKS", "NUM_LINKS");
	link = Required_Field ("LINK", "LINK_DIR", "LINKDIR");

	if (route < 0 || links < 0 || link < 0) return (false);

	//---- optional fields ----

	type = Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	subtype = Optional_Field ("SUBTYPE", "SUB_TYPE");

	dir = LinkDir_Type_Field ();
	Num_Nest_Field (links);
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (type, VEH_TYPE);

	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Driver_File::Default_Definition (void)
{
	Nest (NESTED);
	Header_Lines (2);
	Create_Fields ();

	return (Write_Def_Header (""));
}
