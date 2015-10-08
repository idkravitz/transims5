//*********************************************************
//	Move_Vehicle.cpp - process the active vehicles
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Move_Vehicle
//---------------------------------------------------------

bool Sim_Method::Move_Vehicle (Travel_Step &step, bool leader)
{
	bool move_flag;
	Sim_Veh_Ptr veh_ptr;
	Cell_Data cell_rec;

	step.Clear_Output ();

	//---- retrieve the vehicle pointers ----

	step.sim_veh_ptr = exe->sim_veh_array [step.Vehicle ()];
	if (step.sim_veh_ptr == 0) return (false);

	if (leader && step.sim_veh_ptr->Leader () >= 0) return (true);

	step.Next_Vehicle (step.sim_veh_ptr->Follower ());

	if (exe->veh_status [step.Vehicle ()] != 0) return (true);

	exe->veh_status [step.Vehicle ()] = -1;

	cell_rec = step.sim_veh_ptr->front ();

#ifdef DEBUG_KEYS
	exe->debug = (exe->debug_time_flag && (exe->debug_list.In_Range (step.Vehicle ()) || exe->debug_link.In_Range (cell_rec.Index ())));
	if (exe->debug) MAIN_LOCK exe->Write (1, "Vehicle=") << step.Vehicle () << " lead=" << leader << " leader=" << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower () << " priority=" << step.sim_veh_ptr->Priority () << " partition=" << partition; END_LOCK
	if (exe->debug && step.sim_veh_ptr->Leader () >= 0) {
		veh_ptr = step.sim_veh_ptr;
		MAIN_LOCK 
		exe->Write (1, "\tleaders");
		while (veh_ptr->Leader () >= 0) {
			exe->Write (0, "=") << veh_ptr->Leader ();
			veh_ptr = exe->sim_veh_array [veh_ptr->Leader ()];
			if (veh_ptr == 0) break;
		}
		END_LOCK
	}
#endif
	step.Traveler (step.sim_veh_ptr->Driver ());
	step.sim_traveler_ptr = exe->sim_traveler_array [step.Traveler ()];
	if (step.sim_traveler_ptr == 0 || !step.sim_traveler_ptr->Active ()) {
		exe->veh_status [step.Vehicle ()] = 3;
		return (false);
	}

	step.veh_type_ptr = &exe->sim_type_array [step.sim_veh_ptr->Type ()];

	if (cell_rec.Index () >= 0) {
		step.Dir_Index (cell_rec.Index ());
		step.sim_dir_ptr = &exe->sim_dir_array [step.Dir_Index ()];
	}
	move_flag = false;

	//---- check the arrival time constraint ----

	if (step.sim_traveler_ptr->plan_ptr->Arrive () + param.max_end_variance < exe->step) {

		step.Problem (ARRIVAL_PROBLEM);
		step.Status (2);

	} else if (step.sim_traveler_ptr->Status () < 0) {

		//---- check vehicle loading ----

		if (step.sim_veh_ptr->Next_Event () > exe->step) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (1, "\tnext_load_event=") << step.sim_veh_ptr->Next_Event (); END_LOCK
#endif
			exe->veh_status [step.Vehicle ()] = 1;
			return (true);
		}
		move_flag = Load_Vehicle (step);

		if (move_flag) {
			if (step.sim_dir_ptr->Method () == MACROSCOPIC) {
				Check_Queue (step);
			} else {
#ifdef DEBUG_KEYS
if (step.sim_dir_ptr->Partition (cell_rec.Cell ()) != partition) {
	MAIN_LOCK exe->Write (1, "VehPartL=") << step.Vehicle () << " part=" << partition << " vs " << step.sim_dir_ptr->Partition (cell_rec.Cell ()) << " step=" << exe->step; END_LOCK
}
#endif
				Check_Ahead (step);
			}
			num_vehicles++;
		}

	} else {

#ifdef DEBUG_KEYS
		if (cell_rec.Lane () < 0) {
			MAIN_LOCK
			exe->Write (1, "\nLANE: vehicle=") << step.Vehicle () << " cell=" << exe->link_array [exe->dir_array [cell_rec.Index ()].Link ()].Link ()<< "-" << cell_rec.Lane () << "-" << cell_rec.Cell ();
			END_LOCK
		} else if (step.sim_dir_ptr->Method () == MESOSCOPIC) {
			int v = exe->sim_dir_array.Cell (cell_rec);
			if (v != step.Vehicle ()) {
				MAIN_LOCK
				exe->Write (1, "\tLOC: vehicle=") << step.Vehicle ()  << " vs " << v << " cell=" << exe->link_array [exe->dir_array [cell_rec.Index ()].Link ()].Link () << "-" << cell_rec.Lane () << "-" << cell_rec.Cell () << " cells=" << step.sim_dir_ptr->Cells () << " step=" << exe->step.Time_String (HOUR_CLOCK) << " part=" << step.sim_dir_ptr->From_Part () << "-" << step.sim_dir_ptr->To_Part () << " type=" << step.sim_veh_ptr->Type () << " rec=" << cell_rec.Index ();
				END_LOCK
			}
		}
		if (step.sim_dir_ptr->Partition (cell_rec.Cell ()) != partition) {
			MAIN_LOCK exe->Write (1, "VehPart=") << step.Vehicle () << " part=" << partition << " vs " << step.sim_dir_ptr->Partition (cell_rec.Cell ()) << " cell=" << cell_rec.Cell () << " split=" << step.sim_dir_ptr->Split_Cell () << " step=" << exe->step; END_LOCK
		}
#endif
		num_vehicles++;

		//---- check output for a vehicle on hold ----

		if (step.sim_veh_ptr->Next_Event () > exe->step) {
#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (1, "\tnext_event=") << step.sim_veh_ptr->Next_Event () << " seconds=" << ((step.sim_veh_ptr->Next_Event () - exe->step)/10); END_LOCK
#endif
			exe->Check_Output (step);

			exe->veh_status [step.Vehicle ()] = 1;
			return (true);
		}

		//---- attempt to move ----

		if (step.sim_dir_ptr->Method () == MACROSCOPIC) {

			if (Check_Queue (step)) move_flag = true;

		} else {

			//---- check overall lane use ----

			if (!Cell_Use (step)) goto output;

			//---- look ahead lane change ----

			if (param.look_ahead > 0 && step.sim_veh_ptr->Change () == 0) {

				if (Look_Ahead (step)) move_flag = true;
			}

			//---- attempt to move forward ----

			if (Check_Ahead (step)) move_flag = true;
		}
	}

	//---- update move flags ----

	if (move_flag) {
		step.sim_veh_ptr->Wait (0);
		if (step.sim_veh_ptr->Change () == 0) {
			step.sim_veh_ptr->Priority (0);
		}
#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (1, "\tMOVE ") << step.size () << " to " << exe->link_array [exe->dir_array [step.back ().Index ()].Link ()].Link () << "-" << step.back ().Lane () << "-" << step.back ().Cell () << " leader=" << step.sim_veh_ptr->Leader () << " follower=" << step.sim_veh_ptr->Follower (); END_LOCK
#endif
		if (step.sim_dir_ptr->Method () != MACROSCOPIC) {
			if (step.sim_veh_ptr->Leader () >= 0) {
				veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
				if (veh_ptr != 0) veh_ptr->Follower (-1);
				step.sim_veh_ptr->Leader (-1);
			}
			if (step.sim_veh_ptr->Follower () >= 0) {
				veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Follower ()];
				if (veh_ptr != 0) veh_ptr->Leader (-1);
				step.sim_veh_ptr->Follower (-1);
			}
		}
	} else {
		int problem, priority;
		Dtime min_time, max_time, priority_time;

		if (step.sim_traveler_ptr->Status () < 0) {
			min_time = param.min_load_time;
			max_time = param.max_load_time;
			priority_time = param.priority_load;
			problem = LOAD_PROBLEM;
		} else {
			min_time = param.min_wait_time;
			max_time = param.max_wait_time;
			priority_time = param.priority_wait;
			problem = WAIT_PROBLEM;
		}

		//---- check the wait time status ----

		step.sim_veh_ptr->Add_Wait (param.step_size);

#ifdef DEBUG_KEYS
		if (exe->debug) MAIN_LOCK exe->Write (1, "\tWait=") << step.sim_veh_ptr->Wait () << " at " << exe->link_array [exe->dir_array [step.sim_veh_ptr->begin ()->Index ()].Link ()].Link () << "-" << step.sim_veh_ptr->begin ()->Lane () << "-" << step.sim_veh_ptr->begin ()->Cell (); END_LOCK
#endif
		if (step.sim_veh_ptr->Wait () > max_time) {
			step.Problem (problem);
			step.Status (1);

#ifdef DEBUG_KEYS
			if (exe->debug) MAIN_LOCK exe->Write (0, " DELETE"); END_LOCK
#endif
		} else {
			if (step.sim_veh_ptr->Wait () > min_time) {
				if (step.sim_veh_ptr->Wait () > priority_time) {
					priority = 2;
				} else {
					priority = 1;
				}
				if (priority > step.sim_veh_ptr->Priority ()) {
					step.sim_veh_ptr->Priority (priority);
				}
#ifdef DEBUG_KEYS
				if (exe->debug) MAIN_LOCK exe->Write (0, " PRIORITY=") << step.sim_veh_ptr->Priority (); END_LOCK
#endif
				if (step.sim_veh_ptr->Leader () < 0) {
					priority_list.push_back (step.Vehicle ());
				}
			}
			if (step.sim_dir_ptr->Method () != MACROSCOPIC && step.sim_veh_ptr->Leader () >= 0) {
				veh_ptr = exe->sim_veh_array [step.sim_veh_ptr->Leader ()];
				if (veh_ptr != 0) {
					if (veh_ptr->Next_Event () > step.sim_veh_ptr->Next_Event ()) {
						step.sim_veh_ptr->Next_Event (veh_ptr->Next_Event ());
					}
				} else {
					step.sim_veh_ptr->Next_Event (0);
				}
			}
		}
	}

	//---- implement the result ----
output:
	if (Output_Step (step)) return (true);
	if (!step.sim_traveler_ptr->Active ()) {
		Next_Travel_Plan (step.Traveler ());
	}
	return (false);
}
