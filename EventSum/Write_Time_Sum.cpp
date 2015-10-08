//*********************************************************
//	Write_Time_Sum.cpp - write the time summary file
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Time_Sum
//---------------------------------------------------------

void EventSum::Write_Time_Sum (void)
{
	int i, trips, num_out;
	Dtime low, high, diff, error, ttime, average, first, last;

	Time_Sum_Data *sum_ptr, total;

	Show_Message (String ("Writing %s -- Period") % time_sum_file.File_Type ());
	Set_Progress ();

	num_out = 0;
	first = last = 0;

	for (i=0; i <= num_inc; i++) {
		Show_Progress ();

		if (i < num_inc) {
			sum_ptr = &time_sum_array [i];

			total.Trip_Start (total.Trip_Start () + sum_ptr->Trip_Start ());
			total.Started (total.Started () + sum_ptr->Started ());
			total.Trip_End (total.Trip_End () + sum_ptr->Trip_End ());
			total.Ended (total.Ended () + sum_ptr->Ended ());
			total.Mid_Trip (total.Mid_Trip () + sum_ptr->Mid_Trip ());
			total.Start_Diff (total.Start_Diff () + sum_ptr->Start_Diff ());
			total.End_Diff (total.End_Diff () + sum_ptr->End_Diff ());
			total.Travel_Time (total.Travel_Time () + sum_ptr->Travel_Time ());
			total.Start_Error (total.Start_Error () + sum_ptr->Start_Error ());
			total.End_Error (total.End_Error () + sum_ptr->End_Error ());
			total.TTime_Diff (total.TTime_Diff () + sum_ptr->TTime_Diff ());
			total.TTime_Error (total.TTime_Error () + sum_ptr->TTime_Error ());
			total.Sum_Trips (total.Sum_Trips () + sum_ptr->Sum_Trips ());
			total.Sum_Error (total.Sum_Error () + sum_ptr->Sum_Error ());

			sum_periods.Period_Range (i, low, high);
			if (i == 0) first = low;
			last = high;
		} else {
			sum_ptr = &total;
			low = first;
			high = last;
		}
		time_sum_file.From_Time (low);
		time_sum_file.To_Time (high);

		time_sum_file.Trip_Start (sum_ptr->Trip_Start ());

		trips = sum_ptr->Started ();

		if (trips > 0) {
			diff = (Dtime) (sum_ptr->Start_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->Start_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		time_sum_file.Started (trips);
		time_sum_file.Start_Diff (diff.Round_Seconds ());
		time_sum_file.Start_Error (error.Round_Seconds ());

		time_sum_file.Trip_End (sum_ptr->Trip_End ());

		trips = sum_ptr->Ended ();

		if (trips > 0) {
			diff = (Dtime) (sum_ptr->End_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->End_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		time_sum_file.Ended (trips);
		time_sum_file.End_Diff (diff.Round_Seconds ());
		time_sum_file.End_Error (error.Round_Seconds ());

		trips = sum_ptr->Mid_Trip ();

		if (trips > 0) {
			ttime = (Dtime) (sum_ptr->Travel_Time () / trips + 0.5);
			diff = (Dtime) (sum_ptr->TTime_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->TTime_Error () / trips + 0.5);
			average = (Dtime) (sum_ptr->Sum_Error () / trips + 0.5);
		} else {
			ttime = diff = error = average = 0;
		}
		time_sum_file.Mid_Trip (trips);
		time_sum_file.Travel_Time (ttime.Round_Seconds ());
		time_sum_file.TTime_Diff (diff.Round_Seconds ());
		time_sum_file.TTime_Error (error.Round_Seconds ());

		trips = sum_ptr->Sum_Trips ();

		if (trips > 0) {
			error = (Dtime) (sum_ptr->Sum_Error () / trips + 0.5);
		} else {
			error = 0;
		}
		time_sum_file.Sum_Trips (trips);
		time_sum_file.Sum_Error (error.Round_Seconds ());
		time_sum_file.Avg_Error (average.Round_Seconds ());

		if (!time_sum_file.Write ()) {
			Error (String ("Writing %s") % time_sum_file.File_Type ());
		}
		num_out++;
	}
	End_Progress ();

	time_sum_file.Close ();
	Print (2, String ("Number of %s Records = %d") % time_sum_file.File_Type () % num_out);
}
