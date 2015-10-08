//********************************************************* 
//	Veh_Type_File.cpp - Vehicle Type File Input/Output
//*********************************************************

#include "Veh_Type_File.hpp"

//-----------------------------------------------------------
//	Veh_Type_File constructors
//-----------------------------------------------------------

Veh_Type_File::Veh_Type_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Veh_Type_File::Veh_Type_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Veh_Type_File::Setup (void)
{
	File_Type ("Vehicle Type File");
	File_ID ("VehType");

	type = length = max_speed = max_accel = max_decel = op_cost = use = capacity = -1;
	load = unload = method = min_dwell = max_dwell = subtype = -1;
	memset (grade, -1, sizeof (grade));
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Veh_Type_File::Create_Fields (void) 
{
	Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
	Add_Field ("LENGTH", DB_DOUBLE, 5.1, METERS);
	Add_Field ("MAX_SPEED", DB_DOUBLE, 5.1, KPH);
	Add_Field ("MAX_ACCEL", DB_DOUBLE, 5.1, MPS2);
	Add_Field ("MAX_DECEL", DB_DOUBLE, 5.1, MPS2);
	Add_Field ("OP_COST", DB_DOUBLE, 5.1, CENT_KM);

	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, VEH_USE_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, 20, VEH_USE_CODE);
	}
	Add_Field ("CAPACITY", DB_UNSIGNED, 4, PPV);
	Add_Field ("LOADING", DB_DOUBLE, 6.2, SPP);
	Add_Field ("UNLOADING", DB_DOUBLE, 6.2, SPP);
	if (Code_Flag ()) {
		Add_Field ("METHOD", DB_UNSIGNED, 1, LOADING_CODE, true);
	} else {
		Add_Field ("METHOD", DB_STRING, 20, LOADING_CODE);
	}
	Add_Field ("MIN_DWELL", DB_TIME, 6.1, SECONDS);
	Add_Field ("MAX_DWELL", DB_TIME, 6.1, SECONDS);
	Add_Field ("GRADE_1", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_2", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_3", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_4", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_5", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_6", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_7", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_8", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_9", DB_DOUBLE, 6.2);
	Add_Field ("GRADE_10", DB_DOUBLE, 6.2);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Veh_Type_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	type = Required_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	length = Required_Field ("LENGTH", "SIZE");
	max_speed = Required_Field ("MAX_SPEED", "MAXVEL", "MAXSPEED", "MAXSPD");
	max_accel = Required_Field ("MAX_ACCEL", "MAXACCEL");
	capacity = Required_Field ("CAPACITY", "PASSENGERS");
	
	if (type < 0 || length < 0 || max_speed < 0 || max_accel < 0 || capacity < 0) return (false);

	//---- optional fields ----

	max_decel = Optional_Field ("MAX_DECEL", "MAXDECEL");
	op_cost = Optional_Field ("OP_COST", "OPCOST", "COST");
	use = Optional_Field ("USE", "ACCESS");
	load = Optional_Field ("LOADING", "LOAD", "BOARD", "BOARDING", "ON_RATE");
	unload = Optional_Field ("UNLOADING", "UNLOAD", "ALIGHT", "ALIGHTING", "OFF_RATE");
	method = Optional_Field ("METHOD", "LOAD_TYPE", "LOAD_METHOD");
	min_dwell = Optional_Field ("MIN_DWELL", "MINDWELL", "DWELL");
	max_dwell = Optional_Field ("MAX_DWELL", "MAXDWELL", "DWELL");

	grade [0] = Optional_Field ("GRADE_1", "GRADE1", "GRADE_FAC_1", "GRADEFAC1");
	grade [1] = Optional_Field ("GRADE_2", "GRADE2", "GRADE_FAC_2", "GRADEFAC2");
	grade [2] = Optional_Field ("GRADE_3", "GRADE3", "GRADE_FAC_3", "GRADEFAC3");
	grade [3] = Optional_Field ("GRADE_4", "GRADE4", "GRADE_FAC_4", "GRADEFAC4");
	grade [4] = Optional_Field ("GRADE_5", "GRADE5", "GRADE_FAC_5", "GRADEFAC5");
	grade [5] = Optional_Field ("GRADE_6", "GRADE6", "GRADE_FAC_6", "GRADEFAC6");
	grade [6] = Optional_Field ("GRADE_7", "GRADE7", "GRADE_FAC_7", "GRADEFAC7");
	grade [7] = Optional_Field ("GRADE_8", "GRADE8", "GRADE_FAC_8", "GRADEFAC8");
	grade [8] = Optional_Field ("GRADE_9", "GRADE9", "GRADE_FAC_9", "GRADEFAC9");
	grade [9] = Optional_Field ("GRADE_10", "GRADE10", "GRADE_FAC_10", "GRADEFAC10");

	//---- Version 4.0 compatibility ----

	subtype = Optional_Field ("SUBTYPE", "SUB_TYPE", "VSUBTYPE");
	Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (type, VEH_TYPE);
	Set_Units (length, METERS);
	Set_Units (max_speed, MPS);
	Set_Units (max_accel, MPS2);
	Set_Units (max_decel, MPS2);
	Set_Units (op_cost, CENT_KM);
	Set_Units (use, VEH_USE_CODE);
	Set_Units (capacity, PPV);
	Set_Units (load, SPP);
	Set_Units (unload, SPP);
	Set_Units (method, LOADING_CODE);
	Set_Units (min_dwell, SECONDS);
	Set_Units (max_dwell, SECONDS);

	return (true);
}
