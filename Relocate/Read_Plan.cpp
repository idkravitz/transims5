//*********************************************************
//	Read_Plan.cpp - Read the Plan File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void Relocate::Read_Plan (void)
{
	int part, num_trips, num_update, num_hhold, location, last_hhold, num_error;
	bool flag;

	Plan_File *plan_file, *new_file;
	Plan_Data plan;
	Plan_Leg_Itr leg_itr;
	Int_Map_Itr loc_itr;
	Trip_Index trip_index;
	Select_Data select_rec;
	Select_Map_Stat map_stat;
		
	plan_file = (Plan_File *) System_File_Handle (PLAN);
	new_file = (Plan_File *) System_File_Handle (NEW_PLAN);

	last_hhold = num_trips = num_update = num_hhold = num_error = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!plan_file->Open (part)) break;
			new_file->Open (part);
		}
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % plan_file->File_Type ());
		}
		Set_Progress ();

		while (plan_file->Read_Plan (plan)) {
			Show_Progress ();

			num_trips++;
			flag = false;

			location = plan.Origin ();
			loc_itr = old_new_loc.find (location);
			if (loc_itr != old_new_loc.end ()) {
				plan.Origin (loc_itr->second);
				if (location != loc_itr->second) flag = true;
			} else {
				num_error++;
			}
			location = plan.Destination ();
			loc_itr = old_new_loc.find (location);
			if (loc_itr != old_new_loc.end ()) {
				plan.Destination (loc_itr->second);
				if (location != loc_itr->second) flag = true;
			} else {
				num_error++;
			}

			//---- update plan legs ----

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
				if (leg_itr->Type () == LOCATION_ID) {
					location = leg_itr->ID ();
					loc_itr = old_new_loc.find (location);
					if (loc_itr != old_new_loc.end ()) {
						leg_itr->ID (loc_itr->second);
						if (location != loc_itr->second) flag = true;
					} else {
						num_error++;
					}
				}
				if (leg_itr->Type () == PARKING_ID) {
					location = leg_itr->ID ();
					loc_itr = old_new_park.find (location);
					if (loc_itr != old_new_park.end ()) {
						leg_itr->ID (loc_itr->second);
						if (location != loc_itr->second) flag = true;
					} else {
						num_error++;
					}
				}
			}
			new_file->Write_Plan (plan);

			if (flag) {

				//---- save the selection data ----
		
				plan.Get_Trip_Index (trip_index);

				select_rec.Type (plan.Type ());
				select_rec.Partition (plan_file->Part_Number ());

				map_stat = select_map.insert (Select_Map_Data (trip_index, select_rec));

				if (map_stat.second) {
					if (part > select_map.Max_Partition ()) {
						select_map.Max_Partition (part);
					}
					if (trip_index.Household () > select_map.Max_Household ()) {
						select_map.Max_Household (trip_index.Household ());
					}
					if (trip_index.Household () != last_hhold) {
						last_hhold = trip_index.Household ();
						select_map.Add_Household ();
						num_hhold++;
					}
				}
			}
		}
	}
	plan_file->Close ();
	new_file->Close ();
	End_Progress ();

	Print (2, String ("Number of %s Trips = %d") % plan_file->File_Type () % num_trips);
	if (part > 1) Print (0, String (" (%d files)") % part);

	Print (1, "Number of Trips Updated = ") << num_update;
	Print (1, "Number of Households Updated = ") << num_hhold;
	if (num_error > 0) {
		Print (1, "Number of Location Errors = ") << num_error;
	}
}
