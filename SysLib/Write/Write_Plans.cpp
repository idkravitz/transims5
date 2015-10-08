//*********************************************************
//	Write_Plans.cpp - write a new plan file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

void Data_Service::Write_Plans (void)
{
	int part, part_num, max_part, count, total;
	bool first;

	Plan_File *file = (Plan_File *) System_File_Handle (NEW_PLAN);

	Trip_Map_Itr trip_itr;
	Time_Map_Itr time_itr;
	Plan_Itr plan_itr;
	Plan_Leg_Itr itr;
	Plan_Data *plan_ptr;

	total = 0;

	//---- process each partition ----

	for (part = max_part = 0; part <= max_part; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}
		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Writing %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part;
			Show_Message (String ("Writing %s -- Record") % file->File_Type ());
		}
		Set_Progress ();
		count = 0;

		for (first = true; ; first = false) {
			if (Trip_Sort () == TRAVELER_SORT) {
				if (first) {
					trip_itr = plan_trip_map.begin ();
				} else {
					trip_itr++;
				}
				if (trip_itr == plan_trip_map.end ()) break;
				plan_ptr = &plan_array [trip_itr->second];
			} else if (Trip_Sort () == TIME_SORT) {
				if (first) {
					time_itr = plan_time_map.begin ();
				} else {
					time_itr++;
				}
				if (time_itr == plan_time_map.end ()) break;
				plan_ptr = &plan_array [time_itr->second];
			} else {
				if (first) {
					plan_itr = plan_array.begin ();
				} else {
					plan_itr++;
				}
				if (plan_itr == plan_array.end ()) break;
				plan_ptr = &(*plan_itr);
			}
			Show_Progress ();

			if (file->Part_Flag () && plan_ptr->Partition () != part_num) {
				if (plan_ptr->Partition () > max_part) {
					max_part = plan_ptr->Partition ();
				}
				continue;
			} else if (plan_ptr->Partition () < 0) {
				continue;
			}
			count += Put_Plan_Data (*file, *plan_ptr);
		}
		Show_Progress (count);
		End_Progress ();
		total += count;
	}
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % total);
	if (part > 1) Print (0, String (" (%d files)") % part);
}

//---------------------------------------------------------
//	Put_Plan_Data
//---------------------------------------------------------

int Data_Service::Put_Plan_Data (Plan_File &file, Plan_Data &data)
{
	int count, veh;

	Trip_Map_Itr trip_itr;
	Time_Map_Itr time_itr;
	Plan_Itr plan_itr;
	Plan_Leg_Itr itr;

	Location_Data *loc_ptr;
	Vehicle_Data *veh_ptr;

	count = 0;

	file.Household (data.Household ());
	file.Person (data.Person ());
	file.Tour (MAX (data.Tour (), 1));
	file.Trip (data.Trip ());
	file.Start (data.Start ());
	file.End (data.End ());
	file.Duration (data.Duration ());

	loc_ptr = &location_array [data.Origin ()];
	file.Origin (loc_ptr->Location ());

	loc_ptr = &location_array [data.Destination ()];
	file.Destination (loc_ptr->Location ());

	file.Purpose (data.Purpose ());
	file.Mode (data.Mode ());
	file.Constraint (data.Constraint ());
	file.Priority (data.Priority ());

	veh = data.Vehicle ();

	if (veh < 0) {
		veh = 0;
	} else {
		veh_ptr = &vehicle_array [veh];
		veh = veh_ptr->Vehicle ();
	}
	file.Vehicle (veh);

	file.Passengers (data.Passengers ());
	file.Type (data.Type ());
	file.Partition (data.Partition ());

	file.Depart (data.Depart ());
	file.Arrive (data.Arrive ());
	file.Activity (data.Activity ());
	file.Walk (data.Walk ());
	file.Drive (data.Drive ());
	file.Transit (data.Transit ());
	file.Wait (data.Wait ());
	file.Other (data.Other ());
	file.Length (UnRound (data.Length ()));
	file.Cost (UnRound (data.Cost ()));
	file.Impedance (data.Impedance ());

	file.Num_Nest ((int) data.size ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;
	file.Add_Trip (data.Household (), data.Person (), data.Tour ());

	//---- write the nested records ----

	for (Plan_Leg_Itr itr = data.begin (); itr != data.end (); itr++) {
		file.Leg_Mode (itr->Mode ());
		file.Leg_Type (itr->Type ());
		file.Leg_ID (itr->ID ());
		file.Leg_Time (itr->Time ());
		file.Leg_Length (UnRound (itr->Length ()));
		file.Leg_Cost (UnRound (itr->Cost ()));
		file.Leg_Impedance (itr->Impedance ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		file.Add_Leg ();
		count++;
	}
	return (count);
}
