//*********************************************************
//	Set_Min_Time.cpp - set the minimum travel time
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Set_Min_Time
//---------------------------------------------------------

void LinkDelay::Set_Min_Time (void)
{
	int record;

	Flow_Time_Itr record_itr;
	Flow_Time_Data *record_ptr;
	Flow_Time_Period_Itr period_itr;

	Flow_Time_Array min_time;
	Flow_Time_Data flow_time;

	Show_Message ("Setting Minimum Travel Time -- Record");
	Set_Progress ();

	//---- allocate work space ----

	period_itr = link_delay_array.begin ();
	record = (int) period_itr->size ();

	if (record == 0) {
		record = (int) dir_array.size ();
	}
	min_time.assign (record, flow_time);

	//---- process each time period ----

	for (; period_itr != link_delay_array.end (); period_itr++) {

		//---- process each link direction ----

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			Show_Progress ();

			record_ptr = &min_time [record];

			if (record_ptr->Time () > record_itr->Time () || record_ptr->Time () == 0) {
				record_ptr->Time (record_itr->Time ());
			}
		}
	}

	//---- reset the period times ----

	for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {

		//---- process each link direction ----

		for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
			Show_Progress ();

			if (record_itr->Flow () < 0.1) {
				record_ptr = &min_time [record];
				record_itr->Time (record_ptr->Time ());
			}
		}
	}
	End_Progress ();
}
