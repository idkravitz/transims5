//*********************************************************
//	Array_Processing.cpp - process an array of plans
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Array_Processing
//---------------------------------------------------------

bool Path_Builder::Array_Processing (Plan_Ptr_Array *array_ptr)
{
	Plan_Data *plan_ptr, *prev_ptr;
	Plan_Ptr_Itr itr;
	int last_person, last_tour, last_time;
	bool duration_flag, problem_flag, first;
	Dtime minute;

	duration_flag = problem_flag = false;
	last_person = last_tour = last_time = -1;
	minute.Minutes (1.0);

	//---- process each trip in the household ----

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		plan_ptr = *itr;
		min_time_limit = 0;

		if (plan_ptr->Person () == last_person) {
			first = false;
			if (problem_flag) {
				exe->Update_Parking (*plan_ptr);
			}
			if (last_time >= plan_ptr->Depart () ||
				(last_tour == plan_ptr->Tour () && duration_flag && 
				plan_ptr->Constraint () != FIXED_TIME &&
				plan_ptr->Constraint () != START_TIME)) {

				if (plan_ptr->Method () == COPY_PATH || plan_ptr->Method () == PATH_FLOWS) {
					plan_ptr->Arrive (plan_ptr->Arrive () - plan_ptr->Depart () + last_time);
				}
				if (!exe->param.ignore_time) {
					if (plan_ptr->Constraint () == START_TIME || plan_ptr->Constraint () == FIXED_TIME) {
						if ((last_time - plan_ptr->Depart ()) > param.end_time) {
							plan_ptr->Problem (TIME_PROBLEM);
							goto skip;
						}
					} else if (plan_ptr->Constraint () == END_TIME && last_time > 0) {
						min_time_limit = last_time;
					}
				}
				plan_ptr->Depart (last_time);
			}
			last_tour = plan_ptr->Tour ();
		} else {
			last_person = plan_ptr->Person ();
			last_tour = plan_ptr->Tour ();
			first = true;
		}

		//---- process the plan ----

		switch (plan_ptr->Method ()) {
			case BUILD_PATH:
				if (!Plan_Build (plan_ptr)) return (false);
				break;
			case RESKIM_PATH:
				if (!Plan_Update (plan_ptr)) return (false);
				break;
			case REROUTE_PATH:
				if (!Plan_ReRoute (plan_ptr)) return (false);
				break;
			case PATH_FLOWS:
				if (!Plan_Flow (plan_ptr)) return (false);
				break;
			case COPY_PATH:
			default:
				break;
		}

		//---- check for a problem ----

skip:
		if (plan_ptr->Problem () > 0 && exe->param.ignore_errors) {
			last_time = -1;
			duration_flag = false;
			problem_flag = true;
		} else {

			//---- gather the end time ----

			last_time = plan_ptr->Arrive ();
			duration_flag = (plan_ptr->Activity () > 0);
			problem_flag = false;

			//---- adjust the duration of this activity ----

			if (duration_flag) {
				if (last_time < plan_ptr->End ()) {

					//---- earlier than expected ----

					if (!param.adjust_schedule || 
						(!param.ignore_duration && plan_ptr->Constraint () == FIXED_TIME)) {
						last_time = plan_ptr->End ();
					}
				} else if (last_time > plan_ptr->End ()) {

					//---- later than expected ----

					if (param.ignore_duration) {
						plan_ptr->Activity (plan_ptr->End () + plan_ptr->Duration () - last_time);
						if (plan_ptr->Activity () < minute) plan_ptr->Activity (minute);
					} 
				}
			}
			last_time += plan_ptr->Activity ();

			//---- adjust duration of previous trip ----

			if (!first && plan_ptr->Constraint () == END_TIME) {
				prev_ptr = *(itr - 1);
				if (prev_ptr->Constraint () != DURATION || param.ignore_duration) {
					time_limit = prev_ptr->Arrive () + prev_ptr->Activity ();
					if (time_limit > plan_ptr->Depart ()) {
						time_limit = prev_ptr->Activity () + plan_ptr->Depart () - time_limit;
						if (time_limit < 0) {
							exe->Set_Problem (DURATION_PROBLEM);
							time_limit = 0;
						}
						prev_ptr->Activity (time_limit);
					}
				}
			}
		}
	}
	return (true);
}
