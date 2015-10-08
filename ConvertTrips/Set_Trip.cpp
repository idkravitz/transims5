//*********************************************************
//	Set_Trip.cpp - generate trip details
//*********************************************************

#include "ConvertTrips.hpp"

#include "math.h"

//---------------------------------------------------------
//	Set_Trips
//---------------------------------------------------------

int ConvertTrips::Set_Trips (File_Group_Itr group, int org, int des, int trips, int num, int t1, int t2, bool return_flag)
{
	int loc, trp, num_out, org_loc, org_park, des_loc, skim, period, type;
	int last, veh, x, y, mode, ntrips, retry_count, passengers, purpose, constraint, priority, veh_type;
	double org_wt, des_wt, prob, dist, cum_wt, dx, dy, share, share1, total;
	bool org_flag, des_flag, script_flag, skip;
	Dtime start, arrive, time, time1, ttim, t, duration;

	Convert_Location *loc_ptr;
	Location_Data *location_ptr;
	Share_Data *share_ptr;
	Integers *org_ptr, *des_ptr;
	Int_Itr int_itr;

	num_out = skim = 0;
	loc_ptr = 0;

	if (zone_loc_flag) {
		org_ptr = zone_loc_map.Locations (org);
		des_ptr = zone_loc_map.Locations (des);
	} else {
		org_ptr = des_ptr = 0;
	}

	//---- set the vehicle access flags ----

	mode = group->Mode ();
	script_flag = (group->Traveler_Script () != 0);

	org_flag = (mode != DRIVE_MODE && mode != PNR_OUT_MODE && mode != KNR_OUT_MODE &&
				mode != HOV2_MODE && mode != HOV3_MODE && mode != HOV4_MODE && mode != TAXI_MODE);
	des_flag = (mode != DRIVE_MODE && mode != PNR_IN_MODE && mode != KNR_IN_MODE &&
				mode != HOV2_MODE && mode != HOV3_MODE && mode != HOV4_MODE && mode != TAXI_MODE);

	//---- calculate the origin weight ----

	org_wt = 0.0;

	for (loc = zone_loc [org]; loc >= 0; loc = loc_ptr->Zone_List ()) {
		loc_ptr = &convert_array [loc];	

		if (org_flag || loc_ptr->Org_Parking () > 0) {
			org_wt += loc_ptr->Weight (group->Org_Wt ());
		}
	}
	if (org_ptr != 0) {
		for (int_itr = org_ptr->begin (); int_itr != org_ptr->end (); int_itr++) {
			loc_ptr = &convert_array [*int_itr];

			if (org_flag || loc_ptr->Org_Parking () > 0) {
				org_wt += loc_ptr->Weight (group->Org_Wt ());
			}
		}
	}
	if (org_wt == 0.0) return (trips);

	//---- check the destination weight ----
	
	des_wt = 0.0;

	for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
		loc_ptr = &convert_array [loc];	

		if (des_flag || loc_ptr->Des_Parking () > 0) {
			des_wt += loc_ptr->Weight (group->Des_Wt ());
		}
	}
	if (des_ptr != 0) {
		for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
			loc_ptr = &convert_array [*int_itr];

			if (des_flag || loc_ptr->Des_Parking () > 0) {
				des_wt += loc_ptr->Weight (group->Des_Wt ());
			}
		}
	}
	if (des_wt == 0.0) return (trips);

	if (return_flag) {
		ntrips = (trips + 1) / 2;
	} else {
		ntrips = trips;
	}

	//---- distribute the trips to the time targets ----

	total = 0.0;

	for (t=t1; t <= t2; t++) {
		total += group->Share (t, num);
	}
	if (total == 0.0) return (trips);

	for (t=t1; t <= t2; t++) {
		share_ptr = group->Share_Ptr (t, num);
		share = share_ptr->Share () / total;
		share_ptr->Add_Target (share * ntrips);
	}

	//---- located each trip ----

	for (trp=0; trp < ntrips; trp++) {

		//---- locate the trip origin ----

		prob = org_wt * random_org.Probability ();

		last = -1;
		cum_wt = 0.0;

		for (loc = zone_loc [org]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	

			if (org_flag || loc_ptr->Org_Parking () > 0) {
				share = loc_ptr->Weight (group->Org_Wt ());

				if (share != 0.0) {
					cum_wt += share;
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc < 0 && org_ptr != 0) {
			for (int_itr = org_ptr->begin (); int_itr != org_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];

				if (org_flag || loc_ptr->Org_Parking () > 0) {
					share = loc_ptr->Weight (group->Org_Wt ());

					if (share != 0.0) {
						cum_wt += share;
						if (prob < cum_wt) {
							loc = *int_itr;
							break;
						}
						last = *int_itr;
					}
				}
			}
		}
		if (loc < 0) {
			if (last < 0) {
				Set_Problem (LOCATION_PROBLEM);
				continue;
			}
			loc = last;
			loc_ptr = &convert_array [last];
		}
		org_park = loc_ptr->Org_Parking ();

		location_ptr = &location_array [loc];

		org_loc = location_ptr->Location ();
		x = location_ptr->X ();
		y = location_ptr->Y ();

		//---- calculate the destination weight ----

		des_wt = 0.0;

		for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];
			location_ptr = &location_array [loc];

			if (location_ptr->Location () != org_loc && (des_flag || loc_ptr->Des_Parking () > 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {

					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist = sqrt (dx * dx + dy * dy);
						if (dist == 0.0) dist = 0.01;
					
						des_wt += share * dist;
					} else {
						des_wt += share;
					}
				}
			}
		}
		if (des_ptr != 0) {
			for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];
				location_ptr = &location_array [*int_itr];

				if (location_ptr->Location () != org_loc && (des_flag || loc_ptr->Des_Parking () > 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {

						if (group->Dist_Wt ()) {

							//---- apply the distance weight ----

							dx = location_ptr->X () - x;
							dy = location_ptr->Y () - y;

							dist = sqrt (dx * dx + dy * dy);
							if (dist == 0.0) dist = 0.01;

							des_wt += share * dist;
						} else {
							des_wt += share;
						}
					}
				}
			}
		}
		if (des_wt == 0.0) {
			Set_Problem (LOCATION_PROBLEM);
			continue;
		}

		//---- locate the destination ----

		prob = des_wt * random_des.Probability ();
		
		last = -1;
		cum_wt = 0.0;

		for (loc = zone_loc [des]; loc >= 0; loc = loc_ptr->Zone_List ()) {
			loc_ptr = &convert_array [loc];	
			location_ptr = &location_array [loc];

			if (location_ptr->Location () != org_loc && (des_flag || loc_ptr->Des_Parking () > 0)) {
				share = loc_ptr->Weight (group->Des_Wt ());

				if (share != 0.0) {
					if (group->Dist_Wt ()) {

						//--- apply the distance weight ----

						dx = location_ptr->X () - x;
						dy = location_ptr->Y () - y;

						dist = sqrt (dx * dx + dy * dy);
						if (dist == 0.0) dist = 0.01;
					
						cum_wt += share * dist;
					} else {
						cum_wt += share;
					}
					if (prob < cum_wt) break;
					last = loc;
				}
			}
		}
		if (loc < 0 && des_ptr != 0) {
			for (int_itr = des_ptr->begin (); int_itr != des_ptr->end (); int_itr++) {
				loc_ptr = &convert_array [*int_itr];
				location_ptr = &location_array [*int_itr];

				if (location_ptr->Location () != org_loc && (des_flag || loc_ptr->Des_Parking () > 0)) {
					share = loc_ptr->Weight (group->Des_Wt ());

					if (share != 0.0) {
						if (group->Dist_Wt ()) {

							//---- apply the distance weight ----

							dx = location_ptr->X () - x;
							dy = location_ptr->Y () - y;

							dist = sqrt (dx * dx + dy * dy);
							if (dist == 0.0) dist = 0.01;

							cum_wt += share * dist;
						} else {
							cum_wt += share;
						}
						if (prob < cum_wt) {
							loc = *int_itr;
							break;
						}
						last = *int_itr;
					}
				}
			}
		}
		if (loc < 0) {
			if (last < 0) {
				Set_Problem (LOCATION_PROBLEM);
				continue;
			}
			loc = last;
			loc_ptr = &convert_array [last];
		}
		location_ptr = &location_array [loc];
		des_loc = location_ptr->Location ();

		dx = location_ptr->X () - x;
		dy = location_ptr->Y () - y;

		dist = sqrt (dx * dx + dy * dy);

		if (org_loc == des_loc) continue;

		retry_count = 0;
retry:
		prob = random_tod.Probability ();
		
		time = time1 = 0;
		share = share1 = 0.0;

		for (t=t1; t <= t2; t++) {
			share_ptr = group->Share_Ptr (t, num);

			share1 = share + share_ptr->Share () / total;

			if (prob < share1) {
				if (share_ptr->Target () < share_ptr->Trips () + 0.5) {
					if (retry_count < 10) {
						retry_count++;
						goto retry;
					}
				}
				group->Diurnal_Time_Range (t, time, time1);
				share_ptr->Add_Trips ();
				break;
			}
			share = share1;
		}
		if (share != share1) {
			time += (int) ((time1 - time) * (prob - share) / (share1 - share) + 0.5);
		}

		//--- estimate the travel time ----

		skim = 0;

		if (skim_ptr != 0) {
			period = skim_file->Period (time);
			if (period >= 0) {
				skim = skim_ptr [period];
			}
		}
		if (skim != 0) {
			ttim = skim + additional_time;
		} else {
			ttim.Seconds (dist / group->Speed () + additional_time.Seconds ());
		}

		//---- time methods ----

		if (group->Time_Point () == TRIP_START) {
			start = time;
		} else if (group->Time_Point () == TRIP_END) {
			start = time - ttim;
			if (start < 0) {
				if (return_flag) {
					start = 0;
				} else {
					while (start < 0) {
						start += midnight;
					}
				}
			}
		} else {
			start = time - ttim / 2;
			if (start < 0) {
				if (return_flag) {
					start = 0;
				} else {
					while (start < 0) {
						start += midnight;
					}
				}
			}
		}
		arrive = start + ttim;

		//---- apply the traveler script ----

		mode = group->Mode ();
		purpose = group->Purpose ();
		duration = group->Duration ();
		constraint = group->Constraint ();
		priority = group->Priority ();
		veh_type = group->Veh_Type ();
		passengers = group->Passengers ();
		type = group->Type ();

		if (script_flag) {
			traveler_file.Put_Field (group_fld, group->Group ());
			traveler_file.Put_Field (org_fld, org);
			traveler_file.Put_Field (des_fld, des);
			traveler_file.Put_Field (start_fld, start.Hours ());
			traveler_file.Put_Field (mode_fld, mode);
			traveler_file.Put_Field (purpose_fld, purpose);
			traveler_file.Put_Field (duration_fld, duration.Hours ());
			traveler_file.Put_Field (constraint_fld, constraint);
			traveler_file.Put_Field (priority_fld, priority);
			traveler_file.Put_Field (veh_type_fld, veh_type);
			traveler_file.Put_Field (passengers_fld, passengers);
			traveler_file.Put_Field (type_fld, type);

			if (group->Execute_Traveler ()) {
				mode = traveler_file.Get_Integer (mode_fld);
				purpose = traveler_file.Get_Integer (purpose_fld);
				duration.Hours (traveler_file.Get_Double (duration_fld));
				constraint = traveler_file.Get_Integer (constraint_fld);
				priority = traveler_file.Get_Integer (priority_fld);
				veh_type = traveler_file.Get_Integer (veh_type_fld);
				passengers = traveler_file.Get_Integer (passengers_fld);
				type = traveler_file.Get_Integer (type_fld);
			}
		}

		//---- store the results ----

		if (!org_flag || !des_flag) {
			veh = 1;
			new_vehicles++;
		} else {
			veh = 0;
		}

		//---- trip data ----

		trip_file->Household (hhold_id);
		trip_file->Person (1);
		trip_file->Tour (1);
		trip_file->Trip (1);
		trip_file->Purpose (purpose);
		trip_file->Mode (mode);
		trip_file->Vehicle (veh);
		trip_file->Start (Round (Resolve (start)));
		trip_file->Origin (org_loc);
		trip_file->End (Round (Resolve (arrive)));
		trip_file->Destination (des_loc);
		trip_file->Duration (duration);
		trip_file->Constraint (constraint);
		trip_file->Priority (priority);
		trip_file->Passengers (passengers);
		trip_file->Type (type);
		
		skip = ((select_start_times && !start_range.In_Range (start)) || 
				(select_end_times && !end_range.In_Range (arrive)));

		if (!skip) {
			if (!trip_file->Write ()) {
				Error ("Writing Trip File");
			}
			new_trips++;
		}

		//---- household data ----

		household_file->Household (hhold_id);
		household_file->Location (org_loc);
		household_file->Persons (1);
		household_file->Workers (1);
		household_file->Vehicles (1);

		if (!household_file->Write (false)) {
			Error ("Writing Household File");
		}
		new_hholds++;

		//---- person data ---- 

		household_file->Person (1);
		household_file->Age (25);
		household_file->Gender (1);
		household_file->Work (1);
		household_file->Drive (1);

		if (!household_file->Write (true)) {
			Error ("Writing Household File");
		}
		new_persons++;

		//---- vehicle data ----

		if (veh != 0) {
			vehicle_file->Household (hhold_id);
			vehicle_file->Vehicle (veh);
			vehicle_file->Parking (org_park);
			vehicle_file->Type (veh_type);

			if (org_park <= 0) {
				Warning (String ("Household %d Vehicle %d could not be Parked") % hhold_id % veh);
				Set_Problem (PARKING_PROBLEM);
			}
			if (!vehicle_file->Write ()) {
				Error ("Writing Vehicle File");
			}
		}
		hhold_id++;
		num_out++;

		//---- check for return trips ----

		if (!return_flag || num_out == trips) continue;

		if (mode == PNR_OUT_MODE) mode = PNR_IN_MODE;
		if (mode == KNR_OUT_MODE) mode = KNR_IN_MODE;

		//---- set start and end times ----

		start = arrive + duration;

		if (start > midnight) {
			start = midnight - 1;
		}
		arrive = start + ttim;

		//---- return trip data ----

		trip_file->Trip (2);
		trip_file->Mode (mode);
		trip_file->Start (Round (Resolve (start)));
		trip_file->Origin (des_loc);
		trip_file->End (Round (Resolve (arrive)));
		trip_file->Destination (org_loc);
		trip_file->Duration (0);
		trip_file->Constraint (0);
		trip_file->Priority (priority);
		trip_file->Passengers (passengers);

		skip = ((select_start_times && !start_range.In_Range (start)) || 
				(select_end_times && !end_range.In_Range (arrive)));

		if (!skip) {
			if (!trip_file->Write ()) {
				Error ("Writing Trip File");
			}
			new_trips++;
		}
		num_out++;
	}
	return (trips - num_out);
}
