//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TripPrep::Program_Control (void)
{
	String key;

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	trip_file = (Trip_File *) System_File_Handle (TRIP);
	if (!trip_file->Part_Flag ()) Num_Threads (1);

	if (System_File_Flag (NEW_TRIP)) {
		new_trip_file = (Trip_File *) System_File_Handle (NEW_TRIP);
		if (Trip_Sort () == UNKNOWN_SORT) {
			new_trip_file->Sort_Type (trip_file->Sort_Type ());
		} else {
			new_trip_file->Sort_Type (Trip_Sort ());
		}
		new_trip_file->Close ();
		new_trip_flag = true;

		if (trip_file->Part_Flag () && !new_trip_file->Part_Flag ()) {
			pathname = new_trip_file->Pathname ();
			new_format = new_trip_file->Dbase_Format ();

			if (MPI_Size () > 1 && Master ()) {
				pathname += ".0";
			}
			if (Trip_Sort () == TRAVELER_SORT || Trip_Sort () == TIME_SORT) {
				combine_flag = true;
				new_trip_file->File_Type ("Temporary File");
				new_trip_file->Dbase_Format (BINARY);
				new_trip_file->Part_Flag (true);
				new_trip_file->Pathname (pathname + ".temp");
			} else {
				output_flag = true;
				new_trip_file->Pathname (pathname);
				if (MPI_Size () > 1) {
					new_trip_file->Dbase_Format (BINARY);
				}
				Num_Threads (1);
			}
		}
	}
	select_flag = System_File_Flag (SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- merge trip file ----

	key = Get_Control_String (MERGE_TRIP_FILE);

	if (!key.empty ()) {
		merge_file.File_Type ("Merge Trip File");
		merge_file.Filename (Project_Filename (key));
		merge_flag = true;

		key = Get_Control_String (MERGE_TRIP_FORMAT);
		if (!key.empty ()) {
			merge_file.Dbase_Format (key);
		}
		merge_file.Open (0);

		if (Num_Threads () > 1 && !merge_file.Part_Flag ()) {
			Error ("Partitioned Input Trips require Partitioned Merge Trips");
		}
	}

	//---- merge vehicle file ----

	if (System_File_Flag (VEHICLE) && System_File_Flag (NEW_VEHICLE)) {
		//if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || !System_File_Flag (PARKING)) {
		//	Error ("Node, Link, and Parking files are Required for Vehicle Processing");
		//}
		vehicle_flag = true;
		veh_part_flag = ((Vehicle_File *) System_File_Handle (NEW_VEHICLE))->Part_Flag ();

		key = Get_Control_String (MERGE_VEHICLE_FILE);

		if (!key.empty ()) {
			merge_veh_file.File_Type ("Merge Vehicle File");
			merge_veh_file.Filename (Project_Filename (key));
			merge_veh_flag = true;

			key = Get_Control_String (MERGE_VEHICLE_FORMAT);
			if (!key.empty ()) {
				merge_veh_file.Dbase_Format (key);
			}
			merge_veh_file.Open (0);
		}
	} else if (Check_Control_Key (MERGE_VEHICLE_FILE)) {
		Error ("Input and Output Vehicle files are Required for Vehicle Merging");
	}

	//---- maximum sort size ----

	if (Trip_Sort () == TRAVELER_SORT || Trip_Sort () == TIME_SORT) {
		sort_size = Get_Control_Integer (MAXIMUM_SORT_SIZE);
	}

	//---- update trip partitions ----

	if (Check_Control_Key (UPDATE_TRIP_PARTITIONS)) {
		update_flag = Get_Control_Flag (UPDATE_TRIP_PARTITIONS);
	} else if (!trip_file->Part_Flag () && new_trip_flag && 
		new_trip_file->Part_Flag () && select_flag && !merge_flag) {
		update_flag = true;
	}

	if (update_flag) {
		if (!new_trip_flag) {
			Error ("A New Trip File is Required to Update Partitions");
		} else if (!new_trip_file->Part_Flag ()) {
			Error ("The New Trip File must be Partitioned to be Updated");
		}
		if (select_flag) {
			Selection_File *file = (Selection_File *) System_File_Handle (SELECTION);
			if (!file->Partition_Flag () && !file->Part_Flag ()) {
				Error ("A Partition Field or File was Not Found on the Selection File");
			}
		} else if (!Check_Control_Key (NUMBER_OF_PARTITIONS)) {
			Error ("A Selection File or Number of Partitions is Required for Partition Updates");
		} else {

			//---- get the number of partitions ----

			num_parts = Get_Control_Integer (NUMBER_OF_PARTITIONS);
			part_count.assign (num_parts, 0);

			if (num_parts > 1 && System_File_Flag (NEW_SELECTION)) {
				Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);
				
				if (!file->Part_Flag ()) {
					file->Clear_Fields ();
					file->Partition_Flag (true);
					file->Create_Fields ();
					file->Write_Header ();
				}
				new_select_flag = true;
			}
		}
	}

	//---- update traveler type ----

	type_flag = Get_Control_Flag (UPDATE_TRAVELER_TYPE);

	//---- read trip processing script ----

	key = Get_Control_String (TRIP_PROCESSING_SCRIPT);

	if (!key.empty ()) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Trip Processing Script");
		program_file.Open (Project_Filename (key));
	}

	if (type_flag && !script_flag) {
		if (select_flag) {
			Selection_File *file = (Selection_File *) System_File_Handle (SELECTION);
			if (!file->Type_Flag ()) {
				Error ("A Type Field was Not Found in the Selection File");
			}
		} else {
			Error ("A Selection File or Processing Script is Required for Traveler Type Updates");
		}
	}

	//---- sort household tours ----

	sort_tours = Get_Control_Flag (SORT_HOUSEHOLD_TOURS);

	if (sort_tours && (Trip_Sort () != UNKNOWN_SORT || merge_flag)) {
		if (merge_flag) {
			Error ("Tour Sorting is Incompatible with Trip Merging");
		} else {
			Error ("Tour Sorting is Incompatible with other Sorting Options");
		}
	}

	//---- synthesize vehicles ----

	if (!System_File_Flag (VEHICLE) && System_File_Flag (NEW_VEHICLE)) {
		make_veh_flag = Get_Control_Flag (SYNTHESIZE_VEHICLES);
		new_veh_file = (Vehicle_File *) System_File_Handle (NEW_VEHICLE);
		veh_part_flag = new_veh_file->Part_Flag ();

		if (make_veh_flag) {
			if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || 
				!System_File_Flag (PARKING) || !System_File_Flag (LOCATION)) {
				Error ("Network Files are Required to Synthesize Vehicles");
			}
		}
	}

	//---- check activity patterns ----

	check_flag = Get_Control_Flag (CHECK_ACTIVITY_PATTERNS);

	if (check_flag) {
		if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || 
			!System_File_Flag (PARKING) || !System_File_Flag (LOCATION)) {
			Error ("Network Files are Required to Check Activity Patterns");
		}

		//---- add a message to the new trip file ----

		if (new_trip_flag) {
			new_trip_file->Add_Field ("Notes", DB_STRING, 40);
		}
	} else if (System_File_Flag (VEHICLE) && !System_File_Flag (NEW_VEHICLE)) {
		System_File_False (VEHICLE);
	}
}
