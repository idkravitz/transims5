//*********************************************************
//	Time_Sum_Data.hpp - Time Period data
//*********************************************************

#ifndef TIME_SUM_DATA_HPP
#define TIME_SUM_DATA_HPP

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Time_Sum_Data class definition
//---------------------------------------------------------

class Time_Sum_Data
{
public:
	Time_Sum_Data (void)                 { Clear (); }

	int    Period (void)                 { return (period); }
	int    Trip_Start (void)             { return (trip_start); }
	int    Started (void)                { return (started); }
	int    Trip_End (void)               { return (trip_end); }
	int    Ended (void)                  { return (ended); }
	int    Mid_Trip (void)               { return (mid_trip); }
	int    Sum_Trips (void)              { return (sum_trip); }
	double Travel_Time (void)            { return (ttime); }
	double Start_Diff (void)             { return (start_diff); }
	double End_Diff (void)               { return (end_diff); }
	double TTime_Diff (void)             { return (ttime_diff); }
	double Start_Error (void)            { return (start_abs); }
	double End_Error (void)              { return (end_abs); }
	double TTime_Error (void)            { return (ttime_abs); }
	double Sum_Error (void)              { return (sum_error); }

	void   Period (int value)            { period = value; }
	void   Trip_Start (int value)        { trip_start = value; }
	void   Started (int value)           { started = value; }
	void   Trip_End (int value)          { trip_end = value; }
	void   Ended (int value)             { ended = value; }
	void   Mid_Trip (int value)          { mid_trip = value; }
	void   Sum_Trips (int value)         { sum_trip = value; }
	void   Travel_Time (double value)    { ttime = value; }
	void   Start_Diff (double value)     { start_diff = value; }
	void   End_Diff (double value)       { end_diff = value; }
	void   TTime_Diff (double value)     { ttime_diff = value; }
	void   Start_Error (double value)    { start_abs = value; }
	void   End_Error (double value)      { end_abs = value; }
	void   TTime_Error (double value)    { ttime_abs = value; }
	void   Sum_Error (double value)      { sum_error = value; }

	void   Add_Trip_Start (void)         { trip_start++; }
	void   Add_Start_Diff (int value)    { started++; start_diff += value; start_abs += abs (value); }
	void   Add_Trip_End (void)           { trip_end++; }
	void   Add_End_Diff (int value)      { ended++; end_diff += value; end_abs += abs (value); }
	void   Add_Travel_Time (int value, int diff) { mid_trip++; ttime += value; ttime_diff += diff; ttime_abs += abs (diff); }
	void   Add_Sum_Error (int value)     { sum_trip++; sum_error += value; }

	void   Clear (void) {
		period = trip_start = started = trip_end = ended = mid_trip = sum_trip = 0;
		ttime = start_diff = end_diff = ttime_diff = start_abs = end_abs = ttime_abs = sum_error = 0.0;
	}

private:
	int    period;
	int    trip_start;
	int    started;
	int    trip_end;
	int    ended;
	int    mid_trip;
	int    sum_trip;
	double ttime;
	double start_diff;
	double end_diff;
	double ttime_diff;
	double start_abs;
	double end_abs;
	double ttime_abs;
	double sum_error;
};

typedef vector <Time_Sum_Data>    Time_Sum_Array;
typedef Time_Sum_Array::iterator  Time_Sum_Itr;

//---- link time map ----

class Link_Time_Key : public Int2_Key
{
public:
	Link_Time_Key (void) : Int2_Key (0, 0) { }
	Link_Time_Key (int link, int period) { Key (link, period); }

	int  Link (void)                 { return (first); }
	int  Period (void)               { return (second); }

	void Link (int value)            { first = value; }
	void Period (int value)          { second = value; }

	void Key (int link, int period)  { Link (link); Period (period); }
};
typedef map <Link_Time_Key, Time_Sum_Data>   Link_Time_Map;
typedef pair <Link_Time_Key, Time_Sum_Data>  Link_Time_Map_Data;
typedef Link_Time_Map::iterator              Link_Time_Map_Itr;
typedef pair <Link_Time_Map_Itr, bool>       Link_Time_Map_Stat;

#endif
