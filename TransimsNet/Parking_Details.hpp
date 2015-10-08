//*********************************************************
//	Parking_Details.hpp - detailed parking data
//*********************************************************

#ifndef PARKING_DETAILS_HPP
#define PARKING_DETAILS_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Parking_Details class definition
//---------------------------------------------------------

class Parking_Details
{
public:
	Parking_Details (void)                { Clear (); }

	int   Area_Type1 (void)               { return (area_type1); }
	int   Area_Type2 (void)               { return (area_type2); }
	Dtime Start (void)                    { return (start); }
	Dtime End (void)                      { return (end); }
	int   Use (void)                      { return (use); }
	int   Time_In (void)                  { return (in); }
	int   Time_Out (void)                 { return (out); }
	int   Hourly (void)                   { return (hourly); }
	int   Daily (void)                    { return (daily); }
	
	void  Area_Type1 (int value)          { area_type1 = (short) value; }
	void  Area_Type2 (int value)          { area_type2 = (short) value; }
	void  Start (Dtime value)             { start = value; }
	void  End (Dtime value)               { end = value; }
	void  Use (int value)                 { use = (unsigned short) value; }
	void  Time_In (int value)             { in = (unsigned short) value; }
	void  Time_Out (int value)            { out = (unsigned short) value; }
	void  Hourly (int value)              { hourly = (unsigned short) value; }
	void  Daily (int value)               { daily = (unsigned short) value; }

	void Clear (void)
	{
		area_type1 = area_type2 = 0; start = end = 0;
		use = in = out = hourly = daily = 0;
	}
private:
	short          area_type1;
	short          area_type2;
	Dtime          start;
	Dtime          end;
	unsigned short use;
	unsigned short in;
	unsigned short out;
	unsigned short hourly;
	unsigned short daily;
};

typedef vector <Parking_Details>     Park_Detail_Array;
typedef Park_Detail_Array::iterator  Park_Detail_Itr;

#endif
