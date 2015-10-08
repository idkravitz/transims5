//********************************************************* 
//	Activity_File.cpp - Activity File Input/Output
//*********************************************************

#include "Activity_File.hpp"

//-----------------------------------------------------------
//	Activity_File constructors
//-----------------------------------------------------------

Activity_File::Activity_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Activity_File::Activity_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Activity_File::Setup (void)
{
	File_Type ("Activity File");
	File_ID ("Activity");

	hhold = person = activity = purpose = priority = -1;
	st_min = st_max = end_min = end_max = time_min = time_max = -1;
	mode = vehicle = location = num_pass = limit = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Activity_File::Create_Fields (void) 
{
	if (Dbase_Format () == VERSION3) {
		Header_Lines (0);

		Add_Field ("HHOLD", DB_INTEGER, 10);
		Add_Field ("PERSON", DB_INTEGER, 3);
		Add_Field ("ACTIVITY", DB_INTEGER, 5);
		Add_Field ("PURPOSE", DB_INTEGER, 2);
		Add_Field ("PRIORITY", DB_INTEGER, 2);
		Add_Field ("START_MIN", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("START_MAX", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("START_A", DB_DOUBLE, 10.6);
		Add_Field ("START_B", DB_DOUBLE, 10.6);
		Add_Field ("END_MIN", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("END_MAX", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("END_A", DB_DOUBLE, 10.6);
		Add_Field ("END_B", DB_DOUBLE, 10.6);
		Add_Field ("TIME_MIN", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("TIME_MAX", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("TIME_A", DB_DOUBLE, 10.6);
		Add_Field ("TIME_B", DB_DOUBLE, 10.6);
		Add_Field ("MODE", DB_INTEGER, 2);
		Add_Field ("VEHICLE", DB_INTEGER, 10);
		Add_Field ("NUM_LOC", DB_INTEGER, 2);
		Add_Field ("LOCATION", DB_INTEGER, 10);
		Add_Field ("NUM_PASS", DB_INTEGER, 3);
		Add_Field ("PASSENGER", DB_INTEGER, 10);
		Add_Field ("CONSTRAINT", DB_INTEGER, 2, CONSTRAINT_CODE);
	} else {
		Add_Field ("HHOLD", DB_INTEGER, 10);
		Add_Field ("PERSON", DB_INTEGER, 3);
		Add_Field ("ACTIVITY", DB_INTEGER, 5);
		Add_Field ("PURPOSE", DB_INTEGER, 2);
		Add_Field ("PRIORITY", DB_INTEGER, 2, PRIORITY_CODE);
		Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("DURATION", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("MODE", DB_INTEGER, 2, MODE_CODE);
		Add_Field ("VEHICLE", DB_INTEGER, 10);
		Add_Field ("LOCATION", DB_INTEGER, 10);
		Add_Field ("PASSENGERS", DB_INTEGER, 3);
		Add_Field ("CONSTRAINT", DB_INTEGER, 2, CONSTRAINT_CODE);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Activity_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	person = Required_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	activity = Required_Field ("ACTIVITY", "ACT");
	st_min = Required_Field ("START_MIN", "START", "START_TIME", "STARTTIME");
	st_max = Required_Field ("START_MAX", "START", "START_TIME", "STARTTIME");
	end_min = Required_Field ("END_MIN", "END", "END_TIME", "ENDTIME");
	end_max = Required_Field ("END_MAX", "END", "END_TIME", "ENDTIME");
	mode = Required_Field ("MODE", "MOD");
	location = Required_Field ("LOCATION", "LOC");

	if (hhold < 0 || person < 0 || activity < 0 || st_min < 0 || st_max < 0 || 
		end_min < 0 || end_max < 0 || mode < 0 || location < 0) return (false);

	//---- optional fields ----

	purpose = Optional_Field ("PURPOSE", "TYPE");
	priority = Optional_Field ("PRIORITY", "LEVEL");
	time_min = Optional_Field ("TIME_MIN", "DURATION");
	time_max = Optional_Field ("TIME_MAX", "DURATION");
	vehicle = Optional_Field ("VEHICLE", "VEH", "DRIVER");
	num_pass = Optional_Field ("NUM_PASS", "PASSENGERS");
	limit = Optional_Field ("CONSTRAINT", "LIMIT");

	Notes_Field (Optional_Field ("NOTES"));
	
	//---- set default units ----

	Set_Units (st_min, SECONDS);
	Set_Units (st_max, SECONDS);
	Set_Units (end_min, SECONDS);
	Set_Units (end_max, SECONDS);
	Set_Units (time_min, SECONDS);
	Set_Units (time_max, SECONDS);
	Set_Units (mode, MODE_CODE);
	Set_Units (limit, CONSTRAINT_CODE);
	Set_Units (priority, PRIORITY_CODE);
	return (true);
}

//-----------------------------------------------------------
//	Default_Definition
//-----------------------------------------------------------

bool Activity_File::Default_Definition (void)
{
	if (Dbase_Format () == VERSION3) {
		Create_Fields ();

		return (Write_Def_Header (0));
	} else {
		return (false);
	}
}
