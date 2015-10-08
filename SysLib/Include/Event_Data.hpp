//*********************************************************
//	Event_Data.hpp - event data classes
//*********************************************************

#ifndef EVENT_DATA_HPP
#define EVENT_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Execution_Service.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Event_Index class definition
//---------------------------------------------------------

class SYSLIB_API Event_Index
{
public:
	Event_Index (void)              { Clear (); }
	Event_Index (int hhold, int person = 0, int tour = 0, int trip = 0, int type = 0)
	{
		Set (hhold, person, tour, trip, type);
	}
	int  Household (void)          { return (hhold); }
	int  Person (void)             { return (person); }
	int  Tour (void)               { return (tour); }
	int  Trip (void)               { return (trip); }
	int  Event (void)              { return (type); }

	void Household (int value)     { hhold = value; }
	void Person (int value)        { person = (char) value; }
	void Tour (int value)          { tour = (char) value; }
	void Trip (int value)          { trip = (char) value; }
	void Event (int value)         { type = (char) value; }

	void Set (int hhold, int person = 0, int tour = 0, int trip = 0, int type = 0)
	{
		Household (hhold); Person (person); Tour (tour); Trip (trip); Event (type); 
	}
	void Clear (void)              { hhold = 0; person = tour = trip = type = 0; }

private:
	int   hhold;
	char  person;
	char  tour;
	char  trip;
	char  type;
};

//---------------------------------------------------------
//	Event_Data class definition
//---------------------------------------------------------

class SYSLIB_API Event_Data
{
public:
	Event_Data (void)                { Clear (); }

	int   Household (void)           { return (hhold); }
	int   Person (void)              { return (person); }
	int   Tour (void)                { return (tour); }
	int   Trip (void)                { return (trip); }
	int   Mode (void)                { return (mode); }
	int   Event (void)               { return (type); }
	Dtime Schedule (void)            { return (schedule); }
	Dtime Actual (void)              { return (actual); }
	int   Dir_Index (void)           { return (dir_index); }
	int   Lane (void)                { return (lane); }
	int   Offset (void)              { return (offset); }
	int   Route (void)               { return (route); }

	void  Household (int value)      { hhold = value; }
	void  Person (int value)         { person = (short) value; }
	void  Tour (int value)           { tour = (char) value; }
	void  Trip (int value)           { trip = (char) value; }
	void  Mode (int value)           { mode = (char) value; }
	void  Event (int value)          { type = (char) value; }
	void  Schedule (Dtime value)     { schedule = value; }
	void  Actual (Dtime value)       { actual = value; }
	void  Dir_Index (int value)      { dir_index = value; }
	void  Lane (int value)           { lane = (short) value; }
	void  Offset (int value)         { offset = value; }
	void  Route (int value)          { route = value; }

	void  Offset (double value)      { offset = exe->Round (value); }

	Event_Index Get_Event_Index (void) { return (Event_Index (hhold, person, tour, trip, type)); }
	void Get_Event_Index (Event_Index &index) { index.Set (hhold, person, tour, trip, type); }

	void  Clear (void)
	{
		hhold = offset = 0; person = lane = 0; tour = trip = mode = type = 0; 
		schedule = actual = 0; dir_index = route = -1;
	}
private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
	short lane;
	char  mode;
	char  type;
	Dtime schedule;
	Dtime actual;
	int   dir_index;
	int   offset;
	int   route;
};

//---- event array ----

typedef vector <Event_Data>    Event_Array;
typedef Event_Array::iterator  Event_Itr;

//---- event map ----

SYSLIB_API bool operator < (Event_Index left, Event_Index right);
SYSLIB_API bool operator <= (Event_Index left, Event_Index right);
SYSLIB_API bool operator == (Event_Index left, Event_Index right);

typedef map <Event_Index, int>      Event_Map;
typedef pair <Event_Index, int>     Event_Map_Data;
typedef Event_Map::iterator         Event_Map_Itr;
typedef pair <Event_Map_Itr, bool>  Event_Map_Stat;

#endif

