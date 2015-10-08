//********************************************************* 
//	Link_Delay_File.cpp - Link Delay Input/Output
//*********************************************************

#include "Link_Delay_File.hpp"

//-----------------------------------------------------------
//	Link_Delay_File constructors
//-----------------------------------------------------------

Link_Delay_File::Link_Delay_File (Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();
}

Link_Delay_File::Link_Delay_File (string filename, Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_Delay_File::Setup (void)
{
	File_Type ("Link Delay File");
	File_ID ("Delay");

	Set_Nesting (turn_flag);

	link = dir = type = start = end = flow = time = -1;
	out_link = out_flow = out_time = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_Delay_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_LinkDir_Field ();
	if (type_flag) {
		Add_Field ("TYPE", DB_INTEGER, 1);
	}
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("FLOW", DB_DOUBLE, 9.2, flow_units);
	Add_Field ("TIME", DB_TIME, 8.1, SECONDS);
	if (turn_flag) {
		Nest (NESTED);
		Header_Lines (2);
		Add_Field ("NCONNECT", DB_INTEGER, 2);
		Add_Field ("OUT_LINK", DB_INTEGER, 10, NO_UNITS, false, NESTED);
		Add_Field ("OUT_FLOW", DB_DOUBLE, 9.2, flow_units, false, NESTED);
		Add_Field ("OUT_TIME", DB_TIME, 8.1, SECONDS, false, NESTED);
	} else {
		Nest (NO_NEST);
		Header_Lines (1);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Link_Delay_File::Set_Field_Numbers (void)
{
	bool flag = false;

	//---- required fields ----

	link = Required_Field ("LINK");
	flow = Required_Field (FLOW_RATE_FIELD_NAMES);

	if (Version () <= 40) {
		start = Optional_Field (START_FIELD_NAMES);
		end = Optional_Field (END_FIELD_NAMES);

		if (start < 0 && end < 0) {
			start = end = Required_Field ("PERIOD", "TIME");
			time = Required_Field ("AVG_TIME", "TTIME", "SUM");
			flag = true;
		} else {
			time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
		}
	} else {
		time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
		start = Required_Field (START_FIELD_NAMES);
		end = Required_Field (END_FIELD_NAMES);
	}

	if (link < 0 || flow < 0 || time < 0 || start < 0 || end < 0) return (false);

	//---- other required fields ----

	dir = LinkDir_Type_Field ();

	type = Optional_Field ("TYPE", "USE", "HOV", "GROUP");

	//---- nested optional fields ----

	Num_Nest_Field (Optional_Field (CONNECTION_FIELD_NAMES));
	if (Num_Nest_Field () < 0) turn_flag = false;

	out_link = Optional_Field (OUT_LINK_FIELD_NAMES);
	out_flow = Optional_Field (FLOW_OUT_FIELD_NAMES);
	out_time = Optional_Field (TIME_OUT_FIELD_NAMES);

	//---- set default units ----

	if (flag) {
		Set_Units (start, SECONDS);
		Set_Units (end, SECONDS);
	} else {
		Set_Units (start, DAY_TIME);
		Set_Units (end, DAY_TIME);
	}
	Set_Units (flow, flow_units);
	Set_Units (time, SECONDS);
	Set_Units (out_flow, flow_units);
	Set_Units (out_time, SECONDS);

	Units_Type units = Get_Units (flow);
	if (units != NO_UNITS) Flow_Units (units);

	Lane_Use_Flows ((type >= 0));

	return (true);
}
