//*********************************************************
//	Diurnal_Data.hpp - diurnal distribution data
//*********************************************************

#ifndef DIURNAL_DATA_HPP
#define DIURNAL_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Share_Data class definition
//---------------------------------------------------------

class SYSLIB_API Share_Data
{
public:
	Share_Data (void)                { Clear (); }

	double Share (void)              { return (share); }
	double Target (void)             { return (target); }
	int    Trips (void)              { return (trips); }

	void   Share (double value)      { share = value; }
	void   Target (double value)     { target = value; }
	void   Trips (int value)         { trips = value; }

	void   Clear (void)              { share = target = 0.0; trips = 0; }

	void   Add_Share (double value)  { share += value; }
	void   Add_Target (double value) { target += value; };
	void   Add_Trips (int value = 1) { trips += value; }

private:
	double share;
	double target;
	int    trips;
};

typedef vector <Share_Data>    Share_Array;
typedef Share_Array::iterator  Share_Itr;

//---------------------------------------------------------
//	Diurnal_Data class definition
//---------------------------------------------------------

class SYSLIB_API Diurnal_Data : public Share_Array
{
public:
	Diurnal_Data (void)                       { Clear (); }

	Dtime  Start_Time (void)                  { return (start); }
	Dtime  End_Time (void)                    { return (end); }
	Dtime  Time_of_Day (void)                 { return ((start + end) / 2); }
	
	void   Start_Time (Dtime value)           { start = value; }
	void   End_Time (Dtime value)             { end = value; }

	void   Clear (void)                       { start = end = 0; clear (); }

	int    Num_Shares (void)                  { return ((int) size ()); }
	void   Num_Shares (int num)               { Share_Data s; assign (num, s); }

private:
	Dtime start;
	Dtime end;
};

typedef vector <Diurnal_Data>    Diurnal_Array;
typedef Diurnal_Array::iterator  Diurnal_Itr;

#endif
