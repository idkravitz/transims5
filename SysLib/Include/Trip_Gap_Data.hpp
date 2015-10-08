//*********************************************************
//	Trip_Gap_Data.hpp - Trip Gap Summary Data
//*********************************************************

#ifndef TRIP_GAP_DATA_HPP
#define TRIP_GAP_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "Data_Pack.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

//---------------------------------------------------------
//	Trip_Gap_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Gap_Data
{
public:
	Trip_Gap_Data (void);

	void Clear (void);

	bool Report_Flags (bool gap_report);
	bool Set_Periods (Time_Periods &periods);

	bool Open_Trip_Gap_File (string filename);
	bool Output_Flag (void)                      { return (output_flag); }
	bool Active_Flag (void)                      { return (active_flag); }
	
	bool Cost_Flag (void)                        { return (cost_flag); }
	void Cost_Flag (bool flag)                   { cost_flag = flag; }

	void Add_Trip_Gap_Data (Dtime tod, double input, double compare);
	void Write_Trip_Gap_File (void);

	void Trip_Gap_Report (int number);
	void Trip_Gap_Header (void);

	void Replicate (Trip_Gap_Data &data);
	void Merge_Data (Trip_Gap_Data &data);

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)                { return (gap_array.Pack (data, true)); }
	bool UnPack (Data_Buffer &data)              { return (gap_array.UnPack (data, true)); }
#endif

private:
	void Initialize (void);

	int num_periods, period;
	bool gap_report, output_flag, cost_flag, period_flag, active_flag;

	typedef struct {
		double input;
		double compare;
		double abs_diff;
		int count;

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

	} Gap_Data;

	typedef Vector <Gap_Data>       Gap_Array;
	typedef Gap_Array::iterator     Gap_Itr;

	Gap_Array gap_array;

	Time_Periods periods;
	Db_File gap_file;
};

#endif
