//*********************************************************
//	Skim_File.cpp - Skim File Input/Output
//*********************************************************

#include "Skim_File.hpp"

//---------------------------------------------------------
//	Skim_File constructor
//---------------------------------------------------------

Skim_File::Skim_File (Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) :
	Matrix_File (access, format, type, od)
{
	Setup ();
}

Skim_File::Skim_File (string filename, Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) : 
	Matrix_File (access, format, type, od)
{
	Setup ();

	Open (filename);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Skim_File::Setup (void)
{
	Matrix_File::Setup ();

	File_Type ("Skim File");
	File_ID ("Skim");

	time_format = SECONDS;
	len_format = METERS;
	time = walk = drive = transit = wait = other = length = cost = impedance = -1;
	time_flag = (Data_Type () == TIME_TABLE);
	table_flag = len_flag = false;
	mode = MAX_MODE;
	near_factor = 0;
	table = 0;
	ttime = 0;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Skim_File::Create_Fields (void) 
{
	Clear_Fields ();

	Matrix_File::Create_Fields ();

	if (time_flag) {
		Add_Field ("TIME", DB_TIME, 4, Time_Format (), true);
	} else {
		if (mode == WAIT_MODE) {
			Add_Field ("WAIT", DB_TIME, 2, Time_Format (), true);
		} else if (mode == BIKE_MODE || mode == OTHER_MODE) {
			Add_Field ("OTHER", DB_TIME, 2, Time_Format (), true);
		} else {
			Add_Field ("WALK", DB_TIME, 2, Time_Format (), true);

			if (mode != WALK_MODE) {
				if (mode != RIDE_MODE && mode != TRANSIT_MODE && mode != TAXI_MODE) {
					Add_Field ("DRIVE", DB_TIME, 4, Time_Format (), true);
				}
				if (mode != DRIVE_MODE) {
					if (mode != RIDE_MODE && mode != TAXI_MODE) {
						Add_Field ("TRANSIT", DB_TIME, 4, Time_Format (), true);
					}
					Add_Field ("WAIT", DB_TIME, 2, Time_Format (), true);
				}
				Add_Field ("OTHER", DB_TIME, 2, Time_Format (), true);
			}
		}
	}
	if (Length_Format () == MILES || Length_Format () == KILOMETERS) {
		Add_Field ("LENGTH", DB_DOUBLE, 4.2, Length_Format (), true);
	} else {
		Add_Field ("LENGTH", DB_INTEGER, 4, Length_Format (), true);
	}
	if (len_flag) {
		Db_Field *fld = Field ("LENGTH");

		if (fld->Units () != Length_Format ()) {
			fld->Units (Length_Format ());
		}
	}
	Add_Field ("COST", DB_INTEGER, 2, CENTS, true);
	Add_Field ("IMPEDANCE", DB_INTEGER, 4, IMPEDANCE, true);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Skim_File::Set_Field_Numbers (void)
{
	Matrix_File::Set_Field_Numbers ();

	//---- optional fields ----

	time = Optional_Field (TIME_SKIM_FIELD_NAMES);
	walk = Optional_Field (WALK_SKIM_FIELD_NAMES);
	drive = Optional_Field (DRIVE_SKIM_FIELD_NAMES);
	transit = Optional_Field (TRANSIT_SKIM_FIELD_NAMES);
	wait = Optional_Field (WAIT_SKIM_FIELD_NAMES);
	other = Optional_Field (OTHER_SKIM_FIELD_NAMES);
	cost = Optional_Field (COST_SKIM_FIELD_NAMES);
	length = Optional_Field (LENGTH_SKIM_FIELD_NAMES);
	impedance = Optional_Field (IMP_SKIM_FIELD_NAMES);

	if (File_Access () != CREATE) Total_Time_Flag ((time >= 0));
	
	//---- set default units ----

	Set_Units (time, SECONDS);
	Set_Units (walk, SECONDS);
	Set_Units (drive, SECONDS);
	Set_Units (transit, SECONDS);
	Set_Units (wait, SECONDS);
	Set_Units (other, SECONDS);
	Set_Units (cost, CENTS);
	Set_Units (length, METERS);
	Set_Units (impedance, IMPEDANCE);

	return (true);
}

//---------------------------------------------------------
//	Set_Table
//---------------------------------------------------------

bool Skim_File::Set_Table (int num_org, int num_des, int periods)
{
	Clear_Table ();
	if (num_org < 1 || num_des < 1 || periods < 1) return (false);

	Num_Org (0);
	Num_Des (0);

	if (table_flag) {
		ttime = new Dtime ** [num_org];
		if (ttime == 0) return (false);

		for (int org=0; org < num_org; org++) {
			ttime [org] = new Dtime * [num_des];
			if (ttime [org] == 0) return (false);

			for (int des=0; des < num_des; des++) {
				ttime [org] [des] = new Dtime [periods];
				if (ttime [org] [des] == 0) return (false);

				memset (ttime [org] [des], '\0', periods * sizeof (Dtime));
			}
		}
	} else {
		table = new Skim_Data ** [num_org];
		if (table == 0) return (false);

		for (int org=0; org < num_org; org++) {
			table [org] = new Skim_Data * [num_des];
			if (table [org] == 0) return (false);

			for (int des=0; des < num_des; des++) {
				table [org] [des] = new Skim_Data [periods];
				if (table [org] [des] == 0) return (false);

				memset (table [org] [des], '\0', periods * sizeof (Skim_Data));
			}
		}
	}
	Num_Org (num_org);
	Num_Des (num_des);
	return (true);
}

//---------------------------------------------------------
//	Clear_Table
//---------------------------------------------------------

void Skim_File::Clear_Table (void)
{
	if (table != 0) {
		for (int org=0; org < Num_Org (); org++) {
			for (int des=0; des < Num_Des (); des++) {
				delete [] table [org] [des];
			}
			delete [] table [org];
		}
		delete [] table;
		table = 0;
	}
	if (ttime != 0) {
		for (int org=0; org < Num_Org (); org++) {
			for (int des=0; des < Num_Des (); des++) {
				delete [] ttime [org] [des];
			}
			delete [] ttime [org];
		}
		delete [] ttime;
		ttime = 0;
	}
}
