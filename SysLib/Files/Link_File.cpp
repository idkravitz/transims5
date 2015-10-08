//********************************************************* 
//	Link_File.cpp - Link File Input/Output
//*********************************************************

#include "Link_File.hpp"

//-----------------------------------------------------------
//	Link_File constructors
//-----------------------------------------------------------

Link_File::Link_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Link_File::Link_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Link_File::Setup (void)
{
	File_Type ("Link File");
	File_ID ("Link");

	link = node_a = node_b = length = setback_a = setback_b = bearing_a = bearing_b = -1;
	type = divided = area_type = use = grade = name = -1;
	lanes_ab = speed_ab = fspd_ab = cap_ab = -1;
	lanes_ba = speed_ba = fspd_ba = cap_ba = -1;
	left_ab = right_ab = left_ba = right_ba = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Link_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 10);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 40);
	}
	Add_Field ("NODE_A", DB_INTEGER, 10);
	Add_Field ("NODE_B", DB_INTEGER, 10);
	Add_Field ("LENGTH", DB_DOUBLE, 8.1, METERS);
	Add_Field ("SETBACK_A", DB_DOUBLE, 5.1, METERS);
	Add_Field ("SETBACK_B", DB_DOUBLE, 5.1, METERS);
	Add_Field ("BEARING_A", DB_INTEGER, 4, DEGREES);
	Add_Field ("BEARING_B", DB_INTEGER, 4, DEGREES);
	if (Code_Flag ()) {
		Add_Field ("TYPE", DB_UNSIGNED, 1, FACILITY_CODE, true);
		Add_Field ("DIVIDED", DB_UNSIGNED, 1, NO_UNITS, true);
		Add_Field ("AREA_TYPE", DB_UNSIGNED, 1, NO_UNITS, true);
		Add_Field ("GRADE", DB_FIXED, 4.1, PERCENT, true);
	} else {
		Add_Field ("TYPE", DB_STRING, 12, FACILITY_CODE);
		Add_Field ("DIVIDED", DB_UNSIGNED, 1);
		Add_Field ("AREA_TYPE", DB_UNSIGNED, 3);
		Add_Field ("GRADE", DB_DOUBLE, 5.1, PERCENT);
	}
	Add_Field ("LANES_AB", DB_UNSIGNED, 2);
	Add_Field ("SPEED_AB", DB_DOUBLE, 5.1, KPH);
	Add_Field ("FSPD_AB", DB_DOUBLE, 5.1, KPH);
	Add_Field ("CAP_AB", DB_UNSIGNED, 8, VPH);
	Add_Field ("LANES_BA", DB_UNSIGNED, 2);
	Add_Field ("SPEED_BA", DB_DOUBLE, 5.1, KPH);
	Add_Field ("FSPD_BA", DB_DOUBLE, 5.1, KPH);
	Add_Field ("CAP_BA", DB_UNSIGNED, 8, VPH);
	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, USE_CODE, true);
	} else {
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

bool Link_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	node_a = Required_Field (ANODE_FIELD_NAMES);
	node_b = Required_Field (BNODE_FIELD_NAMES);
	length = Required_Field ("LENGTH", "DISTANCE", "LEN");
	lanes_ab = Required_Field ("LANES_AB", "PERMLANESB", "LANESAB");
	lanes_ba = Required_Field ("LANES_BA", "PERMLANESA", "LANESBA");
	type = Required_Field ("TYPE", "FUNCTCLASS", "FUNCL", "CLASS", "FTYPE");
	use = Required_Field ("USE", "VEHICLE");

	if (node_a < 0 || node_b < 0 || length < 0 || type < 0 || use < 0 || lanes_ab < 0 || lanes_ba < 0) return (false);

	//---- at least one speed field is required ---

	speed_ab = Optional_Field ("SPEED_AB", "SPEEDLMTB", "SPD_AB", "SPEEDAB", "MAXSPD_AB");
	fspd_ab = Optional_Field ("FSPD_AB", "FREESPDB", "FREESPD_AB", "FSPDAB");
	
	if (!speed_ab && !fspd_ab) {
		speed_ab = Required_Field ("SPEED_AB", "SPEEDLMTB", "FSPD_AB", "FREESPDB");
		return (false);
	}
	speed_ba = Optional_Field ("SPEED_BA", "SPEEDLMTA", "SPD_BA", "SPEEDBA", "MAXSPD_BA");
	fspd_ba = Optional_Field ("FSPD_BA", "FREESPDA", "FREESPD_BA", "FSPDBA");

	if (!speed_ba && !fspd_ba) {
		speed_ba = Required_Field ("SPEED_BA", "SPEEDLMTA", "FSPD_BA", "FREESPDA");
		return (false);
	}

	//---- optional fields ----

	link = Optional_Field ("LINK", "ID");
	area_type = Optional_Field (AREA_TYPE_FIELD_NAMES);
	cap_ab = Optional_Field ("CAP_AB", "CAPACITYB", "CAPACITY_AB", "CAPABITY_A");
	cap_ba = Optional_Field ("CAP_BA", "CAPACITYA", "CAPACITY_BA", "CAPACITY_B");

	setback_a = Optional_Field ("SETBACK_A", "SETBACKA");
	setback_b = Optional_Field ("SETBACK_B", "SETBACKB");
	
	bearing_a = Optional_Field ("BEARING_A", "BEARINGA");
	bearing_b = Optional_Field ("BEARING_B", "BEARINGB");

	divided = Optional_Field ("DIVIDED", "MEDIAN", "SPLIT", "D", "M");
	grade = Optional_Field ("GRADE", "PERCENT");

	left_ab = Optional_Field ("LEFT_AB", "LEFTPCKTSB", "LEFTAB");
	right_ab = Optional_Field ("RIGHT_AB", "RGHTPCKTSB", "RIGHTAB");

	left_ba = Optional_Field ("LEFT_BA", "LEFTPCKTSA", "LEFTBA");
	right_ba = Optional_Field ("RIGHT_BA", "RGHTPCKTSA", "RIGHTBA");

	if (exe->Notes_Name_Flag ()) {
		name = Optional_Field ("NAME", "STREET", "STREET_NAME", "ST_NAME", "STNAME");
	}
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (length, METERS);
	Set_Units (setback_a, METERS);
	Set_Units (setback_b, METERS);
	Set_Units (bearing_a, DEGREES);
	Set_Units (bearing_b, DEGREES);
	Set_Units (type, FACILITY_CODE);
	Set_Units (use, USE_CODE);
	Set_Units (grade, PERCENT);
	Set_Units (speed_ab, MPS);
	Set_Units (fspd_ab, MPS);
	Set_Units (cap_ab, VPH);
	Set_Units (speed_ba, MPS);
	Set_Units (fspd_ba, MPS);
	Set_Units (cap_ba, VPH);

	return (true);
}
