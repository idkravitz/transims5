//*********************************************************
//	Event_Data.cpp - event index comparison functions
//*********************************************************

#include "Event_Data.hpp"

//---------------------------------------------------------
//	event index less than operator
//---------------------------------------------------------

bool operator < (Event_Index left, Event_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Tour () < right.Tour ()) return (true);
			if (left.Tour () == right.Tour ()) {
				if (left.Trip () < right.Trip ()) return (true);
				if (left.Trip () == right.Trip ()) {
					if (left.Event () == right.Event ()) return (true);
				}
			}
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	event index less than or equal to operator
//---------------------------------------------------------

bool operator <= (Event_Index left, Event_Index right)
{
	if (left.Household () < right.Household ()) return (true);
	if (left.Household () == right.Household ()) {
		if (left.Person () < right.Person ()) return (true);
		if (left.Person () == right.Person ()) {
			if (left.Tour () < right.Tour ()) return (true);
			if (left.Tour () == right.Tour ()) {
				if (left.Trip () < right.Trip ()) return (true);
				if (left.Trip () == right.Trip ()) {
					if (left.Event () <= right.Event ()) return (true);
				}
			}
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	event index equal to operator
//---------------------------------------------------------

bool operator == (Event_Index left, Event_Index right)
{
	return (left.Household () == right.Household () &&
		left.Person () == right.Person () && 
		left.Tour () == right.Tour () &&
		left.Trip () == right.Trip () &&
		left.Event () == right.Event ());
}

