//*********************************************************
//	Process_Step.cpp - process the time step
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Process_Step
//---------------------------------------------------------

void Sim_Method::Process_Step (void)
{
	int status;
	Travel_Step step;
	Int_List_Itr list_itr;

	//---- process the active vehicles ----

	step.Partition (partition);

	for (list_itr = veh_list.begin (); list_itr != veh_list.end (); list_itr++) {
		step.Vehicle (*list_itr);
		step.Position (0);

		Move_Vehicle (step, true);

		while (step.Next_Vehicle () >= 0) {
			step.Vehicle (step.Next_Vehicle ());
			if (step.sim_veh_ptr != 0) {
				step.Next_Position ((int) step.sim_veh_ptr->size ());
			} else {
				step.Next_Position ();
			}
			Move_Vehicle (step, false);
		}
	}

	//---- connect remaining priority vehicles ----

	for (list_itr = priority_list.begin (); list_itr != priority_list.end (); list_itr++) {
		step.Vehicle (*list_itr);
#ifdef DEBUG_KEYS
		if (exe->debug_time_flag) {
			exe->debug = exe->debug_list.In_Range (step.Vehicle ());
			if (exe->debug) MAIN_LOCK exe->Write (1, "Priority Prep=") << step.Vehicle (); END_LOCK
		}
#endif
		Priority_Prep (step);
	}
	priority_list.clear ();

	//---- check vehicle processing ----

	for (list_itr = veh_list.begin (); list_itr != veh_list.end ();) {
		status = exe->veh_status [*list_itr];

#ifdef DEBUG_KEYS
		exe->debug = exe->debug_time_flag && exe->debug_list.In_Range (*list_itr);

		if (status == 0) {
			MAIN_LOCK exe->Write (1, "Not Processed=") << *list_itr << " step=" << exe->step.Time_String (HOUR_CLOCK) << " partition=" << partition; END_LOCK
			list_itr = veh_list.erase (list_itr);
			continue;
		}
#endif
		if (status == 3) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (1, "Remove=") << *list_itr << " partition=" << partition; END_LOCK
#endif
			list_itr = veh_list.erase (list_itr);
		} else {
			list_itr++;
		}
	}
	return;
}
