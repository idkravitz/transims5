//*********************************************************
//	Lane_Change.cpp - attempt a lane change 
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Lane_Change
//---------------------------------------------------------

bool Sim_Method::Lane_Change (Travel_Step &step)
{
	int index, cell, lane, new_lane, vehicle, length, speed, change;
	bool replace_flag;

	Sim_Veh_Ptr veh_ptr;
	Cell_Data cell_rec;
	Cell_Itr cell_itr;
	Sim_Dir_Ptr sim_dir_ptr = 0;
	Sim_Lane_Ptr sim_lane_ptr, new_lane_ptr;
	Veh_Type_Data *veh_type_ptr;

	//---- get the current location ----

	if (step.size () > 0) {
		cell_rec = step.back ();
		replace_flag = true;
	} else {
		cell_rec = step.sim_veh_ptr->front ();
		replace_flag = false;
	}
	lane = cell_rec.Lane ();
	cell = cell_rec.Cell ();

	sim_dir_ptr = &exe->sim_dir_array [cell_rec.Index ()];

	//---- simple lane change ----

	change = step.sim_veh_ptr->Change ();
	cell_rec.Lane_Change (change);
	
	if (Check_Behind (cell_rec, step)) {
		step.push_back (cell_rec);
		sim_dir_ptr->Cell (cell_rec, step.Vehicle ());
		step.sim_veh_ptr->Change (0);
		stats.num_change++;
		return (true);
	}
	speed = step.sim_veh_ptr->Speed ();
	if (speed > param.max_swap_speed) goto exit;

	//---- check for lane swap ----

	new_lane = cell_rec.Lane ();
	if (new_lane < 0) goto exit;

	index = cell_rec.Index ();

	sim_dir_ptr = &exe->sim_dir_array [index];
	if (new_lane >= sim_dir_ptr->Lanes ()) goto exit;

	new_lane_ptr = sim_dir_ptr->Lane (new_lane);

	vehicle = (*new_lane_ptr) [cell];
	if (vehicle < 0 || vehicle == step.Vehicle ()) goto exit;

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Cell_Out ()) {
		if (!Cell_Use (new_lane_ptr, new_lane, cell, step)) {
			goto exit;
		}
	}
	length = step.veh_type_ptr->Length ();

	//---- check the vehicle in the new lane ----

	veh_ptr = exe->sim_veh_array [vehicle];
	if (veh_ptr == 0) goto exit;

	if (veh_ptr->Change () != -change) {
		if (veh_ptr->Change () != 0 || step.sim_veh_ptr->Priority () < 2) goto exit;
	
		Sim_Traveler_Ptr sim_traveler_ptr = exe->sim_traveler_array [veh_ptr->Driver ()];
		if (sim_traveler_ptr == 0) goto exit;

		Sim_Plan_Ptr plan_ptr = sim_traveler_ptr->plan_ptr;
		if (plan_ptr == 0) goto exit;

		Sim_Leg_Itr leg_itr = plan_ptr->begin ();
		if (leg_itr == plan_ptr->end ()) goto exit;

		if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) goto exit;
	}
	if (veh_ptr->Speed () > param.max_swap_speed) goto exit;
	if (abs (speed - veh_ptr->Speed ()) > param.max_swap_diff) goto exit;

	if (!veh_ptr->Front (index, new_lane, cell)) goto exit;

	//---- check the vehicle size and use permissions ----

	veh_type_ptr = &exe->sim_type_array [veh_ptr->Type ()];

	if (length > 1 && veh_type_ptr->Length () > 1) goto exit;

	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Cell_Out ()) {
		if (!Cell_Use (sim_lane_ptr, lane, cell, step)) {
			goto exit;
		}
	}

	//---- swap the vehicles ----

	(*new_lane_ptr) [cell] = step.Vehicle ();
	if (replace_flag) {
		step.back ().Lane_Change (change);
	} else {
		step.sim_veh_ptr->Set_Front (index, new_lane, cell);
	}
	step.sim_veh_ptr->Next_Event (0);
	step.sim_veh_ptr->Change (0);

	(*sim_lane_ptr) [cell] = vehicle;
	veh_ptr->Set_Front (index, lane, cell);
	veh_ptr->Next_Event (0);
	veh_ptr->Change (0);
	stats.num_swap++;
	return (true);

exit:
	return (false);
}
