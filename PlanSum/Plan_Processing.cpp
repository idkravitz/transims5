//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor
//---------------------------------------------------------

PlanSum::Plan_Processing::Plan_Processing (PlanSum *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);

	if (exe->new_delay_flag) {
		Link_Delay_File *file = (Link_Delay_File *) exe->System_File_Handle (NEW_LINK_DELAY);

		if (file != 0) {
			turn_flag = file->Turn_Flag ();
		} else {
			turn_flag = exe->System_File_Flag (CONNECTION);
		}
	}
	Plan_File *file = (Plan_File *) exe->System_File_Handle (PLAN);

	plan_file.File_Access (file->File_Access ());
	plan_file.Dbase_Format (file->Dbase_Format ());
	plan_file.Part_Flag (file->Part_Flag ());
	plan_file.Pathname (file->Pathname ());
	plan_file.First_Open (false);

	if (!thread_flag) return;

	//---- initialize link delay data ----

	if (exe->new_delay_flag) {
		link_delay_array.Initialize (&exe->time_periods);

		if (turn_flag) {
			int num = (int) exe->connect_array.size ();

			if (num > 0) {
				turn_delay_array.Initialize (&exe->time_periods, num);
			}
		}
	}
	
	//---- initialize time data ----

	if (exe->time_flag) {
		int periods = exe->sum_periods.Num_Periods ();

		start_time.assign (periods, 0);
		mid_time.assign (periods, 0);
		end_time.assign (periods, 0);
	}

	//---- transfer arrays ----

	if (exe->xfer_flag || exe->xfer_detail) {
		Integers rail;

		rail.assign (10, 0);
		total_on_array.assign (10, rail);

		if (exe->xfer_detail) {
			int num = exe->sum_periods.Num_Periods ();
			if (num < 1) num = 1;

			walk_on_array.assign (num, total_on_array);
			drive_on_array.assign (num, total_on_array);
		}
	}

	//---- initialize summary report data ----

	if (exe->travel_flag) {
		trip_sum_data.Replicate (exe->trip_sum_data);
	}
	if (exe->passenger_flag) {
		pass_sum_data.Replicate (exe->pass_sum_data);
	}

	//---- initialize transit summaries ----

	if (exe->transfer_flag) {
		transfer_array.Replicate (exe->transfer_array);
	}
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void PlanSum::Plan_Processing::operator()()
{
	int part = 0;
	while (exe->partition_queue.Get (part)) {
		Read_Plans (part);
	}
	MAIN_LOCK
	Plan_File *file = (Plan_File *) exe->System_File_Handle (PLAN);	
	file->Add_Counters (&plan_file);
	
	if (thread_flag) {

		//---- combine the link flow data ----

		if (exe->new_delay_flag) {
			exe->link_delay_array.Combine_Flows (link_delay_array, true);

			if (turn_flag) {
				exe->turn_delay_array.Combine_Flows (turn_delay_array, true);
			}
		}

		//---- combine the trip time data ----

		if (exe->time_flag) {
			int i, periods;

			periods = exe->sum_periods.Num_Periods ();

			for (i=0; i < periods; i++) {
				exe->start_time [i] += start_time [i];
				exe->mid_time [i] += mid_time [i];
				exe->end_time [i] += end_time [i];
			}
		}

		//---- transfer arrays ----

		if (exe->xfer_flag || exe->xfer_detail) {
			int i, j, p;
			Ints_Itr ints_itr;
			Int_Itr int_itr;
			Board_Itr on_itr;

			for (i=0, ints_itr = total_on_array.begin (); ints_itr != total_on_array.end (); ints_itr, i++) {
				for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
					exe->total_on_array [i] [j] += *int_itr;
				}
			}
			if (exe->xfer_detail) {
				for (p=0, on_itr = walk_on_array.begin (); on_itr != walk_on_array.end (); on_itr++, p++) {
					for (i=0, ints_itr = on_itr->begin (); ints_itr != on_itr->end (); ints_itr, i++) {
						for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
							exe->walk_on_array [p] [i] [j] += *int_itr;
						}
					}
				}
				for (p=0, on_itr = drive_on_array.begin (); on_itr != drive_on_array.end (); on_itr++, p++) {
					for (i=0, ints_itr = on_itr->begin (); ints_itr != on_itr->end (); ints_itr, i++) {
						for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
							exe->drive_on_array [p] [i] [j] += *int_itr;
						}
					}
				}
			}
		}

		//---- combine summary report data ----

		if (exe->travel_flag) {
			exe->trip_sum_data.Merge_Data (trip_sum_data);
		}
		if (exe->passenger_flag) {
			exe->pass_sum_data.Merge_Data (pass_sum_data);
		}
		if (exe->transfer_flag) {
			exe->transfer_array.Merge_Data (transfer_array);
		}
	}
	END_LOCK
}
