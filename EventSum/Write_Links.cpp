//*********************************************************
//	Write_Links.cpp - Create Link Event File
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Write_Links
//---------------------------------------------------------

void EventSum::Write_Links (void)
{
	int trips;
	Dtime low, high, diff, error, ttime, average;

	Link_Time_Map_Itr map_itr;
	Link_Time_Key key;
	Time_Sum_Data *sum_ptr;

	Show_Message (String ("Writing %s -- Record") % link_event_file.File_Type ());
	Set_Progress ();

	//---- write each link ----

	for (map_itr = link_time_map.begin (); map_itr != link_time_map.end (); map_itr++) {
		Show_Progress ();

		key = map_itr->first;
		sum_ptr = &map_itr->second;

		//---- write the link ids ----

		link_event_file.Link (key.Link ());

		sum_periods.Period_Range (key.Period (), low, high);

		link_event_file.From_Time (low);
		link_event_file.To_Time (high);

		link_event_file.Trip_Start (sum_ptr->Trip_Start ());

		trips = sum_ptr->Started ();

		if (trips > 0) {
			diff = (Dtime) (sum_ptr->Start_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->Start_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		link_event_file.Started (trips);
		link_event_file.Start_Diff (diff.Round_Seconds ());
		link_event_file.Start_Error (error.Round_Seconds ());

		link_event_file.Trip_End (sum_ptr->Trip_End ());

		trips = sum_ptr->Ended ();

		if (trips > 0) {
			diff = (Dtime) (sum_ptr->End_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->End_Error () / trips + 0.5);
		} else {
			diff = error = 0;
		}
		link_event_file.Ended (trips);
		link_event_file.End_Diff (diff.Round_Seconds ());
		link_event_file.End_Error (error.Round_Seconds ());

		trips = sum_ptr->Mid_Trip ();

		if (trips > 0) {
			ttime = (Dtime) (sum_ptr->Travel_Time () / trips + 0.5);
			diff = (Dtime) (sum_ptr->TTime_Diff () / trips + 0.5);
			error = (Dtime) (sum_ptr->TTime_Error () / trips + 0.5);
			average = (Dtime) (sum_ptr->Sum_Error () / trips + 0.5);
		} else {
			ttime = diff = error = average = 0;
		}
		link_event_file.Mid_Trip (trips);
		link_event_file.Travel_Time (ttime.Round_Seconds ());
		link_event_file.TTime_Diff (diff.Round_Seconds ());
		link_event_file.TTime_Error (error.Round_Seconds ());

		trips = sum_ptr->Sum_Trips ();

		if (trips > 0) {
			error = (Dtime) (sum_ptr->Sum_Error () / trips + 0.5);
		} else {
			error = 0;
		}
		link_event_file.Sum_Trips (trips);
		link_event_file.Sum_Error (error.Round_Seconds ());
		link_event_file.Avg_Error (average.Round_Seconds ());

		if (!link_event_file.Write ()) {
			Error (String ("Writing %s") % link_event_file.File_Type ());
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % link_event_file.File_Type () % Progress_Count ());

	link_event_file.Close ();
}
