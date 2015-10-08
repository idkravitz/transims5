//*********************************************************
//	Max_Check.cpp - check the link maximums
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Maximum_Check
//---------------------------------------------------------

void ArcSnapshot::Maximum_Check (void)
{
	Cell_Data_Itr cell_itr;

	for (cell_itr = cell_array.begin (); cell_itr != cell_array.end (); cell_itr++) {
		if (cell_itr->total == 0) continue;
		if (cell_itr->max_total < cell_itr->total) {
			cell_itr->maximum.swap (cell_itr->data);
		}
		cell_itr->total = 0;
		cell_itr->data.assign ((cell_itr->lanes * cell_itr->cells), 0);
	}
}
