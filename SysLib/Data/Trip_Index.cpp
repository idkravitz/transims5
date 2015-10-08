//*********************************************************
//	Trip_Index.cpp - trip index comparison functions
//*********************************************************

#include "Trip_Index.hpp"

//---------------------------------------------------------
//	trip index less than operator
//---------------------------------------------------------

bool operator < (Trip_Index left, Trip_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Tour () < right.Tour ()) return (true);
			if (left.Tour () == right.Tour ()) {
				if (left.Trip () < right.Trip ()) return (true);
			}
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	trip index less than or equal to operator
//---------------------------------------------------------

bool operator <= (Trip_Index left, Trip_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Tour () < right.Tour ()) return (true);
			if (left.Tour () == right.Tour ()) {
				if (left.Trip () <= right.Trip ()) return (true);
			}
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	trip index equal to operator
//---------------------------------------------------------

bool operator == (Trip_Index left, Trip_Index right)
{
	return (left.Household () == right.Household () &&
		left.Person () == right.Person () && 
		left.Tour () == right.Tour () &&
		left.Trip () == right.Trip ());
}

//---------------------------------------------------------
//	trip index not equal to operator
//---------------------------------------------------------

bool operator != (Trip_Index left, Trip_Index right)
{
	return (left.Household () != right.Household () ||
		left.Person () != right.Person () ||
		left.Tour () != right.Tour () ||
		left.Trip () != right.Trip ());
}
