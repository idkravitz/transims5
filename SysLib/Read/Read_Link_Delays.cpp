//*********************************************************
//	Read_Link_Delays.cpp - read the link delay file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Link_Delays
//---------------------------------------------------------

void Data_Service::Read_Link_Delays (Link_Delay_File &file, Flow_Time_Period_Array &link_delay_array, Flow_Time_Period_Array &turn_delay_array)
{
	int i, num, start, end, link, dir, link_count, turn_count, index, size;
	double flow, p1, p2, share;
	Dtime time;
	bool keep_flag;

	Time_Periods *period_ptr;
	Dir_Data *dir_ptr;
	Int2_Map_Itr map_itr;
	Connect_Data *connect_ptr;
	Link_Delay_Data delay_rec;
	Turn_Delay_Itr turn_itr;
	Flow_Time_Data *flow_time_ptr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Itr flow_time_itr;

	//---- store the link delay data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();

	Initialize_Link_Delays (file, link_delay_array, turn_delay_array);
	link_count = turn_count = 0;

	while (file.Read (false)) {
		Show_Progress ();

		delay_rec.Clear ();

		keep_flag = Get_Link_Delay_Data (file, delay_rec);

		num = file.Num_Nest ();
		if (num > 0) delay_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file.Read (true)) {
				Warning (String ("Number of Nested Records for Link Delay %d = %d vs %d") % file.Link () % i % num);
			}
			Show_Progress ();

			Get_Link_Delay_Data (file, delay_rec);
		}
		if (keep_flag) {

			//---- get the time period ----

			period_ptr = link_delay_array.periods;

			if (period_ptr->Period_Range (delay_rec.Start (), delay_rec.End (), start, end)) {

				p1 = period_ptr->Period_Offset (start, delay_rec.Start ());
				p2 = period_ptr->Period_Offset (end, delay_rec.End ());

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
					array_ptr = &link_delay_array [i];

					if (delay_rec.Type () == 0 || !Lane_Use_Flows ()) {
						index = delay_rec.Dir_Index ();
					} else {
						dir_ptr = &dir_array [delay_rec.Dir_Index ()];
						index = dir_ptr->Flow_Index ();
					}
					flow_time_ptr = &array_ptr->at (index);

					flow_time_ptr->Combine_Flow_Time (delay_rec.Flow () * share, delay_rec.Time ());
					if (i == start) link_count++;
			
					//---- process the turn records ----

					if (file.Turn_Flag () && delay_rec.size () > 0) {
						array_ptr = &turn_delay_array [i];

						for (turn_itr = delay_rec.begin (); turn_itr != delay_rec.end (); turn_itr++) {
							map_itr = connect_map.find (Int2_Key (delay_rec.Dir_Index (), turn_itr->To_Index ()));

							if (map_itr != connect_map.end ()) {
								flow_time_ptr = &array_ptr->at (map_itr->second);

								flow_time_ptr->Combine_Flow_Time (turn_itr->Flow () * share, turn_itr->Time ());
								if (i == start) turn_count++;
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

	if (num > 0) System_Data_True (LINK_DELAY);

	//---- calculate the travel times ----

	link = dir = start = end = 0;
	size = (int) dir_array.size ();

	for (period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++) {
		start++;
		keep_flag = false;

		for (num=0, flow_time_itr = period_itr->begin (); flow_time_itr != period_itr->end (); flow_time_itr++, num++) {
			link++;
			if (flow_time_itr->Time () > 0) {
				dir++;
				keep_flag = true;
				time = flow_time_itr->Time ();
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
			flow_time_itr->Time (time);

			if (Clear_Flow_Flag ()) flow_time_itr->Flow (0);
		}
		if (keep_flag) end++;
	}
	Break_Check (6);
	Print (2, "Number of Summary Time Periods = ") << link_delay_array.periods->Num_Periods ();
	Print (1, String ("Percent of Link Periods with Travel Time Data = %.1lf%%") % 
				((link) ? dir * 100.0 / link : 0.0) % FINISH);
	Print (1, String ("Percent of Time Periods with Link Delay Data = %.1lf%%") % 
				((start) ? end * 100.0 / start : 0.0) % FINISH);

	//---- adjust connection times ----

	if (file.Turn_Flag ()) {
		link = dir = start = end = 0;

		if (turn_count > 0) {

			for (i=0, period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++, i++) {
				array_ptr = &link_delay_array [i];

				start++;
				keep_flag = false;

				for (num=0, flow_time_itr = period_itr->begin (); flow_time_itr != period_itr->end (); flow_time_itr++, num++) {
					link++;
					if (flow_time_itr->Time () > 0) {
						dir++;
						keep_flag = true;

						connect_ptr = &connect_array [num];

						flow_time_ptr = &array_ptr->at (connect_ptr->Dir_Index ());

						time = flow_time_itr->Time () - flow_time_ptr->Time ();
					} else {
						time = 0;
					}
					flow_time_itr->Time (time);

					if (Clear_Flow_Flag ()) flow_time_itr->Flow (0);
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
//	Initialize_Link_Delays
//---------------------------------------------------------

void Data_Service::Initialize_Link_Delays (Link_Delay_File &file, Flow_Time_Period_Array &link_delay_array, Flow_Time_Period_Array &turn_delay_array)
{
	Required_File_Check (file, LINK);
	
	if (System_Data_Reserve (LINK_DELAY) == 0) return;

	if (link_delay_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			link_delay_array.Initialize (&sum_periods);
		} else {
			link_delay_array.Initialize (&time_periods);
		}
	}
	if (file.Turn_Flag () && turn_delay_array.size () == 0) {
		int num = (int) connect_array.size ();

		if (num > 0) {
			if (Sum_Flow_Flag ()) {
				turn_delay_array.Initialize (&sum_periods, num);
			} else {
				turn_delay_array.Initialize (&time_periods, num);
			}
		} else {
			file.Turn_Flag (false);
		}
	}
}

//---------------------------------------------------------
//	Get_Link_Delay_Data
//---------------------------------------------------------

bool Data_Service::Get_Link_Delay_Data (Link_Delay_File &file, Link_Delay_Data &delay_rec)
{
	int dir_index, link, dir, num;
	static int node; 
	static Dtime period;

	Link_Data *link_ptr;

	//---- process a header line ----

	if (!file.Nested ()) {

		//---- check/convert the link number and direction ----
		
		link = file.Link ();
		dir = file.Dir ();

		link_ptr = Set_Link_Direction (file, link, dir, (file.Version () <= 40));

		if (link_ptr == 0) return (false);

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
			node = link_ptr->Anode ();
		} else {
			dir_index = link_ptr->AB_Dir ();
			node = link_ptr->Bnode ();
		}
		if (dir_index < 0) {
			Warning (String ("%s %d Link %d Direction %s was Not Found") % file.File_ID () % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
			node = 0;
			return (false);
		}
		delay_rec.Dir_Index (dir_index);
		delay_rec.Type (file.Type ());

		//---- store the data ----

		delay_rec.End (file.End ());
		delay_rec.Flow (file.Flow ());

		if (file.Version () <= 40 && file.Dbase_Format () == VERSION3) {
			if (period == 0) {
				period = delay_rec.End ();
			}
			delay_rec.Start (delay_rec.End () - period);
			if (delay_rec.Flow () > 0) {
				delay_rec.Time (UnRound (file.Time ()) / delay_rec.Flow ());
			} else {
				delay_rec.Time (file.Time ());
			}
		} else {
			delay_rec.Start (file.Start ());
			delay_rec.Time (file.Time ());
		}
		num = file.Num_Nest ();
		delay_rec.clear ();

		if (num > 0) delay_rec.reserve (num);
		return (true);
	}
	if (!file.Turn_Flag ()) return (false);

	Int_Map_Itr map_itr;
	Turn_Delay_Data turn_rec;

	//---- convert the to-link number ----

	link = file.Out_Link ();

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("%s %d Link %d was Not Found") % file.File_ID () % Progress_Count () % 
			link);
		return (false);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (link_ptr->Anode () == node) {
		dir_index = link_ptr->AB_Dir ();
		dir = 0;
	} else if (link_ptr->Bnode () == node) {
		dir_index = link_ptr->BA_Dir ();
		dir = 1;
	} else {
		Node_Data *ptr = &node_array [node];
		Warning (String ("%s %d Node %d is Not on Link %d") % file.File_ID () % Progress_Count () % 
			ptr->Node () % link_ptr->Link ());
		return (false);
	}
	if (dir_index < 0) {
		Warning (String ("%s %d Link %d Direction %s was Not Found") % file.File_ID () % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	turn_rec.To_Index (dir_index);
	turn_rec.Flow (file.Out_Flow ());
	turn_rec.Time (file.Out_Time ());

	delay_rec.push_back (turn_rec);
	return (true);
}
