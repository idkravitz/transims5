//*********************************************************
//	Check_Trips.cpp - check the activity patterns
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Check_Trips
//---------------------------------------------------------

void TripPrep::Check_Trips (void)
{
	int last_hhold, last_person;

	Trip_Array trip_array;
	Trip_Data trip_data;

	Show_Message ("Checking Activity Patterns");

	if (new_trip_flag) {
		new_trip_file->Open (0);
	}
	vehicle_flag = System_File_Flag (VEHICLE);

	last_hhold = last_person = -1;

	while (trip_file->Read_Trip (trip_data)) {
		Show_Progress ();

		if (trip_data.Household () != last_hhold || trip_data.Person () != last_person) {
			if (last_hhold >= 0) {
				Person_Trips (trip_array);
			}
			last_hhold = trip_data.Household ();
			last_person = trip_data.Person ();
			trip_array.clear ();
		}
		trip_array.push_back (trip_data);
	}
	if (last_hhold >= 0) {
		Person_Trips (trip_array);
	}
	End_Progress ();
	
	trip_file->Print_Summary ();

	if (new_trip_flag) {
		new_trip_file->Print_Summary ();
	}
	Print (1);
	Write (1, "Number of Persons with Activity Pattern Errors = ") << error_count;
	if (check_count > 0) {
		Write (0, String (" (%.1%%)") % (100.0 * error_count / check_count) % FINISH);
	}
}

//---------------------------------------------------------
//	Person_Trips
//---------------------------------------------------------

void TripPrep::Person_Trips (Trip_Array &trip_array)
{
	int i, location, vehicle, parking, index, type, notes_field;
	Dtime last_time;
	bool drive_flag;

	Int_Map_Itr map_itr;
	Trip_Itr trip_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Vehicle_Data *vehicle_ptr;

	char *message [] = {
		"Start Time < Previous End",
		"Vehicle Not Found",
		"No Parking Lot at the Origin",
		"Vehicle Not Parked at the Origin",
		"No Parking Lot at the Destination",
		"Origin != Previous Destination",
	};

	location = vehicle = parking = type = index = -1;
	last_time = 0;
	drive_flag = false;

	if (new_trip_flag) {
		notes_field = new_trip_file->Field_Number ("Notes");
	} else {
		notes_field = -1;
	}
	check_count++;

	for (index=0, trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++, index++) {
		if (trip_itr->Start () < last_time) { type=0; goto output; }
		last_time = trip_itr->End ();

		drive_flag = (trip_itr->Mode () == DRIVE_MODE || trip_itr->Mode () == PNR_OUT_MODE || trip_itr->Mode () >= HOV2_MODE);

		if (drive_flag) {
			if (vehicle_flag) {
				if (vehicle < 0) {
					veh_index.Household (trip_itr->Household ());
					veh_index.Vehicle (trip_itr->Vehicle ());

					veh_itr = vehicle_map.find (veh_index);
					if (veh_itr == vehicle_map.end ()) { type=1; goto output; }

					vehicle = trip_itr->Vehicle ();
					vehicle_ptr = &vehicle_array [veh_itr->second];

					parking = vehicle_ptr->Parking ();
					map_itr = location_parking.find (trip_itr->Origin ());

					if (map_itr == location_parking.end ()) { type=2; goto output; }
					if (map_itr->second != parking) { type=3; goto output; }
				} else {
					if (vehicle != trip_itr->Vehicle ()) {
						veh_index.Household (trip_itr->Household ());
						veh_index.Vehicle (trip_itr->Vehicle ());

						veh_itr = vehicle_map.find (veh_index);
						if (veh_itr == vehicle_map.end ()) { type=1; goto output; }

						vehicle = trip_itr->Vehicle ();
						vehicle_ptr = &vehicle_array [veh_itr->second];

						parking = vehicle_ptr->Parking ();
						map_itr = location_parking.find (trip_itr->Origin ());

						if (map_itr == location_parking.end ()) { type=2; goto output; }
						if (map_itr->second != parking) { type=3; goto output; }
					} else {
						map_itr = location_parking.find (trip_itr->Origin ());

						if (map_itr == location_parking.end ()) { type=2; goto output; }
						if (map_itr->second != parking) { type=3; goto output; }
					}
				}
				map_itr = location_parking.find (trip_itr->Destination ());
				if (map_itr == location_parking.end ()) { type=4; goto output; }

				parking = map_itr->second;
			} else {
				if (parking > 0 && trip_itr->Origin () != parking) { type=3; goto output; }
				parking = trip_itr->Destination ();
			}
		}
		if (location > 0 && trip_itr->Origin () != location) { type=5; goto output; }
		location = trip_itr->Destination ();
	}
	return;

output:
	error_count++;

	if (new_trip_flag) {
		for (i=0, trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++, i++) {
			if (notes_field >= 0) {
				if (i == index) {
					new_trip_file->Put_Field (notes_field, message [type]);
				} else {
					new_trip_file->Put_Field (notes_field, "");
				}
			}
			new_trip_file->Write_Trip (*trip_itr);

		}
	} 
}
