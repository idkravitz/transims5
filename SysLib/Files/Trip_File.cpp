//********************************************************* 
//	Trip_File.cpp - Trip File Input/Output
//*********************************************************

#include "Trip_File.hpp"

//-----------------------------------------------------------
//	Trip_File constructors
//-----------------------------------------------------------

Trip_File::Trip_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Trip_File::Trip_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Trip_File::Setup (void)
{
	File_Type ("Trip File");
	File_ID ("Trip");

	num_record = num_hhold = num_person = num_tour = num_trip = last_hhold = last_person = last_tour = 0;
	hhold = person = tour = trip = start = end = duration = origin = destination = purpose = mode = -1;
	constraint = priority = vehicle = passengers = type = partition = -1;
	opt_flag = false;
}

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

bool Trip_File::Read_Trip (Trip_Data &trip_rec)
{
	if (!Read (false)) return (false);

	Get_Data (trip_rec);

	if (trip_rec.Household () <= 0) return (true);

	Add_Trip (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour ());

	return (true);
}

//---------------------------------------------------------
//	Write_Trip
//---------------------------------------------------------

bool Trip_File::Write_Trip (Trip_Data &trip_rec)
{
	Put_Data (trip_rec);

	if (!Write (false)) {
		exe->Error (String ("Writing %s") % File_Type ());
	}
	Add_Trip (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour ());

	return (true);
}

//---------------------------------------------------------
//	Get_Data
//---------------------------------------------------------

void Trip_File::Get_Data (Trip_Data &trip_rec)
{
	trip_rec.Household (Household ());
	trip_rec.Person (Person ());
	trip_rec.Tour (Tour ());
	trip_rec.Trip (Trip ());

	trip_rec.Start (Start ());
	trip_rec.End (End ());
	trip_rec.Duration (Duration ());
	trip_rec.Origin (Origin ());
	trip_rec.Destination (Destination ());

	trip_rec.Purpose (Purpose ());
	trip_rec.Mode (Mode ());
	trip_rec.Constraint (Constraint ());
	trip_rec.Priority (Priority ());

	trip_rec.Vehicle (Vehicle ());
	trip_rec.Passengers (Passengers ());
	trip_rec.Type (Type ());

	if (Partition_Flag ()) {
		trip_rec.Partition (Partition ());
	} else {
		trip_rec.Partition (Part_Number ());
	}
}

//---------------------------------------------------------
//	Put_Data
//---------------------------------------------------------

void Trip_File::Put_Data (Trip_Data &trip_rec)
{
	Household (trip_rec.Household ());
	Person (trip_rec.Person ());
	Tour (trip_rec.Tour ());
	Trip (trip_rec.Trip ());

	Start (trip_rec.Start ());
	End (trip_rec.End ());
	Duration (trip_rec.Duration ());

	Origin (trip_rec.Origin ());
	Destination (trip_rec.Destination ());

	Purpose (trip_rec.Purpose ());
	Mode (trip_rec.Mode ());
	Constraint (trip_rec.Constraint ());
	Priority (trip_rec.Priority ());

	Vehicle (trip_rec.Vehicle ());
	Passengers (trip_rec.Passengers ());
	Type (trip_rec.Type ());
	Partition (trip_rec.Partition ());
}

//---------------------------------------------------------
//	Get_Index
//---------------------------------------------------------

void Trip_File::Get_Index (Trip_Index &index)
{
	index.Household (Household ());
	index.Person (Person ());
	index.Tour (Tour ());
	index.Trip (Trip ());
}

void Trip_File::Get_Index (Time_Index &index)
{
	index.Start (Start ());
	index.Household (Household ());
	index.Person (Person ());
}

//---------------------------------------------------------
//	Add_Trip
//---------------------------------------------------------

void Trip_File::Add_Trip (int hhold, int person, int tour)
{
	num_record++;
	num_trip++;
	if (hhold != last_hhold) {
		num_hhold++;
		num_person++;
		num_tour++;
	} else if (person != last_person) {
		num_person++;
		num_tour++;
	} else if (tour != last_tour) {
		num_tour++;
	}
	last_hhold = hhold;
	last_person = person;
	last_tour = tour;
}

//---------------------------------------------------------
//	Print_Summary
//---------------------------------------------------------

void Trip_File::Print_Summary (void)
{
	String message ("Number of ");
	message += File_Type ();

	if (Num_Files () > 1) {
		exe->Break_Check (7);
		exe->Print (2, message) << " Partitions = " << Num_Files ();
	} else {
		exe->Break_Check (6);
		exe->Print (1);
	}
	exe->Print (1, message) << " Records    = " << Num_Records ();
	exe->Print (1, message) << " Households = " << Num_Households ();
	exe->Print (1, message) << " Persons    = " << Num_Persons ();
	exe->Print (1, message) << " Tours      = " << Num_Tours ();
	exe->Print (1, message) << " Trips      = " << Num_Trips ();
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Trip_File::Create_Fields (void) 
{
	Add_Field ("HHOLD", DB_INTEGER, 4, NO_UNITS, true);
	Add_Field ("PERSON", DB_INTEGER, 2, NO_UNITS, true);
	Add_Field ("TOUR", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("TRIP", DB_INTEGER, 1, NO_UNITS, true);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("DURATION", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("ORIGIN", DB_INTEGER, 10);
	Add_Field ("DESTINATION", DB_INTEGER, 10);
	Add_Field ("PURPOSE", DB_INTEGER, 2);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_INTEGER, 1, MODE_CODE, true);
		Add_Field ("CONSTRAINT", DB_INTEGER, 1, CONSTRAINT_CODE, true);
		Add_Field ("PRIORITY", DB_INTEGER, 1, PRIORITY_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 12, MODE_CODE);
		Add_Field ("CONSTRAINT", DB_STRING, 14, CONSTRAINT_CODE);
		Add_Field ("PRIORITY", DB_STRING, 10, PRIORITY_CODE);
	}
	Add_Field ("VEHICLE", DB_INTEGER, 4);
	Add_Field ("PASSENGERS", DB_INTEGER, 2);
	Add_Field ("TYPE", DB_INTEGER, 4);

	//if (exe->Notes_Name_Flag ()) {
	//	Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	//}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Trip_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	hhold = Required_Field (HHOLD_FIELD_NAMES);

	if (opt_flag) {
		if (hhold < 0) return (false);

		trip = Optional_Field (TRIP_FIELD_NAMES);
		mode = Optional_Field (MODE_FIELD_NAMES);
		start = Optional_Field (START_FIELD_NAMES);
		end = Optional_Field (END_FIELD_NAMES);
		origin = Optional_Field (ORIGIN_FIELD_NAMES);
		destination = Optional_Field (DESTINATION_FIELD_NAMES);
	} else {
		trip = Required_Field (TRIP_FIELD_NAMES);
		mode = Required_Field (MODE_FIELD_NAMES);
		start = Required_Field (START_FIELD_NAMES);
		end = Required_Field (END_FIELD_NAMES);
		origin = Required_Field (ORIGIN_FIELD_NAMES);
		destination = Required_Field (DESTINATION_FIELD_NAMES);

		if (hhold < 0 || trip < 0 || mode < 0 || start < 0 || origin < 0 || end < 0 || destination < 0) return (false);
	}

	//---- optional fields ----

	person = Optional_Field (PERSON_FIELD_NAMES);
	tour = Optional_Field (TOUR_FIELD_NAMES);
	purpose = Optional_Field (PURPOSE_FIELD_NAMES);
	duration = Optional_Field (DURATION_FIELD_NAMES);
	constraint = Optional_Field (CONSTRAINT_FIELD_NAMES);
	priority = Optional_Field (PRIORITY_FIELD_NAMES);
	vehicle = Optional_Field (VEHICLE_FIELD_NAMES);
	passengers = Optional_Field (NUM_PASS_FIELD_NAMES);
	type = Optional_Field ("TYPE", "HHTYPE", "HH_TYPE");
	partition = Optional_Field (PARTITION_FIELD_NAMES);

	//Notes_Field (Optional_Field ("NOTES"));

	//---- set default units ----

	Set_Units (start, SECONDS);
	Set_Units (end, SECONDS);
	Set_Units (duration, SECONDS);
	Set_Units (mode, MODE_CODE);
	Set_Units (constraint, CONSTRAINT_CODE);
	Set_Units (priority, PRIORITY_CODE);

	return (true);
}
