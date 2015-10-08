//******************************************************** 
//	Location_File.hpp - Activity Location File Input/Output
//********************************************************

#ifndef LOCATION_FILE_HPP
#define LOCATION_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Location_File Class definition
//---------------------------------------------------------

class SYSLIB_API Location_File : public Db_Header
{
public:
	Location_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Location_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Location (void)            { return (Get_Integer (location)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	double Offset (void)              { return (Get_Double (offset)); }
	double Setback (void)             { return (Get_Double (setback)); }
	int    Zone (void)                { return (Get_Integer (zone)); }

	void   Location (int value)       { Put_Field (location, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Setback (double value)     { Put_Field (setback, value); }
	void   Zone (int value)           { Put_Field (zone, value); }

	int    Zone_Field (void)          { return (zone); }
	bool   Setback_Flag (void)        { return (setback >= 0); }

	void   Add_User_Fields (Location_File *file);

	virtual bool Create_Fields (void);
	
protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int location, link, dir, offset, setback, zone;
};

#endif
