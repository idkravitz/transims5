//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TripPrep::Execute (void)
{
#ifdef BOOST_THREADS		
	boost::thread_group threads;
#endif
	int part, num;

	//---- check the user script ----

	if (script_flag && Master ()) {
		Db_Base_Array files;

		files.push_back (trip_file);

		Write (1, "Compiling Processing Script");

		if (Report_Flag (PRINT_SCRIPT)) {
			Header_Number (PRINT_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		program.Initialize (files);
		program.Read_Only (0, false);

		if (!program.Compile (program_file, Report_Flag (PRINT_SCRIPT))) {
			Error ("Compiling Processing Script");
		}
		if (Report_Flag (PRINT_STACK)) {
			Header_Number (PRINT_STACK);

			program.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- read the network data ----

	Data_Service::Execute ();

	//---- build the location-parking map ----

	if (make_veh_flag || check_flag) {
		int diff, best_diff, best_park;
		Location_Itr loc_itr;
		Int_Map_Itr map_itr;
		Parking_Data *park_ptr;
		Parking_Itr park_itr;

		for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
			map_itr = parking_map.find (loc_itr->Location ());
			if (map_itr != parking_map.end ()) {
				park_ptr = &parking_array [map_itr->second];
				if (park_ptr->Link_Dir () == loc_itr->Link_Dir () && park_ptr->Offset () == loc_itr->Offset ()) {
					location_parking.insert (Int_Map_Data (loc_itr->Location (), park_ptr->Parking ()));
					continue;
				}
			}
			best_diff = MAX_INTEGER;
			best_park = 0;

			for (park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++) {
				diff = abs (park_itr->Offset () - loc_itr->Offset ());
				if (park_itr->Link_Dir () == loc_itr->Link_Dir () && diff < best_diff) {
					best_park = park_itr->Parking ();
					if (diff == 0) break;
					best_diff = diff;
				}			
			}
			location_parking.insert (Int_Map_Data (loc_itr->Location (), best_park));
		}
	}

	//---- check the activity patterns ----

	if (check_flag) {
		Check_Trips ();
		Exit_Stat (DONE);
	}

	//---- merge vehicle data ----

	if (merge_veh_flag) {
		Merge_Vehicles ();
	}

	//---- update file partitions ----

	if (update_flag) {
		if (select_flag) {
			num = select_map.Max_Partition () + 1;
		} else {
			num = num_parts;
		}
		new_file_set.Initialize (new_trip_file, num);
	}

	//---- set the processing queue ----

	num = trip_file->Num_Parts ();

	for (part=0; part < num; part++) {
		if (part > 0 && merge_flag && !merge_file.Find_File (part)) {
			Error (String ("%s %d was Not Found") % merge_file.File_Type () % part);
		}
		partition_queue.Put (part);
	}
	trip_file->Close ();
	trip_file->Reset_Counters ();

	merge_file.Close ();
	merge_file.Reset_Counters ();

	partition_queue.End_of_Queue ();

	//---- processing threads ---

	trip_processing = new Trip_Processing * [Num_Threads ()];

	for (int i=0; i < Num_Threads (); i++) {
		trip_processing [i] = new Trip_Processing (this, i);

#ifdef BOOST_THREADS
		if (Num_Threads () > 1) {
			threads.create_thread (boost::ref (*(trip_processing [i])));
		}
#endif
	}

	if (Num_Threads () > 1) {
#ifdef BOOST_THREADS
		threads.join_all ();
#endif
	} else {
		(**trip_processing) ();
	}

	//---- write the vehicle file ----

	if (vehicle_flag) {
		Write_Vehicles ();
	} else if (make_veh_flag && !veh_part_flag) {
		Vehicle_Map_Itr map_itr;
		Vehicle_Index veh_index;

		Show_Message (String ("Writing %s -- Record") % new_veh_file->File_Type ());
		Set_Progress ();

		for (map_itr = vehicle_map.begin (); map_itr != vehicle_map.end (); map_itr++) {
			Show_Progress ();
			veh_index = map_itr->first;

			new_veh_file->Household (veh_index.Household ());
			new_veh_file->Vehicle (veh_index.Vehicle ());
			new_veh_file->Parking (map_itr->second);
			new_veh_file->Type (1);

			new_veh_file->Write ();
		}
		End_Progress ();
		new_veh_file->Close ();
		Print (2, "Number of New Vehicle Records = ") << Progress_Count ();
	}

	//---- combine trips ----

	if (combine_flag) {
		Combine_Trips ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- write the selection file ----

	if (new_select_flag) {
		Write_Selections ();
	}

	//---- report the records per partition ----

	if (update_flag && !select_flag) {
		Int_Itr int_itr;
		double total = select_map.size () / 100.0;
		if (total == 0.0) total = 1.0;

		Break_Check (num_parts + 3);
		Print (2, "Partition Distribution");

		for (part=0, int_itr = part_count.begin (); int_itr != part_count.end (); int_itr++, part++) {
			Print (1, String ("%5d %10d (%5.1lf%%)") % part % *int_itr % (*int_itr / total) % FINISH);
		}
		Print (1, String ("Total %10d") % (int) select_map.size ());
	}

	//---- print processing summary ----

	trip_file->Print_Summary ();

	if (merge_flag) {
		merge_file.Print_Summary ();
	}
	if (System_File_Flag (NEW_TRIP)) {
		if (update_flag) {
			num = (int) new_file_set.size ();
			for (int p=0; p < num; p++) {
				new_trip_file->Add_Counters (new_file_set [p]);
			}
		}
		new_trip_file->Print_Summary ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TripPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Processing Script ----
			Print (1, "Trip Processing Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Processing Stack ----
			Print (1, "Trip Processing Stack");
			Print (1);
			break;
		default:
			break;
	}
}

