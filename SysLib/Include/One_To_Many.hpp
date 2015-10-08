//*********************************************************
//	One_To_Many.hpp - one to many path building data
//*********************************************************

#ifndef ONE_TO_MANY_HPP
#define ONE_TO_MANY_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Many_Data class definition
//---------------------------------------------------------

class SYSLIB_API Many_Data
{
public:
	Many_Data (void)                 { Clear (); }

	int    Location (void)           { return (location); }
	int    Problem (void)            { return (problem); }
	int    Zone (void)               { return (zone); }
	int    Walk (void)               { return (walk); }
	int    Drive (void)              { return (drive); }
	int    Transit (void)            { return (transit); }
	int    Wait (void)               { return (wait); }
	int    Other (void)              { return (other); }
	int    Length (void)             { return (length); }
	int    Cost (void)               { return (cost); }
	int    Impedance (void)          { return (impedance); }

	void   Location (int value)      { location = value; }
	void   Problem (int value)       { problem = (unsigned short) value; }
	void   Zone (int value)          { zone = (unsigned short) value; }
	void   Walk (int value)          { walk = (unsigned short) value; }
	void   Drive (int value)         { drive = value; }
	void   Transit (int value)       { transit = value; }
	void   Wait (int value)          { wait = (unsigned short) value; }
	void   Other (int value)         { other = (unsigned short) value; }
	void   Length (int value)        { length = value; }
	void   Cost (int value)          { cost = (unsigned short) value; }
	void   Impedance (int value)     { impedance = value; }
	
	void   Add_Walk (int value)      { walk = (unsigned short) (walk + value); }
	void   Add_Drive (int value)     { drive += value; }
	void   Add_Transit (int value)   { transit += value; }
	void   Add_Wait (int value)      { wait = (unsigned short) (wait + value); }
	void   Add_Other (int value)     { other = (unsigned short) (other + value); }
	void   Add_Length (int value)    { length += value; }
	void   Add_Cost (int value)      { cost = (unsigned short) (cost + value); }
	void   Add_Impedance (int value) { impedance += value; }

	void   Clear_Totals (void) 
	{
		drive = transit = length = 0; walk = wait = other = cost = 0; impedance = 0;
	}
	void   Clear (void) 
	{
		location = 0; problem = zone = 0; Clear_Totals ();
	}

private:
	int            location;
	unsigned short problem;
	unsigned short zone;
	unsigned short walk;
	unsigned short wait;
	int            drive;
	int            transit;
	unsigned short other;
	unsigned short cost;
	int            length;
	unsigned       impedance;
};

typedef vector <Many_Data>    Many_Array;
typedef Many_Array::iterator  Many_Itr;

//---------------------------------------------------------
//	One_To_Many class definition
//---------------------------------------------------------

class SYSLIB_API One_To_Many : public Many_Array
{
public:
	One_To_Many (void)           { Clear (); }

	int   Location (void)        { return (location); }
	Dtime Time (void)            { return (time); }
	int   Mode (void)            { return (mode); }
	int   Use (void)             { return (use); }
	int   Veh_Type (void)        { return (veh_type); }
	int   Type (void)            { return (type); }
	int   Direction (void)       { return (direction); }
	int   Load (void)            { return (load); }
	int   Problem (void)         { return (problem); }
	int   Zone (void)            { return (zone); }
	int   Period (void)          { return (period); }

	void  Location (int value)   { location = value; }
	void  Time (Dtime value)     { time = value; }
	void  Mode (int value)       { mode = (char) value; }
	void  Use (int value)        { use = (char) value; }
	void  Veh_Type (int value)   { veh_type = (char) value; }
	void  Type (int value)       { type = (char) value; }
	void  Direction (int value)  { direction = (char) value; }
	void  Load (int value)       { load = (char) value; }
	void  Problem (int value)    { problem = (unsigned short) value; }
	void  Zone (int value)       { zone = (unsigned short) value; }
	void  Period (int value)     { period = (unsigned short) value; }

	void  Clear (void) 
	{
		location = 0; time = 0; mode = use = veh_type = type = direction = load = 0; problem = zone = period = 0; clear ();
	}

private:
	int   location;
	Dtime time;
	char  mode;
	char  use;
	char  veh_type;
	char  type;
	char  direction;
	char  load;
	unsigned short problem;
	unsigned short zone;
	unsigned short period;
};

#endif
