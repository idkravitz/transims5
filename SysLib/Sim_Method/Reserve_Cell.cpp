//*********************************************************
//	Reserve_Cell.cpp - reserve a cell movement
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Reserve_Cell
//---------------------------------------------------------

bool Sim_Method::Reserve_Cell (Cell_Data cell_rec, Travel_Step &step)
{
	int dir_index, lane, cell, leader, follow, vehicle;
	double permission;

	Sim_Lane_Ptr sim_lane_ptr;
	Cell_Data veh_cell;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Ptr lead_ptr, follow_ptr, veh_ptr;
	Sim_Traveler_Ptr sim_traveler_ptr;

	if (step.sim_veh_ptr->Leader () >= 0) return (false);

	//---- check the lane range ----

	lane = cell_rec.Lane ();
	if (lane < 0) return (false);

	dir_index = cell_rec.Index ();
	cell = cell_rec.Cell ();

	if (dir_index == step.Dir_Index ()) {
		if (step.sim_dir_ptr == 0) {
			step.sim_dir_ptr = &exe->sim_dir_array [dir_index];
		}
		sim_dir_ptr = step.sim_dir_ptr;
	} else {
		sim_dir_ptr = &exe->sim_dir_array [dir_index];
	}
	if (lane >= sim_dir_ptr->Lanes () || cell >= sim_dir_ptr->Cells ()) return (false);

//if (sim_dir_ptr->Partition () != partition) return (false);
#ifdef DEBUG_KEYS
	if (exe->debug) MAIN_LOCK exe->Write (1, "\tReserve"); END_LOCK
#endif

	sim_lane_ptr = sim_dir_ptr->Lane (lane);
	
	leader = (*sim_lane_ptr) [cell];
	if (leader < 0 || leader == step.Vehicle ()) return (false);
	if (exe->veh_status [leader] <= 0) return (false);

	//---- check the use restrictions ----

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Cell_Out ()) {
		if (!Cell_Use (sim_lane_ptr, lane, cell, step)) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " NoUse"); END_LOCK
#endif
			return (false);
		}
	}

	//---- check reservations on the leader ----

	lead_ptr = exe->sim_veh_array [leader];
	if (lead_ptr == 0) return (false);

	follow = lead_ptr->Follower ();
	if (follow == step.Vehicle ()) {
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " follower=") << follow; END_LOCK
#endif
		return (false);
	}
	
	//---- check the partition ----

	sim_traveler_ptr = exe->sim_traveler_array [lead_ptr->Driver ()];
	if (!sim_traveler_ptr->plan_ptr->In_Partition (partition)) {
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " partition=") << sim_traveler_ptr->plan_ptr->Partition () << " vs " << partition; END_LOCK
#endif
		return (false);
	}

	//---- check leader of the list ----

	if (lead_ptr->Leader () >= 0) {
		vehicle = leader;
		veh_ptr = lead_ptr;

		while (veh_ptr->Leader () >= 0) {
			vehicle = veh_ptr->Leader ();
			veh_ptr = exe->sim_veh_array [vehicle];
			if (veh_ptr == 0) break;
		}
		if (vehicle == step.Vehicle ()) {
			vehicle = lead_ptr->Leader ();
			veh_ptr = exe->sim_veh_array [vehicle];
			lead_ptr->Leader (-1);
			if (veh_ptr != 0) veh_ptr->Follower (-1);
			follow = lead_ptr->Follower ();
		}
	}
	if (follow >= 0) {
		follow_ptr = exe->sim_veh_array [follow];
	} else {
		follow_ptr = 0;
	}
	if (follow_ptr == 0) {
		lead_ptr->Follower (step.Vehicle ());
		step.sim_veh_ptr->Leader (leader);
		step.sim_veh_ptr->Next_Event (lead_ptr->Next_Event ());
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (0, " Make leader=") << leader; END_LOCK
#endif
		return (true);
	}

	//---- check the lane alignment ----

	if (step.size () > 0) {
		veh_cell = step.back ();
	} else {
		veh_cell = step.sim_veh_ptr->front ();
	}
	if (dir_index == veh_cell.Index () && lane == veh_cell.Lane () && cell == (veh_cell.Cell () + 1)) {

		//---- attach to the leader's follower ----

		leader = follow;
		lead_ptr = follow_ptr;
		follow = lead_ptr->Follower ();

		if (follow >= 0) {
			follow_ptr = exe->sim_veh_array [follow];
		} else {
			follow_ptr = 0;
		}
		if (follow_ptr == 0) {
			lead_ptr->Follower (step.Vehicle ());
			step.sim_veh_ptr->Leader (leader);
			step.sim_veh_ptr->Next_Event (lead_ptr->Next_Event ());
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " Make leader=") << leader; END_LOCK
#endif
			return (true);
		}

	} else {

		//---- relative priority check ----

		if (follow_ptr->Priority () > step.sim_veh_ptr->Priority ()) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " priority=") << follow << " level=" << follow_ptr->Priority () << " vs " << step.sim_veh_ptr->Priority (); END_LOCK
#endif
			return (false);
		}

		if (follow_ptr->Priority () == step.sim_veh_ptr->Priority ()) {

			//---- permissive probablity ----
		
			sim_traveler_ptr = exe->sim_traveler_array [follow_ptr->Driver ()];

			permission = param.permission_prob [sim_dir_ptr->Type ()];
			if (param.traveler_flag) permission *= param.traveler_fac.Best (sim_traveler_ptr->Type ());

			if (sim_traveler_ptr->random.Probability () > permission) {
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " NoPermission"); END_LOCK
#endif
				return (false);
			}
		}
	}

	//---- update the processing order ----

	vehicle = step.Vehicle ();
	veh_ptr = step.sim_veh_ptr;

	for (;;) {
		lead_ptr->Follower (vehicle);
		follow_ptr->Leader (vehicle);

		vehicle = veh_ptr->Follower ();
		veh_ptr->Follower (follow);
		veh_ptr->Leader (leader);
		veh_ptr->Next_Event (lead_ptr->Next_Event ());

		if (vehicle < 0) break;
		veh_ptr = exe->sim_veh_array [vehicle];
		if (veh_ptr == 0) break;

		leader = follow;
		lead_ptr = follow_ptr;

		follow = lead_ptr->Follower ();
		if (follow >= 0) {
			follow_ptr = exe->sim_veh_array [follow];
		} else {
			follow_ptr = 0;
		}
		if (follow_ptr == 0) {
			lead_ptr->Follower (vehicle);
			veh_ptr->Leader (leader);
			veh_ptr->Next_Event (lead_ptr->Next_Event ());
			break;
		}
	}
	return (true);
}
