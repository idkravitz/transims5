//*********************************************************
//	Read_Lane_Uses.cpp - read the lane use file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Lane_Uses
//---------------------------------------------------------

void Data_Service::Read_Lane_Uses (void)
{
	Lane_Use_File *file = (Lane_Use_File *) System_File_Handle (LANE_USE);
	
	int i, num, index, first_index, rec, lanes0, lanes1, lane, group, periods, size;

	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period period_rec;
	Use_Period_Array period_array;
	Use_Period_Itr period_itr;
	Int_Set time_sort;
	Int_Set_Itr time_itr;
	Link_Dir_Data index_rec, *index_ptr;
	Integers lane_type;
	Int_RItr lane_ritr;
	Lane_Use_Data lane_use_rec;

	//---- store the lane use data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Lane_Uses (*file);

	num = 0;

	while (file->Read ()) {
		Show_Progress ();

		lane_use_rec.Clear ();

		if (Get_Lane_Use_Data (*file, lane_use_rec)) {
			lane_use_array.push_back (lane_use_rec);
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) lane_use_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) {
		System_Data_True (LANE_USE);

		Lane_Use_Data *lane_use_ptr;
		Dir_Data *dir_ptr;

		//---- create directional link list ---- 

		while (num--) {
			lane_use_ptr = &lane_use_array [num];
			dir_ptr = &dir_array [lane_use_ptr->Dir_Index ()];

			lane_use_ptr->Next_Index (dir_ptr->First_Lane_Use ());
			dir_ptr->First_Lane_Use (num);
		}

		//----- identify time period restrictions ----

		size = (int) dir_array.size ();

		for (index=0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, index++) {
			first_index = dir_itr->First_Lane_Use ();
			if (first_index < 0) continue;

			//---- sort lane use time points ----

			link_ptr = &link_array [dir_itr->Link ()];
			time_sort.clear ();

			for (num = first_index; num >= 0; num = lane_use_ptr->Next_Index ()) {
				lane_use_ptr = &lane_use_array [num];

				time_sort.insert (lane_use_ptr->Start ());
				time_sort.insert (lane_use_ptr->End ());
			}

			//---- create start-end time combinations ----

			period_array.clear ();
			period_rec.Clear ();
			periods = 0;

			for (time_itr = time_sort.begin (); time_itr != time_sort.end (); time_itr++) {
				period_rec.End (*time_itr);
				
				if (periods > 0) {
					period_array.push_back (period_rec);
				}
				period_rec.Start (*time_itr);
				periods++;
			}
			periods = 0;

			//---- identify lane use records within each time period ----

			dir_itr->First_Lane_Use ((int) use_period_array.size ());

			for (period_itr = period_array.begin (); period_itr != period_array.end (); period_itr++) {
				period_itr->Index ((int) use_period_index.size ());

				for (num = first_index; num >= 0; num = lane_use_ptr->Next_Index ()) {
					lane_use_ptr = &lane_use_array [num];

					if (period_itr->Start () >= lane_use_ptr->Start () && period_itr->End () <= lane_use_ptr->End ()) {
						period_itr->Add_Record ();
						index_rec.Link (num);
						index_rec.Dir (0);
						use_period_index.push_back (index_rec);
					}
				}
				if (period_itr->Records () == 0) continue;
				periods++;

				//---- check for lane groups ----

				num = period_itr->Records ();
				lanes1 = 0;
				lanes0 = dir_itr->Lanes ();
				lane_type.assign (lanes0, 0);

				for (i=1, rec=period_itr->Index (); i <= num; i++, rec++) {
					index_ptr = &use_period_index [rec];
					lane_use_ptr = &lane_use_array [index_ptr->Link ()];

					//---- must be a full length restriction ----

					if (lane_use_ptr->Offset () > 0 || lane_use_ptr->Length () > 0) continue;
					group = i;

					if (lane_use_ptr->Type () == APPLY) {

						//---- must apply to a subset of lanes ----

						if (lane_use_ptr->Low_Lane () <= dir_itr->Left () && 
							lane_use_ptr->High_Lane () >= (dir_itr->Left () + dir_itr->Lanes ())) {
							continue;
						}
					} else if ((lane_use_ptr->Type () == LIMIT && dat->Use_Permission (lane_use_ptr->Use (), NONE)) ||
						(lane_use_ptr->Type () == PROHIBIT && lane_use_ptr->Use () == ANY_USE_CODE)) {
						group = -1;
					}
					lanes0 = lane_use_ptr->Low_Lane () - dir_itr->Left ();
					if (lanes0 < 0) lanes0 = 0;
					lanes1 = lane_use_ptr->High_Lane () - dir_itr->Left ();
					if (lanes1 >= dir_itr->Lanes ()) lanes1 = dir_itr->Lanes () - 1;
					for (lane = lanes0; lane <= lanes1; lane++) {
						lane_type [lane] = group;
					}
				}
				lanes0 = lanes1 = group = 0;

				for (lane_ritr = lane_type.rbegin (); lane_ritr != lane_type.rend (); lane_ritr++) {
					if (*lane_ritr < 0) continue;
					if (lanes0 == 0) {
						lanes0++;
						group = *lane_ritr;
					} else if (*lane_ritr == group) {
						lanes0++;
					} else {
						lanes1++;
					}
				}
				period_itr->Lanes0 (lanes0);
				period_itr->Lanes1 (lanes1);

				if (lanes0 != 0 && lanes1 != 0) {

					//----- set flow index for lane groups ----

					if (Lane_Use_Flows () && dir_itr->Flow_Index () < 0) {
						dir_itr->Flow_Index (size + Add_Lane_Use_Flows ());
						lane_use_flow_index.push_back (index);
					}

					//---- set the group flags ----

					for (i=1, rec=period_itr->Index (); i <= num; i++, rec++) {
						index_ptr = &use_period_index [rec];
						if (i > group) index_ptr->Dir (1);
					}
				}
			}

			//---- save the period records ----

			for (period_itr = period_array.begin (); period_itr != period_array.end (); period_itr++) {
				if (period_itr->Records () > 0) {
					period_itr->Periods (--periods);
					use_period_array.push_back (*period_itr);
				}
			}
		}
		Print (1, "Number of Lane Use Flow Records = ") << Num_Lane_Use_Flows ();
	}
}

//---------------------------------------------------------
//	Initialize_Lane_Uses
//---------------------------------------------------------

void Data_Service::Initialize_Lane_Uses (Lane_Use_File &file)
{
	Required_File_Check (file, LINK);
	Required_File_Check (file, VEHICLE_TYPE);

	if (file.Version () <= 40) {
		Required_File_Check (file, POCKET);
	}
	int percent = System_Data_Reserve (LANE_USE);

	if (lane_use_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 1.5);
		}
		if (num > 1) {
			lane_use_array.reserve (num);
			if (num > (int) lane_use_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Lane_Use_Data
//---------------------------------------------------------

bool Data_Service::Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &lane_use_rec)
{
	int link, dir, lanes, offset, dir_index, low, high;
	double rate;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;

	//---- check/convert the link number and direction ----

	link = file.Link ();
	dir = file.Dir ();
	offset = Round (file.Offset ());

	link_ptr = Set_Link_Direction (file, link, dir, offset);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
	} else {
		dir_index = link_ptr->AB_Dir ();
	}
	if (dir_index < 0) {
		Warning (String ("Lane Use %d Link %d Direction %s was Not Found") % Progress_Count () % link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	lane_use_rec.Dir_Index (dir_index);

	//---- set the restriction type ----

	lane_use_rec.Type (file.Type ());
	lane_use_rec.Use (file.Use ());

	//---- convert the vehicle type range ----

	low = file.Min_Veh_Type ();
	high = file.Max_Veh_Type ();

	if (low > 0) {
		map_itr = veh_type_map.find (low);
		if (map_itr == veh_type_map.end ()) {
			map_itr = veh_type_map.lower_bound (low);
			if (map_itr == veh_type_map.end ()) {
				Warning (String ("Lane Use %d Vehicle Type %d was Not Found") % Progress_Count () % low);
				low = -1;
			} else {
				low = map_itr->second;
			}
		} else {
			low = map_itr->second;
		}
	} else {
		low = -1;
	}
	if (high > 0) {
		map_itr = veh_type_map.find (high);
		if (map_itr == veh_type_map.end ()) {
			int h = high;
			while (h > 0) {
				map_itr = veh_type_map.find (--h);
				if (map_itr != veh_type_map.end ()) break;
			}
			if (h >= 0) {
				high = map_itr->second;
			} else {
				Warning (String ("Lane Use %d Vehicle Type %d was Not Found") % Progress_Count () % high);
				high = -1;
			}
		} else {
			high = map_itr->second;
		}
		if (high >= 0 && low < 0) low = 0;
	} else {
		high = -1;
	}
	if (low > high) {
		if (high == -1) {
			high = low;
		} else {
			Warning (String ("Lane Use %d Vehicle Type Range %d-%d is Illegal") % Progress_Count () % file.Min_Veh_Type () % file.Max_Veh_Type ());
			high = low;
		}
	}
	lane_use_rec.Min_Veh_Type (low);
	lane_use_rec.Max_Veh_Type (high);

	//---- convert the traveler type range ----

	low = file.Min_Traveler ();
	high = file.Max_Traveler ();

	if (low <= 0) low = -1;
	if (high > 0) {
		if (low < 0) low = 0;
	} else {
		high = -1;
	}
	if (low > high) {
		if (high == -1) {
			high = low;
		} else {
			Warning (String ("Lane Use %d Traveler Type Range %d-%d is Illegal") % Progress_Count () % file.Min_Traveler () % file.Max_Traveler ());
			high = low;
		}
	}
	lane_use_rec.Min_Traveler (low);
	lane_use_rec.Max_Traveler (high);

	//----- length and offset ----

	lane_use_rec.Length (file.Length ());

	if (lane_use_rec.Length () > 0) {
		if (offset > 0 || lane_use_rec.Length () < link_ptr->Length ()) {
			lane_use_rec.Offset (offset);
		} else {
			lane_use_rec.Offset (0);
			lane_use_rec.Length (0);
		}
	} else {
		lane_use_rec.Offset (0);
	}

	//---- lane number ----

	lanes = file.Lanes ();

	if (file.Version () <= 40 && lanes > 0) {
		low = high = lanes - 1;
	} else {
		Convert_Lane_Range (dir_index, lanes, low, high);
	}
	lane_use_rec.Low_Lane (low);
	lane_use_rec.High_Lane (high);

	//----- optional fields ----

	lane_use_rec.Start (file.Start ());
	lane_use_rec.End (file.End ());
	if (lane_use_rec.End () == 0) lane_use_rec.End (Model_End_Time ());

	lane_use_rec.Toll (Round (file.Toll ()));
	lane_use_rec.Toll_Rate (file.Toll_Rate ());

	if (lane_use_rec.Toll_Rate () > 0) {
		rate = UnRound (lane_use_rec.Toll_Rate ());

		if (Metric_Flag ()) {
			rate /= 1000.0;
		} else {
			rate /= MILETOFEET;
		}
		lane_use_rec.Toll (lane_use_rec.Toll () + DTOI (rate * link_ptr->Length ()));
	}
	lane_use_rec.Min_Delay (file.Min_Delay ());
	lane_use_rec.Max_Delay (file.Max_Delay ());
	
	lane_use_rec.Notes (file.Notes ());

	return (true);
}
