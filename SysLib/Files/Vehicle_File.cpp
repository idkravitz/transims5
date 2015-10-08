//********************************************************* 
//	Vehicle_File.cpp - Vehicle File Input/Output
//*********************************************************

#include "Vehicle_File.hpp"

//-----------------------------------------------------------
//	Vehicle_File constructors
//-----------------------------------------------------------

Vehicle_File::Vehicle_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Vehicle_File::Vehicle_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Vehicle_File::Setup (void)
{
	File_Type ("Vehicle File");
	File_ID ("Vehicle");

	vehicle = hhold = parking = type = subtype = partition = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Vehicle_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 10);
	Add_Field ("VEHICLE", DB_INTEGER, 4);
	Add_Field ("PARKING", DB_INTEGER, 10);
	Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
	//if (exe->Notes_Name_Flag ()) {
	//	Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	//}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Vehicle_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);
	vehicle = Required_Field (VEHICLE_FIELD_NAMES);

	parking = Required_Field ("PARKING", "LOCATION", "LOT");
	type = Required_Field ("TYPE", "VEHTYPE", "VEH_TYPE");

	if (vehicle < 0 || hhold < 0 || parking < 0 || type < 0) return (false);

	partition = Optional_Field (PARTITION_FIELD_NAMES);
	//Notes_Field (Optional_Field ("NOTES"));

	//---- Version 4.0 compatibility ----

	subtype = Optional_Field ("SUBTYPE", "SUB_TYPE", "VSUBTYPE");

	//---- set default units ----

	Set_Units (type, VEH_TYPE);

	return (true);
}
