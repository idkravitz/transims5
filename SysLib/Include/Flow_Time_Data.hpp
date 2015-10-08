//*********************************************************
//	Flow_Time_Data.hpp - flow rate and travel time data
//*********************************************************

#ifndef FLOW_TIME_DATA_HPP
#define FLOW_TIME_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Data_Pack.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Flow_Time_Data class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Data
{
public:
	Flow_Time_Data (void)                { Clear (); }

	double Flow (void)                   { return (flow); }
	Dtime  Time (void)                   { return (time); }

	void   Flow (double value)           { flow = (float) value; }
	void   Time (Dtime value)            { time = value; }
	void   Time (int value)              { time = value; }
	void   Time (double value)           { time = exe->Round (value); }

	void   Add_Flow (double value)       { flow = (float) (flow + value); }
	void   Add_Time (Dtime value)        { time += value; }

	void   Combine_Flow (double flw);

	void   Combine_Flow_Time (double flw, Dtime tim);
	void   Combine_Flow_Time (Flow_Time_Data &rec) 
	                                     { Combine_Flow_Time (rec.Flow (), rec.Time ()); }

	void   Clear (void)                  { flow = 0; time = 0; }

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

private:
	float flow;
	Dtime time;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & flow;
		ar & time;
	}
#endif
};

//---------------------------------------------------------
//	Flow_Time_Array class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Array : public Vector <Flow_Time_Data>
{
public:
	Flow_Time_Array (void) { }

	Dtime  Time (int index)       { return (at (index).Time ()); }
	double Flow (int index)       { return (at (index).Flow ()); }

	Flow_Time_Data * Data_Ptr (int index) { return (&at (index)); }

	Flow_Time_Data Total_Flow_Time (int index);
	Flow_Time_Data Total_Flow_Time (int index, int flow_index);
};
typedef Flow_Time_Array::iterator  Flow_Time_Itr;

//---------------------------------------------------------
//	Flow_Time_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Period_Array : public Vector <Flow_Time_Array>
{
public:
	Flow_Time_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);

	void Replicate (Flow_Time_Period_Array &period_array);

	void Set_Time0 (void);
	void Zero_Flows (void);

	void Copy_Flow_Data (Flow_Time_Period_Array &period_array, bool zero_flag = false);
	void Combine_Flows (Flow_Time_Period_Array &period_array, bool zero_flag = false);

	Flow_Time_Array * Period_Ptr (Dtime time);

	Time_Periods * periods;
	int  Num_Records (void)  { return (num_records); }

private:
	int num_records;
};
typedef Flow_Time_Period_Array::iterator  Flow_Time_Period_Itr;

#endif
