//*********************************************************
//	Smooth_Delay.cpp - smooth the link delay data
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Smooth_Delay
//---------------------------------------------------------

void LinkDelay::Smooth_Delay (void)
{
	int record, period, num_periods;

	Flow_Time_Itr record_itr;
	Flow_Time_Data *record_ptr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Array *period_ptr;

	num_periods = (int) link_delay_array.size ();
	if (num_periods < 3) return;

	Show_Message ("Smoothing Link Delay Data -- Record");
	Set_Progress ();

	//---- process each link direction ----

	period_itr = link_delay_array.begin (); 

	for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
		Show_Progress ();

		//---- smooth the flow ----

		smooth_data.Input (0, record_itr->Flow ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &link_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			smooth_data.Input (period, (double) record_ptr->Flow ());
		}
		smooth_data.Smooth ();

		record_itr->Flow (smooth_data.Output (0));

		for (period=1; period < num_periods; period++) {
			period_ptr = &link_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			record_ptr->Flow (smooth_data.Output (period));
		}

		//---- smooth the travel time ----

		smooth_data.Input (0, (double) record_itr->Time ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &link_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			smooth_data.Input (period, (double) record_ptr->Time ());
		}
		smooth_data.Smooth ();

		record_itr->Time (Dtime (smooth_data.Output (0)));

		for (period=1; period < num_periods; period++) {
			period_ptr = &link_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			record_ptr->Time (Dtime (smooth_data.Output (period)));
		}
	}
	if (!turn_flag) {
		End_Progress ();
		return;
	}

	//---- process each turning movement ----

	period_itr = turn_delay_array.begin (); 

	if (period_itr == turn_delay_array.end ()) {
		End_Progress ();
		return;
	}

	for (record=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, record++) {
		Show_Progress ();

		//---- smooth the flow ----

		smooth_data.Input (0, record_itr->Flow ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			smooth_data.Input (period, (double) record_ptr->Flow ());
		}
		smooth_data.Smooth ();

		record_itr->Flow (smooth_data.Output (0));

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			record_ptr->Flow (smooth_data.Output (period));
		}

		//---- smooth the travel time ----

		smooth_data.Input (0, (double) record_itr->Time ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			smooth_data.Input (period, (double) record_ptr->Time ());
		}
		smooth_data.Smooth ();

		record_itr->Time (Dtime (smooth_data.Output (0)));

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_delay_array [period];
			record_ptr = period_ptr->Data_Ptr (record);

			record_ptr->Time (Dtime (smooth_data.Output (period)));
		}
	}
	End_Progress ();
}
