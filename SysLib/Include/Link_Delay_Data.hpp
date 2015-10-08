//*********************************************************
//	Link_Delay_Data.hpp - network link delay data
//*********************************************************

#ifndef LINK_DELAY_DATA_HPP
#define LINK_DELAY_DATA_HPP

#include "APIDefs.hpp"
#include "Flow_Time_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Turn_Delay_Data class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Delay_Data : public Flow_Time_Data
{
public:
	Turn_Delay_Data (void)         { Clear (); }

	int    To_Index (void)         { return (to_index); }
	void   To_Index (int value)    { to_index = value; }

	void   Clear (void)            { to_index = 0; Flow_Time_Data::Clear (); }

private:
	int    to_index;
};

typedef vector <Turn_Delay_Data>    Turn_Delay_Array;
typedef Turn_Delay_Array::iterator  Turn_Delay_Itr;

//---------------------------------------------------------
//	Link_Delay_Data class definition
//---------------------------------------------------------

class SYSLIB_API Link_Delay_Data : public Turn_Delay_Array, public Flow_Time_Data
{
public:
	Link_Delay_Data (void)            { Clear (); }

	int    Dir_Index (void)           { return (dir_index); }
	int    Type (void)                { return (type); }
	Dtime  Start (void)               { return (start); }
	Dtime  End (void)                 { return (end_time); }

	void   Dir_Index (int value)      { dir_index = value; }
	void   Type (int value)           { type = value; }
	void   Start (Dtime value)        { start = value; }
	void   End (Dtime value)          { end_time = value; }

	void   Clear (void)
	{
		dir_index = type = 0; start = end_time = 0; Flow_Time_Data::Clear (); clear ();
	}
private:
	int    dir_index;
	int    type;
	Dtime  start;
	Dtime  end_time;
};

typedef vector <Link_Delay_Data>    Link_Delay_Array;
typedef Link_Delay_Array::iterator  Link_Delay_Itr;

#endif
