//*********************************************************
//	Read_Plans.cpp - Read the Plan File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void Data_Service::Read_Plans (void)
{
	int i, part, num, count, num_rec, part_num, first;
	bool keep_flag;

	Plan_File *file = (Plan_File *) System_File_Handle (PLAN);

	Trip_Index trip_index;
	Trip_Map_Stat trip_stat;
	Time_Index time_index;
	Time_Map_Stat time_stat;
	Plan_Data plan_rec;

	count = num_rec = first = 0;
	Initialize_Plans (*file);

	//---- check the partition number ----

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}
	
		//---- store the plan data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read (false)) {
			Show_Progress ();

			plan_rec.Clear ();

			keep_flag = Get_Plan_Data (*file, plan_rec, part_num);

			num = file->Num_Nest ();
			if (num > 0) plan_rec.reserve (num);

			for (i=1; i <= num; i++) {
				if (!file->Read (true)) {
					Error (String ("Number of Nested Records for Plan %d") % file->Household ());
				}
				Show_Progress ();

				Get_Plan_Data (*file, plan_rec, part_num);
			}
			if (keep_flag) {
				plan_rec.Tour (MAX (plan_rec.Tour (), 1));

				if (Trip_Sort () == TRAVELER_SORT) {
					plan_rec.Get_Trip_Index (trip_index);

					trip_stat = plan_trip_map.insert (Trip_Map_Data (trip_index, (int) plan_array.size ()));

					if (!trip_stat.second) {
						Warning (String ("Duplicate Plan Index = %d-%d-%d-%d") % 
							trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
						keep_flag = false;
					}
				} else if (Trip_Sort () == TIME_SORT) {
					plan_rec.Get_Time_Index (time_index);

					time_stat = plan_time_map.insert (Time_Map_Data (time_index, (int) plan_array.size ()));

					if (!time_stat.second) {
						Warning (String ("Duplicate Plan Index = %s-%d-%d") % 
							time_index.Start ().Time_String () % 
							time_index.Household () % time_index.Person ());
						keep_flag = false;
					}
				}
				if (keep_flag) {
					num = (int) plan_rec.size ();
					file->Add_Trip (plan_rec.Household (), plan_rec.Person (), plan_rec.Tour ());
					file->Add_Leg (num);

					plan_array.push_back (plan_rec);
					count += num;
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) plan_array.size ();

	if (num && num != num_rec) {
		Print (1, "Number of Trips = ") << num;
		Print (1, "Number of Legs = ") << count;
	}
	if (num > 0) System_Data_True (PLAN);
}

//---------------------------------------------------------
//	Initialize_Plans
//---------------------------------------------------------

void Data_Service::Initialize_Plans (Plan_File &file)
{
	Required_File_Check (file, LOCATION);
	Required_File_Check (file, VEHICLE);

	int percent = System_Data_Reserve (PLAN);

	if (plan_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () > 40) {
			num = (int) (num / 2.0);
		}
		if (num > 1) {
			plan_array.reserve (num);
			if (num > (int) plan_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Plan_Data
//---------------------------------------------------------

bool Data_Service::Get_Plan_Data (Plan_File &file, Plan_Data &plan_rec, int partition)
{
	int hhold, lvalue;

	//---- process a header line ----

	if (!file.Nested ()) {
		Int_Map_Itr map_itr;
		Vehicle_Index veh_index;
		Vehicle_Map_Itr veh_itr;

		//---- check the household id ----

		hhold = file.Household ();
		if (hhold < 1) return (false);

		plan_rec.Household (hhold);

		plan_rec.Person (file.Person ());
		plan_rec.Tour (MAX (file.Tour (), 1));
		plan_rec.Trip (file.Trip ());

		plan_rec.Start (file.Start ());
		plan_rec.End (file.End ());

		//---- convert the origin ----

		lvalue = file.Origin ();

		map_itr = location_map.find (lvalue);

		if (map_itr == location_map.end ()) {
			Warning (String ("Plan %d Origin %d was Not Found") % Progress_Count () % lvalue);
			return (false);
		}
		plan_rec.Origin (map_itr->second);

		//---- convert the destination ----

		lvalue = file.Destination ();

		map_itr = location_map.find (lvalue);

		if (map_itr == location_map.end ()) {
			Warning (String ("Plan %d Destination %d was Not Found") % Progress_Count () % lvalue);
			return (false);
		}
		plan_rec.Destination (map_itr->second);

		lvalue = file.Vehicle ();

		if (file.Version () <= 40) {
			lvalue = Fix_Vehicle_ID (lvalue);
		}
		if (lvalue > 0) {
			veh_index.Household (hhold);
			veh_index.Vehicle (lvalue);

			veh_itr = vehicle_map.find (veh_index);
			if (veh_itr == vehicle_map.end ()) {
				Warning (String ("Trip %d Vehicle %d was Not Found") % Progress_Count () % lvalue);
				return (false);
			}
			lvalue = veh_itr->second;
		} else {
			lvalue = -1;
		}
		plan_rec.Vehicle (lvalue);

		if (file.Version () <= 40) {
			plan_rec.Mode (Trip_Mode_Map (file.Mode ()));
		} else {
			plan_rec.Mode (file.Mode ());
		}
		plan_rec.Purpose (file.Purpose ());
		plan_rec.Constraint (file.Constraint ());
		plan_rec.Passengers (file.Passengers ());

		if (file.Partition_Flag ()) {
			plan_rec.Partition (file.Partition ());
		} else {
			plan_rec.Partition (partition);
		}
		plan_rec.Depart (file.Depart ());
		plan_rec.Arrive (file.Arrive ());
		plan_rec.Activity (file.Activity ());
		plan_rec.Walk (file.Walk ());
		plan_rec.Drive (file.Drive ());
		plan_rec.Transit (file.Transit ());
		plan_rec.Other (file.Other ());
		plan_rec.Length (file.Length ());
		plan_rec.Cost (file.Cost ());
		plan_rec.Impedance (file.Impedance ());

		return (true);
	}

	//---- process a nested leg record ----

	Plan_Leg leg_rec;

	leg_rec.Mode (file.Leg_Mode ());
	leg_rec.Type (file.Leg_Type ());
	leg_rec.ID (file.Leg_ID ());
	leg_rec.Time (file.Leg_Time ());
	leg_rec.Length (file.Leg_Length ());
	leg_rec.Cost (file.Leg_Cost ());
	leg_rec.Impedance (file.Leg_Impedance ());

	plan_rec.push_back (leg_rec);
	return (true);
}
