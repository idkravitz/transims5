//*********************************************************
//	Tour_Map.cpp - tour sort map methods
//*********************************************************

#include "Tour_Map.hpp"

//---------------------------------------------------------
//	tour index less than operator
//---------------------------------------------------------

bool operator < (Tour_Index left, Tour_Index right)
{
	if (left.Person () < right.Person ()) return (true);
	if (left.Person () == right.Person ()) {
		if (left.Start () < right.Start ()) return (true);
		if (left.Start () == right.Start ()) {
			if (left.Work_Code () < right.Work_Code ()) return (true);
			if (left.Work_Code () == right.Work_Code ()) {
				if (left.End () < right.End ()) return (true);
				if (left.End () == right.End ()) {
					if (left.Record () < right.Record ()) return (true);
					if (left.Record () == right.Record ()) {
						if (left.Leg () < right.Leg ()) return (true);
					}
				}
			}
		}
	}
	return (false); 
}
