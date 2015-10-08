//********************************************************* 
//	Db_Field.cpp - database field
//*********************************************************

#include "Db_Field.hpp"

//-----------------------------------------------------------
//	Db_Field constructor
//-----------------------------------------------------------

Db_Field::Db_Field (void)
{
	offset = size = 0;
	type = DB_INTEGER;
	units = NO_UNITS;
	nest = NO_NEST;
}

//-----------------------------------------------------------
//	Set_Field
//-----------------------------------------------------------

bool Db_Field::Set_Field (const char *name, Field_Type type, int offset, double size, Units_Type units, Nest_Type nest) 
{
	if (*name == '\0') return (false);
	Name (name);
	Type (type);
	Offset (offset);
	Size (size);
	Units (units);
	Nest (nest);
	return (Size () > 0);
}

bool Db_Field::Set_Field (string name, Field_Type type, int offset, double size, Units_Type units, Nest_Type nest) 
{
	if (name.empty ()) return (false);
	Name (name);
	Type (type);
	Offset (offset);
	Size (size);
	Units (units);
	Nest (nest);
	return (Size () > 0);
}
