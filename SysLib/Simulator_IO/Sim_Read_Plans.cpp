//*********************************************************
//	Sim_Read_Plans.cpp - Read Simulation Plans Class
//*********************************************************

#include "Sim_Read_Plans.hpp"

//---------------------------------------------------------
//	Sim_Read_Plans constructor
//---------------------------------------------------------

Sim_Read_Plans::Sim_Read_Plans () : Static_Service ()
{
	Initialize ();
}

void Sim_Read_Plans::operator()()
{
	while (exe->io_barrier.Go ()) {
		exe->io_barrier.Finish (Read_Plans ());
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Read_Plans::Initialize (Simulator_Service *_exe)
{
	num_files = first_num = -1;
	first = true;
	stat = false;

	exe = _exe;
	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Sim_Read_Plans requires Simulator Base");
	}
}

//---------------------------------------------------------
//	First_Plan
//---------------------------------------------------------

bool Sim_Read_Plans::First_Plan (void)
{
	first = true;
	stat = false;

	if (exe->System_File_Flag (PLAN)) {
		Plan_File *plan_file = (Plan_File *) exe->System_File_Handle (PLAN);

		if (plan_file) {
			int i, j, num;
			Plan_Data *plan_ptr;
			Time_Index *time_ptr;
	
			num_files = plan_file->Num_Parts ();
			first_num = -1;

			file_set.Initialize (plan_file, num_files);
			plan_set.Initialize (num_files);
			time_set.Initialize (num_files);
			next.assign (num_files, -1);

			//---- initialize the first index for each partition -----

			for (num=0; num < num_files; num++) {
				plan_ptr = plan_set [num];
				time_ptr = time_set [num];

				if (file_set [num]->Read_Plan (*plan_ptr)) {
					plan_ptr->Get_Time_Index (*time_ptr);
					stat = true;

					//---- sort the partition numbers by time ----

					if (first_num < 0) {
						first_num = num;
					} else {
						for (i=j=first_num; i >= 0; i = next [j = i]) {
							if (*time_ptr < *time_set [i]) {
								if (i == first_num) {
									next [num] = first_num;
									first_num = num;
								} else {
									next [j] = num;
									next [num] = i;
								}
								break;
							}
							if (next [i] < 0) {
								next [i] = num;
								next [num] = -1;
								break;
							}
						}
					}
				} else {
					time_ptr->Set (MAX_INTEGER, 0, 0);
				}
			}
		}
	}
	return (stat);
}

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

bool Sim_Read_Plans::Read_Plans (void)
{
	int i, j, num;
	Dtime time;

	Plan_Data *plan_ptr;
	Time_Index *time_ptr;

	while (stat) {
		if (first_num < 0) return (false);

		plan_ptr = plan_set [first_num];
		time_ptr = time_set [first_num];

		if (time_ptr->Household () == MAX_INTEGER) return (false);
		time = plan_ptr->Depart ();

		if (first) exe->Show_Progress ();

		if (plan_ptr->Depart () > exe->param.end_time_step) {
			stat = false;
			break;
		}
		if (plan_ptr->Depart () < time) {
			exe->Error ("Plans are Not Time Sorted");
			return (false);
		}
		if (plan_ptr->Depart () > exe->step) break;

		//---- check the selection criteria ----

		if (exe->select_households && !exe->hhold_range.In_Range (plan_ptr->Household ())) goto skip;
		if (!exe->select_mode [plan_ptr->Mode ()]) goto skip;
		if (exe->select_purposes && !exe->purpose_range.In_Range (plan_ptr->Purpose ())) goto skip;
		if (exe->select_start_times && !exe->start_range.In_Range (plan_ptr->Depart ())) goto skip;
		if (exe->select_end_times && !exe->end_range.In_Range (plan_ptr->Arrive ())) goto skip;
		if (exe->select_travelers && !exe->traveler_range.In_Range (plan_ptr->Type ())) goto skip;

		if (exe->select_vehicles && plan_ptr->Vehicle () > 0) {
			Vehicle_Index veh_index;
			Vehicle_Map_Itr veh_itr;
			Vehicle_Data *vehicle_ptr;
			Veh_Type_Data *veh_type_ptr;

			veh_index.Household (plan_ptr->Household ());
			veh_index.Vehicle (plan_ptr->Vehicle ());

			veh_itr = exe->vehicle_map.find (veh_index);
			if (veh_itr == exe->vehicle_map.end ()) goto skip;

			vehicle_ptr = &exe->vehicle_array [veh_itr->second];
			veh_type_ptr = &exe->sim_type_array [vehicle_ptr->Type ()];	

			if (!exe->vehicle_range.In_Range (veh_type_ptr->Type ())) goto skip;
		}

		if (plan_ptr->Depart () < exe->param.start_time_step) {

			//---- update the vehicle/traveler location ----

			Reposition_Plan (plan_ptr);

		} else {

			//---- process a new travel plan ----

			Process_Plan (plan_ptr);
		}
skip:
		//---- get the next record for the current partition ----

		num = first_num;
		first_num = next [num];

		if (file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Time_Index (*time_ptr);

			//---- update the record order ---

			if (first_num < 0) {
				first_num = num;
			} else {
				for (i=j=first_num; i >= 0; i = next [j = i]) {
					if (*time_ptr < *time_set [i]) {
						if (i == first_num) {
							next [num] = first_num;
							first_num = num;
						} else {
							next [j] = num;
							next [num] = i;
						}
						break;
					}
					if (next [i] < 0) {
						next [i] = num;
						next [num] = -1;
						break;
					}
				}
			}
		} else {
			time_ptr->Set (MAX_INTEGER, 0, 0);
		}
	}

	//---- aggregate the plan file statistics ----

	if (num_files > 0) {
		Plan_File *plan_file = (Plan_File *) exe->System_File_Handle (PLAN);

		plan_file->Reset_Counters ();

		for (i=0; i < num_files; i++) {
			plan_file->Add_Counters (file_set [i]);
		}
	}
	first = false;
	return (stat);
}

//---------------------------------------------------------
//	Process_Plan
//---------------------------------------------------------

void Sim_Read_Plans::Process_Plan (Plan_Data *plan_ptr)
{
	int index, traveler, partition, mode, type, dir, leg, parking, first_dir;
	string label;
	bool drive_flag, new_flag;

	Person_Index person_index;
	Person_Map_Stat person_stat;
	Sim_Traveler_Data *sim_traveler_ptr;
	Sim_Plan_Map_Itr plan_map_itr;
	Sim_Plan_Map_Stat plan_map_stat;
	Plan_Index plan_index;
	Plan_Leg_Itr leg_itr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr sim_leg_ptr;
	Sim_Leg_Data sim_leg_rec;
	Sim_Park_Ptr sim_park_ptr;

	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Vehicle_Index veh_index;
	//Vehicle_Hash_Itr veh_itr;
	Vehicle_Map_Itr veh_itr;
	Cell_Data veh_cell;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	index = partition = 0;

	//---- deal with non-simulation legs ----

	mode = plan_ptr->Mode ();

	if (mode == OTHER_MODE || mode == RIDE_MODE) return;
	if (mode == DRIVE_MODE && plan_ptr->Passengers () > 0) {
		if (plan_ptr->Passengers () > 3) {
			mode = HOV4_MODE;
		} else {
			mode = HOV2_MODE + plan_ptr->Passengers () - 1;
		}
	} else {
		if (!exe->param.transit_flag && (mode == TRANSIT_MODE || mode == PNR_IN_MODE || mode == PNR_OUT_MODE || 
			mode == KNR_IN_MODE || mode == KNR_OUT_MODE)) return;
	}
	if (plan_ptr->Depart () == 0 && plan_ptr->Arrive () == 0) {
		exe->Warning ("Departure and Arrival Times are Zero");
	}

	//---- find the traveler ----

	person_index.Household (plan_ptr->Household ());
	person_index.Person (plan_ptr->Person ());
	traveler = (int) exe->sim_traveler_array.size ();

	person_stat = exe->person_map.insert (Person_Map_Data (person_index, traveler));

	if (!person_stat.second) {

		//---- update existing traveler ----

		traveler = person_stat.first->second;
		sim_traveler_ptr = exe->sim_traveler_array [traveler];

		if (sim_traveler_ptr->Active ()) {

			//---- store subsequent plans for an active traveler ----

			plan_index.Traveler (traveler);
			plan_index.Sequence (plan_ptr->Tour (), plan_ptr->Trip ());

			sim_plan_ptr = new Sim_Plan_Data ();

			plan_map_stat = exe->sim_plan_map.insert (Sim_Plan_Map_Data (plan_index, sim_plan_ptr));

			if (!plan_map_stat.second) {
				exe->Warning (String ("Duplicate Plan Index for %d-%d-%d-%d") % 
					plan_ptr->Household () % plan_ptr->Person () % plan_ptr->Tour () % plan_ptr->Trip ());
				delete sim_plan_ptr;
				return;
			}
			sim_plan_ptr = plan_map_stat.first->second;
			new_flag = false;

		} else {

			//---- create a new travel plan for an inactive traveler ----

			sim_plan_ptr = sim_traveler_ptr->Create ();
			if (sim_plan_ptr == 0) goto mem_error;
			new_flag = true;
		}

	} else {

		//---- create a new traveler ----

		sim_traveler_ptr = new Sim_Traveler_Data ();
		if (sim_traveler_ptr == 0) goto mem_error;
					    
		sim_traveler_ptr->Type (plan_ptr->Type ());
		
		if (exe->script_flag || exe->hhfile_flag) {

			//---- update the household type ----

			Int2_Map_Itr map2_itr = exe->hhold_type.find (Int2_Key (plan_ptr->Household (), plan_ptr->Person ()));
			if (map2_itr != exe->hhold_type.end ()) {
				sim_traveler_ptr->Type (map2_itr->second);
			}
		}

		//---- save the traveler pointer ----

		sim_traveler_ptr->random.Seed (exe->Random_Seed () + traveler);
		sim_plan_ptr = sim_traveler_ptr->Create ();
		if (sim_plan_ptr == 0) goto mem_error;

		MAIN_LOCK
		exe->sim_traveler_array.push_back (sim_traveler_ptr);
		END_LOCK
		new_flag = true;
	}
	sim_plan_ptr->Household (plan_ptr->Household ());
	sim_plan_ptr->Person (plan_ptr->Person ());
	sim_plan_ptr->Tour (plan_ptr->Tour ());
	sim_plan_ptr->Trip (plan_ptr->Trip ());
	sim_plan_ptr->Partition (0);

	//---- create a new plan record ----

	sim_plan_ptr->assign (plan_ptr->size (), sim_leg_rec);

	sim_plan_ptr->Depart (plan_ptr->Depart ());
	sim_plan_ptr->Arrive (plan_ptr->Arrive ());
	sim_plan_ptr->Duration (plan_ptr->Activity ());
	sim_plan_ptr->Schedule (plan_ptr->Depart ());

	//---- convert the origin ----

	index = plan_ptr->Origin ();

	map_itr = exe->location_map.find (index);

	if (map_itr == exe->location_map.end ()) {
		label = "Origin";
		goto clean_up;
	}
	sim_plan_ptr->Origin (map_itr->second);

	if (exe->Num_Parts () > 1) {
		partition = exe->location_part [sim_plan_ptr->Origin ()];
		sim_plan_ptr->Partition (partition);
	}

	//---- convert the destination ----

	index = plan_ptr->Destination ();

	map_itr = exe->location_map.find (index);

	if (map_itr == exe->location_map.end ()) {
		label = "Destination";
		goto clean_up;
	}
	sim_plan_ptr->Destination (map_itr->second);

	index = plan_ptr->Vehicle ();

	if (index >= 0) {
		veh_index.Household (plan_ptr->Household ());
		veh_index.Vehicle (index);

		//veh_itr = vehicle_hash.find (veh_index);
		//if (veh_itr == vehicle_hash.end ()) {
		veh_itr = exe->vehicle_map.find (veh_index);
		if (veh_itr == exe->vehicle_map.end ()) {
			label = "Vehicle";
			goto clean_up;
		}
		index = veh_itr->second;
	} else {
		index = -1;
	}
	sim_plan_ptr->Vehicle (index);
	parking = first_dir = -1;

	sim_plan_ptr->Purpose (plan_ptr->Purpose ());
	sim_plan_ptr->Mode (mode);
	sim_plan_ptr->Constraint (plan_ptr->Constraint ());
	sim_plan_ptr->Priority (plan_ptr->Priority ());
	sim_plan_ptr->Type (plan_ptr->Type ());
	sim_plan_ptr->Passengers (plan_ptr->Passengers ());

	//---- process each leg ----

	drive_flag = false;

	for (leg = 0, leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++, leg++) {
		sim_leg_ptr = &sim_plan_ptr->at (leg);

		type = leg_itr->Type ();
		index = leg_itr->ID ();

		mode = leg_itr->Mode ();
		if (mode == DRIVE_MODE) drive_flag = true;

		sim_leg_ptr->Mode (mode);
		sim_leg_ptr->Type (type);
		sim_leg_ptr->Time (leg_itr->Time ());

		switch (type) {
			case LOCATION_ID:
				map_itr = exe->location_map.find (index);

				if (map_itr == exe->location_map.end ()) {
					label = "Location";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case PARKING_ID:
				map_itr = exe->parking_map.find (index);

				if (map_itr == exe->parking_map.end ()) {
					label = "Parking Lot";
					goto clean_up;
				}
				index = map_itr->second;
				if (parking < 0) parking = index;
				break;
			case STOP_ID:
				map_itr = exe->stop_map.find (index);

				if (map_itr == exe->stop_map.end ()) {
					label = "Stop";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case NODE_ID:
				map_itr = exe->node_map.find (index);

				if (map_itr == exe->node_map.end ()) {
					label = "Node";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case ACCESS_ID:
			case ACCESS_AB:
			case ACCESS_BA:
				index = leg_itr->Access_ID ();
				dir = leg_itr->Access_Dir ();

				map_itr = exe->access_map.find (index);

				if (map_itr == exe->access_map.end ()) {
					label = "Access";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case ROUTE_ID:
				map_itr = exe->line_map.find (index);

				if (map_itr == exe->line_map.end ()) {
					label = "Route";
					goto clean_up;
				}
				exe->stats.num_transit++;
				index = map_itr->second;
				sim_plan_ptr->Vehicle (index);
				break;
			case LINK_ID:
			case LINK_AB:
			case LINK_BA:
			case USE_AB:
			case USE_BA:
				index = leg_itr->Link_ID ();
				dir = leg_itr->Link_Dir ();

				map_itr = exe->link_map.find (index);

				if (map_itr == exe->link_map.end ()) {
					label = "Link";
					goto clean_up;
				}
				index = map_itr->second;

				if (mode == DRIVE_MODE) {
					link_ptr = &exe->link_array [index];
					if (dir == 1) {
						index = link_ptr->BA_Dir ();
					} else {
						index = link_ptr->AB_Dir ();
					}
					sim_leg_ptr->Type (DIR_ID);

					if (index < 0) {
						index = leg_itr->ID ();
						label = "Link Direction";
						goto clean_up;
					}
					if (drive_flag && first_dir < 0) first_dir = dir;
				}
				break;
			case DIR_ID:
				map_itr = exe->dir_map.find (index);

				if (map_itr == exe->dir_map.end ()) {
					label = "Link Direction";
					goto clean_up;
				}
				index = map_itr->second;

				if (drive_flag && first_dir < 0) {
					dir_ptr = &exe->dir_array [index];
					first_dir = dir_ptr->Dir ();
				}
				break;
			default:
				label = "Type";
				index = type;
				goto clean_up;
		}
		sim_leg_ptr->Index (index);
	}
	exe->stats.num_trips++;
	if (drive_flag) {
		exe->stats.num_veh_trips++;

		if (parking >= 0) {
			sim_park_ptr = &exe->sim_park_array [parking];

			if (first_dir == 0) {
				partition = sim_park_ptr->Part_AB ();
			} else if (first_dir == 1) {
				partition = sim_park_ptr->Part_BA ();
			}
		}
	}
	if (new_flag) {
		if (partition >= exe->First_Part () && partition <= exe->Last_Part ()) {
			exe->work_step.Put (traveler, (partition - exe->First_Part ()));
#ifdef MPI_EXE
		} else {
			exe->mpi_parts [partition].push_back (traveler);
#endif
		}
	}
	return;

clean_up:
	exe->Warning (String ("Plan %d-%d-%d-%d %s %d was Not Found") % plan_ptr->Household () % 
		plan_ptr->Person () % plan_ptr->Tour () % plan_ptr->Trip () % label % index);
	sim_traveler_ptr->Delete ();
	return;

mem_error:
	exe->Warning ("Insufficient Memory for Travel Plans");
	return;
}

//---------------------------------------------------------
//	Reposition_Plan
//---------------------------------------------------------

void Sim_Read_Plans::Reposition_Plan (Plan_Data *plan_ptr)
{
	if (plan_ptr->Vehicle () >= 0) {
		int last_parking = -1;
		Vehicle_Index veh_index;
		Vehicle_Map_Itr veh_itr;
		Vehicle_Data *veh_ptr;
		Plan_Leg_Itr leg_itr;
		Int_Map_Itr map_itr;

		veh_index.Household (plan_ptr->Household ());
		veh_index.Vehicle (plan_ptr->Vehicle ());

		veh_itr = exe->vehicle_map.find (veh_index);
		if (veh_itr != exe->vehicle_map.end ()) {
			veh_ptr = &exe->vehicle_array [veh_itr->second];

			for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
				if (leg_itr->Type () == PARKING_ID) last_parking = leg_itr->ID ();
			}
			map_itr = exe->parking_map.find (last_parking);
			if (map_itr != exe->parking_map.end ()) {
				veh_ptr->Parking (map_itr->second);
			}
		}
	}
}
