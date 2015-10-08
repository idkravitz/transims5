//*********************************************************
//	Link_Delay_Output.hpp - Output Interface Class
//*********************************************************

#ifndef LINK_DELAY_OUTPUT_HPP
#define LINK_DELAY_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Link_Delay_File.hpp"
#include "Link_Delay_Data.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Link_Delay_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Delay_Output : public Static_Service
{
public:
	Link_Delay_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }

	void Summarize (Travel_Step &step);

private:
	enum Link_Delay_Keys { 
		NEW_LINK_DELAY_FILE = LINK_DELAY_OUTPUT_OFFSET, NEW_LINK_DELAY_FORMAT, 
		NEW_LINK_DELAY_TIME_FORMAT, NEW_LINK_DELAY_INCREMENT, NEW_LINK_DELAY_TIME_RANGE, 
		NEW_LINK_DELAY_LINK_RANGE, NEW_LINK_DELAY_SUBAREA_RANGE, NEW_LINK_DELAY_COORDINATES, 
		NEW_LINK_DELAY_VEH_TYPES, NEW_LINK_DELAY_TURN_FLAG, NEW_LINK_DELAY_FLOW_TYPE,
		NEW_LINK_DELAY_LANE_FLOWS
	};

	typedef struct {
		Link_Delay_File  *file;
		Time_Periods  time_range;
		Data_Range  link_range;
		Data_Range  subarea_range;
		Data_Range  veh_types;
		Flow_Time_Array  link_delay;
		Flow_Time_Array  turn_delay;
		int   x1, y1, x2, y2;            //---- rounded ----
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

	Simulator_Service *exe;

#ifdef MPI_EXE
	Data_Buffer data;

	void MPI_Processing (Output_Itr output);
#endif
};
#endif
