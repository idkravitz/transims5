//*********************************************************
//	Operators.cpp - warrant index operators
//*********************************************************

#include "Pocket_Warrant.hpp"
#include "Control_Warrant.hpp"
#include "Access_Warrant.hpp"

//---------------------------------------------------------
//	pocket index less than operator
//---------------------------------------------------------

bool operator < (Pocket_Index left, Pocket_Index right)
{
	if (left.From_Type () < right.From_Type ()) return (true);
	if (left.From_Type () == right.From_Type ()) {
		if (left.To_Type () < right.To_Type ()) return (true);
		if (left.To_Type () == right.To_Type ()) {
			if (left.Area_Type () < right.Area_Type ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	control index less than operator
//---------------------------------------------------------

bool operator < (Control_Index left, Control_Index right)
{
	if (left.Primary () < right.Primary ()) return (true);
	if (left.Primary () == right.Primary ()) {
		if (left.Secondary () < right.Secondary ()) return (true);
		if (left.Secondary () == right.Secondary ()) {
			if (left.Area_Type () < right.Area_Type ()) return (true);
		}
	}
	return (false); 
}

//---------------------------------------------------------
//	access index less than operator
//---------------------------------------------------------

bool operator < (Access_Index left, Access_Index right)
{
	if (left.Facility () < right.Facility ()) return (true);
	if (left.Facility () == right.Facility ()) {
		if (left.Area_Type () < right.Area_Type ()) return (true);
	}
	return (false); 
}
