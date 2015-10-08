//*********************************************************
//	Db_Record.hpp - database record
//*********************************************************

#ifndef DB_RECORD_HPP
#define DB_RECORD_HPP

#include "APIDefs.hpp"
#include "Db_Status.hpp"
#include "Buffer.hpp"

//---------------------------------------------------------
//	Db_Record Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Record : public Db_Status
{
public:
	Db_Record (Format_Type format = UNFORMATED);

	bool  Record_Size (int size)            { return (Record ().Size (size)); }
	int   Record_Size (void)                { return (Record ().Size ()); }

	int   Nest_Size (bool flag = true)      { return ((flag) ? nest_rec.Max_Size () : record.Max_Size ()); }
	int   Max_Size (void)                   { return (Record ().Max_Size ()); }
	int   Max_Read (void)                   { return ((String_Flag ()) ? Record ().Max_Read () : Max_Size ()); }
	
	int   Set_Size (void)                   { return (Record ().Set_Size (String_Flag ())); }

	void  Record_Format (Format_Type format);
	Format_Type  Record_Format (void)       { return (record_format); }

	void  Delimiters (const char *limits)   { if (limits != 0) delimiters = limits; }
	char *Delimiters (void)                 { return ((char *) delimiters); }

	bool  Record (void *data, int size = 0);
	bool  Record (Buffer &buf)              { return (Record ().Data (buf)); }

	Buffer & Record (void)                  { return ((nested) ? nest_rec : record); }
	Buffer & Reset_Record (void);

	void   Reset_Buffers (void);

	char * Record_String (void)             { return (Record ().Data ()); }

	string Get_Field_Number (int number);
	bool  Set_Field_Number (int number, string field);

	Nest_Type Nest (void)                   { return (nest); }
	void Nest (Nest_Type type)              { nest = type; nested = false;}
	bool Nest_Flag (void)                   { return (nest == NESTED); }

	bool Nested (void)                      { return (nested); }
	void Nested (bool flag)                 { nested = (nest == NESTED) ? flag : false; }
	void Nested (Nest_Type type)            { nested = (nest == NESTED) ? (type == NESTED) : false; }

protected:
	bool Max_Size (int size)                { return (Record ().Max_Size (size)); }

private:
	Format_Type  record_format;

	Nest_Type nest;
	bool nested;

	Buffer record, nest_rec;

	const char *delimiters;

	bool String_Flag (void)                 { return (record_format != BINARY && record_format != FIXED_COLUMN); }
};
#endif
