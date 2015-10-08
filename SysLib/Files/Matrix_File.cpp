//*********************************************************
//	Matrix_File.cpp - Matrix File Input/Output
//*********************************************************

#include "Matrix_File.hpp"

//---------------------------------------------------------
//	Matrix_File constructor
//---------------------------------------------------------

Matrix_File::Matrix_File (Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) :
	Db_Header (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();
}

Matrix_File::Matrix_File (string filename, Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) : 
	Db_Header (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();

	Open (filename);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Matrix_File::Setup (void)
{
	File_Type ("Matrix File");
	File_ID ("Matrix");

	Header_Lines (2);

	origin = destination = period = data = -1;
	num_org = num_des = 0;
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Matrix_File::Read_Header (bool stat)
{
	String meta = Header_Record ();
	if (!meta.empty ()) {
		Strings parts;
		Str_Itr itr;

		meta.Parse (parts, "=;");

		for (itr = parts.begin (); itr != parts.end (); itr++) {
			if (itr->Equals ("NUM_ORG")) {
				num_org = (++itr)->Integer ();
			} else if (itr->Equals ("NUM_DES")) {
				num_des = (++itr)->Integer ();
			} else if (itr->Equals ("PERIODS")) {
				clear ();
				Merge_Flag (false);
				if (!Add_Ranges (*(++itr))) return (false);
			} else if (itr->Equals ("MERGE")) {
				clear ();
				Merge_Flag (true);
				if (!Add_Ranges (*(++itr))) return (false);
			}
		}
	}
	return (Db_Header::Read_Header (stat));
}

//-----------------------------------------------------------
//	Write_Header
//-----------------------------------------------------------

bool Matrix_File::Write_Header (string user_lines) 
{
	user_lines = String ("NUM_ORG=%d; NUM_DES=%d; ") % num_org % num_des;
	if (Merge_Flag ()) {
		user_lines += "MERGE=";
	} else {
		user_lines += "PERIODS=";
	}
	for (int i=0; i < Num_Periods (); i++) {
		if (i) user_lines += ", ";
		user_lines += Range_Format (i);
	}
	return (Db_Header::Write_Header (user_lines));
}

//-----------------------------------------------------------
//	Org_Index
//-----------------------------------------------------------

int Matrix_File::Org_Index (int origin)
{
	Int_Map_Itr itr = org_map.find (origin);
	if (itr == org_map.end ()) {
		return (-1);
	} else {
		return (itr->second);
	}
}

//-----------------------------------------------------------
//	Des_Index
//-----------------------------------------------------------

int Matrix_File::Des_Index (int destination)
{
	Int_Map_Itr itr = des_map.find (destination);
	if (itr == des_map.end ()) {
		return (-1);
	} else {
		return (itr->second);
	}
}

//-----------------------------------------------------------
//	Add_Org
//-----------------------------------------------------------

int Matrix_File::Add_Org (int origin)
{
	Int_Map_Stat stat = org_map.insert (Int_Map_Data (origin, (int) org_map.size ()));
	return (stat.first->second);
}

//-----------------------------------------------------------
//	Add_Des
//-----------------------------------------------------------

int Matrix_File::Add_Des (int destination)
{
	Int_Map_Stat stat = des_map.insert (Int_Map_Data (destination, (int) des_map.size ()));
	return (stat.first->second);
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Matrix_File::Create_Fields (void) 
{
	if (od_units != LOCATION_OD) {
		Add_Field ("ORIGIN", DB_UNSIGNED, 2, od_units, true);
		Add_Field ("DESTINATION", DB_UNSIGNED, 2, od_units, true);
	} else {
		Add_Field ("ORIGIN", DB_INTEGER, 10, od_units);
		Add_Field ("DESTINATION", DB_INTEGER, 10, od_units);
	}
	Add_Field ("PERIOD", DB_UNSIGNED, 1, NO_UNITS, true);

	if (Data_Type () == TRIP_TABLE) {
		Add_Field ("TRIPS", DB_INTEGER, 4, NO_UNITS, true);
	} else if (Data_Type () == DATA_TABLE) {
		Add_Field ("FACTOR", DB_DOUBLE, 2.6, NO_UNITS, true);
	} else {
		Add_Field ("COUNT", DB_INTEGER, 2, NO_UNITS, true);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Matrix_File::Set_Field_Numbers (void)
{
	origin = Required_Field (ORIGIN_FIELD_NAMES);
	destination = Required_Field (DESTINATION_FIELD_NAMES);

	//---- optional fields ----

	period = Optional_Field ("PERIOD", "TIME_CODE", "INTERVAL");
	data = Optional_Field ("DATA", "TRIPS", "COUNT", "FACTOR", "TIME");

	if (Optional_Field ("TRIPS") >= 0) {
		Data_Type (TRIP_TABLE);
	} else if (Optional_Field ("DATA", "FACTOR") >= 0) {
		Data_Type (DATA_TABLE);
	} else if (Optional_Field ("TIME") >= 0) {
		Data_Type (TIME_TABLE);
	} else {
		Data_Type (SKIM_TABLE);
	}
	Set_Units (origin, od_units);
	Set_Units (destination, od_units);

	Units_Type units = Get_Units (origin);
	if (units != NO_UNITS) OD_Units (units);

	return (true);
}

