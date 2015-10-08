//******************************************************** 
//	Zone_File.hpp - Zone File Input/Output
//********************************************************

#ifndef ZONE_FILE_HPP
#define ZONE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Zone_File Class definition
//---------------------------------------------------------

class SYSLIB_API Zone_File : public Db_Header
{
public:
	Zone_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Zone_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Zone (void)              { return (Get_Integer (zone)); }
	double X (void)                 { return (Get_Double (x)); }
	double Y (void)                 { return (Get_Double (y)); }
	double Z (void)                 { return (Get_Double (z)); }
	int    Area_Type (void)         { return (Get_Integer (area)); }
	double Min_X (void)             { return (Get_Double (min_x)); }
	double Min_Y (void)             { return (Get_Double (min_y)); }
	double Max_X (void)             { return (Get_Double (max_x)); }
	double Max_Y (void)             { return (Get_Double (max_y)); }

	void Zone (int value)           { Put_Field (zone, value); }
	void X (double value)           { Put_Field (x, value); }
	void Y (double value)           { Put_Field (y, value); }
	void Z (double value)           { Put_Field (z, value); }
	void Area_Type (int value)      { Put_Field (area, value); }
	void Min_X (double value)       { Put_Field (min_x, value); }
	void Min_Y (double value)       { Put_Field (min_y, value); }
	void Max_X (double value)       { Put_Field (max_x, value); }
	void Max_Y (double value)       { Put_Field (max_y, value); }

	virtual bool Create_Fields (void);

	int  Zone_Field (void)           { return (zone); }

	void Add_User_Fields (Zone_File *file);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int zone, x, y, z, area, min_x, min_y, max_x, max_y;
};

#endif
