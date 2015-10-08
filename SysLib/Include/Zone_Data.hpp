//*********************************************************
//	Zone_Data.hpp - network zone data
//*********************************************************

#ifndef ZONE_DATA_HPP
#define ZONE_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Zone_Data class definition
//---------------------------------------------------------

class SYSLIB_API Zone_Data : public Notes_Data
{
public:
	Zone_Data (void)            { Clear (); }

	int  Zone (void)            { return (zone); }
	int  X (void)               { return (x); }
	int  Y (void)               { return (y); }
	int  Z (void)               { return (z); }
	int  Area_Type (void)       { return (area_type); }

	void Zone (int value)       { zone = value; }
	void X (int value)          { x = value; }
	void Y (int value)          { y = value; }
	void Z (int value)          { z = value; }
	void Area_Type (int value)  { area_type = value; }
	
	void X (double value)       { x = exe->Round (value); }
	void Y (double value)       { y = exe->Round (value); }
	void Z (double value)       { z = exe->Round (value); }

	void Clear (void)
	{
		zone = x = y = z = area_type = 0; Notes_Data::Clear ();
	}

private:
	int  zone;
	int  x;
	int  y;
	int  z;
	int  area_type;
};

typedef vector <Zone_Data>    Zone_Array;
typedef Zone_Array::iterator  Zone_Itr;

#endif
