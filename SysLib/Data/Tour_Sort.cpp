//*********************************************************
//	Tour_Sort.cpp - household tour sort comparison functions
//*********************************************************

#include "Tour_Sort.hpp"

//---------------------------------------------------------
//	tour sort less than operator
//---------------------------------------------------------

bool operator < (Tour_Sort left, Tour_Sort right)
{
	if (left.Person () < right.Person ()) return (true);
	if (left.Person () == right.Person ()) {
		if (left.Start () < right.Start ()) return (true);
		if (left.Start () == right.Start ()) {
			if (left.End () < right.End ()) return (true);
			if (left.End () == right.End ()) {
				if (left.Tour () < right.Tour ()) return (true);
				if (left.Tour () == right.Tour ()) {
					if (left.Trip () < right.Trip ()) return (true);
				}
			}
		}
	}
	return (false); 
}
