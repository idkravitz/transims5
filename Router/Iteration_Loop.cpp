//*********************************************************
//	Iteration_Loop.cpp - Interate Parition Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Iteration_Loop
//---------------------------------------------------------

void Router::Iteration_Loop (void)
{
	int i, p, num, num_car, max_hhold, veh;
	int last_hhold, skip_hhold, skip_person;
	
	Dtime second;
	bool old_flag, duration_flag, last_skip, last_flag, link_last_flag, trip_last_flag;
	bool last_plan_set_flag, last_new_plan_flag, last_problem_flag, last_skim_only;
	double gap;

	Trip_Data trip_data;
	Plan_Data *plan_ptr, *old_plan_ptr;
	Path_Parameters param;
	Vehicle_Itr vitr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Select_Map_Itr sel_itr;
	Trip_Index trip_index, old_trip_index;
	Trip_Gap_Data trip_gap_data;
	Trip_Gap_Map_Stat map_stat;

	Set_Parameters (param);

	num_car = veh = num = 0;
	last_hhold = skip_hhold = skip_person = -1;
	max_hhold = MAX_INTEGER;
	plan_ptr = old_plan_ptr = 0;
	old_flag = duration_flag = last_skip = false;
	last_plan_set_flag = plan_set_flag;
	last_new_plan_flag = new_plan_flag;
	last_problem_flag = problem_flag;
	last_skim_only = param.skim_only;

	second.Seconds (1);

	//---- backup parking locations ----

	if (max_iteration == 0) max_iteration = 1;

	if (max_iteration > 1) {
		veh_parking.assign (vehicle_array.size (), 0);

		for (p=0, vitr = vehicle_array.begin (); vitr != vehicle_array.end (); vitr++, p++) {
			veh_parking [p] = vitr->Parking ();
		}
	}

	//---- process each iteration ----

	for (i=1; i <= max_iteration; i++) {
		last_flag = (i == max_iteration);
		first_iteration = (i == 1);

		if (Master ()) {
			if (max_iteration > 1) {
				Write (2, "Iteration Number ") << i << ":  Weighting Factor = " << factor;
				if (!thread_flag) Show_Message (1);
			}
			if (trip_set_flag) Show_Message (1);

			if (link_gap_flag) {
				if (i == 1) {
					link_gap_file.File () << "ITERATION";
					int num = sum_periods.Num_Periods ();
					for (int j=0; j < num; j++) {
						link_gap_file.File () << "\t" << sum_periods.Range_Label (j);
					}
					link_gap_file.File () << "\tTOTAL" << endl;
				}
				link_gap_file.File () << i;
			}
			if (trip_gap_flag) {
				if (i == 1) {
					trip_gap_file.File () << "ITERATION";
					int num = sum_periods.Num_Periods ();
					for (int j=0; j < num; j++) {
						trip_gap_file.File () << "\t" << sum_periods.Range_Label (j);
					}
					trip_gap_file.File () << "\tTOTAL" << endl;
				}
				trip_gap_file.File () << i;
			}
		}
		total_records = 0;

		if (last_flag) {
			plan_set_flag = last_plan_set_flag;
			new_plan_flag = last_new_plan_flag;
			problem_flag = last_problem_flag;
			param.skim_only = last_skim_only;
		} else {
			plan_set_flag = new_plan_flag = problem_flag = false;
			param.skim_only = true;
		}

		//---- process each partition ----

		part_processor.Read_Trips ();

		link_last_flag = trip_last_flag = true;

		//---- calculate the link gap ----

		if (Time_Updates ()) {
			if (MPI_Size () > 1) {
				gap = MPI_Link_Delay (last_flag);
			} else {
				Update_Travel_Times ();
				gap = Merge_Delay (factor, !last_flag);
			}
			if (Master ()) {
				if (iteration_flag || plan_flag) {
					//Write (1, "Link Convergence Gap = ") << gap;
					if (!iteration_flag) Print (1);
					Print (1, "Link Convergence Gap = ") << gap;
					if (thread_flag || !iteration_flag) {
						Show_Message ("Link Convergence Gap = ") << gap;
					} else {
						Show_Message (0, "\tLink Convergence Gap = ") << gap;
					}
				}
				num_time_updates++;
			}
			if (link_gap > 0.0 && gap > link_gap) link_last_flag = false;

			factor += increment;
			if (factor > max_factor) factor = max_factor;
		}

		//---- calculate the trip gap ----

		if (trip_gap_map_flag) {
			gap = Get_Trip_Gap ();

			if (Master ()) {
				Write (1, "Trip Convergence Gap = ") << gap;
			}
			if (trip_gap > 0.0 && gap > trip_gap) trip_last_flag = false;
		}

		//---- convergence check ----

		if (!last_flag && link_last_flag && trip_last_flag) {
			max_iteration = i + 1;
		}

		//---- print the iteration problems ----

		if (!last_flag && max_iteration > 1 && Report_Flag (ITERATION_PROBLEMS)) {
#ifdef MPI_EXE
			int lvalue [MAX_PROBLEM + 1];

			if (Master ()) {
				for (int j=1; j < MPI_Size (); j++) {
					Get_MPI_Array (lvalue, MAX_PROBLEM + 1, j);

					Add_Problems (lvalue);

					total_records += lvalue [MAX_PROBLEM];
				}
				Report_Problems (total_records, false);
			} else {
				memcpy (lvalue, Get_Problems (), MAX_PROBLEM * sizeof (int));
				lvalue [MAX_PROBLEM] = total_records;

				Send_MPI_Array (lvalue, MAX_PROBLEM + 1);
			}
#else
			if (Master ()) {
				Report_Problems (total_records, false);
			}
#endif
		}

		//---- reset the parking locations ----

		if (!last_flag) {
			for (p=0, vitr = vehicle_array.begin (); vitr != vehicle_array.end (); vitr++, p++) {
				vitr->Parking (veh_parking [p]);
			}
			if (trip_flag) {
				if (trip_set_flag) {
					for (p=0; p < num_file_sets; p++) {
						trip_set [p]->Open (p);
						trip_set [p]->Reset_Counters ();
					}
				} else {
					trip_file->Open (0);
					trip_file->Reset_Counters ();
				}
			}
			Reset_Problems ();
		}
	}
}
