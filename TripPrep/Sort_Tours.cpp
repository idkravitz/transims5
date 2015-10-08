//*********************************************************
//	Sort_Tours.cpp - sort and write household tours
//*********************************************************

#include "TripPrep.hpp"
#include "Tour_Sort.hpp"

//---------------------------------------------------------
//	Sort_Tours
//---------------------------------------------------------

void TripPrep::Trip_Processing::Sort_Tours (void)
{
	int i, home, tour, trip, person, vehicle, parking;
	bool first_flag;

	Trip_Data *trip_ptr, *last_trip;
	Trip_Ptr_Itr trip_itr;
	Tour_Sort sort_data;
	Tour_Sort_Map sort_map;
	Tour_Sort_Map_Stat sort_stat;
	Tour_Sort_Map_Itr sort_itr;
	Vehicle_Index veh_index;
	Int_Map_Itr map_itr;

	//---- sort the person

	for (i=0, trip_itr = trip_ptr_array.begin (); trip_itr != trip_ptr_array.end (); trip_itr++, i++) {
		trip_ptr = *trip_itr;

		sort_data.Set (trip_ptr->Person (), trip_ptr->Start (), trip_ptr->End (), trip_ptr->Tour (), trip_ptr->Trip ());

		sort_stat = sort_map.insert (Tour_Sort_Map_Data (sort_data, i));

		if (!sort_stat.second) {
			exe->Warning (String ("Duplicate Tour Sort (%d-%d-%d-%d)") %
				trip_ptr->Household () % trip_ptr->Person () % trip_ptr->Tour () % trip_ptr->Trip ());
		}
	}

	//---- renumber the person tours and trips ----

	last_trip = 0;
	home = tour = trip = person = 0;

	for (sort_itr = sort_map.begin (); sort_itr != sort_map.end (); sort_itr++) {
		trip_ptr = trip_ptr_array [sort_itr->second];

		if (person != trip_ptr->Person ()) {
			person = trip_ptr->Person ();
			tour = trip = 1;
			home = trip_ptr->Origin ();
			last_trip = 0;
		} else if (trip_ptr->Origin () == home) {
			tour++;
			trip = 1;
		} else {
			trip++;
		}
		if (last_trip != 0) {
			last_trip->Duration (trip_ptr->Start () - last_trip->End ());
		}
		trip_ptr->Tour (tour);
		trip_ptr->Trip (trip);
		last_trip = trip_ptr;
	}

	//---- save the sorted tours ----

	vehicle = person = parking = 0;
	first_flag = true;
	
	for (sort_itr = sort_map.begin (); sort_itr != sort_map.end (); sort_itr++) {
		trip_ptr = trip_ptr_array [sort_itr->second];

		if (exe->make_veh_flag) {
			if (person != trip_ptr->Person ()) {
				person = trip_ptr->Person ();
				parking = 0;
			}
			if (trip_ptr->Mode () == DRIVE_MODE) {
				if (parking > 0) {
					map_itr = exe->location_parking.find (trip_ptr->Origin ());

					if (map_itr != exe->location_parking.end ()) {
						if (parking != map_itr->second) {
							parking = 0;
						}
					} else {
						parking = 0;
					}
				}
				if (parking == 0) {
					vehicle++;
					map_itr = exe->location_parking.find (trip_ptr->Origin ());

					if (map_itr != exe->location_parking.end ()) {
						parking = map_itr->second;
					} else {
						parking = trip_ptr->Origin ();
					}
					if (exe->veh_part_flag) {
						new_veh_file.Household (trip_ptr->Household ());
						new_veh_file.Vehicle (vehicle);
						new_veh_file.Parking (parking);
						new_veh_file.Type (1);

						new_veh_file.Write ();
					} else {
						veh_index.Household (trip_ptr->Household ());
						veh_index.Vehicle (vehicle);
						vehicle_map.insert (Vehicle_Map_Data (veh_index, parking));
					}
				}
				trip_ptr->Vehicle (vehicle);

				map_itr = exe->location_parking.find (trip_ptr->Destination ());
				if (map_itr != exe->location_parking.end ()) {
					parking = map_itr->second;
				} else {
					parking = 0;
				}
			} else {
				trip_ptr->Vehicle (0);
			}
		}
		new_trip_file.Write_Trip (*trip_ptr);

		delete trip_ptr;
		trip_ptr = 0;

		trip_ptr_array [sort_itr->second] = trip_ptr;
	}
	trip_ptr_array.clear ();
}
