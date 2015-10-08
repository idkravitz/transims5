//*********************************************************
//	Street_Parking.hpp - street parking time permits
//*********************************************************

#ifndef STREET_PARKING_HPP
#define STREET_PARKING_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Street_Parking class definition
//---------------------------------------------------------

class Street_Parking
{
public:
	Street_Parking (void)                 { Clear (); }

	Dtime Start (void)                    { return (start); }
	Dtime End (void)                      { return (end); }
	int   Next_Index (void)               { return (next_index); }
	
	void  Start (Dtime value)             { start = value; }
	void  End (Dtime value)               { end = value; }
	void  Next_Index (int value)          { next_index = value; }

	void Clear (void)
	{
		start = end = 0; next_index = -1;
	}
private:
	Dtime  start;
	Dtime  end;
	int    next_index;
};

typedef vector <Street_Parking>      Street_Park_Array;
typedef Street_Park_Array::iterator  Street_Park_Itr;

#endif
