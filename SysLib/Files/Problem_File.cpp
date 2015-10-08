//********************************************************* 
//	Problem_File.cpp - Problem File Input/Output
//*********************************************************

#include "Problem_File.hpp"

//-----------------------------------------------------------
//	Problem_File constructors
//-----------------------------------------------------------

Problem_File::Problem_File (Access_Type access, Format_Type format) : 
	Trip_File (access, format)
{
	Setup ();
}

Problem_File::Problem_File (string filename, Access_Type access, Format_Type format) : 
	Trip_File (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Problem_File::Setup (void)
{
	Trip_File::Setup ();

	File_Type ("Problem File");
	File_ID ("Problem");

	program_code = 0;
	problem = time = link = dir = lane = offset = route = survey = -1;
}

//-----------------------------------------------------------
//	Program_Type
//-----------------------------------------------------------

void Problem_File::Program_Type (int code)
{
	program_code = code;

	if (File_Access () == CREATE) {
		Clear_Fields ();
		Create_Fields ();
		Write_Header ();
	} else {
		Set_Field_Numbers ();
	}
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Problem_File::Create_Fields (void) 
{
	//bool flag = exe->Notes_Name_Flag ();
	//exe->Notes_Name_Flag (false);

	if (program_code == POPSYN_CODE) {			//---- population synthesizer ----
		Add_Field ("PROBLEM", DB_INTEGER, 2, PROBLEM_CODE);
		Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
		Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
		Add_Field ("LOCATION", DB_INTEGER, 10);
		Add_Field ("VEHICLE", DB_INTEGER, 4);
	} else if (program_code == ACTGEN_CODE) {	//---- activity generator ----
		Add_Field ("PROBLEM", DB_INTEGER, 2, PROBLEM_CODE);
		Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
		Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
		Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
		Add_Field ("ACTIVITY", DB_INTEGER, 1, NO_UNITS, true);
		Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("DURATION", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("LOCATION", DB_INTEGER, 10);
		Add_Field ("PURPOSE", DB_INTEGER, 2);
		if (Code_Flag ()) {
			Add_Field ("MODE", DB_INTEGER, 1, MODE_CODE, true);
			Add_Field ("CONSTRAINT", DB_INTEGER, 1, CONSTRAINT_CODE, true);
			Add_Field ("PRIORITY", DB_INTEGER, 1, PRIORITY_CODE, true);
		} else {
			Add_Field ("MODE", DB_STRING, 12, MODE_CODE);
			Add_Field ("CONSTRAINT", DB_STRING, 14, CONSTRAINT_CODE);
			Add_Field ("PRIORITY", DB_STRING, 10, PRIORITY_CODE);
		}
		Add_Field ("VEHICLE", DB_INTEGER, 4);
		Add_Field ("PASSENGERS", DB_INTEGER, 2);
		Add_Field ("SURVEY", DB_INTEGER, 10);
	} else {									//---- router / simulator ----
		Add_Field ("PROBLEM", DB_INTEGER, 2, PROBLEM_CODE);

		Trip_File::Create_Fields ();

		if (program_code == MSIM_CODE) {		//---- simulator ----
			Add_Field ("TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
			Add_Field ("LINK", DB_INTEGER, 10);
			Add_LinkDir_Field ();
			if (Code_Flag ()) {
				Add_Field ("LANE", DB_UNSIGNED, 1, LANE_ID_CODE, true);
			} else {
				Add_Field ("LANE", DB_STRING, 3, LANE_ID_CODE);
			}
			Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
			Add_Field ("ROUTE", DB_INTEGER, 10);
		}
	}
	Add_Field ("NOTES", DB_STRING, 40);
	//exe->Notes_Name_Flag (flag);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Problem_File::Set_Field_Numbers (void)
{
	problem = Required_Field (PROBLEM_FIELD_NAMES);
	if (problem < 0) return (false);

	//---- optional fields ----

	if (program_code == POPSYN_CODE) {			//---- population synthesizer ----
		Option_Flag (true);
	} else if (program_code == ACTGEN_CODE) {	//---- activity generator ----
		Option_Flag (true);
	} else {									//---- router / simulator ----
		Option_Flag (false);
	}
	survey = Optional_Field ("SURVEY", "SURVEY_ID", "REC_ID", "SURVEYID", "RECID");
	time = Optional_Field ("TIME", "TOD");
	link = Optional_Field ("LINK");
	dir = Optional_Field ("DIR", "NODE");
	lane = Optional_Field ("LANE");
	offset = Optional_Field ("OFFSET");
	route = Optional_Field ("ROUTE", "LINE", "TRANSIT");

	if (link >= 0) program_code = MSIM_CODE;

	bool flag = exe->Notes_Name_Flag ();
	exe->Notes_Name_Flag (true);

	Notes_Field (Optional_Field ("NOTES"));

	exe->Notes_Name_Flag (flag);

	Trip_File::Set_Field_Numbers ();

	//---- set default units ----

	Set_Units (problem, PROBLEM_CODE);
	Set_Units (time, DAY_TIME);
	Set_Units (lane, LANE_ID_CODE);
	Set_Units (offset, METERS);

	return (true);
}
