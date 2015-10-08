//*********************************************************
//	Write_VISSIM.cpp - write a vissim xml file
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	Write_VISSIM
//---------------------------------------------------------

void ExportPlans::Write_VISSIM (void)
{
	int period, num_periods, hour, minute, second, type, volume, org, des, vol;
	double bucket, trips;
	String start, end;
	Dtime low, high;

	Matrix_Index trip_index;
	Trip_Table_Map_Itr table_itr;
	Trip_Table *trip_table;
	Trip_Table_Itr trip_itr;
	Link_Itr link_itr;
	Subzone_Map_Itr org_map_itr, des_map_itr;
	Subzone_Itr org_itr, des_itr;

	Int2_Key od_key;
	I2_Dbl_Map od_trips;
	I2_Dbl_Map_Stat od_stat;
	I2_Dbl_Map_Itr od_itr;

	//---- write the file ----

	fstream &fh = xml_file.File ();

	fh << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << endl;

	num_periods = sum_periods.Num_Periods ();

	if (num_periods > 0) {
		low = sum_periods.Start ();
		high = sum_periods.End ();

		second = DTOI (low.Seconds ());
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		start ("%02d:%02d:%02d") % hour % minute % second;

		second = DTOI (high.Seconds ());
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		end ("%02d:%02d:%02d") % hour % minute % second;
	} else {
		start ("00:00:00");
		end ("23:59:59");
	}
	fh << "<ABSTRACTNETWORKMODEL VERSNO=\"1.0\" FROMTIME=\"" << start << "\" TOTIME=\"" << end << "\" NAME=\"" << Title () << "\">" << endl;
	fh << "<MATRICES>" << endl;

	type = volume = 1;

	//---- write trip records ----

	for (period = 0; period < num_periods || period == 0; period++) {

		if (num_periods > 0) {
			sum_periods.Period_Range (period, low, high);

			second = DTOI (low.Seconds ());
			hour = second / 3600;
			second = second - hour * 3600;
			minute = second / 60;
			second = second - minute * 60;

			start ("%02d:%02d:%02d") % hour % minute % second;

			second = DTOI (high.Seconds ());
			hour = second / 3600;
			second = second - hour * 3600;
			minute = second / 60;
			second = second - minute * 60;

			end ("%02d:%02d:%02d") % hour % minute % second;
		} else {
			start ("00:00:00");
			end ("23:59:59");
		}
		for (table_itr = trip_tables.begin (); table_itr != trip_tables.end (); table_itr++) {
			type = table_itr->first;
			trip_table = &table_itr->second;

			fh << "<MATRIX VEHTYPEID=\"" << type << "\" FROMTIME=\"" << start << "\" TOTIME=\"" << end << "\">" << endl;
			fh << "<ELEMS>" << endl;

			od_trips.clear ();

			for (trip_itr = trip_table->begin (); trip_itr != trip_table->end (); trip_itr++) {
				volume = trip_itr->second [period];
				if (volume == 0) continue;

				trip_index = trip_itr->first;
				org = trip_index.Origin ();
				des = trip_index.Destination ();

				if (subzone_flag) {
					org_map_itr = subzone_map.find (org);
					des_map_itr = subzone_map.find (des);

					if (org_map_itr != subzone_map.end ()) {
						if (des_map_itr != subzone_map.end ()) {
							for (org_itr = org_map_itr->second.begin (); org_itr != org_map_itr->second.end (); org_itr++) {
								for (des_itr = des_map_itr->second.begin (); des_itr != des_map_itr->second.end (); des_itr++) {
									if (org_itr->zone == des_itr->zone) continue;

									trips = volume * org_itr->weight * des_itr->weight;

									od_key.first = org_itr->zone;
									od_key.second = des_itr->zone;

									od_stat = od_trips.insert (I2_Dbl_Map_Data (od_key, trips));

									if (!od_stat.second) {
										od_stat.first->second += trips;
									}
								}
							}
						} else {
							for (org_itr = org_map_itr->second.begin (); org_itr != org_map_itr->second.end (); org_itr++) {
								if (org_itr->zone == des) continue;

								trips = volume * org_itr->weight;

								od_key.first = org_itr->zone;
								od_key.second = des;

								od_stat = od_trips.insert (I2_Dbl_Map_Data (od_key, trips));

								if (!od_stat.second) {
									od_stat.first->second += trips;
								}
							}
						}
						continue;
					} else if (des_map_itr != subzone_map.end ()) {
						for (des_itr = des_map_itr->second.begin (); des_itr != des_map_itr->second.end (); des_itr++) {
							if (org == des_itr->zone) continue;

							trips = volume * des_itr->weight;

							od_key.first = org;
							od_key.second = des_itr->zone;

							od_stat = od_trips.insert (I2_Dbl_Map_Data (od_key, trips));

							if (!od_stat.second) {
								od_stat.first->second += trips;
							}
						}
						continue;
					}
				}
				if (org == des) continue;

				od_key.first = org;
				od_key.second = des;

				od_stat = od_trips.insert (I2_Dbl_Map_Data (od_key, volume));

				if (!od_stat.second) {
					od_stat.first->second += volume;
				}
			}

			//---- write the records ----
			
			bucket = 0.5;

			for (od_itr = od_trips.begin (); od_itr != od_trips.end (); od_itr++) {
				od_key = od_itr->first;

				trips = od_itr->second + bucket;
				vol = (int) trips;
				bucket = trips - vol;
				if (vol == 0) continue;

				fh << "<ELEM FROMZONENO=\"" << od_key.first;
				fh << "\" TOZONENO=\"" << od_key.second;
				fh << "\" VOLUME=\"" << vol << "\"/>" << endl;
			}
			fh << "</ELEMS>" << endl;
			fh << "</MATRIX>" << endl;
		}
	}
	fh << "</MATRICES>" << endl;
	
	//---- end the program ----

	fh << "</ABSTRACTNETWORKMODEL>" << endl;
}

