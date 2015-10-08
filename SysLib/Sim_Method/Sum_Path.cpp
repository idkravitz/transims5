//*********************************************************
//	Sum_Path.cpp - count vehicles and lane changes on path
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Sum_Path
//---------------------------------------------------------

int Sim_Method::Sum_Path (Cell_Data cell_rec, Travel_Step &step)
{
	int cells, num_cells, sum_speed, num_lanes, num_veh, best_weight, weight, out_lane;
	int speed, first_cell, end_cell, max_cell;
	int dir_index, cell, lane, vehicle, from_lane, index;

	Dir_Data *dir_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg;
	Sim_Veh_Ptr veh_ptr;
	Connect_Data *connect_ptr;
	Sim_Connection *sim_con_ptr;
	Lane_Map_Itr map_itr;
	Sim_Park_Ptr sim_park_ptr;

	//---- initial conditions ----

	num_cells = param.look_ahead;
	sum_speed = num_lanes = num_veh = 0;

	dir_index = cell_rec.Index ();
	lane = cell_rec.Lane ();
	cell = cell_rec.Cell ();

	sim_dir_ptr = &exe->sim_dir_array [dir_index];
	if (!sim_dir_ptr->Active ()) return (0);

	first_cell = sim_dir_ptr->In_Cell ();

	sim_lane_ptr = sim_dir_ptr->Lane (lane);

	speed = step.sim_veh_ptr->Speed ();

	//---- get the plan data ----

	plan_ptr = step.sim_traveler_ptr->plan_ptr;
	if (plan_ptr == 0) return (0);

	leg_itr = plan_ptr->begin ();
	if (leg_itr == plan_ptr->end ()) return (0);

	max_cell = sim_dir_ptr->Cells ();
	end_cell = max_cell - 1;
	if (!boundary_flag && sim_dir_ptr->Boundary ()) max_cell = sim_dir_ptr->Split_Cell ();

	if (leg_itr->Connect () < 0) {
		next_leg = leg_itr + 1;
		if (next_leg != plan_ptr->end () && next_leg->Type () == PARKING_ID) {
			sim_park_ptr = &exe->sim_park_array [next_leg->Index ()];
			if (sim_dir_ptr->Dir () == 0) {
				end_cell = sim_park_ptr->Cell_AB ();
			} else {
				end_cell = sim_park_ptr->Cell_BA ();
			}
		}
	}
	connect_ptr = 0;

	for (cells=1; cells <= num_cells; cells++) {

		if (++cell == max_cell) break;

		//---- move forward ----

		if (cell > end_cell) {

			//---- set the exit speed ----

			if (leg_itr->Max_Speed () < speed) {
				speed = leg_itr->Max_Speed ();
			}
		
			//---- check the exit lane ----

			if (lane < leg_itr->Out_Lane_Low ()) {
				num_lanes += leg_itr->Out_Lane_Low () - lane;
				lane = leg_itr->Out_Lane_Low ();
				speed = 0;
			} else if (lane > leg_itr->Out_Lane_High ()) {
				num_lanes += lane - leg_itr->Out_Lane_High ();
				lane = leg_itr->Out_Lane_High ();
				speed = 0;
			}
			from_lane = lane;

			if (connect_ptr == 0) break;

			dir_index = connect_ptr->To_Index ();

			sim_dir_ptr = &exe->sim_dir_array [dir_index];

			if (sim_dir_ptr->Method () != MESOSCOPIC || !sim_dir_ptr->Active ()) break;

			end_cell = sim_dir_ptr->Cells ();

			index = leg_itr->Connect ();
			if (index < 0) break;

			if (++leg_itr == plan_ptr->end ()) break;

			//---- find the best connection ----

			sim_con_ptr = &exe->sim_connection [index];

			lane = -1;
			best_weight = 0;

			for (map_itr = sim_con_ptr->begin (); map_itr != sim_con_ptr->end (); map_itr++) {
				if (map_itr->In_Lane () != from_lane) continue;

				out_lane = map_itr->Out_Lane ();

				sim_lane_ptr = sim_dir_ptr->Lane (out_lane);
				if (sim_lane_ptr->at (0) != -1) continue;

				weight = map_itr->In_Thru () + map_itr->Out_Thru ();

				if (out_lane >= leg_itr->In_Best_Low () && out_lane <= leg_itr->In_Best_High ()) {
					weight += param.connect_lane_weight;
				}
				if (Cell_Use (sim_lane_ptr, out_lane, first_cell, step)) {
					weight += param.lane_use_weight;
				}
				if (weight > best_weight) {
					lane = out_lane;
					best_weight = weight;
				}
			}
			if (lane < 0) break;
			cell = 0;

			sim_lane_ptr = sim_dir_ptr->Lane (lane);

			max_cell = sim_dir_ptr->Cells ();
			end_cell = max_cell - 1;
			if (!boundary_flag && sim_dir_ptr->Boundary ()) max_cell = sim_dir_ptr->Split_Cell ();

			if (leg_itr->Connect () < 0) {
				next_leg = leg_itr + 1;
				if (next_leg != plan_ptr->end () && next_leg->Type () == PARKING_ID) {
					sim_park_ptr = &exe->sim_park_array [next_leg->Index ()];
					if (sim_dir_ptr->Dir () == 0) {
						end_cell = sim_park_ptr->Cell_AB ();
					} else {
						end_cell = sim_park_ptr->Cell_BA ();
					}
				}
			}
		}

		//---- check the use restrictions ----

		if (!Cell_Use (sim_lane_ptr, lane, cell, step)) {
			if (lane > 0) {
				while (lane > 0) {
					num_lanes++;
					sim_lane_ptr = sim_dir_ptr->Lane (--lane);
					if (Cell_Use (sim_lane_ptr, lane, cell, step)) break;
				}
			} else {
				while (lane < (sim_dir_ptr->Lanes () - 1)) {
					num_lanes++;
					sim_lane_ptr = sim_dir_ptr->Lane (++lane);
					if (Cell_Use (sim_lane_ptr, lane, cell, step)) break;
				}
			}
			speed = 0;

		} else {

			//---- check the cell availability -----

			vehicle = sim_lane_ptr->at (cell);

			if (vehicle == -2) {

				//---- pocket lane ----

				dir_ptr = &exe->dir_array [dir_index];

				if (lane <= dir_ptr->Left ()) {
					while (lane <= dir_ptr->Left ()) {
						num_lanes++;
						sim_lane_ptr = sim_dir_ptr->Lane (++lane);
						if ((*sim_lane_ptr) [cell] >= -1) break;
					}
				} else {
					while (lane >= (dir_ptr->Left () + dir_ptr->Lanes ())) {
						num_lanes++;
						sim_lane_ptr = sim_dir_ptr->Lane (--lane);
						if ((*sim_lane_ptr) [cell] >= -1) break;
					}
				}
				speed = 0;

			} else {

				//---- accelerate ----

				speed += step.veh_type_ptr->Max_Accel ();

				if (speed > step.veh_type_ptr->Max_Speed ()) {
					speed = step.veh_type_ptr->Max_Speed ();
				}
				if (speed > sim_dir_ptr->Speed ()) {
					speed = sim_dir_ptr->Speed ();
				}

				//---- check the vehicle speed ----

				if (vehicle >= 0) {
					veh_ptr = exe->sim_veh_array [vehicle];

					if (veh_ptr != 0 && veh_ptr->Speed () < speed) {
						speed = veh_ptr->Speed ();
					}
					num_veh++;
				}
			}
		}
		sum_speed += speed - step.Delay ();
	}
	sum_speed = DTOI (sum_speed * param.time_factor - num_lanes * param.lane_factor - num_veh * param.veh_factor);

	return (sum_speed);	
}
