//********************************************************* 
//	Performance_File.cpp - Link Performance Input/Output
//*********************************************************

#include "Performance_File.hpp"

//-----------------------------------------------------------
//	Performance_File constructors
//-----------------------------------------------------------

Performance_File::Performance_File (Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Link_Delay_File (access, format, flow, turn_flag, flow_flag)
{
	Setup ();
}

Performance_File::Performance_File (string filename, Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Link_Delay_File (access, format, flow, turn_flag, flow_flag)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Performance_File::Setup (void)
{
	File_Type ("Performance File");
	File_ID ("Performance");

	speed = delay = density = max_den = ratio = queue = max_que = fail = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Performance_File::Create_Fields (void) 
{
	Link_Delay_File::Create_Fields ();

	Add_Field ("AVG_SPEED", DB_DOUBLE, 8.1, KPH);
	Add_Field ("AVG_DELAY", DB_TIME, 8.1, SECONDS);
	Add_Field ("AVG_DENSITY", DB_DOUBLE, 8.1, LANE_KM);
	Add_Field ("MAX_DENSITY", DB_DOUBLE, 8.1, LANE_KM);
	Add_Field ("TIME_RATIO", DB_DOUBLE, 8.3, RATIO);
	Add_Field ("AVG_QUEUE", DB_DOUBLE, 8.1, STOPPED);
	Add_Field ("MAX_QUEUE", DB_INTEGER, 10, STOPPED);
	Add_Field ("NUM_FAIL", DB_INTEGER, 10, Flow_Units ());

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Performance_File::Set_Field_Numbers (void)
{
	Link_Delay_File::Set_Field_Numbers ();

	//---- optional fields ----

	speed = Optional_Field ("AVG_SPEED", "SPEED", "VSUM");
	delay = Optional_Field ("AVG_DELAY", "DELAY");
	density = Optional_Field ("AVG_DENSITY", "DENSITY");
	max_den = Optional_Field ("MAX_DENSITY", "MAX_DEN");
	ratio = Optional_Field ("TIME_RATIO", "RATIO");
	queue = Optional_Field ("AVG_QUEUE", "QUEUE");
	max_que = Optional_Field ("MAX_QUEUE", "MAX_QUE");
	fail = Optional_Field ("NUM_FAIL", "CYCLE_FAIL", "FAILURE");

	//---- set default units ----

	Set_Units (speed, MPS);
	Set_Units (delay, SECONDS);
	Set_Units (density, LANE_KM);
	Set_Units (max_den, LANE_KM);
	Set_Units (ratio, RATIO);
	Set_Units (queue, STOPPED);
	Set_Units (max_que, STOPPED);
	Set_Units (fail, Flow_Units ());
	return (true);
}
