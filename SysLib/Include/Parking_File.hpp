//******************************************************** 
//	Parking_File.hpp - Parking File Input/Output
//********************************************************

#ifndef PARKING_FILE_HPP
#define PARKING_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Parking_File Class definition
//---------------------------------------------------------

class SYSLIB_API Parking_File : public Db_Header
{
public:
	Parking_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Parking_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Parking (void)             { return (Get_Integer (parking)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	double Offset (void)              { return (Get_Double (offset)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	int    Space (void)               { return (Get_Integer (space)); }
	Dtime  Time_In (void)             { return (Get_Time (time_in)); }
	Dtime  Time_Out (void)            { return (Get_Time (time_out)); }
	int    Hourly (void)              { return (Get_Integer (hourly)); }
	int    Daily (void)               { return (Get_Integer (daily)); }

	void   Parking (int value)        { Put_Field (parking, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Space (int value)          { Put_Field (space, value); }
	void   Time_In (Dtime value)      { Put_Field (time_in, value); }
	void   Time_Out (Dtime value)     { Put_Field (time_out, value); }
	void   Hourly (int value)         { Put_Field (hourly, value); }
	void   Daily (int value)          { Put_Field (daily, value); }

	virtual bool Create_Fields (void);

protected:

	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int parking, link, dir, offset, type, use, start, end, space, time_in, time_out, hourly, daily;
};

#endif
