//*********************************************************
//	Smooth_Data.hpp - data smoothing class
//*********************************************************

#ifndef SMOOTH_DATA_HPP
#define SMOOTH_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Smooth_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Smooth_Data
{
public:
	Smooth_Data (int num_in = 0, int iterations = 3, bool loop_flag = false, int group_size = 3, 
		double forward = 20.0, double backward = 20.0, int num_sub = 1, bool dup_flag = false);

	bool Setup (int num_in, int iterations = 3, bool loop_flag = false,	int group_size = 3, 
		double forward = 20.0, double backward = 20.0, int num_sub = 1, bool dup_flag = false);

	bool Interpolate (int num_in, double increment, int iterations = 3, 
		int group_size = 3, double forward = 20.0, double backward = 20.0, bool dup_flag = true);

	bool Num_Input (int value);
	void Input (int num, double value)            { if (Check_In (num)) input [num] = value; };
	double Input (int num)                        { return ((Check_In (num)) ? input [num] : 0); }

	int  Num_Input (void)                         { return (num_in); }
	int  Num_Output (void)                        { return (num_out); }

	void Output (int num, double value)           { if (Check_Out (num)) output [num] = value; };
	double Output (int num)                       { return ((Check_Out (num)) ? output [num] : 0.0); }

	int Smooth (int num = 0);
	
	void Add_Keys (void);
	bool Read_Control (void);

	void Duplicate_Flag (bool flag)               { dup_flag = flag; }
	void Num_Subdivide (int value)                { num_sub = value; }

protected:
	enum SmoothData_Keys { 
		SMOOTH_GROUP_SIZE = SMOOTH_DATA_OFFSET, PERCENT_MOVED_FORWARD, PERCENT_MOVED_BACKWARD, 
		NUMBER_OF_ITERATIONS, CIRCULAR_GROUP_FLAG, 
	};
	bool Check_In (int num)                       { return (num >= 0 && num < num_in); }
	bool Check_Out (int num)                      { return (num >= 0 && num < num_out); }

	void Clear (void);

private:
	int num_in, num_out, niter, size, num_sub;
	double increment, forward, backward, weight;	
	bool loop_flag, dup_flag, interpolate;
	Doubles input, output, current;
};
#endif
