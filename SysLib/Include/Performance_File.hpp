//******************************************************** 
//	Performance_File.hpp - Link Performance Input/Output
//********************************************************

#ifndef PERFORMANCE_FILE_HPP
#define PERFORMANCE_FILE_HPP

#include "APIDefs.hpp"
#include "Link_Delay_File.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Performance_File Class definition
//---------------------------------------------------------

class SYSLIB_API Performance_File : public Link_Delay_File
{
public:
	Performance_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);
	Performance_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);

	double Speed (void)               { return (Get_Double (speed)); }
	Dtime  Delay (void)               { return (Get_Time (delay)); }
	double Density (void)             { return (Get_Double (density)); }
	double Max_Density (void)         { return (Get_Double (max_den)); }
	double Time_Ratio (void)          { return (Get_Double (ratio)); }
	double Queue (void)               { return (Get_Double (queue)); }
	int    Max_Queue (void)           { return (Get_Integer (max_que)); }
	int    Cycle_Failure (void)       { return (Get_Integer (fail)); }

	void   Speed (double value)       { Put_Field (speed, value); }
	void   Delay (Dtime value)        { Put_Field (delay, value); }
	void   Density (double value)     { Put_Field (density, value); }
	void   Max_Density (double value) { Put_Field (max_den, value); }
	void   Time_Ratio (double value)  { Put_Field (ratio, value); }
	void   Queue (double value)       { Put_Field (queue, value); }
	void   Max_Queue (int value)      { Put_Field (max_que, value); }
	void   Cycle_Failure (int value)  { Put_Field (fail, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int speed, delay, density, max_den, ratio, queue, max_que, fail;
};

#endif
