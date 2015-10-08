//*********************************************************
//	Performance_Output.hpp - Output Interface Class
//*********************************************************

#ifndef Performance_OUTPUT_HPP
#define Performance_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Performance_File.hpp"
#include "Performance_Data.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Performance_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Performance_Output : public Static_Service
{
public:
	Performance_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }

	void Summarize (Travel_Step &step);

private:
	enum Performance_Keys { 
		NEW_PERFORMANCE_FILE = PERFORMANCE_OUTPUT_OFFSET, NEW_PERFORMANCE_FORMAT, 
		NEW_PERFORMANCE_TIME_FORMAT, NEW_PERFORMANCE_INCREMENT, NEW_PERFORMANCE_TIME_RANGE, 
		NEW_PERFORMANCE_LINK_RANGE, NEW_PERFORMANCE_SUBAREA_RANGE, NEW_PERFORMANCE_COORDINATES, 
		NEW_PERFORMANCE_VEH_TYPES, NEW_PERFORMANCE_TURN_FLAG, NEW_PERFORMANCE_FLOW_TYPE,
		NEW_PERFORMANCE_LANE_FLOWS
	};

	typedef struct {
		Performance_File  *file;
		Time_Periods  time_range;
		Data_Range  link_range;
		Data_Range  subarea_range;
		Data_Range  veh_types;
		Link_Perf_Array  link_perf;
		Flow_Time_Array  turn_perf;
		int   x1, y1, x2, y2;        //---- rounded ----
		bool  data_flag;
		bool  flow_flag;
		bool  turn_flag;
		bool  coord_flag;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	Output_Array output_array;

	bool output_flag;

	void Write_Summary (Dtime step, Output_Data *output);
	void Cycle_Failure (Dtime step, int dir_index, int vehicles, int persons, int veh_type);

	Simulator_Service *exe;

#ifdef MPI_EXE
	Data_Buffer data;

	void MPI_Processing (Output_Itr output);
#endif
};
#endif
