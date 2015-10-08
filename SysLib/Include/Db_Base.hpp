//********************************************************* 
//	Db_Base.hpp - database file
//*********************************************************

#ifndef DB_BASE_HPP
#define DB_BASE_HPP

#include "APIDefs.hpp"
#include "Db_File.hpp"
#include "Db_Field.hpp"
#include "Dtime.hpp"
#include "String.hpp"

#include <vector>
using namespace std;

#include <stdio.h>
#include <time.h>
	
typedef vector <Db_Field>      Field_Array;
typedef Field_Array::iterator  Field_Itr;
typedef Field_Array::pointer   Field_Ptr;

//---------------------------------------------------------
//	Db_Base Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Base : public Db_File
{
public:
	Db_Base (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Db_Base (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Db_Base (void);

	void Initialize (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	Format_Type Dbase_Format (void)                  { return (format_type); }
	void  Dbase_Format (Format_Type type);
	void  Dbase_Format (string text)                 { Dbase_Format (Format_Code (text)); }

	int   Add_Field (const char *name, Field_Type type, double size = 0, Units_Type units = NO_UNITS, bool binary = false, Nest_Type nest = NO_NEST, int offset = END_OF_RECORD);
	int   Add_Field (string name, Field_Type type, double size = 0, Units_Type units = NO_UNITS, bool binary = false, Nest_Type nest = NO_NEST, int offset = END_OF_RECORD)
	                                                 { return (Add_Field (name.c_str (), type, size, units, binary, nest, offset)); }
	
	int   Num_Fields (void)                          { return ((int) field.size ()); }

	int   Field_Number (const char *name);
	int   Field_Number (string name)                 { return (Field_Number (name.c_str ())); }

	Field_Ptr Field (int fld);
	Field_Ptr Field (const char *name);
	Field_Ptr Field (string name)                    { return (Field (name.c_str ())); }

	bool   Get_Field (int fld, int &data)            { return (Read_Field (Field (fld), (void *) &data, DB_INTEGER)); }
	bool   Get_Field (int fld, double &data)         { return (Read_Field (Field (fld), (void *) &data, DB_DOUBLE)); }
	bool   Get_Field (int fld, string &data)         { return (Read_Field (Field (fld), (void *) &data, DB_STRING)); }
	bool   Get_Field (int fld, Dtime &data)          { return (Read_Field (Field (fld), (void *) &data, DB_TIME)); }
	
	bool   Get_Field (const char *name, int &data)   { return (Read_Field (Field (name), (void *) &data, DB_INTEGER)); }
	bool   Get_Field (const char *name, double &data){ return (Read_Field (Field (name), (void *) &data, DB_DOUBLE)); }
	bool   Get_Field (const char *name, string &data){ return (Read_Field (Field (name), (void *) &data, DB_STRING)); }
	bool   Get_Field (const char *name, Dtime &data) { return (Read_Field (Field (name), (void *) &data, DB_TIME)); }

	int    Get_Integer (int fld)                     { int data; Get_Field (fld, data); return (data); }
	double Get_Double (int fld)                      { double data; Get_Field (fld, data); return (data); }
	String Get_String (int fld)                      { String data; Get_Field (fld, data); return (data); }
	Dtime  Get_Time (int fld)                        { Dtime data; Get_Field (fld, data); return (data); }
	bool   Get_Bool (int fld)                        { String data; Get_Field (fld, data); return (data.Bool ()); }
	
	int    Get_Integer (const char *name)            { return (Get_Integer (Field_Number (name))); }
	double Get_Double (const char *name)             { return (Get_Double (Field_Number (name))); }
	String Get_String (const char *name)             { return (Get_String (Field_Number (name))); }
	Dtime  Get_Time (const char *name)               { return (Get_Time (Field_Number (name))); }
	bool   Get_Bool (const char *name)               { return (Get_Bool (Field_Number (name))); }
	
	bool   Put_Field (int fld, int data)             { return (Write_Field (Field (fld), (void *) &data, DB_INTEGER)); }
	bool   Put_Field (int fld, double data)          { return (Write_Field (Field (fld), (void *) &data, DB_DOUBLE)); }
	bool   Put_Field (int fld, char *data)           { string s ((data) ? data : ""); return (Put_Field (fld, s)); } 
	bool   Put_Field (int fld, string data)          { return (Write_Field (Field (fld), (void *) &data, DB_STRING)); }
	bool   Put_Field (int fld, Dtime data)           { return (Write_Field (Field (fld), (void *) &data, DB_TIME)); }
	
	bool   Put_Field (const char *name, int data)    { return (Write_Field (Field (name), (void *) &data, DB_INTEGER)); }
	bool   Put_Field (const char *name, double data) { return (Write_Field (Field (name), (void *) &data, DB_DOUBLE)); }
	bool   Put_Field (const char *name, char *data)  { string s ((data) ? data : ""); return (Put_Field (name, s)); }
	bool   Put_Field (const char *name, string data) { return (Write_Field (Field (name), (void *) &data, DB_STRING)); }
	bool   Put_Field (const char *name, Dtime data)  { return (Write_Field (Field (name), (void *) &data, DB_TIME)); }

	void   Blank_Field (int fld)                     { Blank_Field (Field (fld)); }
	void   Blank_Field (const char *name)            { Blank_Field (Field (name)); }
	void   Blank_Field (string name)                 { Blank_Field (Field (name)); }
	void   Blank_Nested_Fields (void);

	int Required_Field (string name);
	int Required_Field (const char *name1, const char *name2 = 0, const char *name3 = 0, const char *name4 = 0, const char *name5 = 0);
	int Optional_Field (const char *name1, const char *name2 = 0, const char *name3 = 0, const char *name4 = 0, const char *name5 = 0);

	bool  Copy_Fields (Db_Base &input, bool nest_flag = false);
	void  Clear_Fields (void)                        { field.clear (); Reset_Buffers (); }

	virtual bool Replicate_Fields (Db_Base *file, bool notes_flag = true, bool nest_flag = true, bool drop_flag = false);
	virtual bool Write_Fields (void)                 { return (false); }

	char * Reset_Record (void);
	
	Units_Type  Time_Format (void)                   { return (time_format); }
	void Time_Format (Units_Type format)             { time_format = format; }

	Units_Type  Get_Units (int fld)                  { Field_Ptr ptr = Field (fld); return ((ptr) ? ptr->Units () : NO_UNITS); }

protected:

	virtual bool Input_Record (void);
	virtual bool Output_Record (void);

	void Binary_Text_Size (Field_Type type, double *size);
	void Text_Binary_Size (Field_Type type, double *size);

	Field_Array field;

private:
	Units_Type time_format;
	Format_Type format_type;

	void  Blank_Field (Db_Field *fld);
	bool  Read_Field (Db_Field *fld, void *data, Field_Type type);
	bool  Write_Field (Db_Field *fld, void *data, Field_Type type);
};

typedef vector <Db_Base *>       Db_Base_Array;
typedef Db_Base_Array::iterator  Db_Base_Itr;
typedef Db_Base_Array::pointer   Db_Base_Ptr;

#endif
