//*********************************************************
//	Ridership_Data.hpp - transit ridership data class
//*********************************************************

#ifndef RIDERSHIP_DATA_HPP
#define RIDERSHIP_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Ridership_Data class definition
//---------------------------------------------------------

class SYSLIB_API Ridership_Data
{
public:
	Ridership_Data (void)             { Clear (); }

	int    Mode (void)                { return (mode); }
	int    Route (void)               { return (route); }
	int    Run (void)                 { return (run); }
	int    Stop (void)                { return (stop); }
	Dtime  Schedule (void)            { return (schedule); }
	Dtime  Time (void)                { return (time); }
	int    Board (void)               { return (board); }
	int    Alight (void)              { return (alight); }
	int    Load (void)                { return (load); }
	int    Factor (void)              { return (factor); }

	void   Mode (int value)           { mode = (short) value; }
	void   Route (int value)          { route = value; }
	void   Run (int value)            { run = (short) value; }
	void   Stop (int value)           { stop = value; }
	void   Schedule (Dtime value)     { schedule = value; }
	void   Time (Dtime value)         { time = value; }
	void   Board (int value)          { board = (short) value; }
	void   Alight (int value)         { alight = (short) value; }
	void   Load (int value)           { load = (short) value; }
	void   Factor (int value)         { factor = (short) value; }
	
	void   Factor (double value)      { factor = (short) exe->Round (value); }

	void   Clear (void)
	{
		route = stop = 0; mode = run = 0; schedule = time = 0; board = alight = load = factor = 0;
	}
private:
	int    route;
	short  mode;
	short  run;
	int    stop;
	Dtime  schedule;
	Dtime  time;
	short  board;
	short  alight;
	short  load;
	short  factor;
};

typedef vector <Ridership_Data>    Ridership_Array;
typedef Ridership_Array::iterator  Ridership_Itr;

#endif
