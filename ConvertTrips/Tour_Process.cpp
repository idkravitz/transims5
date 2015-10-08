//*********************************************************
//	Tour_Process.cpp - process records in the tour file
//*********************************************************

#include "ConvertTrips.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Tour_Processing
//---------------------------------------------------------

void ConvertTrips::Tour_Processing (void)
{
	int hhold, person, tour, org, des, stop, mode, park, veh, skim, period;
	int num_work, num_veh, last_person, o, d, index, group, diurnal, record;
	int group_out, group_in, diurnal_out, diurnal_in, mode_out, mode_in, work_code;
	int last_hhold, home, trip, work, last_des, leg, vehicle;
	int purpose, constraint, priority, passenger;
	double dist1, dist2;
	Dtime ttim, start, end, duration, last_time;
	bool stat;
	Problem_Type problem_type;

	File_Group *group_ptr;
	Tour_Index tour_index;
	Tour_Data tour_rec;
	Tour_Map_Itr map_itr;
	Tour_Map tour_map;
	Int_Map_Stat hhold_stat;
	Person_Map_Stat person_stat;
	Person_Index person_index;
	Integers persons, work_end;
	Int_Itr int_itr;
	Time_Periods *period_ptr;
	Trip_Data trip_data;
	Trip_Itr trip_itr;

	//---- read the tour file ----

	Show_Message (String ("Reading %s -- Record") % tour_file.File_Type ());
	Set_Progress ();

	last_hhold = -1;
	last_time = 0;
	home = num_work = num_veh = record = tour = trip = 0;
	work_end.assign (100, 0);

	//---- sort the tour records -----

	for (;;) {
		stat = tour_file.Read ();

		hhold = tour_file.Household ();
		if (hhold > max_hh_in) max_hh_in = hhold;

		if (!stat || hhold != last_hhold) {
			if (last_hhold >= 0) {
				if (last_hhold == 0) last_hhold = hhold_id++;

				last_person = veh = -1;
				tour = trip = home = work = last_des = vehicle = num_work = num_veh = 0;
				purpose = constraint = priority = passenger = 0;
				persons.clear ();
				problem_type = TOTAL_PROBLEM;

				for (map_itr = tour_map.begin (); map_itr != tour_map.end (); map_itr++) {
					tour_index = map_itr->first;

					person = tour_index.Person ();

					if (person != last_person) {
						last_person = person;
						last_time = 0;
						tour = trip = work = last_des = vehicle = 0;
						persons.push_back (person);
					}
					trip++;

					period = tour_index.Start ();
					work_code = tour_index.Work_Code ();
					leg = tour_index.Leg ();

					org = map_itr->second.Origin ();
					des = map_itr->second.Destination ();
					stop = map_itr->second.Stop ();

					group = map_itr->second.Group ();
					diurnal = map_itr->second.Diurnal ();
					mode = map_itr->second.Mode ();

					if (skim_flag) {
						o = skim_file->Org_Index (org);
						d = skim_file->Des_Index (des);

						if (o >= 0 && d >= 0) {
							skim_ptr = skim_file->Time_Skim (o, d);
						} else {
							skim_ptr = 0;
						}
					} else {
						skim_ptr = 0;
					}
					if (group < 1 || group >= (int) group_index.size ()) {
						index = -1;
					} else {
						index = group_index [group];
					}
					if (index < 0) {
						problem_type = TYPE_PROBLEM;
						goto problem_message;
					}
					group_ptr = &file_group [index];

					duration = group_ptr->Duration ();
					purpose = group_ptr->Purpose ();
					constraint = group_ptr->Constraint ();
					priority = group_ptr->Priority ();
					passenger = group_ptr->Passengers ();;
					if (mode == 0) mode = group_ptr->Mode ();

					//---- set home and work locations ----

					if (leg == 0) {
						if (work_code < 2) {
							if (home != 0) org = -home;
							tour++;
							trip = 1;
						} else if (work_code == 2) {
							if (work != 0) org = -work;
						} else if (work_code > 3) {
							if (home != 0) org = -home;
							tour++;
							trip = 1;
						}
					} else {
						if (work_code == 3) {
							org = -work;
							des = -home;
						} else if (work_code == 2) {
							org = -last_des;
							des = -work;
						} else {
							org = -last_des;
							des = -home;
						}
					}

					//---- locate the tour leg ----

					if (!Locate_OD (group_ptr, org, des, stop, park, dist1, dist2)) {
						problem_type = LOCATION_PROBLEM;
						goto problem_message;
					}

					//---- save the anchor points ----

					if (leg == 0) {
						if (work_code < 2) {
							if (home == 0) home = org;
							if (work_code == 1) {
								if (work == 0) {
									work = des;
									num_work++;
								}
							}
						} else if (work_code == 2 && work == 0) {
							work = org;
						}
						last_des = des;
					}

					//--- get the trip start and end time ----

					if (time_flag) {
						period_ptr = time_equiv.Group_Period (period);
						if (period_ptr != 0) {
							period_ptr->Period_Range (0, start, end);
						} else {
							start = 0;
						}
					} else {
						tour_periods.Period_Range (period, start, end);
					}
					if (last_time > 0 && start < (last_time - one_hour)) {
						start = last_time - one_hour;
					}
					if (start < 0) start = 0;
					if (end > midnight) end = midnight;

					//---- estimate the travel time ----

					skim = 0;

					if (skim_ptr != 0) {
						if (group_ptr->Time_Point () == START_TIME) {
							period = skim_file->Period (end);
						} else {
							period = skim_file->Period (start);
						}
						if (period >= 0) {
							skim = skim_ptr [period];
						}
					}
					if (skim != 0) {
						ttim = skim;
					} else {
						ttim.Seconds ((dist1 + dist2) / group_ptr->Speed ());
					}
					ttim += additional_time;
					if (stop > 0) ttim += group_ptr->Stop_Time ();

					//---- schedule the trip ----

					if (!Set_Time (group_ptr, start, end, ttim, diurnal)) {
						problem_type = TIME_PROBLEM;
						goto problem_message;
					}

					//---- vehicle data ----

					if (mode == DRIVE_MODE || mode == PNR_OUT_MODE || mode == KNR_OUT_MODE ||
						mode == HOV2_MODE || mode == HOV3_MODE || mode == HOV4_MODE || mode == TAXI_MODE) {

						veh = vehicle;

						if (veh == 0) {
							veh = vehicle = ++num_veh;
							vehicle_file->Household (last_hhold);
							vehicle_file->Vehicle (veh);
							vehicle_file->Parking (park);
							vehicle_file->Type (group_ptr->Veh_Type ());

							if (!vehicle_file->Write ()) {
								Error ("Writing Vehicle File");
							}
							new_vehicles++;
						}
					} else {
						veh = 0;
					}

					//---- add the trip to the stop ----

					trip_data.Household (last_hhold);
					trip_data.Person (person);
					trip_data.Tour (tour);
					trip_data.Trip (trip);
					trip_data.Mode (mode);
					trip_data.Purpose (purpose);
					trip_data.Start (start);
					trip_data.Origin (org);
					trip_data.Priority (priority);
					trip_data.Passengers (passenger);
					trip_data.Vehicle (veh);
					trip_data.Type (group);
					trip_data.Partition (diurnal);

					if (stop > 0) {
						if (skim == 0) skim = ttim;
						skim = ttim - group_ptr->Stop_Time ();
						ttim = (int) (skim * dist1 / (dist1 + dist2) + 0.5);
						skim -= ttim;
						ttim += start;

						trip_data.End (ttim);
						trip_data.Duration (group_ptr->Stop_Time ());
						trip_data.Destination (stop);
						trip_data.Constraint (NO_CONSTRAINT);
						//trip_data.Type (-1);

						trip_array.push_back (trip_data);

						start = ttim + group_ptr->Stop_Time ();

						trip_data.Start (start.Round_Seconds ());
						trip_data.Origin (stop);
						trip_data.Trip (++trip);
					}
					trip_data.End (end);
					trip_data.Duration (duration);
					trip_data.Destination (des);
					trip_data.Constraint (constraint);
					//trip_data.Type (group);

					last_time = end;

					trip_array.push_back (trip_data);
					continue;
problem_message:
					Set_Problem (problem_type);
					tot_errors++;

					if (problem_flag) {
						problem_file->Problem (problem_type);
						problem_file->Household (last_hhold);
						problem_file->Person (person);
						problem_file->Tour (tour);
						problem_file->Trip (trip);
						problem_file->Start (start.Round_Seconds ());
						problem_file->End (end.Round_Seconds ());
						problem_file->Duration (duration.Round_Seconds ());
						problem_file->Origin (abs (org));
						problem_file->Destination (abs (des));
						problem_file->Purpose (purpose);
						problem_file->Mode (mode);
						problem_file->Constraint (constraint);
						problem_file->Priority (priority);
						problem_file->Vehicle (veh);
						problem_file->Passengers (passenger);
						problem_file->Type (group);
						problem_file->Notes ((char *) Problem_Code (problem_type));

						if (!problem_file->Write ()) {
							Warning ("Writing ") << problem_file->File_Type ();
							problem_flag = false;
						}
						problem_file->Add_Trip (last_hhold, person, tour);
					}
				}

				//---- resolve trip conflicts ----

				Adjust_Trips ();

				//---- write the trip records ----

				for (trip_itr = trip_array.begin (); trip_itr != trip_array.end (); trip_itr++) {
					if (trip_itr->Partition () < 0) continue;

					trip_file->Household (trip_itr->Household ());
					trip_file->Person (trip_itr->Person ());
					trip_file->Tour (trip_itr->Tour ());
					trip_file->Trip (trip_itr->Trip ());
					trip_file->Mode (trip_itr->Mode ());
					trip_file->Purpose (trip_itr->Purpose ());
					trip_file->Start (trip_itr->Start ().Round_Seconds ());
					trip_file->End (trip_itr->End ().Round_Seconds ());
					trip_file->Duration (trip_itr->Duration ().Round_Seconds ());
					trip_file->Origin (trip_itr->Origin ());
					trip_file->Destination (trip_itr->Destination ());
					trip_file->Priority (trip_itr->Priority ());
					trip_file->Passengers (trip_itr->Passengers ());
					trip_file->Vehicle (trip_itr->Vehicle ());

					if (!trip_file->Write ()) goto trip_error;
					new_trips++;
				}

				//---- write new household records ----

				hhold_stat = hhold_map.insert (Int_Map_Data (last_hhold, 1));
					
				if (hhold_stat.second) {
					household_file->Household (last_hhold);
					household_file->Location (home);
					household_file->Persons ((int) persons.size ());
					household_file->Workers (num_work);
					household_file->Vehicles (num_veh);

					if (!household_file->Write (false)) {
						Error ("Writing Household File");
					}
					new_hholds++;
				}

				//---- write new person records ---- 

				for (int_itr = persons.begin (); int_itr != persons.end (); int_itr++) {
					person = *int_itr;

					person_index.Household (last_hhold);
					person_index.Person (person);

					person_stat = person_map.insert (Person_Map_Data (person_index, 1));

					if (person_stat.second) {
						household_file->Person (person);
						household_file->Age (25);
						household_file->Gender (1);
						household_file->Work (1);
						household_file->Drive (1);

						if (!household_file->Write (true)) {
							Error ("Writing Person File");
						}
						new_persons++;
					}
				}
			}
			last_hhold = hhold;
			record = 0;
			tour_map.clear ();
			trip_array.clear ();
			work_end.assign (100, 0);
		}
		if (!stat) break;
		Show_Progress ();

		tot_tours++;

		//---- check the hhold list ----
	
		if (select_households && !hhold_range.In_Range (hhold)) continue;
		
		org = tour_file.Origin ();
		des = tour_file.Destination ();

		if (select_org_zones && !org_zone_range.In_Range (org)) continue;
		if (select_des_zones && !des_zone_range.In_Range (des)) continue;

		//---- save the tour record and sort index ----

		group_out = tour_file.Group ();
		if (group_out == 0) group_out = 1;
		group_in = group_out;

		diurnal_out = diurnal_in = 0;
		mode_out = mode_in = tour_file.Mode ();

		purpose = tour_file.Purpose ();
		if (purpose == 1) {
			work_code = 1;
		} else if (purpose == 9) {
			work_code = 2;
		} else {
			work_code = 0;
		}
		if (script_flag) {
			script_base.Put_Field (0, group_out);
			script_base.Put_Field (1, group_in);
			script_base.Put_Field (2, diurnal_out);	
			script_base.Put_Field (3, diurnal_in);
			script_base.Put_Field (4, mode_out);
			script_base.Put_Field (5, mode_in);
			script_base.Put_Field (6, work_code);

			if (!user_group.Execute ()) continue;

			group_out = script_base.Get_Integer (0);
			group_in = script_base.Get_Integer (1);
			diurnal_out = script_base.Get_Integer (2);
			diurnal_in = script_base.Get_Integer (3);
			mode_out = script_base.Get_Integer (4);
			mode_in = script_base.Get_Integer (5);
			work_code = script_base.Get_Integer (6);
		}
		if (mode_out >= 0 && mode_out < MAX_MODE && !select_mode [mode_out]) continue;

		person = tour_file.Person ();
		start = tour_file.Start ();
		end = tour_file.Return ();

		//---- save the outbound leg ----

		if (work_code == 0 && person < 100 && work_end [person] > 0 && work_end [person] <= start) {
			work_code = 4;
		}
		tour_index.Person (person);
		tour_index.Start (start);
		tour_index.Work_Code (work_code);
		tour_index.End (end);
		tour_index.Record (record++);
		tour_index.Leg (0);

		tour_rec.Purpose (purpose);
		tour_rec.Mode (mode_out);
		tour_rec.Origin (org);
		tour_rec.Destination (des);
		tour_rec.Stop (tour_file.Stop_Out ());
		tour_rec.Group (group_out);
		tour_rec.Diurnal (diurnal_out);

		if (tour_rec.Stop () > 0) {
			tot_trips += 2;
		} else {
			tot_trips++;
		}
		tour_map.insert (Tour_Map_Data (tour_index, tour_rec));

		//---- save the inbound leg ----

		if (work_code == 1) {
			work_code = 3;
			if (person < 100 && end > work_end [person]) work_end [person] = end;
		}
		tour_index.Start (end);
		tour_index.Leg (1);
		tour_index.Work_Code (work_code);

		tour_rec.Mode (mode_in);
		tour_rec.Origin (des);
		tour_rec.Destination (org);
		tour_rec.Stop (tour_file.Stop_In ());
		tour_rec.Group (group_in);
		tour_rec.Diurnal (diurnal_in);

		if (tour_rec.Stop () > 0) {
			tot_trips += 2;
		} else {
			tot_trips++;
		}
		tour_map.insert (Tour_Map_Data (tour_index, tour_rec));
	}
	End_Progress ();
	return;

trip_error:
	Error ("Writing Trip File");
}
