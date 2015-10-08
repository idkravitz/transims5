//********************************************************* 
//	Diurnal_File.cpp - Diurnal Distribution Input/Output
//*********************************************************

#include "Diurnal_File.hpp"

//-----------------------------------------------------------
//	Diurnal_File constructors
//-----------------------------------------------------------

Diurnal_File::Diurnal_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Diurnal_File::Diurnal_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Diurnal_File::Setup (void)
{
	File_Type ("Diurnal Distribution File");
	File_ID ("Diurnal");

	start = end = share = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Diurnal_File::Create_Fields (void) 
{
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("SHARE", DB_DOUBLE, 14.6);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Diurnal_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);

	if (start < 0 || end < 0) return (false);

	if (share < 0) {
		share = Optional_Field ("SHARE", "PERCENT", "COUNT", "VOLUME");

		if (share < 0) {
			if (Num_Fields () > 2 && start == 0 && end == 1) {
				share = 2;
			} else {
				Required_Field ("SHARE", "PERCENT", "COUNT", "VOLUME");
				return (false);
			}
		}
	}

	//---- set default units ----
	
	Set_Units (start, HOURS);
	Set_Units (end, HOURS);

	return (true);
}
