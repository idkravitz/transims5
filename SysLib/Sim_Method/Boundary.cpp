//*********************************************************
//	Boundary.cpp - process boundary crossings
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Boundary
//---------------------------------------------------------

void Sim_Method::Boundary (void)
{
	Travel_Step step;
	Int_Itr int_itr;
	Integers boundary_list;

	if (boundary.size () == 0) return;

	boundary_list.swap (boundary);
	boundary_flag = true;

	//---- process vehicles on boundary links ----

	step.Partition (partition);

	for (int_itr = boundary_list.begin (); int_itr != boundary_list.end (); int_itr++) {
		step.Vehicle (*int_itr);
		step.Position (0);
#ifdef DEBUG_KEYS
		exe->debug = (exe->debug_time_flag && exe->debug_list.In_Range (*int_itr));
		if (exe->debug) MAIN_LOCK exe->Write (1, "BOUNDARY MOVE"); END_LOCK
#endif
		Move_Vehicle (step, false);
	}
	return;
}
