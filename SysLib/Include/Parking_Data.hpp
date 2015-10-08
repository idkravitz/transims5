//*********************************************************
//	Parking_Data.hpp - network parking data
//*********************************************************

#ifndef PARKING_DATA_HPP
#define PARKING_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Dtime.hpp"
#include "Link_Dir_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Parking_Nest class definition
//---------------------------------------------------------

class SYSLIB_API Parking_Nest
{
public:
	Parking_Nest (void)           { Clear (); }

	int   Use (void)              { return (use); }
	Dtime Start (void)            { return (start); }
	Dtime End (void)              { return (end); }
	int   Space (void)            { return (space); }
	int   Time_In (void)          { return (time_in); }
	int   Time_Out (void)         { return (time_out); }
	int   Hourly (void)           { return (hourly); }
	int   Daily (void)            { return (daily); }

	void  Use (int value)         { use = (unsigned short) value; }
	void  Start (Dtime value)     { start = value; }
	void  End (Dtime value)       { end = value; }
	void  Space (int value)       { space = (unsigned short) value; }
	void  Time_In (int value)     { time_in = (unsigned short) value; }
	void  Time_Out (int value)    { time_out = (unsigned short) value; }
	void  Hourly (int value)      { hourly = (unsigned short) value; }
	void  Daily (int value)       { daily = (unsigned short) value; }
	
	void  Clear (void)
	{
		start = end = 0; use = space = time_in = time_out = hourly = daily = 0;
	}
private:
	Dtime          start;
	Dtime          end;
	unsigned short use;
	unsigned short space;
	unsigned short time_in;
	unsigned short time_out;
	unsigned short hourly;
	unsigned short daily;
};

typedef vector <Parking_Nest>      Park_Nest_Array;
typedef Park_Nest_Array::iterator  Park_Nest_Itr;

//---------------------------------------------------------
//	Parking_Data class definition
//---------------------------------------------------------

class SYSLIB_API Parking_Data : public Notes_Data, public Park_Nest_Array
{
public:
	Parking_Data (void)           { Clear (); }

	int   Parking (void)          { return (parking); }
	int   Link_Dir (void)         { return (link_dir.Link_Dir ()); }
	int   Link (void)             { return (link_dir.Link ()); }
	int   Dir (void)              { return (link_dir.Dir ()); }
	int   Offset (void)           { return (offset); }
	int   Type (void)             { return (type); }

	void  Parking (int value)     { parking = value; }
	void  Link_Dir (int value)    { link_dir.Link_Dir (value); }
	void  Link (int value)        { link_dir.Link (value); }
	void  Dir (int value)         { link_dir.Dir (value); }
	void  Offset (int value)      { offset = value; }
	void  Type (int value)        { type = value; }
	
	void  Offset (double value)   { offset = exe->Round (value); }

	void  Clear (void)
	{
		parking = offset = type = 0; link_dir.Link_Dir (0); clear ();
		Notes_Data::Clear ();
	}
private:
	int           parking;
	Link_Dir_Data link_dir;
	int           offset;
	int           type;
};

typedef vector <Parking_Data>    Parking_Array;
typedef Parking_Array::iterator  Parking_Itr;
#endif
