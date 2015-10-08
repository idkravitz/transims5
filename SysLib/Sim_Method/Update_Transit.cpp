//*********************************************************
//	Update_Transit.cpp - update transit service
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Update_Transit
//---------------------------------------------------------

void Sim_Method::Update_Transit (void)
{
	int i, j, traveler;

	Sim_Plan_Itr plan_itr;
	Sim_Plan_Ptr plan_ptr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Person_Index person_index;
	Person_Map_Stat person_stat;
	Sim_Traveler_Data *sim_traveler_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	run_update_time = MAX_INTEGER;

	for (i=0, plan_itr = exe->transit_plans.begin (); plan_itr != exe->transit_plans.end (); plan_itr++, i++) {
		if ((*plan_itr)->Partition () != partition) continue;

		line_ptr = &exe->line_array [i];

		stop_itr = line_ptr->begin ();

		for (j=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, j++) {
			if (run_itr->Schedule () < exe->step) continue;
			if (run_itr->Schedule () > exe->step) {
				if (run_itr->Schedule () < run_update_time) {
					run_update_time = run_itr->Schedule ();
				}
				continue;
			}
			stats.num_runs++;

			//---- find the traveler ----

			person_index.Household (exe->line_array.Vehicle_ID (line_ptr->Route (), j));
			person_index.Person (0);

			traveler = (int) exe->sim_traveler_array.size ();

			person_stat = exe->person_map.insert (Person_Map_Data (person_index, traveler));

			//---- create a new traveler ----

			sim_traveler_ptr = new Sim_Traveler_Data ();
			sim_traveler_ptr->Type (0);
			sim_traveler_ptr->Status (-2);

			//---- save the traveler pointer ----

			sim_traveler_ptr->random.Seed (exe->Random_Seed () + traveler);
			sim_traveler_ptr->Create ();

			MAIN_LOCK
			exe->sim_traveler_array.push_back (sim_traveler_ptr);
			END_LOCK

			plan_ptr = sim_traveler_ptr->plan_ptr;

			*plan_ptr = *(*plan_itr);

			plan_ptr->Household (person_index.Household ());
			plan_ptr->Person (0);
			plan_ptr->Tour (1);
			plan_ptr->Trip (j);
			plan_ptr->Partition ((*plan_itr)->Partition ());

			plan_ptr->Depart (run_itr->Schedule ());
			plan_ptr->Arrive (plan_ptr->Depart () + plan_ptr->Duration ());
			plan_ptr->Schedule (plan_ptr->Depart ());

			veh_index.Household (plan_ptr->Household ());
			veh_index.Vehicle (0);

			veh_itr = exe->vehicle_map.find (veh_index);

			plan_ptr->Vehicle (veh_itr->second);

			exe->work_step.Put (traveler, (partition - exe->First_Part ()));
		}
	}
}
