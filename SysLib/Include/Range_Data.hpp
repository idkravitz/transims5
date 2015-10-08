//*********************************************************
//	Range_Data.hpp - range data classes
//*********************************************************

#ifndef RANGE_DATA_HPP
#define RANGE_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Range_Data class definition
//---------------------------------------------------------

class SYSLIB_API Range_Data
{
public:
	Range_Data (void);

	int  Low (void)           { return (low); }
	int  High (void)          { return (high); }

	void Low (int value)      { low = value; }
	void High (int value)     { high = value; }

private:
	int  low;
	int  high;
};

//---------------------------------------------------------
//	Range_Array
//---------------------------------------------------------

class SYSLIB_API Range_Array : public vector <Range_Data> 
{
public:
	Range_Array (int max_records = 0)  { if (max_records > 0) reserve (max_records); }

	bool Add_Range (int low, int high, int increment = 0);
	int  Num_Ranges (void)              { return ((int) size ()); };

	bool In_Range (int value)           { return (In_Index (value) >= 0); }
	bool Span_Range (int low, int high) { return (Span_Low_Index (low, high) >= 0); }
	bool Span_Range (Range_Data range)  { return (Span_Range (range.Low (), range.High ())); }

	int  Max_Value (void);
	int  Min_Value (void);
	int  Max_Count (void);
	int  Range_Count (int value);

	int  In_Index (int value);
	int  Span_Low_Index (int low, int high);
	int  Span_High_Index (int low, int high);
};

typedef vector <Range_Data>::iterator          Range_Array_Itr;
typedef vector <Range_Data>::reverse_iterator  Range_Array_RItr;
#endif
