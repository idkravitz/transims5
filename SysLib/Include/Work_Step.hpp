//*********************************************************
//	Work_Step.hpp - coordinated work steps
//*********************************************************

#ifndef WORK_STEP_HPP
#define WORK_STEP_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Barrier.hpp"

//---------------------------------------------------------
//	Work_Step Class definition
//---------------------------------------------------------

class SYSLIB_API Work_Step : public Barrier, public Ints_Array
{
public:
	Work_Step (int num_workers = 1) : Barrier (num_workers) { Num_Workers (num_workers); }

	void Put (int value, int worker = 0)   { at (worker).push_back (value); }
	Integers * Get (int worker = 0)        { return (Go () ? &at (worker) : 0); }
	Integers * Next (int &worker)          { return (Go (worker) ? &at (worker) : 0); }

	void Reset (int worker = 0)            { at (worker).clear (); }

	void Num_Workers (int num)             { Integers rec; assign (num, rec); Num_Barriers (num); }
	int  Num_Workers (void)                { return (Num_Barriers ()); }
};
#endif

