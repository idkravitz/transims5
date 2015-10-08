//*********************************************************
//	Ridership_Output.hpp - Output Interface Class
//*********************************************************

#ifndef RIDERSHIP_OUTPUT_HPP
#define RIDERSHIP_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Ridership_File.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Ridership_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Ridership_Output : public Static_Service
{
public:
	Ridership_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }

	void Output (void);

private:
	enum Ridership_Keys { 
		NEW_RIDERSHIP_FILE = RIDERSHIP_OUTPUT_OFFSET, NEW_RIDERSHIP_FORMAT, NEW_RIDERSHIP_TIME_FORMAT,
		NEW_RIDERSHIP_TIME_RANGE, NEW_RIDERSHIP_ROUTE_RANGE, NEW_RIDERSHIP_ALL_STOPS
	};
	typedef struct {
		Ridership_File *file;
		Time_Periods time_range;
		Data_Range route_range;
		bool all_stops;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	Output_Array output_array;

	bool output_flag;

	Simulator_Service *exe;
};

#endif
