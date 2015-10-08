//*********************************************************
//	Time_Index.cpp - time index comparison functions
//*********************************************************

#include "Time_Index.hpp"

//---------------------------------------------------------
//	time index less than operator
//---------------------------------------------------------

bool operator < (Time_Index left, Time_Index right)
{
	if (left.Start () < right.Start ()) return (true);
	if (left.Start () == right.Start ()) {
		if (left.Household () < right.Household ()) return (true);
		if (left.Household () == right.Household ()) {
			if (left.Person () < right.Person ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	time index less than or equal to operator
//---------------------------------------------------------

bool operator <= (Time_Index left, Time_Index right)
{
	if (left.Start () < right.Start ()) return (true);
	if (left.Start () == right.Start ()) {
		if (left.Household () < right.Household ()) return (true);
		if (left.Household () == right.Household ()) {
			if (left.Person () <= right.Person ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	time index equal to operator
//---------------------------------------------------------

bool operator == (Time_Index left, Time_Index right)
{
	return (left.Start () == right.Start () && 
		left.Household () == right.Household () && 
		left.Person () == right.Person ());
}

//---------------------------------------------------------
//	time index not equal to operator
//---------------------------------------------------------

bool operator != (Time_Index left, Time_Index right)
{
	return (left.Start () != right.Start () ||
		left.Household () != right.Household () || 
		left.Person () != right.Person ());
}
