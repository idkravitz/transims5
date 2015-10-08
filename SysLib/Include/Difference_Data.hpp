//*********************************************************
//	Difference_Data.hpp - Difference Distribution Data
//*********************************************************

#ifndef DIFFERENCE_DATA_HPP
#define DIFFERENCE_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "Data_Pack.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

#define NUM_DISTRIBUTION	151
#define MIN_DIFFERENCE		-60

//---------------------------------------------------------
//	Difference_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Difference_Data
{
public:
	Difference_Data (void);

	void Clear (void);

	bool Report_Flags (bool total_distrib, bool period_distrib, bool total_sum, bool period_sum);
	bool Set_Periods (Time_Periods &periods);

	bool Open_Distribution (string filename);
	bool Output_Flag (void)                      { return (output_flag); }
	bool Active_Flag (void)                      { return (active_flag); }

	bool Cost_Flag (void)                        { return (cost_flag); }
	void Cost_Flag (bool flag)                   { cost_flag = flag; }

	void Add_Trip (int tod, int current, int base);

	void Distribution_Range (int period, double percent, double &low, double &high);

	void Write_Distribution (void);

	void Distribution_Report (int number, bool total_flag = true);
	void Distribution_Header (bool total_flag = true);

	void Total_Summary (void);

	void Period_Summary (int number);
	void Period_Header (void);

	void Replicate (Difference_Data &data);
	void Merge_Data (Difference_Data &data);

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)                { summary_array.Pack (data, true);  return (diff_distrib.Pack (data)); }
	bool UnPack (Data_Buffer &data)              { summary_array.UnPack (data, true); return (diff_distrib.UnPack (data)); }
#endif

private:
	void Initialize (void);

	int num_periods, period;
	int num_distribution, min_difference;
	double scaling_factor, units_factor;
	bool total_distrib, period_distrib, total_sum, period_sum;
	bool output_flag, cost_flag, period_flag, active_flag;

	typedef struct {
		double current;
		double base;
		double abs_diff;
		int num_diff;
		int max_diff;
		int min_diff;
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
	} Summary_Data;

	typedef Vector <Summary_Data>       Summary_Array;
	typedef Summary_Array::iterator     Summary_Itr;

	Summary_Array summary_array;

	Packed_Ints_Array diff_distrib;

	Time_Periods periods;
	Db_File distrib_file;
};

#endif
