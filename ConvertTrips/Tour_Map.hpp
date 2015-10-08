//*********************************************************
//	Tour_Map.hpp - tour sort map class
//*********************************************************

#ifndef TOUR_MAP_HPP
#define TOUR_MAP_HPP

#include "APIDefs.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Tour_Index class definition
//---------------------------------------------------------

class SYSLIB_API Tour_Index
{
public:
	Tour_Index (void)              { Clear (); }
	Tour_Index (int person, int start = 0, int work_code = 0, int end = 0, int record = 0, int leg = 0)
	{
		Set (person, start, work_code, end, record, leg);
	}
	int  Person (void)             { return (person); }
	int  Start (void)              { return (start); }
	int  Work_Code (void)          { return (work_code); }
	int  End (void)                { return (end); }
	int  Record (void)             { return (record); }
	int  Leg (void)                { return (leg); }

	void Person (int value)        { person = (short) value; }
	void Start (int value)         { start = (short) value; }
	void Work_Code (int value)     { work_code = (char) value; }
	void End (int value)           { end = (short) value; }
	void Record (int value)        { record = (short) value; }
	void Leg (int value)           { leg = (char) value; }	

	void Set (int person, int start = 0, int work_code = 0, int end = 0, int record = 0, int leg = 0)
	{
		Person (person); Start (start); Work_Code (work_code); End (end); Record (record); Leg (leg); 
	}
	void Clear (void)              { person = start = work_code = end = record = leg = 0; }

private:
	short person;
	short start;
	short work_code;
	short end;
	short record;
	short leg;
};

//---------------------------------------------------------
//	Tour_Data class definition
//---------------------------------------------------------

class SYSLIB_API Tour_Data
{
public:
	Tour_Data (void)               { Clear (); }

	int  Purpose (void)            { return (purpose); }
	int  Mode (void)               { return (mode); }
	int  Origin (void)             { return (origin); }
	int  Destination (void)        { return (destination); }
	int  Stop (void)               { return (stop); }
	int  Group (void)              { return (group); }
	int  Diurnal (void)            { return (diurnal); }

	void Purpose (int value)       { purpose = value; }
	void Mode (int value)          { mode = value; }
	void Origin (int value)        { origin = value; }
	void Destination (int value)   { destination = value; }
	void Stop (int value)          { stop = value; }
	void Group (int value)         { group = value; }
	void Diurnal (int value)       { diurnal = value; }

	void Clear (void) { 
		purpose = mode = origin = destination = stop = group = diurnal = 0;
	}

private:
	int   purpose;
	int   mode;
	int   origin;
	int   destination;
	int   stop;
	int   group;
	int   diurnal;
};

//---- tour map ----

SYSLIB_API bool operator < (Tour_Index left, Tour_Index right);

typedef map <Tour_Index, Tour_Data>   Tour_Map;
typedef pair <Tour_Index, Tour_Data>  Tour_Map_Data;
typedef Tour_Map::iterator            Tour_Map_Itr;
typedef pair <Tour_Map_Itr, bool>     Tour_Map_Stat;

#endif

