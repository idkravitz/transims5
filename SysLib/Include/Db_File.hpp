//********************************************************* 
//	Db_File.hpp - database file
//*********************************************************

#ifndef DB_FILE_HPP
#define DB_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Record.hpp"
#include "String.hpp"

#include "../../sqlite3/sqlite3.h"

#include <iostream>
#include <fstream>
using namespace std;

#ifdef BOOST_THREADS
#include <boost/thread/mutex.hpp>
#endif

//---------------------------------------------------------
//	Db_File Class definition
//---------------------------------------------------------

class SYSLIB_API Db_File : public Db_Record
{
public:
	Db_File (Access_Type access = READ, Format_Type format = UNFORMATED);
	Db_File (string filename, Access_Type access = READ, Format_Type format = UNFORMATED);
	virtual ~Db_File (void);

	void    Initialize (Access_Type access = READ, Format_Type format = UNFORMATED);

	bool    Filename (string rootname);
	bool    Filename (int part_index);
	String& Filename (void)                  { return (filename); }

	virtual bool Find_File (int part_index);
	virtual int  Num_Parts (void);

	virtual bool Open (string path = "");
	virtual bool Open (int part_index);

	bool    Create (string path = "")        { File_Access (CREATE); return (Open (path)); }
	bool    First_Open (void)                { return (first_open); }
	void    First_Open (bool flag)           { first_open = flag; }
	
	String& Pathname (void)                  { return (pathname); };
	void    Pathname (string name)           { pathname = name; }

	int     Part_Number (void)               { return (part_num); }
	bool    Part_Flag (void)                 { return (part_flag); };
	void    Part_Flag (bool flag)            { part_flag = flag;};

	int     Extend (void)                    { return (extend); }
	void    Extend (int num)                 { extend = num; }
	bool    Extend_Flag (void)               { return (extend == 3); }

	fstream &File (void)                     { return (file); }
	bool    Check_File (void)                { return (Is_Open () ? true : Status (NOT_OPEN)); }
	
	virtual bool Is_Open (void)              { return ((File_Format () == SQLITE3) ? (db_file != 0) : file.is_open ()); }

	int     Version (void)                   { return (version); }
	void    Version (int code)               { version = code; }

	virtual bool  Read_Record (int number = 0);

	bool  Read (void *record, int size);
	bool  Read (void);
	bool  Read (bool nested)                 { Nested (nested); return (Read ()); }
	bool  Read (void *record, int size, off_t offset);

	virtual bool  Write_Record (int number = 0);

	bool  Write (void *record, int size);
	bool  Write (void);
	bool  Write (bool nested);
	bool  Write (char *record)               { return (Write (record, (int) strlen (record))); }
	bool  Write (void *record, int size, off_t offset);

	virtual bool  Close (void);
	virtual bool  Rewind (void);
	virtual off_t File_Size (void);

	off_t Offset (void);
	bool  Offset (off_t offset);

	void  First_Offset (off_t offset)       { first_offset = offset; }
	off_t First_Offset (void)               { return (first_offset); }

	void  File_Access (Access_Type access)  { file_access = access; }
	Access_Type  File_Access (void)         { return (file_access); }
	
	Format_Type  File_Format (void)         { return (file_format); }
	void  File_Format (Format_Type format);
	void File_Format (string text);

	int  Num_Files (void)                   { return (num_files); }
	void Num_Files (int files)              { num_files = files; }
	void Add_Files (int num)                { num_files += num; }
	void Max_File (int max = 1)             { if (max > num_files) num_files = max; }

	virtual int  Num_Records (void);
	void Num_Records (int num)              { num_records = num; }
	void Add_Records (int num)              { num_records += num; }

	virtual int Estimate_Records (void);

	int  Record_Number (void)               { return (record_num); }
	virtual bool Record_Number (int number);

	int  Max_Record_Number (void)           { return (max_record_num); }

	void Flush (void)                       { file.flush (); }

	void Clean_Flag (bool flag)             { clean_flag = flag; }

#ifdef BOOST_THREADS
	void Lock (void)                        { file_lock.lock (); }
	void UnLock (void)                      { file_lock.unlock (); }
private:
	boost::mutex  file_lock;
#else
	void Lock (void)                        { }
	void UnLock (void)                      { }
#endif

protected:
	String  Extension (int part_number);

	virtual bool Db_Open (string filename);
	virtual bool Input_Record (void)        { return (File_Format () != SQLITE3); }
	virtual bool Output_Record (void)       { return (File_Format () != SQLITE3); }
	
	bool  Binary_Read (void *buffer, int num_records = 1, bool nested = false);
	bool  Binary_Write (void *buffer, int num_records = 1, bool nested = false);

	bool first_open, new_nest_flag, clean_flag;
	int parent_id;
	sqlite3 *db_file;
	sqlite3_stmt *insert_stmt, *insert_nest;
	sqlite3_stmt *read_stmt, *read_nest;

private:
	fstream file;

	Access_Type file_access;
	Format_Type file_format;

	bool  part_flag;
	int   num_files, num_records, record_num, max_record_num, part_num, version, extend;
	off_t first_offset;

	String filename, pathname;
};
#endif
