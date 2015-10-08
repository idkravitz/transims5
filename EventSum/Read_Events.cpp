//*********************************************************
//	Read_Events.cpp - Process Event Records
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_Events
//---------------------------------------------------------

void EventSum::Read_Events (void)
{
	int hhold, type, num_rec, period, link, diff, mode;
	Dtime schedule, actual;

	Travel_Time_Data *time_ptr, time_data;
	Time_Sum_Data *sum_ptr, sum_data;
	Link_Time_Map_Stat link_map_stat;
	Link_Time_Key link_key;
	Trip_Index trip_index;
	Trip_Map_Itr time_map_itr;
	Trip_Map_Stat time_map_stat;

	Event_File *event_file = (Event_File *) System_File_Handle (EVENT);

	//----- initialize the time summary memory ----

	if (!compare_flag) {
		num_rec = event_file->Estimate_Records () / 2;
		travel_time_array.reserve (num_rec);
	}
	if (time_sum_flag) {
		time_sum_array.assign (num_inc, sum_data);
	}

	//---- read the event file ----

	Show_Message (String ("Reading %s -- Record") % event_file->File_Type ());
	Set_Progress ();

	num_rec = 0;

	while (event_file->Read ()) {
		Show_Progress ();

		//---- check the event type ----

		type = event_file->Event ();
		if (type != TRIP_START_EVENT && type != TRIP_END_EVENT) continue;

		//---- check the household id ----

		hhold = event_file->Household ();
		if (hhold <= 0) continue;

		if (select_households && !hhold_range.In_Range (hhold)) continue;

		mode = event_file->Mode ();
		if (mode > 0 && mode < MAX_MODE && !select_mode [mode]) continue;

		//---- check the selection records ----
		
		time_data.Household (hhold);
		time_data.Person (event_file->Person ());
		time_data.Tour (event_file->Tour ());
		time_data.Trip (event_file->Trip ());

		time_data.Get_Trip_Index (trip_index);

		if (select_flag && select_map.Best (trip_index) == select_map.end ()) continue;

		//---- get the event times ----

		schedule = event_file->Schedule ();
		actual = event_file->Actual ();

		link = abs (event_file->Link ());

		//---- save the trip data ----

		if (compare_flag || type == TRIP_END_EVENT) {
			time_map_itr = travel_time_map.find (trip_index);
			if (time_map_itr == travel_time_map.end ()) continue;

			time_ptr = &travel_time_array [time_map_itr->second];

			if (link == 0) {
				link = (type == TRIP_START_EVENT) ? time_ptr->Start_Link () : time_ptr->End_Link ();
			}
		} else {
			time_data.Mode (mode);
			time_data.Purpose (0);
			time_data.Constraint (FIXED_TIME);

			num_rec = (int) travel_time_array.size ();

			time_map_stat = travel_time_map.insert (Trip_Map_Data (trip_index, num_rec));
			if (!time_map_stat.second) {
				Warning (String ("Duplicate Travel Time Index %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
			} else {
				travel_time_array.push_back (time_data);
			}
			time_ptr = &travel_time_array [time_map_stat.first->second];
		}

		//---- save the trip start time ----

		if (type == TRIP_START_EVENT) {
			if (!compare_flag || link == time_ptr->Start_Link ()) {
				if (!trip_file_flag) {
					time_ptr->Trip_Start (schedule);
				}
				if (!time_in_flag) {
					time_ptr->Base_Start (schedule);
				}
				time_ptr->Started (actual);
				time_ptr->Start_Link (link);
			}
		} else {
			if (!compare_flag || link == time_ptr->End_Link ()) {

				//---- complete the trip ----

				if (!trip_file_flag) {
					time_ptr->Trip_End (schedule);
				}
				if (!time_in_flag) {
					time_ptr->Base_End (schedule);
				}
				time_ptr->Ended (actual);
				time_ptr->End_Link (link);
			}

			//---- process complete trips ----

			if (time_ptr->Ended () > time_ptr->Started ()) {

				//---- add to the time summary ----

				if (time_sum_flag) {

					//---- process the start time ----

					schedule = time_ptr->Trip_Start ();
					actual = time_ptr->Started ();

					period = sum_periods.Period (schedule);

					if (period >= 0 && period < num_inc) {

						sum_ptr = &time_sum_array [period];
						sum_ptr->Period (period);
						sum_ptr->Add_Trip_Start ();
					}
					period = sum_periods.Period (actual);

					if (period >= 0 && period < num_inc) {
						diff = actual - schedule;

						sum_ptr = &time_sum_array [period];
						sum_ptr->Period (period);
						sum_ptr->Add_Start_Diff (diff);
					}

					//---- process the end time ----

					schedule = time_ptr->Trip_End ();
					actual = time_ptr->Ended ();

					period = sum_periods.Period (schedule);

					if (period >= 0 && period < num_inc) {
						sum_ptr = &time_sum_array [period];
						sum_ptr->Period (period);
						sum_ptr->Add_Trip_End ();
					}
					period = sum_periods.Period (actual);

					if (period >= 0 && period < num_inc) {
						diff = actual - schedule;

						sum_ptr =&time_sum_array [period];
						sum_ptr->Period (period);
						sum_ptr->Add_End_Diff (diff);
					}

					//---- travel time difference ----
					
					actual = time_ptr->Mid_Trip ();

					period = sum_periods.Period (actual);

					if (period >= 0 && period < num_inc) {
						sum_ptr = &time_sum_array [period];

						diff = time_ptr->Travel_Time () - time_ptr->Trip_Time ();

						sum_ptr->Add_Travel_Time (time_ptr->Travel_Time (), diff);

						diff = abs (time_ptr->Trip_Start_Diff ()) + abs (time_ptr->Trip_End_Diff ());

						if (diff >= min_variance) {
							sum_ptr->Add_Sum_Error (diff);
						}
					}
				}

				//---- add to link and period totals ----

				if (link_event_flag) {
					schedule = time_ptr->Trip_Start ();
					actual = time_ptr->Started ();
					link = time_ptr->Start_Link ();

					period = sum_periods.Period (schedule);

					if (period >= 0) {
						link_key.Key (link, period);

						link_map_stat = link_time_map.insert (Link_Time_Map_Data (link_key, sum_data));

						sum_ptr = &link_map_stat.first->second;

						sum_ptr->Period (period);
						sum_ptr->Add_Trip_Start ();
					}
					period = sum_periods.Period (actual);

					if (period >= 0) {
						diff = actual - schedule;

						link_key.Key (link, period);

						link_map_stat = link_time_map.insert (Link_Time_Map_Data (link_key, sum_data));

						sum_ptr = &link_map_stat.first->second;

						sum_ptr->Period (period);
						sum_ptr->Add_Start_Diff (diff);
					}

					//---- end time ----

					schedule = time_ptr->Trip_End ();
					actual = time_ptr->Ended ();
					link = time_ptr->End_Link ();

					period = sum_periods.Period (schedule);

					if (period >= 0) {
						link_key.Key (link, period);

						link_map_stat = link_time_map.insert (Link_Time_Map_Data (link_key, sum_data));

						sum_ptr = &link_map_stat.first->second;

						sum_ptr->Period (period);
						sum_ptr->Add_Trip_End ();
					}
					period = sum_periods.Period (actual);

					if (period >= 0) {
						diff = actual - schedule;

						link_key.Key (link, period);

						link_map_stat = link_time_map.insert (Link_Time_Map_Data (link_key, sum_data));

						sum_ptr = &link_map_stat.first->second;

						sum_ptr->Period (period);
						sum_ptr->Add_End_Diff (diff);
					}

					//---- travel time difference ----
					
					actual = time_ptr->Mid_Trip ();

					period = sum_periods.Period (actual);

					if (period >= 0 && period < num_inc) {
						link_key.Key (link, period);

						link_map_stat = link_time_map.insert (Link_Time_Map_Data (link_key, sum_data));

						sum_ptr = &link_map_stat.first->second;

						diff = time_ptr->Travel_Time () - time_ptr->Trip_Time ();

						sum_ptr->Add_Travel_Time (time_ptr->Travel_Time (), diff);

						diff = abs (time_ptr->Trip_Start_Diff ()) + abs (time_ptr->Trip_End_Diff ());

						if (diff >= min_variance) {
							sum_ptr->Add_Sum_Error (diff);
						}
					}
				}

				//---- add to the time distribution -----

				if (time_flag && time_ptr->Base_Start () < time_ptr->Base_End () &&
					time_ptr->Started () < time_ptr->Ended ()) {

					time_diff.Add_Trip (time_ptr->Mid_Trip (), time_ptr->Travel_Time (), time_ptr->Base_Time ());
				}

				//---- add to time gap data ----

				if (time_gap_flag) {
					if (sum_periods.Period_Control_Point () == MID_TRIP) {
						schedule = (time_ptr->Base_End () + time_ptr->Base_Start ()) >> 1;
					} else if (sum_periods.Period_Control_Point () == TRIP_START) {
						schedule = time_ptr->Base_Start ();
					} else {
						schedule = time_ptr->Base_End ();
					}
					time_gap.Add_Trip_Gap_Data (schedule, time_ptr->Travel_Time (), time_ptr->Base_Time ());
				}
			}
		}
		num_rec++;
	}
	End_Progress ();

	event_file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % event_file->File_Type () % Progress_Count ());
	Print (1, "Number of Records Processed = ") << num_rec;
}
