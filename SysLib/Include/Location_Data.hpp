//*********************************************************
//	Location_Data.hpp - network location data
//*********************************************************

#ifndef LOCATION_DATA_HPP
#define LOCATION_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Link_Dir_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Location_Data class definition
//---------------------------------------------------------

class SYSLIB_API Location_Data : public Notes_Data
{
public:
	Location_Data (void)          { Clear (); }

	int  Location (void)          { return (location); }
	int  Link_Dir (void)          { return (link_dir.Link_Dir ()); }
	int  Link (void)              { return (link_dir.Link ()); }
	int  Dir (void)               { return (link_dir.Dir ()); }
	int  Offset (void)            { return (offset); }
	int  Setback (void)           { return (setback); }
	int  Zone (void)              { return (zone); }
	int  X (void)                 { return (x); }
	int  Y (void)                 { return (y); }

	void Location (int value)     { location = value; }
	void Link_Dir (int value)     { link_dir.Link_Dir (value); }
	void Link (int value)         { link_dir.Link (value); }
	void Dir (int value)          { link_dir.Dir (value); }
	void Offset (int value)       { offset = value; }
	void Setback (int value)      { setback = value; }
	void Zone (int value)         { zone = value; }
	void X (int value)            { x = value; }
	void Y (int value)            { y = value; }

	//---- units rounding ----

	void Offset (double value)    { offset = exe->Round (value); }
	void Setback (double value)   { setback = exe->Round (value); }	
	void X (double value)         { x = exe->Round (value); }
	void Y (double value)         { y = exe->Round (value); }

	void Clear (void)
	{
		Link_Dir (0); location = offset = setback = x = y = 0; zone = -1; Notes_Data::Clear (); 
	}
private:
	int           location;
	Link_Dir_Data link_dir;
	int           offset;
	int           setback;
	int           zone;
	int           x;
	int           y;
};

typedef vector <Location_Data>    Location_Array;
typedef Location_Array::iterator  Location_Itr;
#endif
