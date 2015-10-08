//******************************************************** 
//	Shape_File.hpp - Shape Point File Input/Output
//********************************************************

#ifndef SHAPE_FILE_HPP
#define SHAPE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Shape_File Class definition
//---------------------------------------------------------

class SYSLIB_API Shape_File : public Db_Header
{
public:
	Shape_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Shape_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Points (void)              { return (Get_Integer (points)); }
	double X (void)                   { return (Get_Double (x)); }
	double Y (void)                   { return (Get_Double (y)); }
	double Z (void)                   { return (Get_Double (z)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Points (int value)         { Put_Field (points, value); }
	void   X (double value)           { Put_Field (x, value); }
	void   Y (double value)           { Put_Field (y, value); }
	void   Z (double value)           { Put_Field (z, value); }

	virtual bool Create_Fields (void);

	bool Z_Flag (void)                { return (z_flag); }
	void Z_Flag (bool flag)           { z_flag = flag; }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	bool z_flag;

	int link, points, x, y, z;
};

#endif
