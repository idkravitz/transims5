//*********************************************************
//	Timing_Data.hpp - network timing plan data
//*********************************************************

#ifndef TIMING_DATA_HPP
#define TIMING_DATA_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Notes_Data.hpp"

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Timing_Phase class definition
//---------------------------------------------------------

class SYSLIB_API Timing_Phase
{
public:
	Timing_Phase (void)                { Clear (); }

	int  Phase (void)                  { return (phase); }
	int  Barrier (void)                { return (barrier); }
	int  Ring (void)                   { return (ring); }
	int  Position (void)               { return (position); }
	int  Min_Green (void)              { return (minimum); }
	int  Max_Green (void)              { return (maximum); }
	int  Extension (void)              { return (extend); }
	int  Yellow (void)                 { return (yellow); }
	int  All_Red (void)                { return (red); }

	void Phase (int value)             { phase = (char) value; }
	void Barrier (int value)           { barrier = (char) value; }
	void Ring (int value)              { ring = (char) value; }
	void Position (int value)          { position = (char) value; }
	void Min_Green (int value)         { minimum = (short) value; }
	void Max_Green (int value)         { maximum = (short) value; }
	void Extension (int value)         { extend = (short) value; }
	void Yellow (int value)            { yellow = (char) value; }
	void All_Red (int value)           { red = (char) value; }
	
	void Clear (void)
	{
		phase = barrier = ring = position = yellow = red = 0; minimum = maximum = extend = 0;
	}
private:
	char  phase;
	char  barrier;
	char  ring;
	char  position;
	short minimum;
	short maximum;
	short extend;
	char  yellow;
	char  red;
};

typedef vector <Timing_Phase>         Timing_Phase_Array;
typedef Timing_Phase_Array::iterator  Timing_Phase_Itr;

//---------------------------------------------------------
//	Timing_Data class definition
//---------------------------------------------------------

class SYSLIB_API Timing_Data : public Notes_Data, public Timing_Phase_Array
{
public:
	Timing_Data (void)             { Clear (); }

	int  Timing (void)             { return (timing); }
	int  Type (void)               { return (type); }
	int  Cycle (void)              { return (cycle); }
	int  Offset (void)             { return (offset); }

	void Timing (int value)        { timing = (short) value; }
	void Type (int value)          { type = (short) value; }
	void Cycle (int value)         { cycle = (short) value; }
	void Offset (int value)        { offset = (short) value; }

	void Clear (void)
	{
		timing = type = cycle = offset = 0; Notes_Data::Clear (); clear ();
	}	
private:
	short timing;
	short type;
	short cycle;
	short offset;
};

typedef vector <Timing_Data>    Timing_Array;
typedef Timing_Array::iterator  Timing_Itr;

//---------------------------------------------------------
//	Timing40_Data
//---------------------------------------------------------

class SYSLIB_API Timing40_Data
{
public:
	Timing40_Data (void)           { Clear (); }

	int  Signal (void)             { return (signal); }
	int  Timing (void)             { return (timing); }
	int  Type (void)               { return (type); }
	int  Offset (void)             { return (offset); }

	void Signal (int value)        { signal = value; }
	void Timing (int value)        { timing = (char) value; }
	void Type (int value)          { type = (char) value; }
	void Offset (int value)        { offset = (short) value; }

	void Clear (void) 
	{
		signal = offset = 0; timing = type = 0;
	}
private:
	int   signal;
	char  timing;
	char  type;
	short offset;
};

//---- select map ----

typedef map <int, Timing40_Data>       Timing40_Map;
typedef pair <int, Timing40_Data>      Timing40_Map_Data;
typedef Timing40_Map::iterator         Timing40_Map_Itr;
typedef pair <Timing40_Map_Itr, bool>  Timing40_Map_Stat;

#endif
