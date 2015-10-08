//********************************************************* 
//	Snapshot_File.cpp - Snapshot File Input/Output
//*********************************************************

#include "Snapshot_File.hpp"

//-----------------------------------------------------------
//	Snapshot_File constructors
//-----------------------------------------------------------

Snapshot_File::Snapshot_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Snapshot_File::Snapshot_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Snapshot_File::Setup (void)
{
	File_Type ("Snapshot File");
	File_ID ("Snapshot");

	hhold = vehicle = time = link = dir = lane = offset = speed = pass = type = -1;
	wait = diff = user = cell = x = y = z = bearing = blob = -1;
	location_flag = cell_flag = status_flag = compress = false;
	rec = 0;
	index = 0;
	units = METERS;
}

//-----------------------------------------------------------
//	Open
//-----------------------------------------------------------

bool Snapshot_File::Open (string filename)
{
	if (filename.empty ()) {
		filename = Filename ();
	}
	if (filename.empty ()) return (false);

	File_Type (File_Type ());
	File_Access (File_Access ());
	
	//---- open the snapshot file ----

	bool status = Db_Header::Open (filename);

	//---- set up and open the index file ----

	if (compress) {
		bool messages;

		messages = exe->Send_Messages ();
		exe->Send_Messages (false);

		index_file.File_Type (File_Type () + " Index");
		index_file.File_Access (File_Access ());
		index_file.File_Format (BINARY);
	
		filename += ".idx";

		status = index_file.Open (filename);

		index_file.Record_Size (sizeof (Index_Record));

		index = (Index_Record *) index_file.Record_String ();

		exe->Send_Messages (messages);
	}
	if (!status) return (Status (NOT_OPEN));
	return (status);
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Snapshot_File::Close (void)
{
	//---- close the index file ----

	if (compress && index_file.Is_Open ()) index_file.Close();

	//---- close the snapshot file -----

	return (Db_Header::Close ());
}

//-----------------------------------------------------------
//	Read_Index
//-----------------------------------------------------------

bool Snapshot_File::Read_Index (Dtime &step, unsigned &record)
{
	if (!index_file.Check_File ()) return (false);

	if (!index_file.Read ()) return (false);

	step = index->time;
	record = index->record;
	return (true);
}

//-----------------------------------------------------------
//	Read_Index
//-----------------------------------------------------------

bool Snapshot_File::Read_Index (Dtime &step, off_t &offset)
{
	if (!index_file.Check_File ()) return (false);

	if (!index_file.Read ()) return (false);

	step = index->time;
	offset = (off_t) index->record * sizeof (Compressed_Snapshot);
	return (true);
}

//-----------------------------------------------------------
//	Write_Index
//-----------------------------------------------------------

bool Snapshot_File::Write_Index (Dtime step)
{
	if (!index_file.Check_File ()) return (false);

	index->time = step;
	index->record = Db_File::Offset () / sizeof (Compressed_Snapshot);

	return (index_file.Write ());
}

//-----------------------------------------------------------
//	Write_Index
//-----------------------------------------------------------

bool Snapshot_File::Write_Index (Dtime step, unsigned record)
{
	if (!index_file.Check_File ()) return (false);

	index->time = step;
	index->record = record;

	return (index_file.Write ());
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Snapshot_File::Create_Fields (void) 
{
	if (compress) {
		Add_Field ("COMPRESS", DB_STRING, (int) sizeof (Compressed_Snapshot), METERS, true);
	} else {
		Add_Field ("HHOLD", DB_INTEGER, 10);
		Add_Field ("VEHICLE", DB_INTEGER, 4);
		if (Cell_Flag ()) {
			Add_Field ("CELL", DB_INTEGER, 1, NO_UNITS, true);
		}
		Add_Field ("TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		Add_Field ("LINK", DB_INTEGER, 10);
		Add_LinkDir_Field ();
		if (Code_Flag ()) {
			Add_Field ("LANE", DB_UNSIGNED, 1, LANE_ID_CODE, true);
		} else {
			Add_Field ("LANE", DB_STRING, 4, LANE_ID_CODE);
		}
		Add_Field ("OFFSET", DB_DOUBLE, 8.1, METERS);
		Add_Field ("SPEED", DB_DOUBLE, 8.1, KPH);
		Add_Field ("PASSENGERS", DB_INTEGER, 4);
		Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
		if (Status_Flag ()) {
			Add_Field ("WAIT", DB_INTEGER, 6, SECONDS);
			Add_Field ("TIME_DIFF", DB_INTEGER, 10, SECONDS);
			Add_Field ("USER", DB_INTEGER, 3);
		}
		if (Location_Flag ()) {
			Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS);
			Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS);
			Add_Field ("Z_COORD", DB_DOUBLE, 14.1, METERS);
			Add_Field ("BEARING", DB_DOUBLE, 6.1, DEGREES);
		}
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Snapshot_File::Set_Field_Numbers (void)
{
	//---- compress check ----

	blob = Optional_Field ("COMPRESS");

	if (blob >= 0) {
		compress = true;
		rec = (Compressed_Snapshot *) Record_String ();
		Set_Units (blob, METERS);
		units = Field (blob)->Units ();
		Status_Flag (true);
		return (true);
	}
	compress = false;

	//---- required fields ----
	
	if (Version () <= 40) {
		hhold = Required_Field ("DRIVER", "VEHICLE");
	} else {
		hhold = Required_Field (HHOLD_FIELD_NAMES);
	}
	vehicle = Required_Field (VEHICLE_FIELD_NAMES);
	link = Required_Field ("LINK");
	time = Required_Field ("TIME");
	link = Required_Field ("LINK");
	lane = Required_Field ("LANE");
	offset = Required_Field ("OFFSET", "DISTANCE");
	speed = Required_Field ("SPEED", "VELOCITY");

	if (hhold < 0 || vehicle < 0 || time < 0 || link < 0 || lane < 0 || offset < 0 || speed < 0) return (false);

	//---- optional fields ----

	dir = LinkDir_Type_Field ();

	cell = Optional_Field ("CELL", "PART");
	pass = Optional_Field ("PASSENGERS", "OCCUPANCY");
	type = Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	wait = Optional_Field ("WAIT", "STOPTIME", "DELAY");
	diff = Optional_Field ("TIME_DIFF", "VARIANCE", "DIFFERENCE");
	user = Optional_Field ("USER", "DATA");
	x = Optional_Field (X_FIELD_NAMES);
	y = Optional_Field (Y_FIELD_NAMES);
	z = Optional_Field (Z_FIELD_NAMES);
	bearing = Optional_Field ("BEARING", "DIRECTION", "DEGREES");

	Cell_Flag (cell >= 0);
	Location_Flag (x >= 0 && y >= 0 && bearing >= 0);
	Status_Flag (wait >= 0 || diff >= 0 || user >= 0);

	//---- set default units ----

	Set_Units (time, DAY_TIME);
	Set_Units (lane, LANE_ID_CODE);
	Set_Units (offset, METERS);
	Set_Units (speed, MPS);
	Set_Units (type, VEH_TYPE);
	Set_Units (wait, SECONDS);
	Set_Units (diff, SECONDS);
	Set_Units (x, METERS);
	Set_Units (y, METERS);
	Set_Units (z, METERS);
	Set_Units (bearing, DEGREES);

	return (true);
}
