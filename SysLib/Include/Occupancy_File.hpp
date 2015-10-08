//******************************************************** 
//	Occupancy_File.hpp - Occupancy File Input/Output
//********************************************************

#ifndef OCCUPANCY_FILE_HPP
#define OCCUPANCY_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Occupancy_File Class definition
//---------------------------------------------------------

class SYSLIB_API Occupancy_File : public Db_Header
{
public:
	Occupancy_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Occupancy_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	int    Lane (void)                { return (Get_Integer (lane)); }
	double Offset (void)              { return (Get_Double (offset)); }
	int    Occupancy (void)           { return (Get_Integer (occupancy)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Lane (int value)           { Put_Field (lane, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Occupancy (int value)      { Put_Field (occupancy, value); }

	virtual bool Create_Fields (void);
	
	int  Link_Field (void)			{ return (link); }
	int  Dir_Field (void)			{ return (dir); }
	int  Start_Field (void)			{ return (start); }
	int  End_Field (void)			{ return (end); }
	int  Lane_Field (void)			{ return (lane); }
	int  Offset_Field (void)		{ return (offset); }
	int  Occupancy_Field (void)		{ return (occupancy); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, start, end, lane, offset, occupancy;
};

#endif
