//*********************************************************
//	Skim_Data.hpp - skim data classes
//*********************************************************

#ifndef SKIM_DATA_HPP
#define SKIM_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Skim_Data class definition
//---------------------------------------------------------

class SYSLIB_API Skim_Data
{
public:
	Skim_Data (void)                 { Clear (); }

	int   Count (void)               { return (count); }
	int   Time (void)                { return (walk + drive + transit + wait + other); }
	int   Walk (void)                { return (walk); }
	int   Drive (void)               { return (drive); }
	int   Transit (void)             { return (transit); }
	int   Wait (void)                { return (wait); }
	int   Other (void)               { return (other); }
	int   Length (void)              { return (length); }
	int   Cost (void)                { return (cost); }
	int   Impedance (void)           { return (impedance); }

	void  Count (int value)          { count = value; }
	void  Time (int value)           { drive = value; }
	void  Walk (int value)           { walk = (unsigned short) value; }
	void  Drive (int value)          { drive = value; }
	void  Transit (int value)        { transit = value; }
	void  Wait (int value)           { wait = (unsigned short) value; }
	void  Other (int value)          { other = (unsigned short) value; }
	void  Length (int value)         { length = value; }
	void  Cost (int value)           { cost = (unsigned short) value; }
	void  Impedance (int value)      { impedance = value; }

	void  Length (double value)      { length = exe->Round (value); }
	void  Cost (double value)        { cost = (unsigned short) exe->Round (value); }

	void  Add_Skim (int walk, int drive, int transit, int wait, int other, int length, int cost, unsigned impedance);
	void  Add_Skim (int time, int length, int cost, unsigned impedance);
	void  Add_Skim (Skim_Data &skim);
	
	void  Clear (void)
	{
		count = drive = transit = length = 0; walk = wait = other = cost = 0; impedance = 0;
	}
private:
	int            count;
	unsigned short walk;
	unsigned short wait;
	int            drive;
	int            transit;
	unsigned short other;
	unsigned short cost;
	int            length;
	unsigned       impedance;
};

//---------------------------------------------------------
//	Skim_Record class definition
//---------------------------------------------------------

class SYSLIB_API Skim_Record : public Skim_Data
{
public:
	Skim_Record (void)               { Clear (); }

	int   Origin (void)              { return (origin); }
	int   Destination (void)         { return (destination); }
	int   Period (void)              { return (period); }

	void  Origin (int value)         { origin = value; }
	void  Destination (int value)    { destination = value; }
	void  Period (int value)         { period = value; }
	
	void  Clear (void)
	{
		origin = destination = period = 0; Skim_Data::Clear ();
	}
private:
	int  origin;
	int  destination;
	int  period;
};

#endif

