//*********************************************************
//	Turn_Vol_Data.hpp - network turn volume data
//*********************************************************

#ifndef TURN_VOL_DATA_HPP
#define TURN_VOL_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Turn_Vol_Data class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Vol_Data
{
public:
	Turn_Vol_Data (void)                   { Clear (); }

	int   Dir_Index (void)                 { return (dir_index); }
	int   To_Index (void)                  { return (to_index); }
	Dtime Start (void)                     { return (start); }
	Dtime End (void)                       { return (end); }
	int   Volume (void)                    { return (volume); }
	
	void  Dir_Index (int value)            { dir_index = value; }
	void  To_Index (int value)             { to_index = value; }
	void  Start (Dtime value)              { start = value; }
	void  End (Dtime value)                { end = value; }
	void  Volume (int value)               { volume = value; }

	void  Clear (void)
	{
		dir_index = to_index = -1; start = end = 0; volume = 0; 
	}	
private:
	int    dir_index;
	int    to_index;
	Dtime  start;
	Dtime  end;
	int    volume;
};

typedef vector <Turn_Vol_Data>    Turn_Vol_Array;
typedef Turn_Vol_Array::iterator  Turn_Vol_Itr;

#endif
