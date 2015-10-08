//*********************************************************
//	Check_Cell.cpp - Check Cell access
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Check_Cell
//---------------------------------------------------------

bool Sim_Method::Check_Cell (Cell_Data cell_rec, Travel_Step &step)
{
	int lane, cell;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;

	//---- check the lane range ----

	lane = cell_rec.Lane ();
	if (lane < 0) return (false);

	sim_dir_ptr = &exe->sim_dir_array [cell_rec.Index ()];

	if (lane >= sim_dir_ptr->Lanes ()) return (false);

	//---- check the cell status ----

	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	cell = cell_rec.Cell ();

	if ((*sim_lane_ptr) [cell] == -2) return (false);

	//---- check the use restrictions ----

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Cell_Out ()) {
		if (!Cell_Use (sim_lane_ptr, lane, cell, step)) return (false);
	}
	return (true);
}
