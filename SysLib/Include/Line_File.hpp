//******************************************************** 
//	Line_File.hpp - Transit Route File Input/Output
//********************************************************

#ifndef LINE_FILE_HPP
#define LINE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Line_File Class definition
//---------------------------------------------------------

class SYSLIB_API Line_File : public Db_Header
{
public:
	Line_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Line_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Route (void)               { return (Get_Integer (route)); }
	int    Stops (void)               { return (Get_Integer (stops)); }
	int    Mode (void)                { return (Get_Integer (mode)); }
	int    Type (void)                { return (Get_Integer (type)); }
	string Name (void)                { return (Get_String (name)); }
	int    Stop (void)                { return (Get_Integer (stop)); }
	int    Zone (void)                { return (Get_Integer (zone)); }
	int    Time_Flag (void)           { return (Get_Integer (flag)); }

	void   Route (int value)          { Put_Field (route, value); }
	void   Stops (int value)          { Put_Field (stops, value); }
	void   Mode (int value)           { Put_Field (mode, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Name (char * value)        { Put_Field (name, value); }
	void   Name (string &value)       { Put_Field (name, value); }
	void   Stop (int value)           { Put_Field (stop, value); }
	void   Zone (int value)           { Put_Field (zone, value); }
	void   Time_Flag (int value)      { Put_Field (flag, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int route, stops, mode, stop, zone, flag, name, type;
};

#endif
