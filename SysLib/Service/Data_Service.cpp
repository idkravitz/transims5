//*********************************************************
//	Data_Service.cpp - system data input service
//*********************************************************

#include "Data_Service.hpp"

Data_Service *dat = 0;

//---------------------------------------------------------
//	Data_Service constructor
//---------------------------------------------------------

Data_Service::Data_Service (void) : File_Service ()
{
	Service_Level (DATA_SERVICE);

	Ratio_Flag (false);
	Location_XY_Flag (false);
	Sum_Flow_Flag (false);
	Clear_Flow_Flag (false);
	Time_Table_Flag (false);
	Transit_Veh_Flag (false);
	Person_Map_Flag (false);

	Congested_Ratio (Round (300));
	compass.Set_Points (360);
	Bearing_Offset (Round (45));
	Max_Zone_Number (0);

	num_fare_zone = num_lane_flows = 0;
	trip_sort = UNKNOWN_SORT;
	turn_shape_setback = TURN_SHAPE_SETBACK;

	dat = this;
}

//---------------------------------------------------------
//	Data_Service_Keys
//---------------------------------------------------------

void Data_Service::Data_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DAILY_WRAP_FLAG, "DAILY_WRAP_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SUMMARY_TIME_RANGES, "SUMMARY_TIME_RANGES", LEVEL0, OPT_KEY, TEXT_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ SUMMARY_TIME_INCREMENT, "SUMMARY_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ PERIOD_CONTROL_POINT, "PERIOD_CONTROL_POINT", LEVEL0, OPT_KEY, TEXT_KEY, "MID-TRIP", "START, END, MID-TRIP", NO_HELP },
		{ CONGESTED_TIME_RATIO, "CONGESTED_TIME_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0", "1.0..5.0", NO_HELP },
		{ TRIP_SORT_TYPE, "TRIP_SORT_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "DO_NOT_SORT", "DO_NOT_SORT, TRAVELER_SORT, TIME_SORT", NO_HELP },
		{ PLAN_SORT_TYPE, "PLAN_SORT_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "DO_NOT_SORT", "DO_NOT_SORT, TRAVELER_SORT, TIME_SORT", NO_HELP },
		{ HIGHEST_ZONE_NUMBER, "HIGHEST_ZONE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "0", "0..32000", NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				Error (String ("Data Service Key %d was Not Found i=%d") % keys [i] % i);
			}
		}
	}
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Data_Service::Program_Control (void)
{
	String key;

	File_Service::Program_Control ();
	
	Print (2, "Data Service Controls:");

	//---- time summary options ----

	bool first = true;

	//---- daily wrap flag ----

	if (Control_Key_Status (DAILY_WRAP_FLAG)) {
		if (first) {
			Print (1);
			first = false;
		}
		sum_periods.Wrap_Flag (Get_Control_Flag (DAILY_WRAP_FLAG));
	}

	//---- time increment ----

	if (Control_Key_Status (SUMMARY_TIME_INCREMENT)) {
		if (first) {
			Print (1);
			first = false;
		}
		if (!sum_periods.Wrap_Flag ()) {
			sum_periods.Start (Model_Start_Time ());
			sum_periods.End (Model_End_Time ());
		}
		sum_periods.Increment (Get_Control_Time (SUMMARY_TIME_INCREMENT));
	}

	//---- time ranges ----

	if (Control_Key_Status (SUMMARY_TIME_RANGES)) {
		if (first) {
			Print (1);
			first = false;
		}
		if (!sum_periods.Add_Ranges (Get_Control_Text (SUMMARY_TIME_RANGES))) {
			Error (String ("Summary Time Ranges = %s") % Get_Control_String (SUMMARY_TIME_RANGES));
		}

		//---- print the time increment key ----

		Print (1, "Number of Time Periods = ") << sum_periods.Num_Periods ();
	}

	//---- period control point ----

	sum_periods.Period_Control_Point (Time_Point_Code (Get_Control_Text (PERIOD_CONTROL_POINT)));

	//---- congested time ratio ----

	if (Control_Key_Status (CONGESTED_TIME_RATIO)) {
		Congested_Ratio (Round (Get_Control_Double (CONGESTED_TIME_RATIO) * 100.0));

		Ratio_Flag (Check_Control_Key (CONGESTED_TIME_RATIO));

		if (Ratio_Flag ()) {
			if (Congested_Ratio () < Round (150) || Congested_Ratio () > Round (500)) {
				Control_Key_Error (CONGESTED_TIME_RATIO, "(1.5..5.0)");
			}
		}
	}

	//---- trip sort type ----

	if (System_File_Flag (PLAN) || System_File_Flag (NEW_PLAN) || System_File_Flag (TRIP) || System_File_Flag (NEW_TRIP)) {
		if (Control_Key_Status (TRIP_SORT_TYPE)) {
			Print (1);
			if (!Check_Control_Key (TRIP_SORT_TYPE) && Check_Control_Key (PLAN_SORT_TYPE)) {
				Trip_Sort (Trip_Sort_Code (Get_Control_Text (PLAN_SORT_TYPE)));
			} else {
				Trip_Sort (Trip_Sort_Code (Get_Control_Text (TRIP_SORT_TYPE)));
			}
		}
	}

	//---- highest zone number ----

	if (System_File_Flag (SKIM) || System_File_Flag (NEW_SKIM)) {
		if (Control_Key_Status (HIGHEST_ZONE_NUMBER)) {
			Print (1);
			Max_Zone_Number (Get_Control_Integer (HIGHEST_ZONE_NUMBER));
		}
	}

	//---- set the turn shape setback based on internal units ----

	turn_shape_setback = Internal_Units ((double) TURN_SHAPE_SETBACK, FEET);
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Data_Service::Execute (void)
{
	File_Data *file;

	if (!Control_Flag ()) {
		Error ("File_Service::Program_Control has Not Been Called");
	}

	//---- nodes ----

	file = System_File (NODE);
	if (file->flag && file->read) Read_Nodes ();

	//---- zones ----

	file = System_File (ZONE);
	if (file->flag && file->read) Read_Zones ();

	////---- shapes ----

	file = System_File (SHAPE);
	if (file->flag && file->read) Read_Shapes ();

	//---- links ----

	file = System_File (LINK);
	if (file->flag && file->read) Read_Links ();

	//---- pocket lanes ----

	file = System_File (POCKET);
	if (file->flag && file->read) Read_Pockets ();

	//---- vehicle types ----

	file = System_File (VEHICLE_TYPE);
	if (file->flag && file->read) Read_Veh_Types ();

	//---- lane use ----

	file = System_File (LANE_USE);
	if (file->flag && file->read) Read_Lane_Uses ();

	//---- connections ----

	file = System_File (CONNECTION);
	if (file->flag && file->read) Read_Connections ();

	//---- turn penalties ----

	file = System_File (TURN_PENALTY);
	if (file->flag && file->read) Read_Turn_Pens ();

	//---- parking lots ----

	file = System_File (PARKING);
	if (file->flag && file->read) Read_Parking_Lots ();

	//---- locations ----

	file = System_File (LOCATION);
	if (file->flag && file->read) Read_Locations ();

	//---- transit stops ----

	file = System_File (TRANSIT_STOP);
	if (file->flag && file->read) Read_Stops ();

	//---- access links ----

	file = System_File (ACCESS_LINK);
	if (file->flag && file->read) Read_Access_Links ();

	//---- signs ----

	file = System_File (SIGN);
	if (file->flag && file->read) Read_Signs ();

	//---- signals ----

	file = System_File (SIGNAL);
	if (file->flag && file->read) Read_Signals ();

	//---- detectors ----

	file = System_File (DETECTOR);
	if (file->flag && file->read) Read_Detectors ();

	//---- timing plans ----

	file = System_File (TIMING_PLAN);
	if (file->flag && file->read) Read_Timing_Plans ();

	//---- phasing plans ----

	file = System_File (PHASING_PLAN);
	if (file->flag && file->read) Read_Phasing_Plans ();

	//---- transit fares ----

	file = System_File (TRANSIT_FARE);
	if (file->flag && file->read) Read_Fares ();

	//---- transit routes ----

	file = System_File (TRANSIT_ROUTE);
	if (file->flag && file->read) Read_Lines ();

	//---- transit schedules ----

	file = System_File (TRANSIT_SCHEDULE);
	if (file->flag && file->read) Read_Schedules ();

	//---- transit drivers ----

	file = System_File (TRANSIT_DRIVER);
	if (file->flag && file->read) Read_Drivers ();

	//---- route nodes ----

	file = System_File (ROUTE_NODES);
	if (file->flag && file->read) Read_Route_Nodes ();

	//---- selections ----

	file = System_File (SELECTION);
	if (file->flag && file->read) Read_Selections ();

	//---- households ----

	file = System_File (HOUSEHOLD);
	if (file->flag && file->read) Read_Households ();
	
	//---- link delay ----

	file = System_File (LINK_DELAY);
	if (file->flag && file->read) {
		Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
		Read_Link_Delays (*file, link_delay_array, turn_delay_array);
	}

	//---- performance ----

	file = System_File (PERFORMANCE);
	if (file->flag && file->read) {
		Performance_File *file = (Performance_File *) System_File_Handle (PERFORMANCE);
		Read_Performance (*file, link_perf_array, turn_perf_array);
	}
	
	//---- ridership ----

	file = System_File (RIDERSHIP);
	if (file->flag && file->read) Read_Ridership ();

	//---- vehicles ----

	file = System_File (VEHICLE);
	if (file->flag && file->read) Read_Vehicles ();
	
	//---- trips ----

	file = System_File (TRIP);
	if (file->flag && file->read) Read_Trips ();
	
	//---- problems ----

	file = System_File (PROBLEM);
	if (file->flag && file->read) Read_Problems ();

	//---- plans ----

	file = System_File (PLAN);
	if (file->flag && file->read) Read_Plans ();

	//---- skims ----

	file = System_File (SKIM);
	if (file->flag && file->read) Read_Skims ();

	//---- events ----

	file = System_File (EVENT);
	if (file->flag && file->read) Read_Events ();
	
	//---- travelers ----

	file = System_File (TRAVELER);
	if (file->flag && file->read) Read_Travelers ();

	//---- zone location map file ----

	if (Zone_Loc_Flag ()) {
		zone_loc_map.Read ();
	}
}

//-----------------------------------------------------------
//	Set_Link_Direction
//-----------------------------------------------------------

Link_Data * Data_Service::Set_Link_Direction (Db_Header &file, int &link, int &dir, int &offset, bool flag)
{
	Link_Data *link_ptr = 0;
	Int_Map_Itr map_itr;

	int node = dir;

	if (link < 0) {
		dir = 1;
		link = -link;
	} else {
		dir = 0;
	}
	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("%s Link %d was Not Found in the Link file") % file.File_ID () % link);
		return (0);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (dir == 0 && file.LinkDir_Type () != LINK_SIGN) {
		if (file.LinkDir_Type () == LINK_NODE) {
			dir = 0;

			map_itr = node_map.find (node);
			if (map_itr == node_map.end ()) {
				Warning (String ("%s Node %d was Not Found in the Node file") % file.File_ID () % node);
				return (0);
			}
			node = map_itr->second;

			dir = -1;
			if (flag) {
				if (link_ptr->Bnode () == node) {
					dir = 1;
				} else if (link_ptr->Anode () == node) {
					dir = 0;
				}
			} else if (link_ptr->Anode () == node) {
				dir = 1;
			} else if (link_ptr->Bnode () == node) {
				dir = 0;
			}
			if (dir < 0) {
				node = map_itr->first;
				Warning (String ("%s Node %d is Not on Link %d") % file.File_ID () % node % link_ptr->Link ());
				return (0);
			}
		} else {
			dir = node;
		}
	}
	if (offset < 0 || offset > link_ptr->Length ()) {
		Warning (String ("%s %d Offset %.1lf is Out of Range (0..%.1lf)") %
			file.File_ID () % Progress_Count () % UnRound (offset) % UnRound (link_ptr->Length ()));

		if (offset < 0) {
			offset = 0;
		} else {
			offset = link_ptr->Length ();
		}
	}
	if (file.Version () <= 40) {
		offset = link_ptr->Length () - offset;
	}
	return (link_ptr);
}

//-----------------------------------------------------------
//	VehType40_Map
//-----------------------------------------------------------

int Data_Service::VehType40_Map (int type, int sub)
{
	Int2_Map_Stat map_stat;

	map_stat = vehtype40_map.insert (Int2_Map_Data (Int2_Key (type, sub), (int) vehtype40_map.size () + 1));
	return (map_stat.first->second);
}

//-----------------------------------------------------------
//	Convert_Lane_ID
//-----------------------------------------------------------

int Data_Service::Convert_Lane_ID (Dir_Data *dir_ptr, int lane_id)
{
	int code, lane;

	lane = lane_id & 0x3F;
	code = (lane_id & 0x00C0) >> 6;

	if (code == LEFT_POCKET) {
		if (lane < 1 || lane > dir_ptr->Left ()) {
			if (lane != 0 && System_File_Flag (POCKET)) {
				Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
				Warning (String ("Link %d Left Pocket Lane %d is Out of Range (0..%d)") %
					link_ptr->Link () % lane % dir_ptr->Left ());
			}
			lane = dir_ptr->Left ();
		}
		lane = dir_ptr->Left () - lane;
	} else if (code == RIGHT_POCKET) {
		if (lane < 1 || lane > dir_ptr->Right ()) {
			if (lane != 0 && System_File_Flag (POCKET)) {
				Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
				Warning (String ("Link %d Right Pocket Lane %d is Out of Range (0..%d)") % 
					link_ptr->Link () % lane % dir_ptr->Right ());
			}
			lane = dir_ptr->Right ();
		}
		lane += dir_ptr->Lanes () + dir_ptr->Left () - 1;
	} else {
		if (lane == 0) return (-1);
		if (lane < 1 || lane > dir_ptr->Lanes ()) {
			Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
			Warning (String ("Link %d Main Lane %d is Out of Range (1..%d)") % 
				link_ptr->Link () % lane % dir_ptr->Lanes ());
			lane = dir_ptr->Lanes ();
		}
		lane = dir_ptr->Lanes () - lane + dir_ptr->Left ();
	}
	return (lane);
}

//-----------------------------------------------------------
//	Make_Lane_ID
//-----------------------------------------------------------

int Data_Service::Make_Lane_ID (Dir_Data *dir_ptr, int lane)
{
	int code = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right ();

	if (lane == -1) return (0);
	if (lane < 0 || lane >= code) {
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
		Warning (String ("Link %d Lane %d is Out of Range (0..%d)") % link_ptr->Link () % lane % (code-1));
		return (1);
	}
	if (lane < dir_ptr->Left ()) {
		code = LEFT_POCKET;
		lane = dir_ptr->Left () - lane;
	} else if (lane >= dir_ptr->Lanes () + dir_ptr->Left ()) {
		code = RIGHT_POCKET;
		lane = lane - dir_ptr->Lanes () - dir_ptr->Left () + 1;
	} else {
		code = MAIN_LANE;
		lane = dir_ptr->Lanes () + dir_ptr->Left () - lane;
	}
	return (lane + (code << 6));
}

//-----------------------------------------------------------
//	Convert_Lane_Range
//-----------------------------------------------------------

void Data_Service::Convert_Lane_Range (Dir_Data *dir_ptr, int lane_range, int &low, int &high)
{
	if (lane_range == 0) {
		low = 0;
		high = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () - 1;
		return;
	}
	high = Convert_Lane_ID (dir_ptr, lane_range);

	low = (lane_range >> 8);
	if (low == 0) {
		low = high;
	} else {
		low = Convert_Lane_ID (dir_ptr, low);
		if (low > high) {
			Warning (String ("Link %d Lane Range %d..%d is Illogical") % link_array [dir_ptr->Link ()].Link () % low % high);
			int temp = low;
			low = high;
			high = temp;
		}
	}
}

//-----------------------------------------------------------
//	Make_Lane_Range
//-----------------------------------------------------------

int Data_Service::Make_Lane_Range (Dir_Data *dir_ptr, int low, int high)
{
	if (low > high) {
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
		Warning (String ("Link %d Lane Range %d..%d is Illogical") % link_ptr->Link () % low % high);
		return (0);
	}
	low = Make_Lane_ID (dir_ptr, low);
	high = Make_Lane_ID (dir_ptr, high);

	if (low == high) {
		return (low);
	} else {
		return (high + (low << 8));
	}
}

//---------------------------------------------------------
//	Lane_Map
//---------------------------------------------------------

void Data_Service::Lane_Map (Connect_Data *connect_ptr, Lane_Map_Array &lane_map)
{
	int first_in, last_in, first_out, last_out;
	int num, num_in, num_out, num_connect, in, out, lane, pocket;

	Dir_Data *dir_ptr, *to_ptr;
	Lane_Map_Data map_rec;

	lane_map.clear ();
	if (connect_ptr == 0) return;

	//---- set the approach lane range ----

	dir_ptr = &dir_array [connect_ptr->Dir_Index ()];

	first_in = connect_ptr->Low_Lane ();
	last_in = connect_ptr->High_Lane ();

	//---- set the departure lane range ----

	to_ptr = &dir_array [connect_ptr->To_Index ()];

	first_out = connect_ptr->To_Low_Lane ();
	last_out = connect_ptr->To_High_Lane ();

	//---- adjust lane balance ----

	num_in = last_in - first_in + 1;
	num_out = last_out - first_out + 1;

	num_connect = MAX (num_in, num_out);
	num = num_in - num_out;

	in = first_in;
	out = first_out;

	if (num_in > num_out) { 	//---- too many input lanes ----
		lane = in;

		//---- remove left pocket lanes ----

		pocket = dir_ptr->Left ();

		while (lane < pocket && num > 0) {
			lane++;
			num--;
			out--;
		}

		//---- remove right pocket lanes ----

		if (num > 0) {
			pocket += dir_ptr->Lanes ();
			lane = last_in;

			while (lane >= pocket && num > 0) {
				lane--;
				num--;
			}
		}

		//---- force outside lanes to merge ----

		while (num > 0) {

			//---- force right lane to merge ----

			num--;

			//---- force left lane to merge ----

			if (num > 0) {
				out--;
				num--;
			}
		}

	} else if (num_in < num_out) {		//---- too many output lanes ----
		lane = out;

		//---- remove left pocket lanes ----

		pocket = to_ptr->Left ();

		while (lane < pocket && num < 0) {
			lane++;
			num++;
			in--;
		}

		//---- remove right pocket lanes ----

		if (num < 0) {
			pocket += to_ptr->Lanes ();
			lane = last_out;

			while (lane >= pocket && num < 0) {
				lane--;
				num++;
			}
		}

		//---- outside lanes diverge ----

		while (num < 0) {

			//---- right lane diverge ----

			num++;

			//---- left lane diverge ----

			if (num < 0) {
				in--;
				num++;
			}
		}
	}
	for (num=0; num < num_connect; num++, in++, out++) {
		if (in < first_in) {
			map_rec.In_Lane (first_in);
			map_rec.In_Thru (0);
		} else if (in > last_in) {
			map_rec.In_Lane (last_in);
			map_rec.In_Thru (0);
		} else {
			map_rec.In_Lane (in);
			map_rec.In_Thru (1);
		}
		if (out < first_out) {
			map_rec.Out_Lane (first_out);
			map_rec.Out_Thru (0);
		} else if (out > last_out) {
			map_rec.Out_Lane (last_out);
			map_rec.Out_Thru (0);
		} else {
			map_rec.Out_Lane (out);
			map_rec.Out_Thru (1);
		}
		lane_map.push_back (map_rec);
	}
}

#ifdef HASH_MAP
SYSLIB_API size_t hash_value (const Vehicle_Index & value) 
{ 
	return ((size_t) (value.first ^ _HASH_SEED)); 
}
#endif
