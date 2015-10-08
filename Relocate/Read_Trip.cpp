//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

void Relocate::Read_Trip (void)
{
	int part, num_rec, num_update, num_hhold, location, last_hhold, num_error;
	bool flag;

	Trip_File *trip_file, *new_file;
	Int_Map_Itr loc_itr;
	Trip_Index trip_index;
	Select_Data select_data;
	Select_Map_Stat map_stat;
		
	trip_file = (Trip_File *) System_File_Handle (TRIP);
	new_file = (Trip_File *) System_File_Handle (NEW_TRIP);

	last_hhold = num_rec = num_update = num_hhold = num_error = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!trip_file->Open (part)) break;
			new_file->Open (part);
		}
		if (trip_file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % trip_file->File_Type () % trip_file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % trip_file->File_Type ());
		}
		Set_Progress ();

		while (trip_file->Read ()) {
			Show_Progress ();

			new_file->Copy_Fields (*trip_file);
			flag = false;

			location = new_file->Origin ();
			loc_itr = old_new_loc.find (location);
			if (loc_itr != old_new_loc.end ()) {
				new_file->Origin (loc_itr->second);
				if (location != loc_itr->second) flag = true;
			} else {
				num_error++;
			}
			location = new_file->Destination ();
			loc_itr = old_new_loc.find (location);
			if (loc_itr != old_new_loc.end ()) {
				new_file->Destination (loc_itr->second);
				if (location != loc_itr->second) flag = true;
			} else {
				num_error++;
			}
			new_file->Write ();
			num_rec++;

			if (flag) {
				num_update++;

				trip_index.Household (new_file->Household ());
				trip_index.Person (new_file->Person ());
				trip_index.Tour (new_file->Tour ());
				trip_index.Trip (new_file->Trip ());

				select_data.Type (0);
				select_data.Partition (part);

				//---- process the record ----

				map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));

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
	trip_file->Close ();
	new_file->Close ();
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % trip_file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	Print (1, "Number of Trips Updated = ") << num_update;
	Print (1, "Number of Households Updated = ") << num_hhold;
	if (num_error > 0) {
		Print (1, "Number of Location Errors = ") << num_error;
	}
}
