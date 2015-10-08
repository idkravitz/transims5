//*********************************************************
//	Route_Stop_Data.hpp - list of routes serving a stop
//*********************************************************

#ifndef ROUTE_STOP_DATA_HPP
#define ROUTE_STOP_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Route_Stop_Data class definition
//---------------------------------------------------------

class SYSLIB_API Route_Stop_Data
{
public:
	Route_Stop_Data (void)         { Clear (); }

	int  Route (void)              { return (route); }
	int  Stop (void)               { return (stop); }
	int  Next (void)               { return (next); }

	
	void Route (int value)         { route = (unsigned short) value; }
	void Stop (int value)          { stop = (unsigned short) value; }
	void Next (int value)          { next = value; }

	void Clear (void)              { route = stop = 0; next = -1; }

private:
	unsigned short route;
	unsigned short stop;
	int            next;
};

typedef vector <Route_Stop_Data>    Route_Stop_Array;
typedef Route_Stop_Array::iterator  Route_Stop_Itr;

#endif
