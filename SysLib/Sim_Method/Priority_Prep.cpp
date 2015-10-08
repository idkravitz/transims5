//*********************************************************
//	Priority_Prep.cpp - prepare priority movements
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Priority_Prep
//---------------------------------------------------------

bool Sim_Method::Priority_Prep (Travel_Step &step)
{
	int dir_index, lane, cell, vehicle;
	bool lane_change = false;
	
	Cell_Data cell_rec;
	Sim_Lane_Data *sim_lane_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Connect_Data *connect_ptr;
	Sim_Connection *sim_con_ptr;
	Lane_Map_Itr map_itr;

	step.Clear_Output ();

	//---- retrieve the vehicle pointers ----

	step.sim_veh_ptr = exe->sim_veh_array [step.Vehicle ()];
	if (step.sim_veh_ptr == 0 || step.sim_veh_ptr->Leader () >= 0) return (false);

	step.Traveler (step.sim_veh_ptr->Driver ());
	step.sim_traveler_ptr = exe->sim_traveler_array [step.Traveler ()];
	if (step.sim_traveler_ptr == 0) return (false);
			
	step.veh_type_ptr = &exe->sim_type_array [step.sim_veh_ptr->Type ()];

	cell_rec = step.sim_veh_ptr->front ();

	dir_index = cell_rec.Index ();
	lane = cell_rec.Lane ();
	cell = cell_rec.Cell ();

	step.Dir_Index (dir_index);
	step.sim_dir_ptr = &exe->sim_dir_array [dir_index];
	if (!step.sim_dir_ptr->Active () || step.sim_dir_ptr->Boundary ()) return (false);

	//---- check for a lane change ----

	if (step.sim_veh_ptr->Change ()) {
		
		lane += step.sim_veh_ptr->Change ();
		lane_change = true;

	} else if (++cell >= step.sim_dir_ptr->Cells ()) {

		//---- get the plan data ----

		plan_ptr = step.sim_traveler_ptr->plan_ptr;
		if (plan_ptr == 0) return (false);

		leg_itr = plan_ptr->begin ();
		if (leg_itr == plan_ptr->end ()) return (false);

		if (leg_itr->Connect () < 0) return (false);

		connect_ptr = &exe->connect_array [leg_itr->Connect ()];

		step.Dir_Index (connect_ptr->To_Index ());
		step.sim_dir_ptr = &exe->sim_dir_array [step.Dir_Index ()];
		if (!step.sim_dir_ptr->Active ()) return (false);

		sim_con_ptr = &exe->sim_connection [leg_itr->Connect ()];

		lane = -1;

		for (map_itr = sim_con_ptr->begin (); map_itr != sim_con_ptr->end (); map_itr++) {
			if (map_itr->In_Lane () == cell_rec.Lane ()) {
				lane = map_itr->Out_Lane ();
				break;
			}
		}
		cell = 0;
	}

	//---- check the new cell ----

	if (step.sim_dir_ptr->Method () != MESOSCOPIC) return (false);
	if (lane < 0 || lane >= step.sim_dir_ptr->Lanes ()) return (false);
	if (step.sim_dir_ptr->Partition (cell) != partition) return (false);

	cell_rec.Location (step.Dir_Index (), lane, cell);

	sim_lane_ptr = step.sim_dir_ptr->Lane (lane);

	vehicle = (*sim_lane_ptr) [cell];

#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (0, " vehicle=") << vehicle; END_LOCK
#endif

	//---- attach to the vehicle ----

	if (vehicle >= 0) {
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " Reserve"); END_LOCK
#endif
		return (Reserve_Cell (cell_rec, step));
	} 

	//---- check for the end of a pocket lane ----

	if (vehicle < -1) return (false);

	//---- try to move into the vacant cell ----

	if (Check_Behind (cell_rec, step)) {
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, "  Behind cell=") << exe->link_array [exe->dir_array [cell_rec.Index ()].Link ()].Link () << "-" << cell_rec.Lane () << "-" << cell_rec.Cell (); END_LOCK
#endif
		step.push_back (cell_rec);
		step.sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
		if (lane_change) {
			step.sim_veh_ptr->Change (0);
			stats.num_change++;
		}
		step.sim_veh_ptr->Wait (0);
		step.sim_veh_ptr->Priority (0);

		Output_Step (step);
		return (true);
	}
	return (false);
}
