//*********************************************************
//	Read_Vehicles.cpp - Read the Vehicle File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Vehicles
//---------------------------------------------------------

void Data_Service::Read_Vehicles (void)
{
	int part, num, num_rec, part_num, first;
	Vehicle_File *file = (Vehicle_File *) System_File_Handle (VEHICLE);

	Vehicle_Data vehicle_rec;
	Vehicle_Index vehicle_index;
	Vehicle_Map_Stat map_stat;

	Initialize_Vehicles (*file);
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
	
		//---- store the selection data ----

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

			vehicle_rec.Clear ();

			if (Get_Vehicle_Data (*file, vehicle_rec, part_num)) {

				vehicle_index.Household (vehicle_rec.Household());
				vehicle_index.Vehicle (vehicle_rec.Vehicle ());

				//---- process the record ----

				map_stat = vehicle_map.insert (Vehicle_Map_Data (vehicle_index, (int) vehicle_array.size ()));

				if (!map_stat.second) {
					Warning (String ("Duplicate Vehicle Record = %d-%d") % 
						vehicle_index.Household () % vehicle_index.Vehicle ());
				} else {
#ifdef HASH_MAP
					//vehicle_hash.insert (Vehicle_Hash_Data (vehicle_index, (int) vehicle_array.size ()));
#endif
					vehicle_array.push_back (vehicle_rec);
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) vehicle_array.size ();

	if (num && num != num_rec) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (VEHICLE);
}
//---------------------------------------------------------
//	Initialize_Vehicles
//---------------------------------------------------------

void Data_Service::Initialize_Vehicles (Vehicle_File &file)
{
	Required_File_Check (file, PARKING);
	Required_File_Check (file, VEHICLE_TYPE);
	
	int percent = System_Data_Reserve (VEHICLE);

	if (vehicle_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (Transit_Veh_Flag ()) {
			num += line_array.Num_Runs ();
		}
		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			vehicle_array.reserve (num);
			if (num > (int) vehicle_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Vehicle_Data
//---------------------------------------------------------

bool Data_Service::Get_Vehicle_Data (Vehicle_File &file, Vehicle_Data &vehicle_rec, int partition)
{
	int hhold, veh, parking, type;
	Int_Map_Itr map_itr;

	//---- check the vehicle ----

	hhold = file.Household ();
	if (hhold <= 0) return (false);

	vehicle_rec.Household (hhold);

	veh = file.Vehicle ();

	if (file.Version () <= 40) {
		veh = Fix_Vehicle_ID (veh);
	}
	vehicle_rec.Vehicle (veh);

	parking = file.Parking ();

	map_itr = parking_map.find (parking);
	if (map_itr == parking_map.end ()) {
		if (System_Data_Flag (PARKING)) {
			Warning (String ("Household %d Vehicle %d Parking %d was Not Found") % hhold % veh % parking);
			return (false);
		} else {
			vehicle_rec.Parking (parking);
		}
	} else {
		vehicle_rec.Parking (map_itr->second);
	}

	type = file.Type ();

	vehicle_rec.Type (type);

	if (file.SubType_Flag () && file.Version () <= 40) {
		vehicle_rec.Type (VehType40_Map (type, file.SubType ()));
	}
	if (vehicle_rec.Type () > 0) {
		map_itr = veh_type_map.find (vehicle_rec.Type ());
		if (map_itr == veh_type_map.end ()) {
			if (System_Data_Flag (VEHICLE_TYPE)) {
				Warning (String ("Household %d Vehicle %d Type %d was Not Found") % hhold % veh % vehicle_rec.Type ());
				vehicle_rec.Type (0);
			}
		} else {
			vehicle_rec.Type (map_itr->second);
		}
	} else {
		vehicle_rec.Type (0);
	}
	if (file.Partition_Flag ()) {
		vehicle_rec.Partition (file.Partition ());
	} else {
		vehicle_rec.Partition (partition);
	}
	//vehicle_rec.Notes (file.Notes ());

	return (true);
}
