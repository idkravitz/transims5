//*********************************************************
//	Read_Fares.cpp - read the transit fare file
//*********************************************************

#include "Data_Service.hpp"
#include "Data_Range.hpp"
#include "Range_Data.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Read_Fares
//---------------------------------------------------------

void Data_Service::Read_Fares (void)
{
	Fare_File *file = (Fare_File *) System_File_Handle (TRANSIT_FARE);

	int num;
	Fare_Index fare_key;
	Fare_Map_Stat map_stat;
	Fare_Data fare_rec;

	int low, high, fare;
	int fz, tz, fm, tm, p, type, p_num, num_p;
	Dtime p_low, p_high;
	String text, str_low, str_high;
	Strings fields;
	Str_Itr str_itr;

	Time_Periods fare_period;
	Data_Range from_zone, to_zone, from_mode, to_mode, type_range;
	Range_Array_Itr fz_itr, tz_itr, fm_itr, tm_itr, t_itr;

	//---- store the fare data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Fares (*file);

	num = 0;

	while (file->Read ()) {
		Show_Progress ();

		//---- process a binary file record ----

		if (file->Code_Flag ()) {
			fare_key.From_Zone (file->iFrom_Zone ());
			fare_key.To_Zone (file->iTo_Zone ());
			fare_key.From_Mode (file->iFrom_Mode ());
			fare_key.To_Mode (file->iTo_Mode ());
			fare_key.Period (file->iPeriod ());
			fare_key.Class (file->iClass ());
			
			fare = file->Fare ();

			map_stat = fare_map.insert (Fare_Map_Data (fare_key, fare));

			if (!map_stat.second) {
				map_stat.first->second = fare;
			} else {
				num++;
			}
			continue;
		}

		//---- process an ascii file record ----

		fare_rec.Clear ();

		if (Get_Fare_Data (*file, fare_rec)) {

			//---- transit mode ----

			if (fare_rec.To_Mode ().empty ()) continue;

			//---- reset the range data ----

			from_zone.clear ();
			to_zone.clear ();
			from_mode.clear ();
			to_mode.clear ();
			fare_period.clear ();
			type_range.clear ();

			//---- process the transit mode range ----

			fare_rec.To_Mode ().Parse (fields);

			for (str_itr = fields.begin (); str_itr != fields.end (); str_itr++) {
				if (!str_itr->Range (str_low, str_high)) continue;

				low = Transit_Code (str_low);

				if (low == NO_TRANSIT) {
					Warning (String ("Transit Fare Mode %s is Out of Range") % str_low);
					continue;
				}
				if (low == ANY_TRANSIT) {
					low = 1;
					high = ANY_TRANSIT - 1;
				} else {
					high = Transit_Code (str_high);
					if (high < low) {
						Warning (String ("Transit Fare Mode %s is Out of Order") % *str_itr);
						continue;
					}
				}
				to_mode.Add_Range (low, high);
			}

			//---- from and to zone ----

			from_zone.Add_Ranges (fare_rec.From_Zone ());
			to_zone.Add_Ranges (fare_rec.To_Zone ());

			//---- from mode ----

			if (fare_rec.From_Mode ().empty ()) {
				from_mode.Add_Range (0, ANY_TRANSIT - 1);
			} else {
				fare_rec.From_Mode ().Parse (fields);

				for (str_itr = fields.begin (); str_itr != fields.end (); str_itr++) {
					if (!str_itr->Range (str_low, str_high)) continue;

					low = Transit_Code (str_low);

					if (low == ANY_TRANSIT) {
						low = 1;
						high = ANY_TRANSIT - 1;
					} else {
						high = Transit_Code (str_high);
						if (high < low) {
							Warning (String ("Transit Fare Mode %s is Out of Order") % *str_itr);
							continue;
						}
					}
					from_mode.Add_Range (low, high);
				}
			}

			//---- time period ----

			if (fare_rec.Period ().empty ()) {
				fare_period.Add_Range ((int) Model_Start_Time (), (int) Model_End_Time () - 1);
			} else {
				fare_period.Add_Ranges (fare_rec.Period ());
			}

			//---- fare class ----

			if (fare_rec.Class ().empty ()) {
				type_range.Add_Range (CASH, SPECIAL);
			} else {
				fare_rec.Class ().Parse (fields);

				for (str_itr = fields.begin (); str_itr != fields.end (); str_itr++) {
					if (!str_itr->Range (str_low, str_high)) continue;

					low = Class_Code (str_low);
					high = Class_Code (str_high);

					if (high < low) {
						Warning (String ("Transit Fare Class %s is Out of Order") % *str_itr);
						continue;
					}
					type_range.Add_Range (low, high);
				}
			}

			//---- get the fare data ----

			fare = fare_rec.Fare ();
			num_p = fare_period.Num_Periods ();

			//---- process the range data ----

			for (fz_itr = from_zone.begin (); fz_itr != from_zone.end (); fz_itr++) {
				for (fz = fz_itr->Low (); fz <= fz_itr->High (); fz++) {
					fare_key.From_Zone (fz);
					if (fz > num_fare_zone) num_fare_zone = fz;

					for (tz_itr = to_zone.begin (); tz_itr != to_zone.end (); tz_itr++) {
						for (tz = tz_itr->Low (); tz <= tz_itr->High (); tz++) {
							fare_key.To_Zone (tz);
							if (tz > num_fare_zone) num_fare_zone = tz;

							for (fm_itr = from_mode.begin (); fm_itr != from_mode.end (); fm_itr++) {
								for (fm = fm_itr->Low (); fm <= fm_itr->High (); fm++) {
									fare_key.From_Mode (fm);

									for (tm_itr = to_mode.begin (); tm_itr != to_mode.end (); tm_itr++) {
										for (tm = tm_itr->Low (); tm <= tm_itr->High (); tm++) {
											fare_key.To_Mode (tm);

											for (p_num = 1; p_num <= num_p; p_num++) {
												fare_period.Period_Range (p_num, p_low, p_high);

												low = fare_map.Period (p_low);
												high = fare_map.Period (p_high);

												for (p = low; p <= high; p++) {
													fare_key.Period (p);

													for (t_itr = type_range.begin (); t_itr != type_range.end (); t_itr++) {
														for (type = t_itr->Low (); type <= t_itr->High (); type++) {
															fare_key.Class (type);

															//---- process the record ----

															map_stat = fare_map.insert (Fare_Map_Data (fare_key, fare));

															if (!map_stat.second) {
																map_stat.first->second = fare;
															} else {
																num++;
															}																
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (TRANSIT_FARE);
}

//---------------------------------------------------------
//	Initialize_Fares
//---------------------------------------------------------

void Data_Service::Initialize_Fares (Fare_File &file)
{
	if (file.Header_Record ().empty ()) {
		int period, num_periods;
		Dtime low, high;
		Int_Set time_sort;
		Int_Set_Itr time_itr;
		String text;
		Time_Periods fare_periods;

		//---- scan the transit fare data ----

		time_sort.insert (Model_End_Time ());

		while (file.Read ()) {
			if (file.To_Mode ().empty ()) continue;

			//---- set the time breaks ----

			text = file.Period ();

			if (!text.empty ()) {
				fare_periods.clear ();
				fare_periods.Add_Ranges (text);

				num_periods = fare_periods.Num_Periods ();

				for (period = 1; period <= num_periods; period++) {
					fare_periods.Period_Range (period, low, high);

					time_sort.insert ((int) low);
					time_sort.insert ((int) high);
				}
			}
		}
		file.Rewind ();

		//---- set the time periods ----

		text.clear ();

		for (time_itr = time_sort.begin (); time_itr != time_sort.end (); time_itr++) {
			text += Dtime (*time_itr).Time_String () + " ";
		}
		fare_map.Periods (text);
	} else {
		fare_map.Periods (file.Header_Record ());
	}
}

//---------------------------------------------------------
//	Get_Fare_Data
//---------------------------------------------------------

bool Data_Service::Get_Fare_Data (Fare_File &file, Fare_Data &fare_rec)
{
	fare_rec.From_Zone (file.From_Zone ());
	fare_rec.To_Zone (file.To_Zone ());
	fare_rec.From_Mode (file.From_Mode ());
	fare_rec.To_Mode (file.To_Mode ());
	fare_rec.Period (file.Period ());
	fare_rec.Class (file.Class ());
	fare_rec.Fare (file.Fare ());
	fare_rec.Notes (file.Notes ());
	return (true);
}

