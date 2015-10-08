//*********************************************************
//	Look_Ahead.cpp - check cells ahead for lane change
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Look_Ahead
//---------------------------------------------------------

bool Sim_Method::Look_Ahead (Travel_Step &step)
{
	int index, lane, cell, sum_change, sum_stay, change;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;
	Cell_Data cell_rec;
	Cell_Itr cell_itr;

	if ((exe->step & one_second) == one_second) {
		change = 1;
	} else {
		change = -1;
	}
	cell_rec = step.sim_veh_ptr->front ();
	cell_rec.Lane_Change (change);

	//---- check the lane range ----

	lane = cell_rec.Lane ();
	if (lane < 0) return (false);

	index = cell_rec.Index ();
	cell = cell_rec.Cell ();

	sim_dir_ptr = &exe->sim_dir_array [index];

	if (lane >= sim_dir_ptr->Lanes ()) return (false);
	if (sim_dir_ptr->In_Cell () > cell || cell > sim_dir_ptr->Cell_Out ()) return (false);

	//---- check the lane occupancy and use type ----

	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	if ((*sim_lane_ptr) [cell] != -1) return (false);

	if (!Cell_Use (sim_lane_ptr, lane, cell, step)) return (false);

	//---- check vehicles behind ----

	if (!Check_Behind (cell_rec, step)) return (false);

	//---- sum the speeds and lane changes using the new lane ----

	sum_change = Sum_Path (cell_rec, step) - (int) param.lane_factor;

	//---- sum the speeds and lane changes using the current lane ----

	cell_rec.Lane_Change (-change);

	sum_stay = Sum_Path (cell_rec, step);

	//---- calculate the change probability ----

	if (sum_change <= sum_stay * 2) return (false);

	index = sum_stay + sum_change;
	sum_stay = DTOI (index * step.sim_traveler_ptr->random.Probability ());

	if (index >= 0) {
		if (sum_change < sum_stay) return (false);
	} else {
		if (sum_change > sum_stay) return (false);
	}

	//---- save the lane change ----

	cell_rec.Lane_Change (change);

	step.push_back (cell_rec);
	sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
	stats.num_look_ahead++;
	return (true);
}
