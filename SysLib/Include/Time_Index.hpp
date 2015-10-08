//*********************************************************
//	Time_Index.hpp - time sort index class
//*********************************************************

#ifndef TIME_INDEX_HPP
#define TIME_INDEX_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Time_Index class definition
//---------------------------------------------------------

class SYSLIB_API Time_Index
{
public:
	Time_Index (void)               { Clear (); }
	Time_Index (Dtime start, int hhold = 0, int person = 0) 
	{
		Set (start, hhold, person);
	}
	Dtime Start (void)              { return (start); }
	int   Household (void)          { return (hhold); }
	int   Person (void)             { return (person); }

	void  Start (Dtime value)       { start = value; }
	void  Household (int value)     { hhold = value; }
	void  Person (int value)        { person = (short) value; }

	void  Set (Dtime start, int hhold = 0, int person = 0) 
	{
		Start (start); Household (hhold); Person (person);
	}
	void  Clear (void)              { start = 0; hhold = 0; person = 0; }

private:
	Dtime start;
	int   hhold;
	short person;
};

//---- time map ----

SYSLIB_API bool operator < (Time_Index left, Time_Index right);
SYSLIB_API bool operator <= (Time_Index left, Time_Index right);
SYSLIB_API bool operator == (Time_Index left, Time_Index right);
SYSLIB_API bool operator != (Time_Index left, Time_Index right);

typedef map <Time_Index, int>      Time_Map;
typedef pair <Time_Index, int>     Time_Map_Data;
typedef Time_Map::iterator         Time_Map_Itr;
typedef pair <Time_Map_Itr, bool>  Time_Map_Stat;

#endif
