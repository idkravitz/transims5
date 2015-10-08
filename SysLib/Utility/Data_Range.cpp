//*********************************************************
//	Data_Range.cpp - data range functions
//*********************************************************

#include "Data_Range.hpp"

#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Data_Range constructor
//---------------------------------------------------------

Data_Range::Data_Range (int factor, int minimum, int maximum, int increment) : 
	Range_Array (), Static_Service ()
{
	Setup (factor, minimum, maximum, increment);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Data_Range::Setup (int fac, int min, int max, int inc)
{
	if (fac < 1 || min >= max || max < 0 || inc < 0) {
		if (exe->Send_Messages ()) {
			exe->Error (String ("Range Setup %d-%d-%d-%d is Out of Range") % fac % min % max % inc);
		}
	} else {
		factor = fac;
		minimum = min * fac;
		maximum = max * fac;
		increment = inc;
	}
	return;
}

//---------------------------------------------------------
//	Add_Ranges
//---------------------------------------------------------

bool Data_Range::Add_Ranges (string text)
{
	int low, high;
	double d1, d2;
	Strings ranges;
	Str_Itr itr;

	//---- check for special conditions ----

	if (text.empty () || text [0] == '\n') return (true);
	
	if (String_Ptr (text)->Equals ("All")) {
		return (Add_Range (minimum, maximum, increment));
	}

	//---- unpack the range string ----	

	String_Ptr (text)->Parse (ranges);

	for (itr = ranges.begin (); itr != ranges.end (); itr++) {
		if (!String_Ptr (*itr)->Range (d1, d2)) continue;

		if (d1 == 0.0 && d2 == 0.0 && !itr->Equals ("0")) continue;

		low = DTOI (d1 * factor);
		high = DTOI (d2 * factor);

		if (low > high || low < minimum || high > maximum) {
			if (exe->Send_Messages ()) {
				exe->Error (String ("Range %g-%g is Out of Range") % d1 % d2);
			}
		}
		if (!Add_Range (low, high, increment)) return (false);
	}
	return (Num_Ranges () > 0);
}

//---------------------------------------------------------
//	Add_Breaks
//---------------------------------------------------------

bool Data_Range::Add_Breaks (string text)
{
	int low, high;
	double d1, d2;
	Strings breaks;
	Str_Itr itr;

	//---- check for special conditions ----

	if (text.empty () || text [0] == '\n') return (true);

	if (String_Ptr (text)->Equals ("None")) {
		return (Add_Range (minimum, maximum));
	}

	//---- unpack the break string ----	

	low = minimum;

	String_Ptr (text)->Parse (breaks);

	for (itr = breaks.begin (); itr != breaks.end (); itr++) {
		if (!String_Ptr (*itr)->Range (d1, d2)) continue;

		if (d1 == 0.0 && d2 == 0.0) continue;
		if (d2 > d1) goto break_error;

		high = DTOI (d2 * factor);

		if (low > high || low < minimum || high > maximum) goto break_error;

		if (low < high) {
			if (!Add_Range (low, high)) return (false);
		}
		low = high;
	}
	high = maximum;

	if (low < high) {
		return (Add_Range (low, high));
	}
	return (Num_Ranges () > 0);

break_error:
	if (exe->Send_Messages ()) {
		exe->Error (String ("Range Breaks %s are Illogical") % text);
	}
	return (false);
}
