//********************************************************* 
//	Route_Nodes_File.cpp - Route_Nodes File Input/Output
//*********************************************************

#include "Route_Nodes_File.hpp"

//-----------------------------------------------------------
//	Route_Nodes_File constructors
//-----------------------------------------------------------

Route_Nodes_File::Route_Nodes_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Route_Nodes_File::Route_Nodes_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Route_Nodes_File::Setup (void)
{
	File_Type ("Route Nodes File");
	File_ID ("RouteNodes");
	
	Nest (NESTED);
	Header_Lines (2);

	dwell_flag = time_flag = speed_flag = type_flag = true;

	num_periods = 8;
	route = mode = veh_type = nodes = name = node = type = dwell = time = speed = -1;
	memset (headway, -1, sizeof (headway));
	memset (offset, -1, sizeof (offset));
	memset (ttime, -1, sizeof (ttime));
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Route_Nodes_File::Create_Fields (void) 
{
	String field;

	Add_Field ("ROUTE", DB_INTEGER, 10);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
	}
	Add_Field ("VEH_TYPE", DB_INTEGER, 4, VEH_TYPE);
	Add_Field ("NODES", DB_INTEGER, 2, NEST_COUNT, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 40);
	}
	for (int i=1; i <= num_periods; i++) {
		field ("HEADWAY%d") % i;
		Add_Field (field, DB_TIME, 2, MINUTES, true);
		field ("OFFSET%d") % i;
		Add_Field (field, DB_TIME, 2, MINUTES, true);
		field ("TTIME%d") % i;
		Add_Field (field, DB_TIME, 4, MINUTES, true);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("NODE", DB_INTEGER, 4, NO_UNITS, true, NESTED);
	if (Code_Flag ()) {
		if (type_flag) Add_Field ("TYPE", DB_INTEGER, 1, STOP_CODE, true, NESTED);
	} else {
		if (type_flag) Add_Field ("TYPE", DB_STRING, 16, STOP_CODE, false, NESTED);
	}
	if (dwell_flag) Add_Field ("DWELL", DB_TIME, 2, SECONDS, true, NESTED);
	if (time_flag) Add_Field ("TIME", DB_TIME, 2, SECONDS, true, NESTED);
	if (speed_flag) Add_Field ("SPEED", DB_DOUBLE, 4.1, KPH, true, NESTED);
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Route_Nodes_File::Set_Field_Numbers (void)
{
	String field1, field2, field3, field4;

	//---- required fields ----

	route = Required_Field (ROUTE_FIELD_NAMES);
	mode = Required_Field (MODE_FIELD_NAMES);
	node = Required_Field ("NODE", "N", "NOD", "STOP");
	nodes = Required_Field ("NODES", "NNODES", "NUM_NODES");

	if (route < 0 || mode < 0 || node < 0 || nodes < 0) return (false);

	//---- optional fields ----

	veh_type = Optional_Field ("VEHICLE", "VEH_TYPE", "VEHTYPE");
	Num_Nest_Field (nodes);

	if (exe->Notes_Name_Flag ()) {
		name = Optional_Field ("NAME", "ROUTE_NAME", "RTNAME", "DESCRIPTION");
	}
	Notes_Field (Optional_Field ("NOTES"));

	for (int i=1; i <= 24; i++) {
		field1 ("HEADWAY%d") % i;
		field2 ("HEAD%d") % i;
		field3 ("FREQ%d") % i;
		field4 ("HEADWAY_%d") % i;
		headway [i-1] = Optional_Field (field1.c_str (), field2.c_str (), field3.c_str (), field4.c_str ());

		field1 ("OFFSET%d") % i;
		field2 ("OFFSET_%d") % i;
		offset [i-1] = Optional_Field (field1.c_str (), field2.c_str ());

		field1 ("TTIME%d") % i;
		field2 ("TTIME_%d") % i;
		field3 ("TTIME");
		ttime [i-1] = Optional_Field (field1.c_str (), field2.c_str (), field3.c_str ());

		if (headway [i-1] < 0 && offset [i-1] < 0 && ttime [i-1] < 0) break;

		num_periods = i;
		Set_Units (headway [i-1], MINUTES);
		Set_Units (offset [i-1], MINUTES);
		Set_Units (ttime [i-1], MINUTES);
	}
	type = Optional_Field ("TYPE", "STOP_TYPE", "STYLE");
	dwell = Optional_Field ("DWELL", "DELAY");
	time = Optional_Field ("TIME");
	speed = Optional_Field ("SPEED");

	type_flag = (type >= 0);
	dwell_flag = (dwell >= 0);
	time_flag = (time >= 0);
	speed_flag = (speed >= 0);

	//---- set default units ----

	Set_Units (mode, TRANSIT_CODE);
	Set_Units (veh_type, VEH_TYPE);
	Set_Units (dwell, SECONDS);
	Set_Units (time, SECONDS);
	Set_Units (speed, MPS);
	Set_Units (type, STOP_CODE);

	return (true);
}
