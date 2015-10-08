//*********************************************************
//	Adjust_Trips.cpp - adjust trip schedules
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Adjust_Trips
//---------------------------------------------------------

void ConvertTrips::Adjust_Trips (void)
{
	int t, num, trip;
	Dtime time, diff, diff1, diff2;

	Trip_Itr trip_itr, next_itr;	
	File_Group *group_ptr;
	Share_Data *share_ptr;
	Problem_Type problem_type;
	Integers count;

	//---- check duration constraints ----

	for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
		if (trip_itr->Constraint () != DURATION) continue;

		next_itr = trip_itr + 1;

		if (next_itr != trip_array.end () && next_itr->Person () == trip_itr->Person ()) {
			time = next_itr->Start () - trip_itr->End ();
			
			if (time < trip_itr->Duration ()) {
				time = trip_itr->End () + trip_itr->Duration () - next_itr->Start ();

				//---- try reducing travel time ----

				diff1 = (trip_itr->End () - trip_itr->Start ()) * 0.1;
				if (diff1 < additional_time) diff1 = additional_time;

				if (next_itr->Constraint () != START_TIME && next_itr->Constraint () != FIXED_TIME) {
					diff2 = (next_itr->End () - next_itr->Start ()) * 0.1;
					if (diff2 < additional_time) diff2 = additional_time;
				} else {
					diff2 = 0;
				}
				diff = diff1 + diff2;

				if (time <= diff) {
					diff1 = time * diff1 / diff;
					diff2 = time * diff2 / diff;
					time = 0;
				} else {
					time = time - diff;
				}
				trip_itr->End (trip_itr->End () - diff1);
				next_itr->Start (next_itr->Start () + diff2);

				//---- push the trips apart ----

				if (time > 0) {
					if (diff2 > 0) {
						time = time / 2;
	
						next_itr->Start (next_itr->Start () + time);
						next_itr->End (next_itr->End () + time);
					}
					trip_itr->Start (trip_itr->Start () - time);
					trip_itr->End (trip_itr->End () - time);

					if (trip_itr->Start () < 0) trip_itr->Start (0);
				}

			} else if (time > trip_itr->Duration ()) {
				if (next_itr->Constraint () == NO_CONSTRAINT && next_itr->Constraint () == DURATION) {
					time = next_itr->End () - next_itr->Start ();
					next_itr->Start (trip_itr->End () + trip_itr->Duration ());
					next_itr->End (next_itr->Start () + time);
				} else {
					time = trip_itr->Duration () - time;
					trip_itr->Start (trip_itr->Start () + time);
					trip_itr->End (trip_itr->End () + time);
				}
			}
		}
	}

	//---- check for negative durations ----

	trip = 0;

	for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
		count.push_back (0);

		next_itr = trip_itr + 1;

		if (next_itr != trip_array.end () && next_itr->Person () == trip_itr->Person ()) {
			time = next_itr->Start () - trip_itr->End ();
			if (trip_itr->Constraint () != DURATION) {
				trip_itr->Duration (time);
				if (time < minute) {
					trip = 1;
				}
			} else if (time < trip_itr->Duration ()) {
				trip = 1;
			}
		}
	}
	if (trip == 0) goto finish;

	//---- search for scheduling conflicts ----

	for (num=0; num < 100; num++) {
		trip = -1;
		time = 1;

		for (t=0, trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++, t++) {
			next_itr = trip_itr + 1;

			if (next_itr != trip_array.end () && next_itr->Person () == trip_itr->Person ()) {
				diff = trip_itr->End () + minute - next_itr->Start ();
				if (diff > time) {
					time = diff;
					trip = t;
				}
			}
		}
		if (trip < 0) break;

		count [trip]++;

		trip_itr = trip_array.begin () + trip;
		next_itr = trip_itr + 1;

		if (count [trip] < 5) {

			//---- try reducing travel time ----

			if (trip_itr->Constraint () != END_TIME && trip_itr->Constraint () != FIXED_TIME) {
				diff1 = (trip_itr->End () - trip_itr->Start ()) * 0.1;
				if (diff1 > two_minutes) diff1 = two_minutes;
			} else {
				diff1 = 0;
			}
			if (next_itr->Constraint () != START_TIME && next_itr->Constraint () != FIXED_TIME && count [trip+1] < 5) {
				count [trip+1]++;
				diff2 = (next_itr->End () - next_itr->Start ()) * 0.1;
				if (diff2 > two_minutes) diff2 = two_minutes;
			} else {
				diff2 = 0;
			}
			diff = diff1 + diff2;

			if (diff > 0) {
				if (time <= diff) {
					diff1 = time * diff1 / diff;
					diff2 = time * diff2 / diff;
					time = 0;
				} else {
					time = time - diff;
				}
				trip_itr->End (trip_itr->End () - diff1);
				next_itr->Start (next_itr->Start () + diff2);

				if (next_itr->Start () > max_start) next_itr->Start (max_start);

				//---- push the trips apart ----

				if (time > 0) {
					if (diff1 > 0 && diff2 > 0) {
						time = time / 2;
						if (time > ten_minutes) time = ten_minutes;
					}
					if (diff1 > 0) {
						trip_itr->Start (trip_itr->Start () - time);
						trip_itr->End (trip_itr->End () - time);

						if (trip_itr->Start () < 0) trip_itr->Start (0);
					}
					if (diff2 > 0) {
						next_itr->Start (next_itr->Start () + time);
						next_itr->End (next_itr->End () + time);
			
						if (next_itr->Start () > max_start) next_itr->Start (max_start);
					}
				}
			}
		}

		//---- adjust fixed constraints ----

		diff = trip_itr->End () + minute - next_itr->Start ();

		if (diff > 0) {
			diff = diff / 2;
			if (diff > five_minutes) diff = five_minutes;

			trip_itr->Start (trip_itr->Start () - diff);
			trip_itr->End (trip_itr->End () - diff);
			next_itr->Start (next_itr->Start () + diff);
			next_itr->End (next_itr->End () + diff);

			if (trip_itr->Start () < 0) trip_itr->Start (0);
			if (next_itr->Start () > max_start) next_itr->Start (max_start);
		}
	}

	//---- set problem flags ----

	for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
		next_itr = trip_itr + 1;

		if (next_itr != trip_array.end () && next_itr->Person () == trip_itr->Person ()) {
			if (trip_itr->End () >= next_itr->Start () || trip_itr->Start () >= trip_itr->End () || 
				trip_itr->End () > max_start || trip_itr->Start () < 0) {
				trip_itr->Partition (-TIME_PROBLEM);
			}
		}
	}

	//---- process the results ----

finish:
	for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
		for (next_itr = trip_itr + 1; next_itr != trip_array.end (); next_itr++) {
			if (next_itr->Partition () >= 0) break;
		}
		if (trip_itr->Partition () < 0) {
			problem_type = (Problem_Type) -trip_itr->Partition ();

			Set_Problem (problem_type);
			tot_errors++;

			if (problem_flag) {
				problem_file->Problem (problem_type);
				problem_file->Household (trip_itr->Household ());
				problem_file->Person (trip_itr->Person ());
				problem_file->Tour (trip_itr->Tour ());
				problem_file->Trip (trip_itr->Trip ());
				problem_file->Start (trip_itr->Start ().Round_Seconds ());
				problem_file->End (trip_itr->End ().Round_Seconds ());
				problem_file->Duration (trip_itr->Duration ().Round_Seconds ());
				problem_file->Origin (trip_itr->Origin ());
				problem_file->Destination (trip_itr->Destination ());
				problem_file->Purpose (trip_itr->Purpose ());
				problem_file->Mode (trip_itr->Mode ());
				problem_file->Constraint (trip_itr->Constraint ());
				problem_file->Priority (trip_itr->Priority ());
				problem_file->Vehicle (trip_itr->Vehicle ());
				problem_file->Passengers (trip_itr->Passengers ());
				problem_file->Type (trip_itr->Type ());
				problem_file->Notes ((char *) Problem_Code (problem_type));

				if (!problem_file->Write ()) {
					Warning ("Writing ") << problem_file->File_Type ();
					problem_flag = false;
				}
				problem_file->Add_Trip (trip_itr->Household (), trip_itr->Person (), trip_itr->Tour ());
			}
			continue;
		}

		//---- set the duration ----

		if (trip_itr->Constraint () != DURATION && next_itr != trip_array.end () && next_itr->Person () == trip_itr->Person ()) {
			time = next_itr->Start () - trip_itr->End ();
			trip_file->Duration (time.Round_Seconds ());
		}

		//---- save the diurnal result ----

		if (trip_itr->Type () < 0) continue;

		group_ptr = &file_group [group_index [trip_itr->Type ()]];

		if (group_ptr->Time_Point () == TRIP_START) {
			time = trip_itr->Start ();
		} else if (group_ptr->Time_Point () == TRIP_END) {
			time = trip_itr->End ();
		} else {
			time = (trip_itr->Start () + trip_itr->End ()) / 2;
		}
		t = (int) time.Minutes ();
		if (t >= group_ptr->Diurnal_Periods ()) {
			t = group_ptr->Diurnal_Periods () - 1;
		}
		if (t < 0) t = 0;

		num = trip_itr->Partition ();
		if (num >= group_ptr->Num_Shares ()) {
			num = group_ptr->Num_Shares () - 1;
		}
		if (num < 0) num = 0;

		share_ptr = group_ptr->Share_Ptr (t, num);
		share_ptr->Add_Trips ();
	}
}
