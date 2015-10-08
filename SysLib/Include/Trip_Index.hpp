//*********************************************************
//	Trip_Index.hpp - trip sort index class
//*********************************************************

#ifndef TRIP_INDEX_HPP
#define TRIP_INDEX_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Trip_Index class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Index
{
public:
	Trip_Index (void)              { Clear (); }
	Trip_Index (int hhold, int person = 0, int tour = 0, int trip = 0)
	{
		Set (hhold, person, tour, trip);
	}
	int  Household (void)          { return (hhold); }
	int  Person (void)             { return (person); }
	int  Tour (void)               { return (tour); }
	int  Trip (void)               { return (trip); }

	void Household (int value)     { hhold = value; }
	void Person (int value)        { person = (short) value; }
	void Tour (int value)          { tour = (char) value; }
	void Trip (int value)          { trip = (char) value; }

	void Set (int hhold, int person = 0, int tour = 0, int trip = 0)
	{
		Household (hhold); Person (person); Tour (tour); Trip (trip);
	}
	void Clear (void)              { hhold = 0; person = 0; tour = trip = 0; }

private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
};

//---- trip map ----

SYSLIB_API bool operator < (Trip_Index left, Trip_Index right);
SYSLIB_API bool operator <= (Trip_Index left, Trip_Index right);
SYSLIB_API bool operator == (Trip_Index left, Trip_Index right);
SYSLIB_API bool operator != (Trip_Index left, Trip_Index right);

typedef map <Trip_Index, int>      Trip_Map;
typedef pair <Trip_Index, int>     Trip_Map_Data;
typedef Trip_Map::iterator         Trip_Map_Itr;
typedef pair <Trip_Map_Itr, bool>  Trip_Map_Stat;

#endif

