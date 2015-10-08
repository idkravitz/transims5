//*********************************************************
//	Db_Status.cpp - database status code
//*********************************************************

#include "Db_Status.hpp"

#include "String.hpp"

String Db_Status::default_type = "Db_Record";
String Db_Status::default_id = "Db_File";

//---------------------------------------------------------
//	Db_Status constructors
//---------------------------------------------------------

Db_Status::Db_Status (void) : Static_Service ()
{
	Status (OK);
}

//---------------------------------------------------------
//	Status
//---------------------------------------------------------

bool Db_Status::Status (Status_Code stat, bool message)
{
	status = stat;

	if (status == OK) return (true);

	if (message && exe->Send_Messages ()) {
		exe->Error (String ("%s %s") % file_type % Status_Message ());
	}
	return (false);
}
