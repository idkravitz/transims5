//*********************************************************
//	Range_Data.cpp - range data classes
//*********************************************************

#include "Range_Data.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Range_Data constructor
//---------------------------------------------------------

Range_Data::Range_Data (void)
{
	Low (0);
	High (MAX_INTEGER);
}

//---------------------------------------------------------
//	In_Index
//---------------------------------------------------------

int Range_Array::In_Index (int value)
{
	Range_Array_Itr itr;

	for (itr = begin (); itr != end (); itr++) {
		if (value >= itr->Low () && value <= itr->High ()) {
			return ((int) (itr - begin ()));
		}
	}
	return (-1);
}

//---------------------------------------------------------
//	Span_Low_Index
//---------------------------------------------------------

int Range_Array::Span_Low_Index (int low, int high)
{
	Range_Array_Itr itr;

	for (itr = begin (); itr != end (); itr++) {
		if (high >= itr->Low () && low <= itr->High ()) {
			return ((int) (itr - begin ()));
		}
	}
	return (-1);
}

//---------------------------------------------------------
//	Span_High_Index
//---------------------------------------------------------

int Range_Array::Span_High_Index (int low, int high)
{
	Range_Array_RItr ritr;

	for (ritr = rbegin (); ritr != rend (); ritr++) {
		if (high >= ritr->Low () && low <= ritr->High ()) {
			return ((int) (size () + rbegin () - ritr - 1));
		}
	}
	return (-1);
}

//---------------------------------------------------------
//	Range_Count
//---------------------------------------------------------

int Range_Array::Range_Count (int value)
{
	Range_Array_Itr itr;
	int count = 0;

	for (itr = begin (); itr != end (); itr++) {
		if (value >= itr->Low () && value <= itr->High ()) {
			count += value - itr->Low () + 1;
			return (count);
		} else {
			count += itr->High () - itr->Low () + 1;
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Max_Value
//---------------------------------------------------------

int Range_Array::Max_Value (void)
{
	Range_Array_Itr itr;
	int max = -1;

	for (itr = begin (); itr != end (); itr++) {
		if (itr->High () > max) max = itr->High ();
	}
	return (max);
}

//---------------------------------------------------------
//	Min_Value
//---------------------------------------------------------

int Range_Array::Min_Value (void)
{
	Range_Array_Itr itr;
	int min = MAX_INTEGER;

	for (itr = begin (); itr != end (); itr++) {
		if (itr->Low () < min) min = itr->Low ();
	}
	return (min);
}

//---------------------------------------------------------
//	Max_Count
//---------------------------------------------------------

int Range_Array::Max_Count (void)
{
	Range_Array_Itr itr;
	int count = 0;

	for (itr = begin (); itr != end (); itr++) {
		count += itr->High () - itr->Low () + 1;
	}
	return (count);
}

//---------------------------------------------------------
//	Add_Range
//---------------------------------------------------------

bool Range_Array::Add_Range (int low, int high, int increment)
{
	Range_Data range;

	if (increment > 0 && (low + increment) <= high) {
		for (int inc = low; inc <= high;) { 
			range.Low (inc);
			inc += increment;
			range.High (inc - 1);

			push_back (range);
		}
		return (true);
	}
	range.Low (low);
	range.High (high);

	push_back (range);
	return (true);
}
