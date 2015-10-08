//*********************************************************
//	Performance_Data.hpp - network performance data
//*********************************************************

#ifndef PERFORMANCE_DATA_HPP
#define PERFORMANCE_DATA_HPP

#include "APIDefs.hpp"
#include "Link_Delay_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Performance_Data class definition
//---------------------------------------------------------

class SYSLIB_API Performance_Data : public Link_Delay_Data
{
public:
	Performance_Data (void)           { Clear (); }

	int    Speed (void)               { return (speed); }
	Dtime  Delay (void)               { return (delay); }
	int    Density (void)             { return (density); }
	int    Max_Density (void)         { return (max_density); }
	int    Time_Ratio (void)          { return (time_ratio); }
	int    Queue (void)               { return (queue); }
	int    Max_Queue (void)           { return (max_queue); }
	int    Failure (void)             { return (failure); }

	void   Speed (int value)          { speed = value; }
	void   Delay (Dtime value)        { delay = value; }
	void   Density (int value)        { density = value; }
	void   Max_Density (int value)    { max_density = value; }
	void   Time_Ratio (int value)     { time_ratio = value; }
	void   Queue (int value)          { queue = value; }
	void   Max_Queue (int value)      { max_queue = value; }
	void   Failure (int value)        { failure = value; }
	
	void   Speed (double value)       { speed = exe->Round (value); }
	void   Delay (int value)          { delay = value; }
	void   Delay (double value)       { delay = exe->Round (value); }
	void   Density (double value)     { density = exe->Round (value); }
	void   Max_Density (double value) { max_density = exe->Round (value); }
	void   Time_Ratio (double value)  { time_ratio = exe->Round (value); }
	void   Queue (double value)       { queue = exe->Round (value); }

	void   Clear (void)
	{
		speed = max_density = time_ratio = queue = max_queue = failure = 0;
		delay = 0; Link_Delay_Data::Clear ();
	}
private:

	int    speed;
	Dtime  delay;
	int    density;
	int    max_density;
	int    time_ratio;
	int    queue;
	int    max_queue;
	int    failure;
};

typedef vector <Performance_Data>    Performance_Array;
typedef Performance_Array::iterator  Performance_Itr;

//---------------------------------------------------------
//	Link_Perf_Data class definition
//---------------------------------------------------------

class SYSLIB_API Link_Perf_Data : public Flow_Time_Data
{
public:
	Link_Perf_Data (void)                  { Clear (); }

	int    Density (void)                  { return (density); }
	int    Max_Density (void)              { return (max_density); }
	int    Queue (void)                    { return (queue); }
	int    Max_Queue (void)                { return (max_queue); }
	int    Failure (void)                  { return (failure); }
	int    Occupancy (void)                { return (occupancy); }
	int    Stop_Count (void)               { return (count); }
	int    Ratio_Count (void)              { return (count); }
	int    Ratio_VMT (void)                { return (ratio_vmt); }
	int    Ratio_VHT (void)                { return (ratio_vht); } 

	void   Density (int value)             { density = value; }
	void   Max_Density (int value)         { max_density = value; }
	void   Queue (int value)               { queue = value; }
	void   Max_Queue (int value)           { max_queue = value; }
	void   Failure (int value)             { failure = value; }
	void   Occupancy (int value)           { occupancy = value; }
	void   Stop_Count (int value)          { count = value; }
	void   Ratio_Count (int value)         { count = value; }
	void   Ratio_VMT (int value)           { ratio_vmt = value; }
	void   Ratio_VHT (int value)           { ratio_vht = value; }

	void   Add_Density (int value)         { density += value; }
	void   Add_Max_Density (int value)     { if (value > max_density) max_density = value; }
	void   Add_Queue (int value)           { queue += value; }
	void   Add_Max_Queue (int value)       { if (value > max_queue) max_queue = value; }
	void   Add_Failure (int value)         { failure += value; }
	void   Add_Occupant (int value = 1)    { occupancy += value; }
	void   Add_Stop (int value = 1)        { count += value; }
	void   Add_Ratio (int value = 1)       { count += value; }
	void   Add_Ratio_VMT (int value)       { ratio_vmt += value; }
	void   Add_Ratio_VHT (int value)       { ratio_vht += value; }

	void   Clear (void)
	{
		density = max_density = queue = max_queue = failure = occupancy = 0;
		count = ratio_vmt = ratio_vht = 0; Flow_Time_Data::Clear ();
	}

private:
	int    density;
	int    max_density;
	int    queue;
	int    max_queue;
	int    failure;
	int    occupancy;
	int    count;
	int    ratio_vmt;
	int    ratio_vht;
};

//---------------------------------------------------------
//	Link_Perf_Array class definition
//---------------------------------------------------------

class SYSLIB_API Link_Perf_Array : public Vector <Link_Perf_Data>
{
public:
	Link_Perf_Array (void) { }
	
	Dtime  Time (int index)       { return (at (index).Time ()); }
	double Flow (int index)       { return (at (index).Flow ()); }

	Link_Perf_Data * Data_Ptr (int index) { return (&at (index)); }

	Flow_Time_Data Total_Flow_Time (int index);
	Flow_Time_Data Total_Flow_Time (int index, int flow_index);

	Link_Perf_Data Total_Link_Perf (int index);
	Link_Perf_Data Total_Link_Perf (int index, int flow_index);
};
typedef Link_Perf_Array::iterator   Link_Perf_Itr;

//---------------------------------------------------------
//	Link_Perf_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Link_Perf_Period_Array : public Vector <Link_Perf_Array>
{
public:
	Link_Perf_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);
	
	void Replicate (Link_Perf_Period_Array &period_array);

	Link_Perf_Array * Period_Ptr (Dtime time);

	Time_Periods *periods;

private:
	int num_records;
};
typedef Link_Perf_Period_Array::iterator  Link_Perf_Period_Itr;

#endif
