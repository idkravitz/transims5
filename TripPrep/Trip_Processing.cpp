//*********************************************************
//	Trip_Processing.cpp - trip processing thread
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Trip_Processing constructor
//---------------------------------------------------------

TripPrep::Trip_Processing::Trip_Processing (TripPrep *_exe, int num)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	num_temp = 0;

	if (exe->update_flag && !exe->select_flag) {
		random_part.Seed (exe->random.Seed () + 1000 * (num + 1));
		if (thread_flag) part_count.assign (exe->num_parts, 0);
	}
	trip_file.File_Access (exe->trip_file->File_Access ());
	trip_file.Dbase_Format (exe->trip_file->Dbase_Format ());
	trip_file.Part_Flag (exe->trip_file->Part_Flag ());
	trip_file.Pathname (exe->trip_file->Pathname ());
	trip_file.First_Open (false);

	if (exe->new_trip_flag) {
		new_trip_file.File_Type (exe->new_trip_file->File_Type ());
		new_trip_file.File_Access (exe->new_trip_file->File_Access ());
		new_trip_file.Dbase_Format (exe->new_trip_file->Dbase_Format ());
		new_trip_file.Part_Flag (exe->new_trip_file->Part_Flag ());
		new_trip_file.Pathname (exe->new_trip_file->Pathname ());
		new_trip_file.Sort_Type (exe->new_trip_file->Sort_Type ());
		new_trip_file.First_Open (false);
	}
	if (exe->merge_flag) {
		merge_file.File_Access (exe->merge_file.File_Access ());
		merge_file.Dbase_Format (exe->merge_file.Dbase_Format ());
		merge_file.Part_Flag (exe->merge_file.Part_Flag ());
		merge_file.Pathname (exe->merge_file.Pathname ());
		merge_file.First_Open (false);
	}
	if (exe->make_veh_flag) {
		if (exe->veh_part_flag) {
			new_veh_file.File_Access (exe->new_veh_file->File_Access ());
			new_veh_file.Dbase_Format (exe->new_veh_file->Dbase_Format ());
			new_veh_file.Part_Flag (exe->new_veh_file->Part_Flag ());
			new_veh_file.Pathname (exe->new_veh_file->Pathname ());
			new_veh_file.First_Open (false);
		}
	}

	//---- compile the user script ----

	if (exe->script_flag) {
		Db_Base_Array files;

		files.push_back (&trip_file);

		program.Initialize (files, exe->random.Seed () + 100 * (num + 1));
		program.Read_Only (0, false);

		program.Replicate (&exe->program);
	}
}

//---------------------------------------------------------
//	Trip_Processing operator
//---------------------------------------------------------

void TripPrep::Trip_Processing::operator()()
{
	int part = 0;
	while (exe->partition_queue.Get (part)) {
		Read_Trips (part);

		num_temp = 0;
		trip_ptr_array.clear ();
		traveler_sort.clear ();
		time_sort.clear ();
	}
	MAIN_LOCK
	exe->trip_file->Add_Counters (&trip_file);
	trip_file.Close ();

	if (exe->new_trip_flag) {
		exe->new_trip_file->Add_Counters (&new_trip_file);
		new_trip_file.Close ();
	}
	if (exe->merge_flag) {
		exe->merge_file.Add_Counters (&merge_file);
		merge_file.Close ();
	}
	if (exe->update_flag && !exe->select_flag && thread_flag) {
		for (part = 0; part < exe->num_parts; part++) {
			exe->part_count [part] += part_count [part];
		}
		exe->select_map.insert (select_map.begin (), select_map.end ());
	}
	if (exe->make_veh_flag) {
		if (exe->veh_part_flag) {
			new_veh_file.Close ();
		} else {
			exe->vehicle_map.insert (vehicle_map.begin (), vehicle_map.end ());
		}
	}
	END_LOCK
}
