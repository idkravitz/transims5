//*********************************************************
//	Traveler_Data.hpp - traveler data classes
//*********************************************************

#ifndef TRAVELER_DATA_HPP
#define TRAVELER_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Execution_Service.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Traveler_Index class definition
//---------------------------------------------------------

class SYSLIB_API Traveler_Index
{
public:
	Traveler_Index (void)           { Clear (); }
	Traveler_Index (int hhold, int person = 0, Dtime time = 0)
	{
		Set (hhold, person, time);
	}
	int   Household (void)          { return (hhold); }
	int   Person (void)             { return (person); }
	Dtime Time (void)               { return (time); }

	void  Household (int value)     { hhold = value; }
	void  Person (int value)        { person = value; }
	void  Time (Dtime value)        { time = value; }

	void Set (int hhold, int person = 0, Dtime time = 0)
	{
		Household (hhold); Person (person); Time (time); 
	}
	void Clear (void)              { hhold = 0; person = 0; time = 0; }

private:
	int   hhold;
	int   person;
	Dtime time;
};

//---------------------------------------------------------
//	Traveler_Data class definition
//---------------------------------------------------------

class SYSLIB_API Traveler_Data
{
public:
	Traveler_Data (void)             { Clear (); }

	int   Household (void)           { return (hhold); }
	int   Person (void)              { return (person); }
	int   Tour (void)                { return (tour); }
	int   Trip (void)                { return (trip); }
	int   Mode (void)                { return (mode); }
	Dtime Time (void)                { return (time); }
	int   Distance (void)            { return (distance); }
	int   Speed (void)               { return (speed); }
	int   Dir_Index (void)           { return (dir_index); }
	int   Lane (void)                { return (lane); }
	int   Offset (void)              { return (offset); }
	int   Route (void)               { return (route); }

	void  Household (int value)      { hhold = value; }
	void  Person (int value)         { person = (short) value; }
	void  Tour (int value)           { tour = (char) value; }
	void  Trip (int value)           { trip = (char) value; }
	void  Mode (int value)           { mode = (short) value; }
	void  Time (Dtime value)         { time = value; }
	void  Distance (int value)       { distance = value; }
	void  Speed (int value)          { speed = (short) value; }
	void  Dir_Index (int value)      { dir_index = value; }
	void  Lane (int value)           { lane = (short) value; }
	void  Offset (int value)         { offset = value; }
	void  Route (int value)          { route = (short) value; }

	void  Distance (double value)    { distance = exe->Round (value); }
	void  Speed (double value)       { speed = (short) exe->Round (value); }
	void  Offset (double value)      { offset = exe->Round (value); }

	Traveler_Index Get_Traveler_Index (void) { return (Traveler_Index (hhold, person, time)); }
	void Get_Traveler_Index (Traveler_Index &index) { index.Set (hhold, person, time); }

	void  Clear (void)
	{
		hhold = offset = 0; person = mode = lane = speed = 0; tour = trip = 0; 
		time = 0; dir_index = -1; route = -1;
	}
private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
	short mode;
	short lane;
	Dtime time;
	int   distance;
	short speed;
	short route;
	int   dir_index;
	int   offset;
};

//---- Traveler array ----

typedef vector <Traveler_Data>    Traveler_Array;
typedef Traveler_Array::iterator  Traveler_Itr;

//---- Traveler map ----

SYSLIB_API bool operator < (Traveler_Index left, Traveler_Index right);
SYSLIB_API bool operator <= (Traveler_Index left, Traveler_Index right);
SYSLIB_API bool operator == (Traveler_Index left, Traveler_Index right);

typedef map <Traveler_Index, int>      Traveler_Map;
typedef pair <Traveler_Index, int>     Traveler_Map_Data;
typedef Traveler_Map::iterator         Traveler_Map_Itr;
typedef pair <Traveler_Map_Itr, bool>  Traveler_Map_Stat;

#endif

