//*********************************************************
//	File_Group.cpp - trip table control parameters
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	File_Group constructor
//---------------------------------------------------------

File_Group::File_Group (void)
{
	Type (0);
	Purpose (0);
	Mode (0);
	Time_Point (0);
	Constraint (0);
	Org_Wt (-1);
	Des_Wt (-1);
	Dist_Wt (true);
	Stop_Wt (-1);
	Return_Flag (false);
	Duration (0);
	Priority (0);
	Speed (0);
	Veh_Type (0);
	Passengers (0);
	Time_Field ("");
	Scaling_Factor (1.0);

	default_factor.Factor (1.0);
	default_factor.Bucket (0.45);

	trip_file = 0;
	factor_file = 0;
	traveler_script = 0;
	diurnal_file = 0;
	diurnal_script = 0;
	diurnal_header = 0;
}

//---------------------------------------------------------
//	File_Group destructor
//---------------------------------------------------------

File_Group::~File_Group (void)
{
	if (trip_file != 0) delete trip_file;
	if (factor_file != 0) delete factor_file;
	if (traveler_script != 0) delete traveler_script;
	if (diurnal_file != 0) delete diurnal_file;
	if (diurnal_script != 0) delete diurnal_script;
	if (diurnal_header != 0) delete  diurnal_header;
}

//---- Time Field ----

bool File_Group::Time_Field (string field)
{
	if (diurnal_file == 0 || field.empty ()) return (false);

	diurnal_file->Share_Field (diurnal_file->Field_Number (field));

	return (Time_Field () >= 0);
}

//---- Set_Periods ----

bool File_Group::Set_Periods (String &str) 
{
	if (str.Equals ("ALL")) return (true);

	return (periods.Add_Ranges (str));
}

//---- Open Trips ----

void File_Group::Open_Trips (string &name, string &format)
{
	if (!name.empty ()) {
		trip_file = new Matrix_File ();
		trip_file->File_Type (String ("Trip Table File #%d") % group);
		if (!format.empty ()) trip_file->Dbase_Format (format); 
		trip_file->Open (exe->Project_Filename (name));
	}
}

//---- Open Factors ----

void File_Group::Open_Factors (string &name, string &format)
{
	if (!name.empty ()) {
		factor_file = new Factor_File ();
		factor_file->File_Type (String ("Trip Factor File #%d") % group);
		if (!format.empty ()) factor_file->Dbase_Format (format); 
		factor_file->Open (exe->Project_Filename (name));
	}
}

//---- Period Factor ----

Factor_Period * File_Group::Period_Factor (int org, int des, int period)
{
	Matrix_Index index;
	Factor_Table_Itr factor_itr;

	index.Origin (org);
	index.Destination (des);
	if (period < 0) period = 0;

	factor_itr = factor_table.find (index);
	if (factor_itr != factor_table.end ()) {
		return (&factor_itr->second [period]);
	}
	return (&default_factor);
}

//---- Open Diurnal ----

void File_Group::Open_Diurnal (string &name, string &format)
{
	if (!name.empty ()) {
		diurnal_file = new Diurnal_File ();
		diurnal_file->File_Type (String ("Time Distribution File #%d") % group);
		if (!format.empty ()) diurnal_file->Dbase_Format (format); 
		diurnal_file->Open (exe->Project_Filename (name));
	}
}

//---- Diurnal Script ----

void File_Group::Diurnal_Script (string name)
{
	if (!name.empty ()) {
		diurnal_script = new Db_File ();
		diurnal_script->File_Type (String ("Time Distribution Script #%d") % group);
		diurnal_script->Open (exe->Project_Filename (name));
	}
}

//---- Execute_Diurnal ----

int File_Group::Execute_Diurnal (void) 
{
	if (diurnal.Compiled ()) {
		return (diurnal.Execute ());
	}
	return (1);
}

//---- Compile_Diurnal ----

bool File_Group::Compile_Diurnal (bool report) 
{
	if (trip_file == 0 || diurnal_file == 0 || diurnal_script == 0) return (false);

	int i, offset;
	Db_Field *fld_ptr, fld_rec;
	Db_Base_Array files;

	diurnal_header = new Db_Base (READ, BINARY);
	diurnal_header->File_ID (diurnal_file->File_ID ());
	diurnal_header->File_Type (diurnal_file->File_Type ());
	offset = 0;

	for (i=0; i < diurnal_file->Num_Fields (); i++) {
		if (i == diurnal_file->Start_Field () || i == diurnal_file->End_Field ()) continue;
	
		fld_ptr = diurnal_file->Field (i);

		diurnal_header->Add_Field (fld_ptr->Name (), DB_INTEGER, sizeof (int), NO_UNITS, true, NO_NEST, offset);

		offset += sizeof (int);
	}
	for (i=0; i < diurnal_header->Num_Fields (); i++) {
		diurnal_header->Put_Field (i, (i+1));
	}
	files.push_back (trip_file);
	files.push_back (diurnal_header);

	if (!diurnal.Initialize (files)) return (false);

	return (diurnal.Compile (*diurnal_script, report));
}

//---- diurnal time range ----

void File_Group::Diurnal_Time_Range (int period, Dtime &low, Dtime &high) 
{
	Diurnal_Data *diurnal_ptr = &diurnal_array [period];
	low = diurnal_ptr->Start_Time ();
	high = diurnal_ptr->End_Time ();
}

//---- Traveler Script ----

void File_Group::Traveler_Script (string name)
{
	if (!name.empty ()) {
		traveler_script = new Db_File ();
		traveler_script->File_Type (String ("Traveler Type Script #%d") % group);
		traveler_script->Open (exe->Project_Filename (name));
	}
}

//---- Execute_Traveler ----

int File_Group::Execute_Traveler (void) 
{
	if (traveler.Compiled ()) {
		return (traveler.Execute ());
	}
	return (1);
}

//---- Compile_Traveler ----

bool File_Group::Compile_Traveler (Db_Base *traveler_file, bool report) 
{
	if (traveler_script == 0) return (false);

	Db_Base_Array files;

	files.push_back (traveler_file);

	if (!traveler.Initialize (files)) return (false);

	return (traveler.Compile (*traveler_script, report));
}

