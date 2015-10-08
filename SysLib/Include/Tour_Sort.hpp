//*********************************************************
//	Tour_Sort.hpp - household tour sort class
//*********************************************************

#ifndef TOUR_SORT_HPP
#define TOUR_SORT_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Tour_Sort class definition
//---------------------------------------------------------

class SYSLIB_API Tour_Sort
{
public:
	Tour_Sort (void)              { Clear (); }
	Tour_Sort (int person, Dtime start, Dtime end, int tour = 0, int trip = 0)
	{
		Set (person, start, end, tour, trip);
	}
	int   Person (void)             { return (person); }
	Dtime Start (void)              { return (start); }
	Dtime End (void)                { return (end); }
	int   Tour (void)               { return (tour); }
	int   Trip (void)               { return (trip); }

	void  Person (int value)        { person = value; }
	void  Start (Dtime value)       { start = value; }
	void  End (Dtime value)         { end = value; }
	void  Tour (int value)          { tour = (short) value; }
	void  Trip (int value)          { trip = (short) value; }

	void Set (int person, Dtime start, Dtime end, int tour = 0, int trip = 0)
	{
		Person (person); Start (start); End (end); Tour (tour); Trip (trip);
	}
	void Clear (void)               { person = 0; start = end = 0; tour = trip = 0; }

private:
	int   person;
	Dtime start;
	Dtime end;
	short tour;
	short trip;
};

//---- tour sort map ----

SYSLIB_API bool operator < (Tour_Sort left, Tour_Sort right);

typedef map <Tour_Sort, int>            Tour_Sort_Map;
typedef pair <Tour_Sort, int>           Tour_Sort_Map_Data;
typedef Tour_Sort_Map::iterator         Tour_Sort_Map_Itr;
typedef pair <Tour_Sort_Map_Itr, bool>  Tour_Sort_Map_Stat;

#endif

