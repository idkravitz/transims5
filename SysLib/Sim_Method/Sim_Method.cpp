//*********************************************************
//	Sim_Method.cpp - Network Step Simulator
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Sim_Method constructor
//---------------------------------------------------------

Sim_Method::Sim_Method (Simulator_Service &_exe, Int2_Map_Data *map, Sim_Parameters *param_ptr) : Static_Service ()
{
	exe = &_exe;
	if (exe->Service_Level () < SIMULATOR_SERVICE) {
		exe->Error ("Simulator Requires Simulator Services");
	}
	if (map == 0) {
		subarea = part = partition = 0;
	} else {
		subarea = map->first.first;
		part = map->first.second;
		partition = map->second;
	}
	transfers = 0;

	if (param_ptr != 0) {
		memcpy (&param, param_ptr, sizeof (param));
	} else {
		exe->Set_Parameters (param);
	}
	method = param.subarea_method.Best (subarea);
	num_vehicles = 0;

	use_update_time = turn_update_time = MAX_INTEGER;
	signal_update_time = timing_update_time = run_update_time = 0;
	one_second = Dtime (1, SECONDS);

	memset (&stats, '\0', sizeof (stats));

	//---- prepare the network ----

	if (method != NO_SIMULATION) {
		Network_Prep ();
	}
}
