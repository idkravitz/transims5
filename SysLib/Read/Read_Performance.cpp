//*********************************************************
//	Read_Performance.cpp - read the performance file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Performance
//---------------------------------------------------------

void Data_Service::Read_Performance (Performance_File &file, Link_Perf_Period_Array &link_perf_array, Flow_Time_Period_Array &turn_perf_array)
{
	int i, num, start, end, link, dir, link_count, turn_count, vmt, vht, index, size;
	double flow, p1, p2, share;
	Dtime time;
	bool keep_flag;

	Time_Periods *period_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Int2_Map_Itr map_itr;
	Connect_Data *connect_ptr;
	Performance_Data perf_rec;
	Turn_Delay_Itr turn_itr;
	Link_Perf_Data *link_perf_ptr;
	Flow_Time_Data *turn_perf_ptr;
	Link_Perf_Array *perf_period_ptr;
	Flow_Time_Array *turn_period_ptr;
	Link_Perf_Itr link_perf_itr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Itr turn_perf_itr;
	Flow_Time_Period_Itr turn_period_itr;

	//---- store the performance data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();
	
	Initialize_Performance (file, link_perf_array, turn_perf_array);
	link_count = turn_count = 0;

	while (file.Read (false)) {
		Show_Progress ();

		perf_rec.Clear ();

		keep_flag = Get_Performance_Data (file, perf_rec);

		num = file.Num_Nest ();
		if (num > 0) perf_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file.Read (true)) {
				Error (String ("Number of Nested Records for Performance %d") % file.Link ());
			}
			Show_Progress ();

			Get_Performance_Data (file, perf_rec);
		}
		if (keep_flag) {

			//---- get the time period ----
			
			period_ptr = link_perf_array.periods;

			if (period_ptr->Period_Range (perf_rec.Start (), perf_rec.End (), start, end)) {

				p1 = period_ptr->Period_Offset (start, perf_rec.Start ());
				p2 = period_ptr->Period_Offset (end, perf_rec.End ());

				//---- allocate the flow to multiple time periods ----

				num = end - start;
				flow = 1.0 / (end - start + p2 - p1);

				for (i = start; i <= end; i++) {
					if (i == start && i == end) {
						share = 1.0;
					} else if (i == start) {
						share = (1.0 - p1) * flow;
					} else if (i == end) {
						share = p2 * flow;
					} else {
						share = flow;
					}
					
					//---- process the link record ----

					perf_period_ptr = &link_perf_array [i];

					dir_ptr = &dir_array [perf_rec.Dir_Index ()];

					if (perf_rec.Type () == 0 || !Lane_Use_Flows ()) {
						index = perf_rec.Dir_Index ();
					} else {
						index = dir_ptr->Flow_Index ();
					}
					link_perf_ptr = &perf_period_ptr->at (index);

					link_perf_ptr->Combine_Flow_Time (perf_rec.Flow () * share, perf_rec.Time ());
					link_perf_ptr->Add_Density (DTOI (perf_rec.Density () * share));
					link_perf_ptr->Add_Max_Density (perf_rec.Max_Density ());
					link_perf_ptr->Add_Queue (DTOI (perf_rec.Queue () * share));
					link_perf_ptr->Add_Max_Queue (perf_rec.Max_Queue ());
					link_perf_ptr->Add_Failure (DTOI (perf_rec.Failure () * share));
					link_perf_ptr->Add_Occupant ();

					if (Ratio_Flag () && perf_rec.Time_Ratio () >= Congested_Ratio ()) {
						link_perf_ptr->Add_Ratio ();
					
						link_ptr = &link_array [dir_ptr->Link ()];

						vmt = DTOI (perf_rec.Flow () * share * link_ptr->Length ());
						vht = DTOI (perf_rec.Time () * perf_rec.Flow () * share);

						link_perf_ptr->Add_Ratio_VMT (vmt);
						link_perf_ptr->Add_Ratio_VHT (vht);
					}
					link_count++;
			
					//---- process the turn records ----

					if (file.Turn_Flag () && perf_rec.size () > 0) {
						turn_period_ptr = &turn_perf_array [i];
						
						for (turn_itr = perf_rec.begin (); turn_itr != perf_rec.end (); turn_itr++) {
							map_itr = connect_map.find (Int2_Key (perf_rec.Dir_Index (), turn_itr->To_Index ()));

							if (map_itr != connect_map.end ()) {
								turn_perf_ptr = &turn_period_ptr->at (map_itr->second);

								turn_perf_ptr->Combine_Flow_Time (turn_itr->Flow () * share, turn_itr->Time ());
								turn_count++;
							}
						}
					}
				}
			}
		}
	}
	End_Progress ();
	file.Close ();

	Break_Check (5);
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	num = link_count + turn_count;

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file.File_ID () % num);
	}
	Print (1, "Number of Link Direction Records = ") << link_count;
	Print (1, "Number of Link Connection Records = ") << turn_count;

	if (num > 0) System_Data_True (PERFORMANCE);

	//---- calculate the travel times ----

	link = dir = start = end = 0;
	size = (int) dir_array.size ();

	for (period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++) {
		start++;
		keep_flag = false;

		for (num=0, link_perf_itr = period_itr->begin (); link_perf_itr != period_itr->end (); link_perf_itr++, num++) {
			link++;
				
			if (link_perf_itr->Time () > 0) {
				dir++;
				keep_flag = true;
				time = link_perf_itr->Time ();
			} else {
				if (num < size) {
					index = num;
				} else {
					index = lane_use_flow_index [num - size];
				}
				dir_ptr = &dir_array [index];
				time = dir_ptr->Time0 ();
			}
			if (time < 1) time = 1;
			link_perf_itr->Time (time);
			
			if (Clear_Flow_Flag ()) link_perf_itr->Flow (0);

			link_count = link_perf_itr->Occupancy ();

			if (link_count > 0) {
				link_perf_itr->Density ((link_perf_itr->Density () + (link_count >> 1)) / link_count);
				link_perf_itr->Queue ((link_perf_itr->Queue () + (link_count >> 1)) / link_count);
				if (!Ratio_Flag ()) link_perf_itr->Occupancy (1);
			}
		}
		if (keep_flag) end++;
	}
	Break_Check (6);
	Print (2, "Number of Summary Time Periods = ") << link_perf_array.periods->Num_Periods ();
	Print (1, String ("Percent of Link Periods with Travel Time Data = %.1lf%%") % 
				((link) ? dir * 100.0 / link : 0.0) % FINISH);
	Print (1, String ("Percent of Time Periods with Performance Data = %.1lf%%") % 
				((start) ? end * 100.0 / start : 0.0) % FINISH);

	//---- adjust connection times ----

	if (file.Turn_Flag ()) {
		link = dir = start = end = 0;

		if (turn_count > 0) {
			for (i=0, turn_period_itr = turn_perf_array.begin (); turn_period_itr != turn_perf_array.end (); turn_period_itr++, i++) {
				perf_period_ptr = &link_perf_array [i];

				start++;
				keep_flag = false;

				for (num=0, turn_perf_itr = turn_period_itr->begin (); turn_perf_itr != turn_period_itr->end (); turn_perf_itr++, num++) {
					link++;
					if (turn_perf_itr->Time () > 0) {
						dir++;
						keep_flag = true;

						connect_ptr = &connect_array [num];

						link_perf_ptr = &perf_period_ptr->at (connect_ptr->Dir_Index ());

						time = turn_perf_itr->Time () - link_perf_ptr->Time ();
					} else {
						time = 0;
					}
					turn_perf_itr->Time (time);

					if (Clear_Flow_Flag ()) turn_perf_itr->Flow (0);
				}
				if (keep_flag) end++;
			}
		}
		Print (1, String ("Percent of Connection Periods with Travel Time Data = %.1lf%%") % 
					((link) ? dir * 100.0 / link : 0.0) % FINISH);
		Print (1, String ("Percent of Time Periods with Connection Delay Data = %.1lf%%") % 
					((start) ? end * 100.0 / start : 0.0) % FINISH);
	}
}

//---------------------------------------------------------
//	Initialize_Performance
//---------------------------------------------------------

void Data_Service::Initialize_Performance (Performance_File &file, Link_Perf_Period_Array &link_perf_array, Flow_Time_Period_Array &turn_perf_array)
{
	Required_File_Check (file, LINK);
	
	if (System_Data_Reserve (PERFORMANCE) == 0) return;

	if (link_perf_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			link_perf_array.Initialize (&sum_periods);
		} else {
			link_perf_array.Initialize (&time_periods);
		}
	}
	if (file.Turn_Flag () && turn_perf_array.size () == 0) {
		int num = (int) connect_array.size ();

		if (num > 0) {
			if (Sum_Flow_Flag ()) {
				turn_perf_array.Initialize (&sum_periods, num);
			} else {
				turn_perf_array.Initialize (&time_periods, num);
			}
		} else {
			file.Turn_Flag (false);
		}
	}
}

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool Data_Service::Get_Performance_Data (Performance_File &file, Performance_Data &perf_rec)
{
	if (!Get_Link_Delay_Data (file, perf_rec)) return (false);

	if (!file.Nested ()) {
		perf_rec.Start (file.Start ());
		perf_rec.End (file.End ());
		perf_rec.Speed (file.Speed ());
		perf_rec.Delay (file.Delay ());
		perf_rec.Density (file.Density ());
		perf_rec.Max_Density (file.Max_Density ());
		perf_rec.Time_Ratio (file.Time_Ratio ());
		perf_rec.Queue (file.Queue ());
		perf_rec.Max_Queue (file.Max_Queue ());
		perf_rec.Failure (file.Cycle_Failure ());
	}
	return (true);
}
