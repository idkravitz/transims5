 //*********************************************************
//	Next_Plan.cpp - next travel plan
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Next_Travel_Plan
//---------------------------------------------------------

bool Sim_Method::Next_Travel_Plan (int traveler)
{
	Sim_Traveler_Ptr sim_traveler_ptr;
	Sim_Plan_Map_Itr plan_map_itr;
	Plan_Index plan_index;

	sim_traveler_ptr = exe->sim_traveler_array [traveler];
	sim_traveler_ptr->Delete ();

	plan_index.Traveler (traveler);
	plan_index.Sequence (0);

	plan_map_itr = exe->sim_plan_map.lower_bound (plan_index);

	if (plan_map_itr != exe->sim_plan_map.end ()) {
		plan_index = plan_map_itr->first;

		if (plan_index.Traveler () == traveler) {
			sim_traveler_ptr->plan_ptr = plan_map_itr->second;

			plan_map_itr->second = 0;
			exe->sim_plan_map.erase (plan_map_itr);

			exe->work_step.Put (traveler, sim_traveler_ptr->plan_ptr->Partition ());
			return (true);
		}
	}
	return (false);
}
