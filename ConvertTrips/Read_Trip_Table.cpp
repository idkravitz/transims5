//*********************************************************
//	Read_Trip_Table.cpp - read trip table data
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Read_Trip_Table
//---------------------------------------------------------

void ConvertTrips::Read_Trip_Table (File_Group_Itr group)
{
	Dtime tod;
	int p, p1, p2, num_p, trp, o, d, period, current, first_p, last_p;
	int total, stat, errors, ext_org, ext_des, org, des, trips, num, num_shares, even_bucket, even;
	bool share_flag, factor_flag, period_flag, scale_flag, return_flag;
	double trip, factor, added, deleted, bucket;

	Matrix_File *file;
	Factor_Period *period_fac;
	Dtime_Table_Itr skim_itr;
	Int_Map_Itr map_itr;

	static string error_msg = "%d Trip%sbetween Zones %d and %d could not be allocated";

	//---- read the trip table ----

	total = errors = 0;
	added = deleted = 0.0;
	file = group->Trip_File ();

	return_flag = group->Return_Flag ();
	even_bucket = 1;
	bucket = 0.45;

	factor_flag = group->Factor_Flag ();
	period_flag = group->Factor_Periods_Flag ();

	scale_flag = (group->Scaling_Factor () != 1.0);

	num_shares = group->Num_Shares ();
	share_flag = (num_shares > 0);

	first_p = p1 = 0;
	last_p = p2 = num_p = group->Diurnal_Periods () - 1;
	period = 0;

	Show_Message (1, String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	while (file->Read ()) {
		Show_Progress ();

		//---- convert the zone numbers ----

		ext_org = file->Origin ();
		if (ext_org == 0) continue;
		if (select_org_zones && !org_zone_range.In_Range (ext_org)) continue;
		
		ext_des = file->Destination ();
		if (select_des_zones && !des_zone_range.In_Range (ext_des)) continue;

		map_itr = zone_map.find (ext_org);

		if (map_itr == zone_map.end ()) {
			Warning (String ("Origin Zone %d is Not in the Zone File") % ext_org);
			continue;
		}
		org = map_itr->second;

		map_itr = zone_map.find (ext_des);

		if (map_itr == zone_map.end ()) {
			Warning (String ("Destination Zone %d is Not in the Zone File") % ext_des);
			continue;
		}
		des = map_itr->second;

		//---- check for a factor period ----

		if (period_flag) {
			period = file->Period ();

			if (period >= 0) {
				first_p = last_p = -1;

				for (p=0; p <= num_p; p++) {
					tod = group->Diurnal_Period_Time (p);

					if (group->Factor_Period_Num (tod) == period) {
						if (first_p < 0) first_p = p;
						last_p = p;
					}
				}
				if (last_p < 0) {
					first_p = 0;
					last_p = num_p;
					period = 0;
				}
			} else {
				first_p = 0;
				last_p = num_p;
				period = 0;
			}
		}
		trips = file->Data ();

		if (trips < 0) {
			Warning (String ("Number of Trips is Out of Range (%d < 0)") % trips);
			continue;
		}

		//---- apply the scaling factor ----

		if (scale_flag) {
			trip = trips * group->Scaling_Factor () + bucket;
			trips = (int) trip;
			if (trips < 0) trips = 0;
			bucket = trip - trips;
		}
		if (trips == 0) continue;
		total += trips;

		//---- apply the selection script ----

		if (share_flag) {
			num = group->Execute_Diurnal ();

			if (num < 1 || num > num_shares) {
				Error (String ("Diurnal Selection Value %d is Out of Range (1..%d)") % num % num_shares);
			}
			num--;
		} else {
			num = 0;
		}

		//---- get the travel time ----

		if (skim_flag) {
			o = skim_file->Org_Index (ext_org);
			d = skim_file->Des_Index (ext_des);

			if (o >= 0 && d >= 0) {
				skim_ptr = skim_file->Time_Skim (o, d);
			} else {
				skim_ptr = 0;
			}
		}

		//---- apply adjustment factors ----

		if (factor_flag) {
			o = (equiv_flag) ? zone_equiv.Zone_Group (ext_org) : ext_org;
			d = (equiv_flag) ? zone_equiv.Zone_Group (ext_des) : ext_des;

			if (period_flag) {
				period = -1;
				p1 = p2 = 0;
				trip = 0.0;

				for (p=first_p; p <= last_p; p++) {
					tod = group->Diurnal_Period_Time (p);

					current = group->Factor_Period_Num (tod);

					if (current != period) {
						if (period >= 0) {
							period_fac = group->Period_Factor (o, d, period);

							factor = trip * period_fac->Factor ();
							if (factor > trip) {
								added += factor - trip;
							} else {
								deleted += trip - factor;
							}
							trp = period_fac->Bucket_Factor (trip);

							if (trp > 0 && return_flag) {
								even = (((trp + even_bucket) / 2) * 2);
								even_bucket += trp - even;
								trp = even;
							}
							if (trp > 0) {
								stat = Set_Trips (group, org, des, trp, num, p1, p2, return_flag);

								if (stat > 0) {
									errors += stat;
									Print (1, String (error_msg) % stat % ((stat > 1) ? "s " : " ") % ext_org % ext_des);
								}
							}
						}
						period = current;
						p1 = p;
						trip = 0.0;
					}
					trip += trips * group->Share (p, num);
					p2 = p;
				}

			} else {
				p1 = first_p;
				p2 = last_p;
				trip = trips;
			}
			period_fac = group->Period_Factor (o, d, period);

			factor = trip * period_fac->Factor ();
			if (factor > trip) {
				added += factor - trip;
			} else {
				deleted += trip - factor;
			}
			trp = period_fac->Bucket_Factor (trip);
		} else {
			p1 = first_p;
			p2 = last_p;
			trp = trips;
		}
		if (trp > 0 && return_flag) {
			even = (((trp + even_bucket) / 2) * 2);
			even_bucket += trp - even;
			trp = even;
		}

		//---- process the trips ----

		if (trp > 0) {
			stat = Set_Trips (group, org, des, trp, num, p1, p2, return_flag);

			if (stat > 0) {
				errors += stat;
				Print (1, String (error_msg) % stat % ((stat > 1) ? "s " : " ") % ext_org % ext_des);
			}
		}
	}
	End_Progress ();

	file->Close ();

	Print (1, String ("%s has %d Records and %d Trips") % file->File_Type () % Progress_Count () % total);

	tot_trips += total;

	if (errors > 0) {
		Warning (String ("A Total of %d Trip%scould not be allocated") % errors % ((errors > 1) ? "s " : " "));

		tot_errors += errors;
	}
	if (factor_flag) {
		Print (1, String ("Trip Adjustments: %.0lf trips added, %.0lf trips deleted") % added % deleted);
		tot_add += added;
		tot_del += deleted;
	}
}
