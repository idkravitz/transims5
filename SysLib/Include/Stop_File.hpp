//******************************************************** 
//	Stop_File.hpp - Transit Stop File Input/Output
//********************************************************

#ifndef STOP_FILE_HPP
#define STOP_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Stop_File Class definition
//---------------------------------------------------------

class SYSLIB_API Stop_File : public Db_Header
{
public:
	Stop_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Stop_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Stop(void)                 { return (Get_Integer (stop)); }
	string Name (void)                { return (Get_String (name)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	double Offset (void)              { return (Get_Double (offset)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Space (void)               { return (Get_Integer (space)); }

	void   Stop (int value)           { Put_Field (stop, value); }
	void   Name (char * value)        { Put_Field (name, value); }
	void   Name (string &value)       { Put_Field (name, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Space (int value)          { Put_Field (space, value); }

	bool   Use_Flag (void)            { return (use >= 0); }
	bool   Type_Flag (void)           { return (type >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int stop, link, dir, offset, use, type, space, name;
};

#endif
