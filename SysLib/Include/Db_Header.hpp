//********************************************************* 
//	Db_Header.hpp - generic database header interface
//*********************************************************

#ifndef DB_HEADER_HPP
#define DB_HEADER_HPP

#include "APIDefs.hpp"
#include "Db_Base.hpp"

//---------------------------------------------------------
//	Db_Header Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Header : public Db_Base
{
public:
	Db_Header (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Db_Header (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Db_Header (void);

	virtual bool Open (int ext_number);
	virtual bool Open (string path = "");
	virtual bool Close (void);

	void Header_Lines (int number)              { header_lines = number; }
	int  Header_Lines (void)                    { return (header_lines); }

	void Scan_Lines (int number = 100)          { scan_lines = number; }

	String & Header_Record (void)               { return (header_record); }
	void Header_Record (string header)          { header_record = header; }
	void Header_Record (const char *header)     { header_record = header; }

	bool Custom_Header (void)                   { return (custom_header); } 
	void Custom_Header (bool flag)              { custom_header = flag; }

	int  LinkDir_Type (void)                    { return (linkdir_type); }
	void LinkDir_Type (Direction_Type type)     { linkdir_type = type; }

	void Set_Nesting (bool flag);

	int  Num_Nest_Field (void)                  { return (num_nest); }
	void Num_Nest_Field (int field);

	int  Num_Nest (void)                        { return (Get_Integer (num_nest)); }
	void Num_Nest (int value)                   { Put_Field (num_nest, value); }

	int  Notes_Field (void)                     { return (notes); }
	void Notes_Field (int field)                { if (exe->Notes_Name_Flag()) notes = field; }

	String Notes (void)                         { return (Get_String (notes)); }
	void Notes (char *value)                    { Put_Field (notes, value); }
	void Notes (string value)                   { Put_Field (notes, value); }

	void Sort_Type (Trip_Sort_Type type)        { sort_type = type; }
	Trip_Sort_Type Sort_Type (void)             { return (sort_type); }

	bool Time_Sort (void)                       { return (sort_type == TIME_SORT); }
	bool Traveler_Sort (void)                   { return (sort_type == TRAVELER_SORT); }
	bool Sorted (void)                          { return (sort_type != UNKNOWN_SORT); }

	void Set_Units (int fld, Units_Type units);
	Units_Type Get_Units (int fld);

	virtual bool Create_Fields (void)           { return (Set_Field_Numbers ()); }
	virtual bool Write_Header (string user_lines = "");

	bool Code_Flag (void)                       { return (Dbase_Format () == BINARY || Dbase_Format () == SQLITE3); }
	bool Update_Def_Header (void)               { return (Write_Def_Header (Header_Record ())); }

	static Format_Type Def_Format (string filename);

	int  LinkDir_Type_Field (void);	

	void   Flatten_File (void);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void)       { return (true); }
	virtual bool Default_Definition (void)      { return (false); }

	void Add_LinkDir_Field (Nest_Type nest = NO_NEST);

	bool Write_Def_Header (string user_lines);

private:

	//---- header structure -----

	typedef struct {
		char  version;
		char  date [3];
		int   num_records;
		short header_size;
		short record_size;
		char  reserve1 [2];
		char  transaction_flag;
		char  encryption_flag;
		char  reserve2 [12];
		char  mdx_flag;
		char  reserve3 [3];
	} dBase_Header;

	//---- field structure ----

	typedef  struct {
		char  name [11];	
		unsigned char  type;
		unsigned char  reserve1 [4];
		unsigned char  width;
		unsigned char  decimal;
		unsigned short offset;
		unsigned char  work_id;
		unsigned char  reserve3 [11];
	} dBase_Field;

	bool Read_CSV_Header (bool stat);
	bool Write_CSV_Header (string user_lines);

	bool Read_dBase_Header (bool stat);
	bool Write_dBase_Header (void);
	
	bool Read_SQLite3_Header (bool stat);
	bool Write_SQLite3_Header (void);

	bool Read_Def_Header (void);

	bool custom_header;
	int  header_lines, scan_lines, notes, num_nest, linkdir_type;
	Trip_Sort_Type sort_type;
	String header_record;
};
#endif
