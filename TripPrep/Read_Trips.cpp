//*********************************************************
//	Read_Trips.cpp - read the travel trip file
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void TripPrep::Trip_Processing::Read_Trips (int part)
{
	int merge_part, partition, last_hhold, person, vehicle, parking;
	double share, total, prob;
	string process_type;
	bool first_flag;

	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Vehicle_Data *veh_ptr, *merge_veh_ptr;
	Select_Map_Itr sel_itr;

	Trip_Data *trip_ptr, *merge_ptr;

	Trip_Index trip_index, last_trip, trip_rec, last_sort;
	Time_Index time_index, last_time, time_rec;
	Trip_Map_Stat trip_stat;
	Time_Map_Stat time_stat;

	//---- open the file partition ----

	if (!trip_file.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % trip_file.Filename ()); 
		END_LOCK
	}
	if (exe->merge_flag) {
		if (!merge_file.Open (part)) {
			MAIN_LOCK 
			exe->Error (String ("Opening %s") % merge_file.Filename ()); 
			END_LOCK
		}
		merge_ptr = new Trip_Data ();
	} else {
		merge_ptr = 0;
	}
	if (exe->new_trip_flag && !exe->update_flag && (!exe->output_flag || part == 0)) {
		new_trip_file.Open (part);
	}
	if (exe->make_veh_flag && exe->veh_part_flag) {
		new_veh_file.Open (part);
	}
	veh_ptr = merge_veh_ptr = 0;

	//---- write the process message ----

	if (exe->Trip_Sort () == TRAVELER_SORT) {
		process_type = "Traveler Sorting";
	} else if (exe->Trip_Sort () == TIME_SORT) {
		process_type = "Time Sorting";
	} else if (exe->merge_flag) {
		process_type = "Merging";

		if (merge_file.Read_Trip (*merge_ptr)) {
			merge_ptr->Get_Trip_Index (trip_index);
		} else {
			trip_index.Set (MAX_INTEGER);
		}
		last_trip = trip_index;
		trip_rec.Clear ();
	} else if (exe->update_flag) {
		process_type = "Partitioning";
	} else if (exe->sort_tours) {
		process_type = "Tour Sorting";
	} else {
		process_type = "Copying";
	}
	if (thread_flag) {
		MAIN_LOCK
		if (trip_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d") % process_type % trip_file.File_Type () % trip_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s") % process_type % trip_file.File_Type ());
		}
		END_LOCK
	} else {
		if (trip_file.Part_Flag ()) {
			exe->Show_Message (String ("%s %s %d -- Record") % process_type % trip_file.File_Type () % trip_file.Part_Number ());
		} else {
			exe->Show_Message (String ("%s %s -- Record") % process_type % trip_file.File_Type ());
		}
		exe->Set_Progress ();
	}
	merge_part = last_hhold = person = vehicle = parking = 0;
	first_flag = true;
	partition = -1;

	if (exe->update_flag && !exe->select_flag) {
		share = 1.0 / exe->num_parts;
	} else {
		share = 0.0;
	}

	//---- read the trip file ----

	trip_ptr = new Trip_Data ();

	while (trip_file.Read_Trip (*trip_ptr)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}

		//---- apply processing script ----

		if (exe->script_flag) {
			if (program.Execute () == 0) continue;

			trip_file.Get_Data (*trip_ptr);
		}

		//---- get the vehicle record ----

		if (exe->vehicle_flag) {
			veh_index.Household (trip_ptr->Household ());
			veh_index.Vehicle (trip_ptr->Vehicle ());

			veh_itr = exe->vehicle_map.find (veh_index);
			if (veh_itr != exe->vehicle_map.end ()) {
				veh_ptr = &exe->vehicle_array [veh_itr->second];
				veh_ptr->Partition (-1);
			} else {
				veh_ptr = 0;
			}
		}

		//---- check the selection records ----

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (trip_ptr->Household (), trip_ptr->Person (), 
				trip_ptr->Tour (), trip_ptr->Trip ());
			if (sel_itr == exe->select_map.end ()) continue;

			if (exe->type_flag) {
				trip_ptr->Type (sel_itr->second.Type ());
			}
			if (exe->update_flag) {
				partition = sel_itr->second.Partition ();
				trip_ptr->Partition (partition);
			}
		} else if (exe->update_flag) {
			if (trip_ptr->Household () != last_hhold) {
				last_hhold = trip_ptr->Household ();
				person = vehicle = 0;
				first_flag = true;

				prob = random_part.Probability ();
				total = share;

				for (partition=0; partition < exe->num_parts; partition++, total += share) {
					if (total > prob) break;
				}
			}
			trip_ptr->Partition (partition);
			if (thread_flag) {
				part_count [partition]++;
			} else {
				exe->part_count [partition]++;
			}
			if (exe->new_select_flag) {
				Trip_Index trip_index;
				Select_Data select_data;

				trip_ptr->Get_Trip_Index (trip_index);

				select_data.Type (trip_ptr->Type ());
				select_data.Partition (partition);

				if (thread_flag) {
					select_map.insert (Select_Map_Data (trip_index, select_data));
				} else {
					exe->select_map.insert (Select_Map_Data (trip_index, select_data));	
				}
			}
		}

		//---- check the selection criteria ----

		if (exe->select_households && !exe->hhold_range.In_Range (trip_ptr->Household ())) continue;
		if (trip_ptr->Mode () < MAX_MODE && !exe->select_mode [trip_ptr->Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (trip_ptr->Purpose ())) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (trip_ptr->Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (trip_ptr->Start ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (trip_ptr->End ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (trip_ptr->Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (trip_ptr->Destination ())) continue;
		
		//---- check the deletion records ----

		if (exe->delete_flag) {
			sel_itr = exe->delete_map.Best (trip_ptr->Household (), trip_ptr->Person (), 
				trip_ptr->Tour (), trip_ptr->Trip ());
			if (sel_itr != exe->delete_map.end ()) continue;
		}
		if (exe->delete_households && exe->hhold_delete.In_Range (trip_ptr->Household ())) continue;
		if (trip_ptr->Mode () < MAX_MODE && exe->delete_mode [trip_ptr->Mode ()]) continue;
		if (exe->delete_travelers && exe->traveler_delete.In_Range (trip_ptr->Type ())) continue;

		if (exe->percent_flag && exe->random.Probability () > exe->select_percent) continue;

		//--- vehicle partition ----

		if (veh_ptr != 0) {
			if (exe->veh_part_flag) {
				veh_ptr->Partition (trip_ptr->Partition ());
			} else {
				veh_ptr->Partition (0);
			}
		}

		//---- save the sort key ----

		if (exe->Trip_Sort () == TRAVELER_SORT) {
			trip_ptr->Get_Trip_Index (trip_index);

			trip_stat = traveler_sort.insert (Trip_Map_Data (trip_index, (int) trip_ptr_array.size ()));

			if (!trip_stat.second) {
				MAIN_LOCK
				exe->Warning (String ("Duplicate Trip Index = %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				END_LOCK
			} else {
				trip_ptr_array.push_back (trip_ptr);
				trip_ptr = new Trip_Data ();

				if (exe->sort_size > 0 && (int) trip_ptr_array.size () > exe->sort_size) {
					Write_Temp ();
				}
			}
		} else if (exe->Trip_Sort () == TIME_SORT) {
			trip_ptr->Get_Time_Index (time_index);

			time_stat = time_sort.insert (Time_Map_Data (time_index, (int) trip_ptr_array.size ()));

			if (!time_stat.second) {
				MAIN_LOCK
				exe->Warning (String ("Duplicate Trip Index = %s-%d-%d") % 
					time_index.Start ().Time_String () % 
					time_index.Household () % time_index.Person ());
				END_LOCK
			} else {
				trip_ptr_array.push_back (trip_ptr);
				trip_ptr = new Trip_Data ();

				if (exe->sort_size > 0 && (int) trip_ptr_array.size () > exe->sort_size) {
					Write_Temp ();
				}
			}
		} else if (exe->merge_flag) {
			trip_ptr->Get_Trip_Index (trip_rec);

			if (trip_rec < last_sort) {
				MAIN_LOCK
				exe->Error ("Trip File is Not Traveler Sorted");
				END_LOCK
			}
			last_sort = trip_rec;

			while (trip_index <= trip_rec) {
				if (exe->new_trip_flag && trip_index < trip_rec) {

					//---- get the vehicle record ----

					if (exe->vehicle_flag) {
						veh_index.Household (merge_ptr->Household ());
						veh_index.Vehicle (merge_ptr->Vehicle ());

						veh_itr = exe->vehicle_map.find (veh_index);
						if (veh_itr != exe->vehicle_map.end ()) {
							merge_veh_ptr = &exe->vehicle_array [veh_itr->second];
							merge_veh_ptr->Partition (-1);
						} else {
							merge_veh_ptr = 0;
						}
					}
					if (exe->delete_flag && exe->delete_map.Best (trip_index) != exe->delete_map.end ()) goto next;
					if (exe->delete_households && exe->hhold_delete.In_Range (merge_ptr->Household ())) goto next;
					if (merge_ptr->Mode () < MAX_MODE && exe->delete_mode [merge_ptr->Mode ()]) goto next;
					if (exe->delete_travelers && exe->traveler_delete.In_Range (merge_ptr->Type ())) goto next;

					if (exe->update_flag) {
						sel_itr = exe->select_map.Best (merge_ptr->Household (), merge_ptr->Person (), 
							merge_ptr->Tour (), merge_ptr->Trip ());
						if (sel_itr == exe->select_map.end ()) goto next;

						merge_part = sel_itr->second.Partition ();
						merge_ptr->Partition (merge_part);

						//--- vehicle partition ----

						if (merge_veh_ptr != 0) {
							if (exe->veh_part_flag) {
								merge_veh_ptr->Partition (merge_part);
							} else {
								merge_veh_ptr->Partition (0);
							}
						}
						exe->new_file_set [merge_part]->Write_Trip (*merge_ptr);
					} else {

						//--- vehicle partition ----

						if (merge_veh_ptr != 0) {
							if (exe->veh_part_flag) {
								merge_veh_ptr->Partition (merge_ptr->Partition ());
							} else {
								merge_veh_ptr->Partition (0);
							}
						}
						new_trip_file.Write_Trip (*merge_ptr);
					}
				}
next:
				if (merge_file.Read_Trip (*merge_ptr)) {
					merge_ptr->Get_Trip_Index (trip_index);

					if (trip_index < last_trip) {
						MAIN_LOCK
						exe->Error ("Merge Trip File is Not Traveler Sorted");
						END_LOCK
					}
					last_trip = trip_index;
				} else {
					trip_index.Set (MAX_INTEGER);
					break;
				}
			}
			if (exe->new_trip_flag) {
				if (exe->update_flag) {
					exe->new_file_set [partition]->Write_Trip (*trip_ptr);
				} else {
					new_trip_file.Write_Trip (*trip_ptr);
				}
			}
		} else if (exe->sort_tours) {
			if (trip_ptr->Household () != last_hhold) {
				if (last_hhold > 0) {
					Sort_Tours ();
				}
				last_hhold = trip_ptr->Household ();
			}
			trip_ptr_array.push_back (trip_ptr);
			trip_ptr = new Trip_Data ();
		} else {
			if (exe->new_trip_flag) {
				if (exe->update_flag) {
					exe->new_file_set [partition]->Write_Trip (*trip_ptr);
				} else {
					if (exe->make_veh_flag) {
						if (trip_ptr->Household () != last_hhold) {
							last_hhold = trip_ptr->Household ();
							person = vehicle = parking = 0;
						}
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
				}
			}
		}
	}
	if (!thread_flag) exe->End_Progress ();

	//---- process the sorted records ----

	if ((exe->new_trip_flag || exe->merge_flag) && (exe->Trip_Sort () == TRAVELER_SORT || exe->Trip_Sort () == TIME_SORT)) {
		if (num_temp > 0) {
			if (exe->Trip_Sort () == TRAVELER_SORT) {
				Temp_Trip_Write (part);
			} else {
				Temp_Time_Write (part);
			}
		} else {
			if (exe->Trip_Sort () == TRAVELER_SORT) {
				Trip_Write ();
			} else {
				Time_Write ();
			}
		}
	} else if (exe->new_trip_flag && exe->merge_flag && trip_index.Household () < MAX_INTEGER) {

		//---- copy the remaining merge records ----

		while (merge_file.Read_Trip (*merge_ptr)) {
			if (exe->update_flag) {
				sel_itr = exe->select_map.Best (merge_ptr->Household (), merge_ptr->Person (), 
					merge_ptr->Tour (), merge_ptr->Trip ());
				if (sel_itr == exe->select_map.end ()) continue;

				merge_part = sel_itr->second.Partition ();
				merge_ptr->Partition (merge_part);

				exe->new_file_set [merge_part]->Write_Trip (*merge_ptr);
			} else {
				new_trip_file.Write_Trip (*merge_ptr);
			}
		}
	} else if (exe->sort_tours && last_hhold > 0) {
		Sort_Tours ();
	}
	trip_file.Close ();
	if (exe->new_trip_flag && !exe->update_flag && !exe->output_flag) {
		new_trip_file.Close ();
	}
	if (exe->merge_flag) {
		merge_file.Close ();

		if (merge_ptr != 0) {
			delete merge_ptr;
		}
	}
	if (trip_ptr != 0) {
		delete trip_ptr;
	}
}
