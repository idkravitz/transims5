//*********************************************************
//	Occupancy_Output.hpp - Output Interface Class
//*********************************************************

#ifndef OCCUPANCY_OUTPUT_HPP
#define OCCUPANCY_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Occupancy_File.hpp"
#include "TypeDefs.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Occupancy_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Occupancy_Output : public Static_Service
{
public:
	Occupancy_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }
	
	void Summarize (Travel_Step &step);

private:
	enum Occupancy_Keys { 
		NEW_OCCUPANCY_FILE = OCCUPANCY_OUTPUT_OFFSET, NEW_OCCUPANCY_FORMAT, NEW_OCCUPANCY_TIME_FORMAT,
		NEW_OCCUPANCY_INCREMENT, NEW_OCCUPANCY_TIME_RANGE, NEW_OCCUPANCY_LINK_RANGE, NEW_OCCUPANCY_SUBAREA_RANGE, 
		NEW_OCCUPANCY_COORDINATES, NEW_OCCUPANCY_MAX_FLAG
	};
	typedef struct {
		int lanes;
		int cells;
		int total;
		Integers occ;
	} Cell_Occ;
	
	typedef vector <Cell_Occ>         Cell_Occ_Array;
	typedef Cell_Occ_Array::iterator  Cell_Occ_Itr;

	typedef struct {
		Occupancy_File *file;
		Time_Periods time_range;
		Data_Range link_range;
		Data_Range subarea_range;
		Cell_Occ_Array occ_array;
		int x1, y1, x2, y2;		//---- rounded ----
		bool data_flag;
		bool coord_flag;
		bool max_flag;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	bool output_flag, total_flag;
	Output_Array output_array;
	
	void Write_Summary (Dtime step, Output_Data *output);

	Simulator_Service *exe;

#ifdef MPI_EXE
	Data_Buffer data;

	void MPI_Processing (Output_Itr output);
#endif
};
#endif
