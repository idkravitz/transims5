//*********************************************************
//	Travel_Time_Data.hpp - travel time data classes
//*********************************************************

#ifndef TRAVEL_TIME_DATA_HPP
#define TRAVEL_TIME_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Trip_Index.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Travel_Time_Data class definition
//---------------------------------------------------------

class SYSLIB_API Travel_Time_Data
{
public:
	Travel_Time_Data (void)          { Clear (); }

	int   Household (void)           { return (hhold); }
	int   Person (void)              { return (person); }
	int   Tour (void)                { return (tour); }
	int   Trip (void)                { return (trip); }
	int   Mode (void)                { return (mode); }
	int   Purpose (void)             { return (purpose); }
	int   Constraint (void)          { return (constraint); }
	int   Start_Link (void)          { return (start_link); }
	int   End_Link (void)            { return (end_link); }
	Dtime Trip_Start (void)          { return (trip_start); }
	Dtime Base_Start (void)          { return (base_start); }
	Dtime Started (void)             { return (started); }
	Dtime Trip_End (void)            { return (trip_end); }
	Dtime Base_End (void)            { return (base_end); }
	Dtime Ended (void)               { return (ended); }
	Dtime Trip_Time (void)           { return (trip_end - trip_start); }
	Dtime Base_Time (void)           { return (base_end - base_start); }
	Dtime Base_Start_Diff (void)     { return (started - base_start); }
	Dtime Base_End_Diff (void)       { return (ended - base_end); }
	Dtime Trip_Start_Diff (void)     { return (started - trip_start); }
	Dtime Trip_End_Diff (void)       { return (ended - trip_end); }
	Dtime Mid_Trip (void)            { return ((started + ended) / 2); }
	Dtime Travel_Time (void)         { return (ended - started); }
	bool  Select_Flag (void)         { return (select == 1); }

	void  Household (int value)      { hhold = value; }
	void  Person (int value)         { person = (short) value; }
	void  Tour (int value)           { tour = (char) value; }
	void  Trip (int value)           { trip = (char) value; }
	void  Mode (int value)           { mode = (char) value; }
	void  Purpose (int value)        { purpose = (char) value; }
	void  Constraint (int value)     { constraint = (char) value; }
	void  Start_Link (int value)     { start_link = value; }
	void  End_Link (int value)       { end_link = value; }
	void  Trip_Start (Dtime value)   { trip_start = value; }
	void  Base_Start (Dtime value)   { base_start = value; }
	void  Started (Dtime value)      { started = value; }
	void  Trip_End (Dtime value)     { trip_end = value; }
	void  Base_End (Dtime value)     { base_end = value; }
	void  Ended (Dtime value)        { ended = value; }
	void  Select_Flag (bool flag)    { select = (flag) ? 1 : 0; }

	Trip_Index Get_Trip_Index (void) { return (Trip_Index (hhold, person, tour, trip)); }

	void Get_Trip_Index (Trip_Index &index) { index.Set (hhold, person, tour, trip); }

	void  Clear (void)
	{
		hhold = start_link = end_link = 0; person = 0; 
		tour = trip = purpose = mode = constraint = select = 0; 
		trip_start = base_start = started = trip_end = base_end = ended = 0;
	}
private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
	char  mode;
	char  purpose;
	char  constraint;
	char  select;
	int   start_link;
	int   end_link;
	Dtime trip_start;
	Dtime base_start;
	Dtime started;
	Dtime trip_end;
	Dtime base_end;
	Dtime ended;
};

//---- travel_time array ----

typedef vector <Travel_Time_Data>    Travel_Time_Array;
typedef Travel_Time_Array::iterator  Travel_Time_Itr;

#endif

