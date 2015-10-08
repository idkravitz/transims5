//*********************************************************
//	Plan_Data.hpp - travel plan data classes
//*********************************************************

#ifndef PLAN_DATA_HPP
#define PLAN_DATA_HPP

#include "APIDefs.hpp"
#include "Trip_Data.hpp"

#include "Execution_Service.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Plan_Leg class definition
//---------------------------------------------------------

class SYSLIB_API Plan_Leg
{
public:
	Plan_Leg (void)                   { Clear (); }

	int   Mode (void)                 { return (mode); }
	int   Type (void)                 { return (type); }
	int   ID (void)                   { return (id); }
	Dtime Time (void)                 { return (time); }
	int   Length (void)               { return (length); }
	int   Cost (void)                 { return (cost); }
	int   Impedance (void)            { return (imped); }

	void  Mode (int value)            { mode = (char) value; }
	void  Type (int value)            { type = (char) value; }
	void  ID (int value)              { id = value; }
	void  Time (Dtime value)          { time = value; }
	void  Length (int value)          { length = value; }
	void  Cost (int value)            { cost = (unsigned short) value; }
	void  Impedance (int value)       { imped = value; }
	
	void  Length (double value)       { length = exe->Round (value); }
	void  Cost (double value)         { cost = (unsigned short) exe->Round (value); }

	void  Add_Time (Dtime value)      { time += value; }
	void  Add_Length (int value)      { length += value; }
	void  Add_Cost (int value)        { cost = (unsigned short) (cost + value); }
	void  Add_Imped (int value)       { imped += value; }

	bool  Link_Type (void)            { return (type == LINK_ID || type == LINK_AB || type == LINK_BA || type == USE_AB || type == USE_BA); }
	int   Link_ID (void)              { return (abs (id)); }
	int   Link_Dir (void)             { return (id < 0 || type == LINK_BA || type == USE_BA); }

	bool  Access_Type (void)          { return (type == ACCESS_ID || type == ACCESS_AB || type == ACCESS_BA); }
	int   Access_ID (void)            { return (abs (id)); }
	int   Access_Dir (void)           { return (id < 0 || type == ACCESS_BA); }

	void  Clear (void)                { memset (this, '\0', sizeof (*this)); }
	
	int            id;
	char           type;
	char           mode;
	unsigned short cost;
	Dtime          time;
	int            length;
	int            imped;
};

typedef vector <Plan_Leg>                 Plan_Leg_Array;
typedef Plan_Leg_Array::iterator          Plan_Leg_Itr;
typedef Plan_Leg_Array::reverse_iterator  Plan_Leg_RItr;

//---------------------------------------------------------
//	Plan_Data class definition
//---------------------------------------------------------

class SYSLIB_API Plan_Data : public Trip_Data, public Plan_Leg_Array 
{
public:
	Plan_Data (void)                        { Clear (); }

	int      Method (void)                  { return (method); }
	int      Problem (void)                 { return (problem); }

	Dtime    Depart (void)                  { return (depart); }
	Dtime    Arrive (void)                  { return (arrive); }
	Dtime    Activity (void)                { return (activity); }
	Dtime    Walk (void)                    { return (Dtime ((int) walk)); }
	Dtime    Drive (void)                   { return (drive); }
	Dtime    Transit (void)                 { return (transit); }
	Dtime    Wait (void)                    { return (wait); }
	Dtime    Other (void)                   { return (Dtime ((int) other)); }
	int      Length (void)                  { return (length); }
	int      Cost (void)                    { return (cost); }
	unsigned Impedance (void)               { return (impedance); }

	Dtime    Total_Time (void)              { return (Dtime ((int) walk + other) + drive + transit + wait); }

	void     Method (int value)             { method = (unsigned char) value; }
	void     Problem (int value)            { problem = (unsigned char) value; }

	void     Depart (Dtime value)           { depart = value; }
	void     Arrive (Dtime value)           { arrive = value; }
	void     Activity (Dtime value)         { activity = value; }
	void     Walk (Dtime value)             { walk = (unsigned short) value; }
	void     Drive (Dtime value)            { drive = value; }
	void     Transit (Dtime value)          { transit = value; }
	void     Wait (Dtime value)             { wait = value; }
	void     Other (Dtime value)            { other = (unsigned short) value; }
	void     Length (int value)             { length = value; }
	void     Cost (int value)               { cost = (unsigned short) value; }
	void     Impedance (unsigned value)     { impedance = value; }

	void     Length (double value)          { length = exe->Round (value); }
	void     Cost (double value)            { cost = (unsigned short) exe->Round (value); }

	void     Zero_Totals (void)             { walk = 0; drive = 0; transit = 0; wait = 0; 
	                                          other = 0; length = 0; cost = 0; impedance = 0; }

	void     Add_Walk (Dtime value)         { walk = (unsigned short) ((int) walk + value); }
	void     Add_Drive (Dtime value)        { drive += value; }
	void     Add_Transit (Dtime value)      { transit += value; }
	void     Add_Wait (Dtime value)         { wait += value; }
	void     Add_Other (Dtime value)        { other = (unsigned short) ((int) other + value); }
	void     Add_Length (int value)         { length += value; }
	void     Add_Cost (int value)           { cost = (unsigned short) (cost + value); }
	void     Add_Impedance (unsigned value) { impedance += value; }

	void     Add_Length (double value)      { length += exe->Round (value); }
	void     Add_Cost (double value)        { cost = (unsigned short) (cost + exe->Round (value)); }

	void     Clear_Plan (void)
	{
		method = problem = 0; depart = arrive = activity = 0; Zero_Totals (); clear ();
	}
	void     Clear (void)
	{
		Clear_Plan (); Trip_Data::Clear ();
	}
	void operator = (Trip_Data data)        { *((Trip_Data *) this) = data; }

	Time_Index Get_Time_Index (void)        { return (Time_Index (depart, Household (), Person ())); }
	void Get_Time_Index (Time_Index &index) { index.Set (depart, Household (), Person ()); }

private:
	Dtime          depart;
	Dtime          arrive;
	Dtime          activity;
	unsigned char  method;
	unsigned char  problem;
	unsigned short walk;
	Dtime          wait;
	Dtime          drive;
	Dtime          transit;
	unsigned short other;
	unsigned short cost;
	int            length;
	unsigned       impedance;
};

typedef vector <Plan_Data>     Plan_Array;
typedef Plan_Array::iterator   Plan_Itr;

typedef vector <Plan_Data *>      Plan_Ptr_Array;
typedef Plan_Ptr_Array::iterator  Plan_Ptr_Itr;

#endif
