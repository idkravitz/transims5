//*********************************************************
//	Select_Travelers.cpp - select travelers
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Select_Travelers
//---------------------------------------------------------

void EventSum::Select_Travelers (void)
{
	int i, size;
	Dtime diff, time;
	double share;
	bool keep;

	Travel_Time_Itr time_itr;
	Travel_Time_Data *time_ptr;
	Select_Data select_data;
	Trip_Index trip_index;

	Int2_Set diff_map;
	Int2_Set_RItr diff_ritr;

	Show_Message ("Selecting Travelers -- Record");
	Set_Progress ();

	//---- select travel time differences ----

	for (i=0, time_itr = travel_time_array.begin (); time_itr != travel_time_array.end (); time_itr++, i++) {
		Show_Progress ();

		//---- check the time range ----

		if (time_itr->Base_Start () >= time_itr->Base_End () ||
			time_itr->Started () >= time_itr->Ended ()) continue;

		if (!sum_periods.Span_Range (time_itr->Base_Start (), time_itr->Base_End ()) &&
			!sum_periods.Span_Range (time_itr->Started (), time_itr->Ended ())) continue;

		time = time_itr->Base_Time ();
		diff = abs (time_itr->Travel_Time () - time);
		share = 1.0;
		keep = false;

		if (diff >= min_time) {
			if (diff < max_time) {
				if (time > 0) {
					share = (double) diff / (double) time;
				}
				if (share > percent_diff) keep = true;
			} else {
				keep = true;
			}
		}
		if (keep) {
			diff_map.insert (Int2_Key ((int) (share * 10000.0 + 0.5), i));
		}
	}
	End_Progress ();

	i = (int) diff_map.size ();
	size = (int) travel_time_array.size ();

	if (size > 0) {
		share = 100.0 * i / size;
	} else {
		share = 0.0;
		size = i;
	}
	Print (2, String ("Number of Trips Selected = %d (%.1lf%%)") % i % share);

	i = (int) (i * percent + 0.5);

	//---- check the maximum selection ----

	if (((double) i / size) > max_percent) {
		i = (int) (size * max_percent + 0.5);
	}
	share = 100.0 * i / size;

	Print (1, String ("Number of Trips Flagged = %d (%.1lf%%)") % i % share);

	//---- select the event records ----

	for (diff_ritr = diff_map.rbegin (); diff_ritr != diff_map.rend (); diff_ritr++) {
		time_ptr = &travel_time_array [diff_ritr->second];

		time_ptr->Select_Flag (true);

		if (new_select_flag) {
			time_ptr->Get_Trip_Index (trip_index);
			select_data.Partition (0);

			selected.insert (Select_Map_Data (trip_index, select_data));
		}
		if (--i == 0) break;
	}
}
