//*********************************************************
//	Time_Periods.hpp - time of day range class
//*********************************************************

#ifndef TIME_PERIODS_HPP
#define TIME_PERIODS_HPP

#include "APIDefs.hpp"
#include "Range_Data.hpp"
#include "Dtime.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Time_Periods Class definition
//---------------------------------------------------------

class SYSLIB_API Time_Periods : public Range_Array, public Static_Service
{
public:
	Time_Periods (void);

	int    Period (Dtime time);
	bool   At_Increment (Dtime time);
	bool   In_Range (Dtime time)               { return (Period (time) >= 0); }
	bool   Span_Range (Dtime low, Dtime high);
	bool   Span_Range (Range_Data range)       { return (Span_Range (range.Low (), range.High ())); }

	int    Num_Periods (void);
	int    Range_Length (void);
	int    Max_Count (void)                    { return (Range_Length ()); }

	Dtime  Start (void)                        { return (start); }
	void   Start (Dtime value)                 { start (value); }
	void   Start (string text)                 { start (text); }

	Dtime  End (void)                          { return (end_time); }
	void   End (Dtime value)                   { end_time (value); }
	void   End (string text)                   { end_time (text); }

	Dtime  Increment (void)                    { return (increment); }
	void   Increment (Dtime value)             { Set_Periods (value, Start (), End (), Wrap_Flag ()); }
	void   Increment (string text)             { Set_Periods (Dtime (text), Start (), End (), Wrap_Flag ()); }

	bool   Wrap_Flag (void)                    { return (wrap_flag); }
	void   Wrap_Flag (bool flag)               { wrap_flag = flag; }
	
	bool   Merge_Flag (void)                   { return (merge_flag); }
	void   Merge_Flag (bool flag)              { merge_flag = flag; }

	bool   Range_Flag (void)                   { return (range_flag); }
	void   Range_Flag (bool flag)              { range_flag = flag; }

	bool   Set_Periods (Dtime increment, Dtime start = 0, Dtime end = 0, bool wrap_flag = false);

	int    Period_Control_Point (void)         { return (period_point); }
	void   Period_Control_Point (int point)    { period_point = point; }

	bool   Add_Label (string label);
	bool   Add_Ranges (string ranges);
	bool   Add_Breaks (string breaks);

	string Range_Format (int num);
	string Range_Label (int num, bool pad_flag = false);
	string Break_String (void);

	bool   Period_Range (int num, Dtime &low, Dtime &high);
	bool   Period_Range (Dtime low, Dtime high, int &p1, int &p2);

	Dtime  Period_Time (int num);

	double Period_Offset (int num, Dtime time);

	bool   Copy_Periods (Time_Periods &periods);

private:
	bool   range_flag, wrap_flag, merge_flag;
	Dtime  start, end_time, increment;
	int    period_point;
};

#endif
