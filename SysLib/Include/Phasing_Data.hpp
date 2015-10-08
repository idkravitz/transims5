//*********************************************************
//	Phasing_Data.hpp - network phasing plan data
//*********************************************************

#ifndef PHASING_DATA_HPP
#define PHASING_DATA_HPP

#include "APIDefs.hpp"

//---------------------------------------------------------
//	Movement_Data class definition
//---------------------------------------------------------

class SYSLIB_API Movement_Data
{
public:
	Movement_Data (void)            { Clear (); }

	int   Movement(void)            { return (movement); }
	int   Connection (void)         { return (connection); }
	int   Protection (void)         { return (protection); }

	void  Movement (int value)      { movement = (short) value; }
	void  Connection (int value)    { connection = value; }
	void  Protection (int value)    { protection = (short) value; }

	void  Clear (void)
	{
		movement = protection = 0; connection = -1;
	}
private:
	short movement;
	short protection;
	int   connection;
};

typedef vector <Movement_Data>    Movement_Array;
typedef Movement_Array::iterator  Movement_Itr;

//---------------------------------------------------------
//	Phasing_Data class definition
//---------------------------------------------------------

class SYSLIB_API Phasing_Data : public Movement_Array
{
public:
	Phasing_Data (void)             { Clear (); }

	int   Phasing (void)            { return (phasing); }
	int   Phase (void)              { return (phase); }

	void  Phasing (int value)       { phasing = (short) value; }
	void  Phase (int value)         { phase = (short) value; }

	Integers detectors;

	void Clear (void)
	{
		phasing = phase = 0; clear (); detectors.clear ();
	}
private:
	short phasing;
	short phase;
};

typedef vector <Phasing_Data>    Phasing_Array;
typedef Phasing_Array::iterator  Phasing_Itr;

#endif
