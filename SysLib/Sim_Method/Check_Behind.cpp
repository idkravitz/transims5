//*********************************************************
//	Check_Behind.cpp - check speed constraints from behind
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Check_Behind
//---------------------------------------------------------

bool Sim_Method::Check_Behind (Cell_Data cell_rec, Travel_Step &step, int num_cells, bool use_flag)
{
	int dir_index, lane, cell, index, control, vehicle, speed, gap, min_gap, max_gap, max_cells, min_step, max_step, min_cell;
	double permission;
	bool link_flag;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;
	Sim_Traveler_Ptr sim_traveler_ptr;
	Sim_Veh_Ptr veh_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr;
	Connect_Data *connect_ptr;
	Veh_Type_Data *veh_type_ptr;

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
	if (lane >= sim_dir_ptr->Lanes ()) return (false);

	if (sim_dir_ptr->Method () != MESOSCOPIC || !sim_dir_ptr->Active ()) return (true);

#ifdef DEBUG_KEYS
if (!boundary_flag && partition != sim_dir_ptr->Partition (cell)) {
	exe->Write (1, "behind1=") << step.Vehicle () << " part=" << partition << " vs " << sim_dir_ptr->Partition (cell);
}
#endif

	//---- check the cell status ----

	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	vehicle = (*sim_lane_ptr) [cell];
	if (vehicle < -1) return (false);
	if (vehicle >= 0 && (num_cells <= 1 || vehicle == step.Vehicle ())) return (false);

	//---- check the use restrictions ----

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Cell_Out () && !use_flag) {
		if (!Cell_Use (sim_lane_ptr, lane, cell, step)) return (false);
	}

	//---- set the search and gap requirements ----

	min_gap = num_cells - 1;
	max_gap = num_cells + step.veh_type_ptr->Length ();
	speed = step.sim_veh_ptr->Speed ();

	for (min_step=0, max_step=1; max_gap > 0; max_step++) {
		speed += step.veh_type_ptr->Max_Accel ();
		if (speed > sim_dir_ptr->Speed ()) {
			speed = sim_dir_ptr->Speed ();
		}
		if (speed > step.veh_type_ptr->Max_Speed ()) {
			speed = step.veh_type_ptr->Max_Speed ();
		}
		max_gap -= speed;
		min_gap -= speed;
		if (min_gap > 0) min_step++;
	}

	//---- check the existing vehicle movements ----

	if (vehicle >= 0) {
		if (min_step < 1) return (false);

		veh_ptr = exe->sim_veh_array [vehicle];
		if (veh_ptr == 0 || veh_ptr->Speed () == 0) return (false);

		min_gap = ((int) veh_ptr->size () + veh_ptr->Speed () / 2) / veh_ptr->Speed ();
		if (exe->veh_status [vehicle] != 0) min_gap++;

		if (min_gap > min_step) return (false);
	}

	//---- search for upstream vehicles ----

	vehicle = -1;
	index = dir_index;
	link_flag = false;
	max_cells = max_step * sim_dir_ptr->Speed ();

	if (!boundary_flag && sim_dir_ptr->Boundary ()) {
		min_cell = sim_dir_ptr->Split_Cell ();
		if (cell == min_cell) return (true);
		if (cell < min_cell) min_cell = -1;
	} else {
		min_cell = -1;
	}

	for (gap=0; gap < max_cells; gap++) {

		//---- end of the link ----

		if (--cell < 0) {

			//---- get the primary input lane ----

			index = sim_lane_ptr->Thru_Link ();
			if (index < 0) break;

			link_flag = true;
			sim_dir_ptr = &exe->sim_dir_array [index];
			if (!sim_dir_ptr->Active ()) return (true);

			if (index == dir_index || sim_dir_ptr->Lanes () == 0 || sim_dir_ptr->Method () != MESOSCOPIC) return (true);

			sim_lane_ptr = sim_dir_ptr->Lane (sim_lane_ptr->Thru_Lane ());

			cell = sim_dir_ptr->Cells () - 1;

			if (!boundary_flag && sim_dir_ptr->Boundary ()) {
				min_cell = sim_dir_ptr->Split_Cell ();
				if (cell < min_cell) min_cell = -1;
			} else {
				min_cell = -1;
			}
		}
#ifdef DEBUG_KEYS
if (!boundary_flag && partition != sim_dir_ptr->Partition (cell)) {
	exe->Write (1, "behind2=") << step.Vehicle () << " part=" << partition << " vs " << sim_dir_ptr->Partition (cell);
}
#endif
		vehicle = (*sim_lane_ptr) [cell];

		if (vehicle != -1 || cell == min_cell) break;
	}
	if (vehicle < 0) return (true);

	//---- check for your own tail ----

	if (vehicle == step.Vehicle ()) return (true);

	veh_ptr = exe->sim_veh_array [vehicle];
	if (veh_ptr == 0) return (true);

	if (!veh_ptr->Front (index, lane, cell)) return (true);

	speed = veh_ptr->Speed ();

	if (gap > 0) {
		
		//---- check gap / speed relationships ----

		min_gap = gap;
		veh_type_ptr = &exe->sim_type_array [veh_ptr->Type ()];

		for (min_step=0; min_gap > 0; min_step++) {
			speed += veh_type_ptr->Max_Accel ();
			if (speed > sim_dir_ptr->Speed ()) {
				speed = sim_dir_ptr->Speed ();
			}
			if (speed > veh_type_ptr->Max_Speed ()) {
				speed = veh_type_ptr->Max_Speed ();
			}
			min_gap -= speed;
		}
		speed = veh_ptr->Speed ();
	} else {
		min_step = 0;
	}
	if (exe->veh_status [vehicle] != 0) min_step++;
	if (veh_ptr->Next_Event () > exe->step) {
		min_step += veh_ptr->Next_Event () - exe->step;
	}
	if (min_step > max_step) return (true);

	//---- priority vehicle check ----

	if (step.sim_veh_ptr->Priority () > 1) {
		return (speed == 0 || min_step > max_step / 2);
	}

	//---- non-priority vehicle check ---
	
	sim_traveler_ptr = exe->sim_traveler_array [veh_ptr->Driver ()];

	if ((speed == 0 && gap == 0) || min_step == max_step) {

		//---- relative priority check ----

		if (veh_ptr->Priority () != step.sim_veh_ptr->Priority ()) {
			return (veh_ptr->Priority () < step.sim_veh_ptr->Priority ());
		}

		//---- permissive probablity ----

		permission = param.permission_prob [sim_dir_ptr->Type ()];
		if (param.traveler_flag) permission *= param.traveler_fac.Best (sim_traveler_ptr->Type ());

		return (sim_traveler_ptr->random.Probability () <= permission);	
	}

	//---- check the link change ----

	if (link_flag) {

		//---- check the traveler's path ----

		plan_ptr = sim_traveler_ptr->plan_ptr;
		if (plan_ptr == 0) return (true);

		leg_itr = plan_ptr->begin ();
		if (leg_itr == plan_ptr->end () || leg_itr->Connect () < 0) return (true);

		connect_ptr = &exe->connect_array [leg_itr->Connect ()];
		if (connect_ptr->To_Index () != dir_index) return (true);

		//---- check for a traffic control ----

		if (cell <= sim_dir_ptr->Cell_Out ()) {
			control = connect_ptr->Control ();

			if (control == RED_LIGHT || control == STOP_SIGN || control == STOP_GREEN) {
				return (true);
			}
		}
	}
	return (false);
}
