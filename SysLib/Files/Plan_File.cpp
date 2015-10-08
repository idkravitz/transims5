
//*********************************************************
//	Plan_File.cpp - Plan File Input/Output
//*********************************************************

#include "Plan_File.hpp"

//---------------------------------------------------------
//	Plan_File constructor
//---------------------------------------------------------

Plan_File::Plan_File (Access_Type access, Format_Type format) : 
	Trip_File (access, format)
{
	Setup ();
}

Plan_File::Plan_File (string filename, Access_Type access, Format_Type format) : 
	Trip_File (access, format)
{
	Setup ();

	Open (filename);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Plan_File::Setup (void)
{
	Trip_File::Setup ();

	File_Type ("Plan File");
	File_ID ("Plan");

	Num_Files (0);
	Nest (NESTED);
	Header_Lines (2);

	depart = arrive = walk = drive = transit = wait = other = length = cost = impedance = -1;
	leg_mode = leg_type = leg_id = leg_time = leg_length = leg_cost = leg_imp = -1;
}

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

bool Plan_File::Read_Plan (Plan_Data &plan_rec)
{
	if (!Read (false)) return (false);

	int i, num;

	plan_rec.Household (Household ());
	if (plan_rec.Household () <= 0) return (true);

	plan_rec.Person (Person ());
	plan_rec.Tour (Tour ());
	plan_rec.Trip (Trip ());

	plan_rec.Start (Start ());
	plan_rec.End (End ());
	plan_rec.Duration (Duration ());

	plan_rec.Origin (Origin ());
	plan_rec.Destination (Destination ());

	plan_rec.Purpose (Purpose ());
	plan_rec.Mode (Mode ());
	plan_rec.Constraint (Constraint ());
	plan_rec.Priority (Priority ());

	plan_rec.Vehicle (Vehicle ());
	plan_rec.Passengers (Passengers ());
	plan_rec.Type (Type ());

	if (Partition_Flag ()) {
		plan_rec.Partition (Partition ());
	} else {
		plan_rec.Partition (Part_Number ());
	}
	plan_rec.Depart (Depart ());
	plan_rec.Arrive (Arrive ());
	plan_rec.Activity (Activity ());

	plan_rec.Walk (Walk ());
	plan_rec.Drive (Drive ());
	plan_rec.Transit (Transit ());
	plan_rec.Wait (Wait ());
	plan_rec.Other (Other ());
	plan_rec.Length (Length ());
	plan_rec.Cost (Cost ());
	plan_rec.Impedance (Impedance ());

	Add_Trip (plan_rec.Household (), plan_rec.Person (), plan_rec.Tour ());

	//---- number of nested records ----

	plan_rec.clear ();

	num = Num_Nest ();

	if (num > 0) {
		plan_rec.reserve (num);
	}
	for (i=1; i <= num; i++) {
		if (!Read (true)) {
			exe->Warning (String ("Number of Nested Records for Plan %d") % Household ());
			return (false);
		}
		//---- process a nested person record ----

		Plan_Leg leg_rec;

		leg_rec.Mode (Leg_Mode ());
		leg_rec.Type (Leg_Type ());
		leg_rec.ID (Leg_ID ());
		leg_rec.Time (Leg_Time ());
		leg_rec.Length (Leg_Length ());
		leg_rec.Cost (Leg_Cost ());
		leg_rec.Impedance (Leg_Impedance ());

		plan_rec.push_back (leg_rec);
		Add_Leg ();
	}
	return (true);
}

//---------------------------------------------------------
//	Write_Plan
//---------------------------------------------------------

bool Plan_File::Write_Plan (Plan_Data &plan_rec)
{
	Household (plan_rec.Household ());
	Person (plan_rec.Person ());
	Tour (plan_rec.Tour ());
	Trip (plan_rec.Trip ());

	Start (plan_rec.Start ());
	End (plan_rec.End ());
	Duration (plan_rec.Duration ());

	Origin (plan_rec.Origin ());
	Destination (plan_rec.Destination ());

	Purpose (plan_rec.Purpose ());
	Mode (plan_rec.Mode ());
	Constraint (plan_rec.Constraint ());
	Priority (plan_rec.Priority ());

	Vehicle (plan_rec.Vehicle ());
	Passengers (plan_rec.Passengers ());
	Type (plan_rec.Type ());
	Partition (plan_rec.Partition ());

	Depart (plan_rec.Depart ());
	Arrive (plan_rec.Arrive ());
	Activity (plan_rec.Activity ());

	Walk (plan_rec.Walk ());
	Drive (plan_rec.Drive ());
	Transit (plan_rec.Transit ());
	Wait (plan_rec.Wait ());
	Other (plan_rec.Other ());
	Length (exe->UnRound (plan_rec.Length ()));
	Cost (exe->UnRound (plan_rec.Cost ()));
	Impedance (plan_rec.Impedance ());

	Num_Nest ((int) plan_rec.size ());

	if (!Write (false)) {
		exe->Error (String ("Writing %s") % File_Type ());
	}
	Add_Trip (plan_rec.Household (), plan_rec.Person (), plan_rec.Tour ());

	//---- write the nested records ----

	for (Plan_Leg_Itr itr = plan_rec.begin (); itr != plan_rec.end (); itr++) {
		Leg_Mode (itr->Mode ());
		Leg_Type (itr->Type ());
		Leg_ID (itr->ID ());
		Leg_Time (itr->Time ());
		Leg_Length (exe->UnRound (itr->Length ()));
		Leg_Cost (exe->UnRound (itr->Cost ()));
		Leg_Impedance (itr->Impedance ());

		if (!Write (true)) {
			exe->Error (String ("Writing %s") % File_Type ());
		}
		Add_Leg ();
	}
	return (true);
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Plan_File::Create_Fields (void) 
{
	//bool temp = exe->Notes_Name_Flag ();
	//exe->Notes_Name_Flag (false);

	Trip_File::Create_Fields ();

	//exe->Notes_Name_Flag (temp);

	Add_Field ("DEPART", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("ARRIVE", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("ACTIVITY", DB_TIME, TIME_FIELD_SIZE, Time_Format ());

	Add_Field ("WALK", DB_TIME, 2, SECONDS, true);
	Add_Field ("DRIVE", DB_TIME, 4, SECONDS, true);
	Add_Field ("TRANSIT", DB_TIME, 4, SECONDS, true);
	Add_Field ("WAIT", DB_TIME, 2, SECONDS, true);
	Add_Field ("OTHER", DB_TIME, 2, SECONDS, true);
	Add_Field ("LENGTH", DB_INTEGER, 4, METERS, true);
	Add_Field ("COST", DB_FIXED, 2.1, CENTS, true);
	Add_Field ("IMPEDANCE", DB_UNSIGNED, 4, IMPEDANCE, true);
	Add_Field ("NUM_LEGS", DB_INTEGER, 2, NEST_COUNT, true);

	if (Code_Flag ()) {
		Add_Field ("LEG_MODE", DB_INTEGER, 1, MODE_CODE, true, NESTED);
		Add_Field ("LEG_TYPE", DB_INTEGER, 1, ID_CODE, true, NESTED);
	} else {
		Add_Field ("LEG_MODE", DB_STRING, 12, MODE_CODE, true, NESTED);
		Add_Field ("LEG_TYPE", DB_STRING, 10, ID_CODE, true, NESTED);
	}
	Add_Field ("LEG_ID", DB_INTEGER, 4, NO_UNITS, true, NESTED);
	Add_Field ("LEG_TIME", DB_TIME, 2, SECONDS, true, NESTED);
	Add_Field ("LEG_LENGTH", DB_UNSIGNED, 2, METERS, true, NESTED);
	Add_Field ("LEG_COST", DB_FIXED, 2.1, CENTS, true, NESTED);
	Add_Field ("LEG_IMPED", DB_INTEGER, 4, IMPEDANCE, true, NESTED);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Plan_File::Set_Field_Numbers (void)
{
	Trip_File::Set_Field_Numbers ();

	//---- required fields ----
	
	Num_Nest_Field (Optional_Field ("NUM_LEGS", "LEGS", "NUMLEGS", "NLEGS", "NUM_NEST"));

	leg_mode = Optional_Field ("LEG_MODE", "LEGMODE", "LMODE");
	leg_type = Optional_Field ("LEG_TYPE", "LEGTYPE", "LTYPE");
	leg_id = Optional_Field ("LEG_ID", "LEGID", "LID");

	//---- optional fields ----

	depart = Optional_Field (DEPART_FIELD_NAMES);
	arrive = Optional_Field (ARRIVE_FIELD_NAMES);
	activity = Optional_Field ("ACTIVITY");

	walk = Optional_Field (WALK_SKIM_FIELD_NAMES);
	drive = Optional_Field (DRIVE_SKIM_FIELD_NAMES);
	transit = Optional_Field (TRANSIT_SKIM_FIELD_NAMES);
	wait = Optional_Field (WAIT_SKIM_FIELD_NAMES);
	other = Optional_Field (OTHER_SKIM_FIELD_NAMES);
	length = Optional_Field (LENGTH_SKIM_FIELD_NAMES);
	cost = Optional_Field (COST_SKIM_FIELD_NAMES);
	impedance = Optional_Field (IMP_SKIM_FIELD_NAMES);

	leg_time = Optional_Field ("LEG_TIME", "LEGTIME", "LTIME");
	leg_length = Optional_Field ("LEG_LENGTH", "LEGLEN", "LEGDIST", "LLENGTH");
	leg_cost = Optional_Field ("LEG_COST", "LEGCOST", "LCOST");
	leg_imp = Optional_Field ("LEG_IMPED", "LEGIMPED", "LEG_IMP", "LEGIMP", "LIMP");
	
	//---- set default units ----

	Set_Units (depart, DAY_TIME);
	Set_Units (arrive, DAY_TIME);
	Set_Units (activity, DAY_TIME);
	Set_Units (walk, SECONDS);
	Set_Units (drive, SECONDS);
	Set_Units (transit, SECONDS);
	Set_Units (wait, SECONDS);
	Set_Units (other, SECONDS);
	Set_Units (length, METERS);
	Set_Units (cost, CENTS);
	Set_Units (impedance, IMPEDANCE);
	Set_Units (leg_mode, MODE_CODE);
	Set_Units (leg_type, ID_CODE);
	Set_Units (leg_time, SECONDS);
	Set_Units (leg_length, METERS);
	Set_Units (leg_cost, CENTS);
	Set_Units (leg_imp, IMPEDANCE);

	return (true);
}

