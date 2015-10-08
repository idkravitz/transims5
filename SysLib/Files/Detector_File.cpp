//********************************************************* 
//	Detector_File.cpp - Detector File Input/Output
//*********************************************************

#include "Detector_File.hpp"

//-----------------------------------------------------------
//	Detector_File constructors
//-----------------------------------------------------------

Detector_File::Detector_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Detector_File::Detector_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Detector_File::Setup (void)
{
	File_Type ("Detector File");
	File_ID ("Detector");

	detector = link = dir = offset = length = lanes = type = use = low = high = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Detector_File::Create_Fields (void) 
{
	Add_Field ("DETECTOR", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
	Add_Field ("LENGTH", DB_DOUBLE, 8.1, METERS);
	if (Code_Flag ()) {
		Add_Field ("LANES", DB_UNSIGNED, 2, LANE_RANGE_CODE, true);
		Add_Field ("TYPE", DB_UNSIGNED, 1, DETECTOR_CODE, true);
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
	} else {
		Add_Field ("LANES", DB_STRING, 8, LANE_RANGE_CODE);
		Add_Field ("TYPE", DB_STRING, 10, DETECTOR_CODE);
		Add_Field ("USE", DB_STRING, STRING_FIELD_SIZE, USE_CODE);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Detector_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	detector = Required_Field ("DETECTOR", "ID");
	link = Required_Field ("LINK");
	length = Required_Field ("LENGTH");

	if (detector < 0 || link < 0 || length < 0) return (false);

	//---- optional fields ----

	offset = Optional_Field ("OFFSET");

	if (Version () > 40) {
		lanes = Optional_Field ("LANES");
	} else {
		low = Required_Field ("LOW_LANE", "LANEBEGIN");
		high = Optional_Field ("HIGH_LANE", "LANEEND");
	}
	type = Optional_Field ("TYPE");
	use = Optional_Field ("USE", "VEHICLE");

	dir = LinkDir_Type_Field ();
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	if (Version () > 40) Set_Units (lanes, LANE_RANGE_CODE);
	Set_Units (type, DETECTOR_CODE);
	Set_Units (use, USE_CODE);	
	Set_Units (length, METERS);
	Set_Units (offset, METERS);

	return (true);
}
