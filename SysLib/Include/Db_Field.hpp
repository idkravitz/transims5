//*********************************************************
//	Db_Field.hpp - database field
//*********************************************************

#ifndef DB_FIELD_HPP
#define DB_FIELD_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Db_Field Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Field
{
public:
	Db_Field (void);
	Db_Field (const char *name, Field_Type type, int offset = 0, double size = 0, Units_Type units = NO_UNITS, Nest_Type nest = NO_NEST) 
	{
		Name (name); Type (type); Offset (offset); Size (size); Units (units); Nest (nest);
	}
	Db_Field (string name, Field_Type type, int offset = 0, double size = 0, Units_Type units = NO_UNITS, Nest_Type nest = NO_NEST) 
	{
		Name (name); Type (type); Offset (offset); Size (size); Units (units); Nest (nest);
	}
	bool Set_Field (const char *name, Field_Type type, int offset = 0, double size = 0, Units_Type units = NO_UNITS, Nest_Type nest = NO_NEST);
	bool Set_Field (string name, Field_Type type, int offset = 0, double size = 0, Units_Type units = NO_UNITS, Nest_Type nest = NO_NEST);

	String &   Name (void)                 { return (name); }
	Field_Type Type (void)                 { return (type); }
	int        Offset (void)               { return (offset); }
	double     Size (void)                 { return (size / 10.0); }
	int        Width (void)                { return (size / 10); }
	int        Decimal (void)              { return (size % 10); }
	Units_Type Units (void)                { return (units); }
	Nest_Type  Nest (void)                 { return (nest); }
	bool       Nested (void)               { return (nest == NESTED); }
	String &   Buffer (void)               { return (buffer); }

	void  Name (const char *_name)         { name = _name; }
	void  Name (string _name)              { name = _name; }
	void  Type (Field_Type value)          { type = value; }
	void  Offset (int value)               { offset = (value > 0) ? value : 0; }
	void  Size (double value)              { size = (value > 0) ? (int) (value * 10.0) : 0; }
	void  Units (Units_Type value)         { units = value; }
	void  Nest (Nest_Type value)           { nest = value; }
	void  Buffer (const char *_buffer)     { buffer = _buffer; }
	void  Buffer (string _buffer)          { buffer = _buffer; }

	void  Clear (void)                     { name.clear (); buffer.clear (); nest = NO_NEST; offset = size = 0; units = NO_UNITS; }

private:
	String      name;
	Field_Type  type;
	int         offset;
	int         size;
	Units_Type  units;
	Nest_Type   nest;
	String      buffer;
};
#endif
