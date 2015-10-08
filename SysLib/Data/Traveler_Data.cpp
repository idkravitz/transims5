//*********************************************************
//	Traveler_Data.cpp - traveler index comparison functions
//*********************************************************

#include "Traveler_Data.hpp"

//---------------------------------------------------------
//	traveler index less than operator
//---------------------------------------------------------

bool operator < (Traveler_Index left, Traveler_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Time () == right.Time ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	traveler index less than or equal to operator
//---------------------------------------------------------

bool operator <= (Traveler_Index left, Traveler_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Time () <= right.Time ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	traveler index equal to operator
//---------------------------------------------------------

bool operator == (Traveler_Index left, Traveler_Index right)
{
	return (left.Household () == right.Household () &&
		left.Person () == right.Person () && 
		left.Time () == right.Time ());
}
