//*********************************************************
//	Driver_Data.hpp - transit driver classes
//*********************************************************

#ifndef DRIVER_DATA_HPP
#define DRIVER_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

typedef vector <int>              Driver_Array;
typedef Driver_Array::iterator    Driver_Itr;

//---------------------------------------------------------
//	Driver_Data class definition
//---------------------------------------------------------

class SYSLIB_API Driver_Data : public Notes_Data, public Driver_Array
{
public:
	Driver_Data (void)           { Clear (); }

	int  Route (void)            { return (route); }
	int  Type (void)             { return (type); }
	int  Links (void)            { return ((int) size ()); }

	void Route (int value)       { route = value; }
	void Type (int value)        { type = value; }
	void Links (int value)       { resize (value); }
	
	void Clear (void)
	{
		route = type = 0; Links (0); Notes_Data::Clear (); 
	}
private:
	int  route;
	int  type;
};

#endif
