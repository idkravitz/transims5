//******************************************************** 
//	Subzone_File.hpp - Subzone File Input/Output
//********************************************************

#ifndef SUBZONE_FILE_HPP
#define SUBZONE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Subzone_File Class definition
//---------------------------------------------------------

class SYSLIB_API Subzone_File : public Db_Header
{
public:
	Subzone_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Subzone_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    ID (void)              { return (Get_Integer (id)); }
	int    Zone (void)            { return (Get_Integer (zone)); }
	double X (void)               { return (Get_Double (x)); }
	double Y (void)               { return (Get_Double (y)); }
	double Data (void)            { return (Get_Double (data)); }

	void   ID (int value)         { Put_Field (id, value); }
	void   Zone (int value)       { Put_Field (zone, value); }
	void   X (double value)       { Put_Field (x, value); }
	void   Y (double value)       { Put_Field (y, value); }
	void   Data (double value)    { Put_Field (data, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int id, zone, x, y, data;
};

#endif
