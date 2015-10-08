//*********************************************************
//	Trip_Sum_Data.hpp - Trip Summary Data
//*********************************************************

#ifndef TRIP_SUM_DATA_HPP
#define TRIP_SUM_DATA_HPP

#include "APIDefs.hpp"
#include "Time_Periods.hpp"
#include "TypeDefs.hpp"
#include "String.hpp"
#include "Dtime.hpp"
#include "Data_Pack.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

#include <math.h>

//---------------------------------------------------------
//	Trip_Sum_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Sum_Data : public Time_Periods
{
public:
	Trip_Sum_Data (void)  {}

	void Add_Trip (int group, double distance, double time, int turns = 0, int count = 1);
	void Add_Trip (Dtime tod, double distance, double time, int turns = 0, int count = 1);

	void Replicate (Trip_Sum_Data &data);
	void Merge_Data (Trip_Sum_Data &data);

	void Trip_Sum_Report (int number, char *title = 0, char *key1 = 0, char *key2 = 0);
	void Trip_Sum_Header (void);

	void Travel_Sum_Report (int number, bool veh_flag = true);
	void Travel_Sum_Header (void);

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)                { return (sum_array.Pack (data, true)); }
	bool UnPack (Data_Buffer &data)              { return (sum_array.UnPack (data, true)); }
#endif

private:

	double Average (int n, double x)             { return ((n > 0) ? x / n : 0.0); }
	double Std_Dev (int n, double x, double x2)  { return ((n > 1) ? sqrt ((x2 - (x * x / n)) / (n - 1)) : 0.0); }

	typedef struct {
		int    group;
		int    count;
		double distance;
		double distance2;
		double min_distance;
		double max_distance;
		double time;
		double time2;
		double min_time;
		double max_time;
		double speed;
		double speed2;
		double min_speed;
		double max_speed;
		double turns;
		double turns2;
		int    min_turns;
		int    max_turns;
#ifdef MAKE_MPI
		bool Pack (Data_Buffer &data)
		{
			return (data.Add_Data (this, sizeof (*this)));
		}
		bool UnPack (Data_Buffer &data)
		{
			return (data.Get_Data (this, sizeof (*this)));
		}
#endif
	} Sum_Data;

	typedef Vector <Sum_Data>    Sum_Array;
	typedef Sum_Array::iterator  Sum_Itr;

	Sum_Array sum_array;
	Int_Map group_map;
	
	int keys;
	String title, key1, key2;
	bool veh_flag;
};

#endif
