//*********************************************************
//	Step_Prep.cpp - prepare time step processing
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Step_Prep
//---------------------------------------------------------

bool Sim_Method::Step_Prep (Integers * new_trips)
{
	Travel_Step step;
	Int_Itr int_itr;
	Integers boundary_list;

	num_vehicles = 0;

	if (veh_list.size () == 0 && trip_list.size () == 0 && new_trips->size () == 0) return (false);

	//---- copy the new trips in the processing list ----

	trip_list.insert (trip_list.end (), new_trips->begin (), new_trips->end ());
	new_trips->clear ();

	transfers = &exe->transfers.at (partition - exe->First_Part ());
	transfers->clear ();
	boundary_flag = false;

	//---- update the network state ----

	if ((exe->step % one_second) == 0 && method != NO_SIMULATION) {
		if (exe->step >= use_update_time || exe->step >= turn_update_time) {
			Update_Network ();
		}
		if (exe->step >= run_update_time) {
			Update_Transit ();
		}
		if (exe->step >= timing_update_time) {
			Traffic_Controls ();
		}
		if (exe->step >= signal_update_time) {
			Update_Signals ();
		}
	}

	//---- process the traveler events ----

	Traveler_Events ();

#ifdef DEBUG_KEYS
	for (Int_List_Itr list_itr = veh_list.begin (); list_itr != veh_list.end (); list_itr++) {
		int status = exe->veh_status [*list_itr];

		if (exe->debug_time_flag && exe->debug_list.In_Range (*list_itr)) {
			MAIN_LOCK exe->Write (1, "Veh_List=") << *list_itr << " partition=" << partition << " status=" << status; END_LOCK
		}
	}
#endif

	//---- process the vehicles after partition boundaries ----

	if (first_list.size () > 0) {
		boundary_list.swap (first_list);

		step.Partition (partition);

		for (int_itr = boundary_list.begin (); int_itr != boundary_list.end (); int_itr++) {
			step.Vehicle (*int_itr);
			step.Position (0);
#ifdef DEBUG_KEYS
			exe->debug = (exe->debug_time_flag && exe->debug_list.In_Range (*int_itr));
			if (exe->debug) MAIN_LOCK exe->Write (1, "BOUNDARY FIRST"); END_LOCK
#endif
			Move_Vehicle (step, false);
		}
	}
	return (veh_list.size () > 0 || trip_list.size () > 0 || transfers->size () > 0);
}
