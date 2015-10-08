//*********************************************************
//	Schedule_Data.hpp - transit schedule data
//*********************************************************

#ifndef SCHEDULE_DATA_HPP
#define SCHEDULE_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Dtime.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Stop_Time class definition
//---------------------------------------------------------

class SYSLIB_API Stop_Time
{
public:
	Stop_Time (void)                   { Clear (); }

	int   Stop (void)                  { return (stop); }
	Dtime Time (int num)               { return ((Check (num)) ? time [num] : Dtime (0)); }

	void  Stop (int value)             { stop = value; }
	void  Time (int num, Dtime value)  { if (Check (num)) time [num] = value; }

	void  Clear (void)
	{
		stop = 0; memset (time, '\0', sizeof (time));
	}
private:
	bool  Check (int num)              { return (num >= 0 && num < NUM_SCHEDULE_COLUMNS); }

	int   stop;
	Dtime time [NUM_SCHEDULE_COLUMNS];
};

typedef vector <Stop_Time>         Stop_Time_Array;
typedef Stop_Time_Array::iterator  Stop_Time_Itr;

//---------------------------------------------------------
//	Schedule_Data class definition
//---------------------------------------------------------

class SYSLIB_API Schedule_Data : public Notes_Data, public Stop_Time_Array
{
public:
	Schedule_Data (void)             { Clear (); }

	int   Route (void)               { return (route); }
	int   Run (int num)              { return ((Check (num)) ? run [num] : 0); }

	void  Route (int value)          { route = value; }
	void  Run (int num, int value)   { if (Check (num)) run [num] = (short) value; }

	void  Clear (void)
	{
		route = 0; memset (run, '\0', sizeof (run)); Notes_Data::Clear (); clear ();
	}
private:
	bool  Check (int num)             { return (num >= 0 && num < NUM_SCHEDULE_COLUMNS); }

	int   route;
	short run [NUM_SCHEDULE_COLUMNS];
};

typedef vector <Schedule_Data>    Schedule_Array;
typedef Schedule_Array::iterator  Schedule_Itr;
#endif
