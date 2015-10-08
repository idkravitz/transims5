//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

bool Router::Read_Trips (int part, Plan_Processor *plan_process_ptr)
{
	int p, p0, num, num_car, max_hhold, hhold, person, mode;
	int last_hhold, last_person, partition;

	bool keep_flag, old_flag, duration_flag, last_skip, gap_flag, gap_ptr_flag;

	Trip_File *file;
	Plan_File *plan_file = 0;
	Plan_Ptr_Array *plan_ptr_array;
	Plan_Data *plan_ptr, *old_plan_ptr;
	Path_Parameters param;
	Select_Map_Itr sel_itr;
	Trip_Index trip_index, old_trip_index;
	Trip_Gap_Data trip_gap_data;
	Trip_Gap_Map_Stat map_stat;
	Trip_Gap_Map *trip_gap_map_ptr;

	Set_Parameters (param);

	num_car = 0;
	last_hhold = last_person = -1;
	max_hhold = MAX_INTEGER;
	plan_ptr = old_plan_ptr = 0;
	old_flag = duration_flag = last_skip = false;

	gap_flag = (trip_gap_map_flag && first_iteration);
	gap_ptr_flag = false;
	trip_gap_map_ptr = &trip_gap_map;

	if (gap_flag) {
		if (thread_flag) {
			trip_gap_map_ptr = trip_gap_map_array [part];
		} else if (part_processor.Thread_Flag ()) {
			gap_ptr_flag = true;
		}
	}
	partition = part;

	num = 1;
	plan_ptr_array = new Plan_Ptr_Array ();
	plan_ptr = new Plan_Data ();

	if (trip_set_flag) {
		file = trip_set [part];
		p0 = part;
		num = part + 1;
	} else {
		file = trip_file;
		p0 = 0;
		num = num_file_sets;
	}
	if (plan_flag && plan_set_flag) {
		for (p=p0; p < num; p++) {
			plan_file = plan_set [p];

			if (old_plan_flag) {
				old_plan_ptr = old_plan_array [p];
				if (old_plan_ptr == 0) {
					old_plan_array [p] = old_plan_ptr = new Plan_Data ();
				}
			} else {
				old_plan_ptr = new Plan_Data ();
			}
			if (!plan_file->Read_Plan (*old_plan_ptr)) {
				old_plan_ptr->Household (0);
			} else {
				old_plan_ptr->Partition (p);
			}
		}
	} else {
		old_plan_ptr = NULL;
		if (select_households) {
			max_hhold = hhold_range.Max_Value ();
		}
		if (select_flag) {
			hhold = select_map.Max_Household ();
			if (hhold < max_hhold) max_hhold = hhold;
		}
	}
	if (Master ()) {
		if (thread_flag) {
			MAIN_LOCK
			if (trip_set_flag) {
				Show_Message (String ("Reading %s %d") % file->File_Type () % file->Part_Number ());
			} else {
				Show_Message (String ("Reading %s") % file->File_Type ());
			}
			END_LOCK
		} else {
			if (trip_set_flag) {
				Show_Message (0, String ("\tReading %s %d -- Trip") % file->File_Type () % file->Part_Number ());
			} else {
				Show_Message (String ("Reading %s -- Trip") % file->File_Type ());
			}
			Set_Progress ();
		}
	}
	last_hhold = last_person = 0;

	while (file->Read_Trip (*plan_ptr)) {
		if (Master ()) {
			if (thread_flag) {
				Show_Dot ();
			} else {
				Show_Progress ();
			}
		}

		//---- check the household id ----

		hhold = plan_ptr->Household ();
		if (hhold < 1) continue;

		if (hhold > max_hhold) break;

		person = plan_ptr->Person ();

		keep_flag = Selection (plan_ptr);
		old_flag = false;

		part = plan_ptr->Partition ();

		//---- plan processing ----

		if (plan_flag) {
			plan_ptr->Get_Trip_Index (trip_index);

			if (old_plan_flag) {
				plan_file = plan_set [part];
				old_plan_ptr = old_plan_array [part];
			}
			old_plan_ptr->Get_Trip_Index (old_trip_index);

			//---- process plans up to the current household ----

			while (old_plan_ptr->Household () > 0 && 
				(old_trip_index < trip_index || 
				(old_trip_index == trip_index && !keep_flag))) {

				//---- check the deletion records ----

				if (delete_flag) {
					old_plan_ptr->Get_Trip_Index (old_trip_index);

					sel_itr = delete_map.Best (old_trip_index);
					if (sel_itr != delete_map.end ()) goto next;
				}
				if (delete_households && hhold_delete.In_Range (old_plan_ptr->Household ())) goto next;
				if (old_plan_ptr->Mode () < MAX_MODE && delete_mode [old_plan_ptr->Mode ()]) goto next;
				if (delete_travelers && traveler_delete.In_Range (old_plan_ptr->Type ())) goto next;

				//---- process plans from the previous household ----

				if (old_plan_ptr->Household () != last_hhold) {
					if (last_hhold > 0 && plan_ptr_array->size () > 0) {
						if (plan_process_ptr) {
							plan_process_ptr->Plan_Build (plan_ptr_array);
						} else {
							part_processor.Plan_Build (partition, plan_ptr_array);
						}
						plan_ptr_array = new Plan_Ptr_Array ();
					}
					last_hhold = old_plan_ptr->Household ();
					last_person = old_plan_ptr->Person ();
					partition = part;
				}

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (old_plan_ptr->Constraint () == END_TIME) {
						trip_gap_data.time = old_plan_ptr->End ();
					} else {
						trip_gap_data.time = old_plan_ptr->Start ();
					}
					trip_gap_data.current = 0;
					trip_gap_data.previous = (int) old_plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [part];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (old_plan_ptr->Get_Trip_Index (), trip_gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) old_plan_ptr->Impedance ();
					}
				}	

				//---- save the old plan to the household array ----

				if (update_flag) {
					old_plan_ptr->Method (RESKIM_PATH);
				} else if (Link_Flows ()) {
					old_plan_ptr->Method (PATH_FLOWS);
				} else {
					old_plan_ptr->Method (COPY_PATH);
				}
				plan_ptr_array->push_back (old_plan_ptr);


				//---- read the next plan ----

				old_plan_ptr = new Plan_Data ();
next:
				if (!plan_file->Read_Plan (*old_plan_ptr)) {
					old_plan_ptr->Household (0);
					break;
				} else {
					old_plan_ptr->Partition (part);
				}
				old_plan_ptr->Get_Trip_Index (old_trip_index);
			}
			if (keep_flag && old_trip_index == trip_index) {

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (old_plan_ptr->Constraint () == END_TIME) {
						trip_gap_data.time = old_plan_ptr->End ();
					} else {
						trip_gap_data.time = old_plan_ptr->Start ();
					}
					trip_gap_data.current = 0;
					trip_gap_data.previous = (int) old_plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [part];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (old_plan_ptr->Get_Trip_Index (), trip_gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) old_plan_ptr->Impedance ();
					}
				}

				//---- read the next plan ----

				if (reroute_flag && old_plan_ptr->Depart () < reroute_time && 
					old_plan_ptr->Arrive () > reroute_time) {

					plan_ptr = old_plan_ptr;
					old_plan_ptr = new Plan_Data ();
					old_flag = true;
				}
				if (!plan_file->Read_Plan (*old_plan_ptr)) {
					old_plan_ptr->Household (0);
				} else {
					old_plan_ptr->Partition (part);
				}
			}
			if (old_plan_flag) {
				old_plan_array [part] = old_plan_ptr;
			}
		}

		if (!keep_flag) {
			continue;
		}

		if (hhold != last_hhold) {
			if (last_hhold > 0 && plan_ptr_array->size () > 0) {
				if (plan_process_ptr) {
					plan_process_ptr->Plan_Build (plan_ptr_array);
				} else {
					part_processor.Plan_Build (partition, plan_ptr_array);
				}
				plan_ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = hhold;
			last_person = person;
			partition = part;
		} else if (person != last_person) {
			last_person = person;
		}

		//---- reroute the existing plan ----

		if (old_flag) {

			//---- update the destination ----

			//plan_ptr->End (trip_data.End ());
			//plan_ptr->Arrive (trip_data.End ());
			//plan_ptr->Duration (trip_data.Duration ());
			//plan_ptr->Destination (trip_data.Destination ());
			//plan_ptr->Type (trip_data.Type ());

			if (plan_ptr->Depart () >= reroute_time) {
				plan_ptr->Method (BUILD_PATH);
			} else if (plan_ptr->Arrive () < reroute_time) {
				if (Link_Flows ()) {
					plan_ptr->Method (PATH_FLOWS);
				} else {
					plan_ptr->Method (COPY_PATH);
				}
			} else {
				plan_ptr->Method (REROUTE_PATH);
				plan_ptr->Arrive (reroute_time);
			}

		} else {
			
			//---- initialize the plan data ----
					
			plan_ptr->Method (BUILD_PATH);
			plan_ptr->Depart (plan_ptr->Start ());
			plan_ptr->Arrive (plan_ptr->End ());
			plan_ptr->Activity (plan_ptr->Duration ());

			//	Update_Parking (*plan_ptr);
		}
		plan_ptr_array->push_back (plan_ptr);
		mode = plan_ptr->Mode ();

		plan_ptr = new Plan_Data ();

		//---- update the link travel times ----

		if (Time_Updates () && update_rate > 0) {
			if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
				mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE) {

				if (!(++num_car % update_rate)) {
					plan_process_ptr->Save_Flows ();
					Update_Travel_Times (MPI_Size ());
					num_time_updates++;
				}
			}
		}
	}
	if (Master () && !thread_flag) End_Progress ();
	file->Close ();

	//---- copy remaining plans ----

	if (plan_flag) {
		for (p=p0; p < num; p++) {
			if (old_plan_flag) {
				plan_file = plan_set [p];
				old_plan_ptr = old_plan_array [p];
			}
			while (old_plan_ptr->Household () > 0) {

				//---- process plans from the previous household ----

				if (old_plan_ptr->Household () != last_hhold) {
					if (last_hhold > 0 && plan_ptr_array->size () > 0) {
						if (plan_process_ptr) {
							plan_process_ptr->Plan_Build (plan_ptr_array);
						} else {
							part_processor.Plan_Build (partition, plan_ptr_array);
						}
						plan_ptr_array = new Plan_Ptr_Array ();
					}
					last_hhold = old_plan_ptr->Household ();
					last_person = old_plan_ptr->Person ();
					partition = p;
				}

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (old_plan_ptr->Constraint () == END_TIME) {
						trip_gap_data.time = old_plan_ptr->End ();
					} else {
						trip_gap_data.time = old_plan_ptr->Start ();
					}
					trip_gap_data.current = 0;
					trip_gap_data.previous = (int) old_plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [p];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (old_plan_ptr->Get_Trip_Index (), trip_gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) old_plan_ptr->Impedance ();
					}
				}

				//---- save the old plan to the household array ----

				if (update_flag) {
					old_plan_ptr->Method (RESKIM_PATH);
				} else if (Link_Flows ()) {
					old_plan_ptr->Method (PATH_FLOWS);
				} else {
					old_plan_ptr->Method (COPY_PATH);
				}
				plan_ptr_array->push_back (old_plan_ptr);

				//---- read the next plan ----

				old_plan_ptr = new Plan_Data ();

				if (!plan_file->Read_Plan (*old_plan_ptr)) {
					old_plan_ptr->Household (0);
				}
			}
		}
		delete old_plan_ptr;
	}
	if (last_hhold > 0 && plan_ptr_array->size () > 0) {
		if (plan_process_ptr) {
			plan_process_ptr->Plan_Build (plan_ptr_array);
		} else {
			part_processor.Plan_Build (partition, plan_ptr_array);
		}
	}
	return (true);
}
