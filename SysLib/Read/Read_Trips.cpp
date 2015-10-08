//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void Data_Service::Read_Trips (void)
{
	int part, num, num_rec, part_num, first;
	Trip_File *file = (Trip_File *) System_File_Handle (TRIP);

	Trip_Data trip_rec;
	Trip_Index trip_index;
	Trip_Map_Stat map_stat;

	Initialize_Trips (*file);
	num_rec = first = 0;

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

		//---- store the trip data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			trip_rec.Clear ();

			if (Get_Trip_Data (*file, trip_rec, part_num)) {
				trip_rec.Get_Trip_Index (trip_index);

				//---- process the record ----

				map_stat = trip_map.insert (Trip_Map_Data (trip_index, (int) trip_array.size ()));

				if (!map_stat.second) {
					Warning (String ("Duplicate Trip Record = %d-%d-%d-%d") % 
						trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				} else {
					trip_array.push_back (trip_rec);
					file->Add_Trip (trip_index.Household (), trip_index.Person (), trip_index.Tour ());
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) trip_array.size ();

	if (num && num != num_rec) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (TRIP);
}

//---------------------------------------------------------
//	Initialize_Trips
//---------------------------------------------------------

void Data_Service::Initialize_Trips (Trip_File &file)
{
	Required_File_Check (file, LOCATION);
	Required_File_Check (file, VEHICLE);
	
	int percent = System_Data_Reserve (TRIP);

	if (trip_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			trip_array.reserve (num);
			if (num > (int) trip_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool Data_Service::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	int hhold, lvalue;
	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	//---- check the household id ----

	hhold = file.Household ();
	if (hhold < 1) return (false);

	trip_rec.Household (hhold);

	trip_rec.Person (file.Person ());
	trip_rec.Tour (MAX (file.Tour (), 1));
	trip_rec.Trip (file.Trip ());

	trip_rec.Start (file.Start ());
	trip_rec.End (file.End ());

	//---- convert the origin ----

	lvalue = file.Origin ();

	map_itr = location_map.find (lvalue);

	if (map_itr == location_map.end ()) {
		Warning (String ("Trip %d Origin %d was Not Found") % Progress_Count () % lvalue);
		return (false);
	}
	trip_rec.Origin (map_itr->second);

	//---- convert the destination ----

	lvalue = file.Destination ();

	map_itr = location_map.find (lvalue);

	if (map_itr == location_map.end ()) {
		Warning (String ("Trip %d Destination %d was Not Found") % Progress_Count () % lvalue);
		return (false);
	}
	trip_rec.Destination (map_itr->second);

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
	trip_rec.Vehicle (lvalue);

	if (file.Version () <= 40) {
		trip_rec.Mode (Trip_Mode_Map (file.Mode ()));
	} else {
		trip_rec.Mode (file.Mode ());
	}
	trip_rec.Purpose (file.Purpose ());
	trip_rec.Constraint (file.Constraint ());
	trip_rec.Passengers (file.Passengers ());

	trip_rec.Type (file.Type ());

	if (file.Partition_Flag ()) {
		trip_rec.Partition (file.Partition ());
	} else {
		trip_rec.Partition (partition);
	}	
	//trip_rec.Notes (file.Notes ());

	return (true);
}
