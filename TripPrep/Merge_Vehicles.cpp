//*********************************************************
//	Merge_Vehicles.cpp - merge the vehicle data
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Merge_Vehicles
//---------------------------------------------------------

void TripPrep::Merge_Vehicles (void)
{
	int part, num_rec, num_kept, num_dup, part_num, hhold, veh, parking, type;

	Int_Map_Itr map_itr;
	Vehicle_Data vehicle_rec;
	Vehicle_Index vehicle_index;
	Vehicle_Map_Stat map_stat;

	num_rec = num_kept = num_dup = 0;

	//---- check the partition number ----

	if (merge_veh_file.Part_Flag () && First_Partition () != merge_veh_file.Part_Number ()) {
		merge_veh_file.Open (0);
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!merge_veh_file.Open (part)) break;
		}
	
		//---- store the selection data ----

		if (merge_veh_file.Part_Flag ()) {
			part_num = merge_veh_file.Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % merge_veh_file.File_Type () % part_num);
		} else {
			part_num = part;
			Show_Message (String ("Reading %s -- Record") % merge_veh_file.File_Type ());
		}
		Set_Progress ();

		while (merge_veh_file.Read ()) {
			Show_Progress ();

			vehicle_rec.Clear ();

			//---- check the vehicle ----

			hhold = merge_veh_file.Household ();
			if (hhold <= 0) continue;

			num_rec++;
			vehicle_rec.Household (hhold);

			veh = merge_veh_file.Vehicle ();

			if (merge_veh_file.Version () <= 40) {
				veh = Fix_Vehicle_ID (veh);
			}
			vehicle_rec.Vehicle (veh);

			parking = merge_veh_file.Parking ();

			map_itr = parking_map.find (parking);
			if (map_itr == parking_map.end ()) {
				if (System_Data_Flag (PARKING)) {
					Warning (String ("Household %d Vehicle %d Parking %d was Not Found") % hhold % veh % parking);
					continue;
				} else {
					vehicle_rec.Parking (parking);
				}
			} else {
				vehicle_rec.Parking (map_itr->second);
			}

			type = merge_veh_file.Type ();

			vehicle_rec.Type (type);

			if (merge_veh_file.SubType_Flag () && merge_veh_file.Version () <= 40) {
				vehicle_rec.Type (VehType40_Map (type, merge_veh_file.SubType ()));
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
			if (merge_veh_file.Partition_Flag ()) {
				vehicle_rec.Partition (merge_veh_file.Partition ());
			} else {
				vehicle_rec.Partition (part_num);
			}

			//---- add to the data map ----

			vehicle_index.Household (vehicle_rec.Household());
			vehicle_index.Vehicle (vehicle_rec.Vehicle ());

			//---- process the record ----

			map_stat = vehicle_map.insert (Vehicle_Map_Data (vehicle_index, (int) vehicle_array.size ()));

			if (!map_stat.second) {
				num_dup++;
			} else {
				vehicle_array.push_back (vehicle_rec);
				num_kept++;
			}
		}
		End_Progress ();
	}
	merge_veh_file.Close ();

	Print (2, String ("Number of %s Records = %d") % merge_veh_file.File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	if (num_rec != num_kept) {
		Print (1, "Number of Merged Vehicle Records = ") << num_kept;
		Print (1, "Number of Duplicate Vehicle Records = ") << num_dup;
	}
}
