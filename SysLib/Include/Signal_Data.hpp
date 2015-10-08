//*********************************************************
//	Signal_Data.hpp - network signal data
//*********************************************************

#ifndef SIGNAL_DATA_HPP
#define SIGNAL_DATA_HPP

#include "APIDefs.hpp"
#include "Timing_Data.hpp"
#include "Phasing_Data.hpp"
#include "Notes_Data.hpp"
#include "TypeDefs.hpp"
#include "Dtime.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Signal_Time_Data class definition
//---------------------------------------------------------

class SYSLIB_API Signal_Time_Data : public Notes_Data
{
public:
	Signal_Time_Data (void)         { Clear (); }

	Dtime Start (void)              { return (start); }
	Dtime End (void)                { return (end); }
	int   Timing (void)             { return (timing); }
	int   Phasing (void)            { return (phasing); }

	void  Start (Dtime value)       { start = value; }
	void  End (Dtime value)         { end = value; }
	void  Timing (int value)        { timing = (short) value; }
	void  Phasing (int value)       { phasing = (short) value; }

	void Clear (void)
	{
		start = end = 0; timing = phasing = 0; Notes_Data::Clear ();
	}
private:
	Dtime start;
	Dtime end;
	short timing;
	short phasing;
};

typedef vector <Signal_Time_Data>    Signal_Time_Array;
typedef Signal_Time_Array::iterator  Signal_Time_Itr;


//---------------------------------------------------------
//	Signal_Data class definition
//---------------------------------------------------------

class SYSLIB_API Signal_Data : public Signal_Time_Array
{
public:
	Signal_Data (void)              { Clear (); }

	int   Signal (void)             { return (signal); }
	int   Group (void)              { return (group); }

	void  Signal (int value)        { signal = value; }
	void  Group (int value)         { group = value; }

	Integers nodes;
	Timing_Array timing_plan;
	Phasing_Array phasing_plan;

	void Clear (void)
	{
		signal = group = 0; nodes.clear (); timing_plan.clear (); phasing_plan.clear (); clear ();
	}
private:
	int   signal;
	int   group;
};

typedef vector <Signal_Data>    Signal_Array;
typedef Signal_Array::iterator  Signal_Itr;

#endif
