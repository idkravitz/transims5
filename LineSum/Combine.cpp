//*********************************************************
//	Combine.cpp - merge ridership files
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Combine
//---------------------------------------------------------

void LineSum::Combine (int period, File_List &list, Db_Header &output)
{
	int i, j, num_out, node, vol;
	String name, name_key;
	double freq;
	bool stat;
	Db_Header *fh;
	File_Itr file_itr;
	Sort_Key sort_key;
	Leg_Data leg_data;
	Line_Map_Stat map_stat;
	On_Off_Data on_off_data;
	Access_Key access_key;
	Access_Map_Stat access_stat;
	On_Off_Key on_off_key;
	On_Off_Map_Stat on_off_stat;
	Line_Map *map;
	Str_ID_Itr service_itr;

	if (list.size () == 0) return;

	if (period == 0) {
		name = "Peak";
		map = &peak_map;
	} else if (period == 1) {
		name = "Offpeak";
		map = &offpeak_map;
	} else {
		name = "Total";
		map = 0;
	}
	Show_Message (String ("Reading %s Ridership Files -- Record") % name);
	Set_Progress ();
	num_out = 0;

	memset (&on_off_data, '\0', sizeof (on_off_data));

	if (list.size () == 1) {
		fh = *list.begin ();

		while (fh->Read ()) {
			Show_Progress ();

			//---- copy fields to the output file ----

			if (output.Is_Open ()) {
				output.Copy_Fields (*fh);
				output.Write ();
			}

			//---- save the data to the line map ----

			if (map == 0) continue;

			name = fh->Get_String ("NAME");
			name.Trim ();
			if (name.Ends_With ("-")) continue;

			if (name [0] == '*') {
				if (access_flag) {
					vol = fh->Get_Integer ("AB_VOL");
					if (vol == 0) continue;

					access_key.stop = fh->Get_Integer ("A");
					access_key.mode = fh->Get_Integer ("MODE");
					access_key.node = fh->Get_Integer ("B");

					access_stat = access_map.insert (Access_Map_Data (access_key, on_off_data));

					if (period == 0) {
						access_stat.first->second.pk_off += vol;
					} else {
						access_stat.first->second.op_off += vol;
					}
					node = access_key.node;
					access_key.node = access_key.stop;
					access_key.stop = node;

					access_stat = access_map.insert (Access_Map_Data (access_key, on_off_data));

					if (period == 0) {
						access_stat.first->second.pk_on += vol;
					} else {
						access_stat.first->second.op_on += vol;
					}
				}
				continue;
			}
			sort_key.mode = (short) fh->Get_Integer ("MODE");
			sort_key.name = name;
			sort_key.leg = (short) fh->Get_Integer ("SEQ");

			if (fh->Get_Integer ("STOP_A") == 0) {
				leg_data.a = -fh->Get_Integer ("A");;
			} else {
				leg_data.a = fh->Get_Integer ("A");;
			}
			if (fh->Get_Integer ("STOP_B") == 0) {
				leg_data.b = -fh->Get_Integer ("B");;
			} else {
				leg_data.b = fh->Get_Integer ("B");;
			}
			leg_data.time = (short) fh->Get_Integer ("TIME");
			leg_data.dist = (short) fh->Get_Integer ("DIST");
			freq = fh->Get_Double ("FREQ");
			if (freq > 0.0) {
				leg_data.runs = 60.0 / freq;
			} else {
				leg_data.runs = 0.0;
			}
			leg_data.ab.ride = fh->Get_Integer ("AB_VOL");
			leg_data.ab.on = fh->Get_Integer ("AB_BRDA");
			leg_data.ab.off = fh->Get_Integer ("AB_XITB");
			leg_data.ba.ride = fh->Get_Integer ("BA_VOL");
			leg_data.ba.on = fh->Get_Integer ("BA_BRDB");
			leg_data.ba.off = fh->Get_Integer ("BA_XITA");

			if (service_flag && period < 2) {
				name_key = name;
				name_key.To_Upper ();

				service_itr = service_map.find (name_key);
				if (service_itr != service_map.end ()) {
					if (period == 0) {
						leg_data.service = service_itr->second >> 16;
					} else {
						leg_data.service = service_itr->second & 0xFFFF;
					}
				}
			}
			map_stat = map->insert (Line_Map_Data (sort_key, leg_data));
			if (!map_stat.second) {
				Warning (String ("Duplicate Line Data %d-%s-%d") % sort_key.mode % sort_key.name % sort_key.leg);
			}

			//---- boarding data ----

			if (on_off_flag) {
				if (leg_data.a > 0) {
					on_off_key.stop = leg_data.a;
					on_off_key.mode = sort_key.mode;
					on_off_key.name = name;

					on_off_stat = on_off_map.insert (On_Off_Map_Data (on_off_key, on_off_data));

					if (period == 0) {
						on_off_stat.first->second.pk_on += leg_data.ab.on;
						on_off_stat.first->second.pk_off += leg_data.ba.off;
					} else {
						on_off_stat.first->second.op_on += leg_data.ab.on;
						on_off_stat.first->second.op_off += leg_data.ba.off;
					}
				}
				if (leg_data.b > 0) {
					on_off_key.stop = leg_data.b;
					on_off_key.mode = sort_key.mode;
					on_off_key.name = name;

					on_off_stat = on_off_map.insert (On_Off_Map_Data (on_off_key, on_off_data));

					if (period == 0) {
						on_off_stat.first->second.pk_on += leg_data.ba.on;
						on_off_stat.first->second.pk_off += leg_data.ab.off;
					} else {
						on_off_stat.first->second.op_on += leg_data.ba.on;
						on_off_stat.first->second.op_off += leg_data.ab.off;
					}
				}
			}
		}

	} else {
		
		TPPlus_Data tpp_rec, *tpp_ptr, *min_ptr;
		TPPlus_Array tpp_array;
	
		Integers read_flag;
		Int_Itr read_itr;

		//---- combine files ----

		tpp_array.assign (list.size (), tpp_rec);
		read_flag.assign (list.size (), 1);
		stat = true;

		while (stat) {
			stat = false;

			//---- read the next file record ----

			for (i=0, file_itr = list.begin (); file_itr != list.end (); file_itr++, i++) {
				if (read_flag [i] == 1) {
					fh = *file_itr;
					if (!fh->Is_Open ()) {
						read_flag [i] = -1;
						continue;
					}
					if (fh->Read ()) {
						tpp_ptr = &tpp_array [i];

						tpp_ptr->a = fh->Get_Integer ("A");
						tpp_ptr->b = fh->Get_Integer ("B");
						tpp_ptr->mode = fh->Get_Integer ("MODE");
						tpp_ptr->name = fh->Get_String ("NAME");
						tpp_ptr->name.Trim ();

						freq = fh->Get_Double ("FREQ");
						if (freq > 0.0) {
							tpp_ptr->runs = 60.0 / freq;
						} else {
							tpp_ptr->runs = 0.0;
						}
						if (service_flag && period < 2) {
							service_itr = service_map.find (tpp_ptr->name);
							if (service_itr != service_map.end ()) {
								if (period == 0) {
									tpp_ptr->service = service_itr->second >> 16;
								} else {
									tpp_ptr->service = service_itr->second & 0xFFFF;
								}
							}
						}
						tpp_ptr->ab.ride = fh->Get_Integer ("AB_VOL");
						tpp_ptr->ab.on_a = fh->Get_Integer ("AB_BRDA");
						tpp_ptr->ab.off_a = fh->Get_Integer ("AB_XITA");
						tpp_ptr->ab.on_b = fh->Get_Integer ("AB_BRDB");
						tpp_ptr->ab.off_b = fh->Get_Integer ("AB_XITB");
						tpp_ptr->ba.ride = fh->Get_Integer ("BA_VOL");
						tpp_ptr->ba.on_a = fh->Get_Integer ("BA_BRDA");
						tpp_ptr->ba.off_a = fh->Get_Integer ("BA_XITA");
						tpp_ptr->ba.on_b = fh->Get_Integer ("BA_BRDB");
						tpp_ptr->ba.off_b = fh->Get_Integer ("BA_XITB");
						stat = true;
					} else {
						read_flag [i] = -1;
						fh->Close ();
					}
				} else if (read_flag [i] == 0) {
					stat = true;
				}
			}
			if (!stat) break;
			Show_Progress ();

			//---- find the record with the minimum sort sequence ----

			min_ptr = 0;

			for (i=0, read_itr = read_flag.begin (); read_itr != read_flag.end (); read_itr++, i++) {
				if (*read_itr < 0) continue;

				tpp_ptr = &tpp_array [i];

				if (min_ptr == 0) {
					min_ptr = tpp_ptr;
					continue;
				}
				if (min_ptr->a < tpp_ptr->a) continue;
				if (min_ptr->a > tpp_ptr->a) {
					min_ptr = tpp_ptr;
					continue;
				}
				if (min_ptr->b < tpp_ptr->b) continue;
				if (min_ptr->b > tpp_ptr->b) {
					min_ptr = tpp_ptr;
					continue;
				}
				if (min_ptr->mode < tpp_ptr->mode) continue;
				if (min_ptr->mode > tpp_ptr->mode) {
					min_ptr = tpp_ptr;
					continue;
				}
				if (min_ptr->name > tpp_ptr->name) {
					min_ptr = tpp_ptr;
				}
			}

			//---- find all records that match the minimum sort sequence ----

			for (i=j=0, read_itr = read_flag.begin (); read_itr != read_flag.end (); read_itr++, i++) {
				if (*read_itr < 0) continue;

				tpp_ptr = &tpp_array [i];

				if (min_ptr == tpp_ptr) {
					*read_itr = 1;
					j = i;
					continue;
				}
				if (min_ptr->a == tpp_ptr->a && min_ptr->b == tpp_ptr->b && min_ptr->mode == tpp_ptr->mode &&
					min_ptr->name == tpp_ptr->name) {
					
					*read_itr = 1;

					min_ptr->ab.ride += tpp_ptr->ab.ride;
					min_ptr->ab.on_a += tpp_ptr->ab.on_a;
					min_ptr->ab.off_a += tpp_ptr->ab.off_a;
					min_ptr->ab.on_b += tpp_ptr->ab.on_b;
					min_ptr->ab.off_b += tpp_ptr->ab.off_b;
					
					min_ptr->ba.ride += tpp_ptr->ba.ride;
					min_ptr->ba.on_a += tpp_ptr->ba.on_a;
					min_ptr->ba.off_a += tpp_ptr->ba.off_a;
					min_ptr->ba.on_b += tpp_ptr->ba.on_b;
					min_ptr->ba.off_b += tpp_ptr->ba.off_b;
				} else {
					*read_itr = 0;
				}
			}

			//---- copy fields to the output file ----

			fh = list [j];

			if (output.Is_Open ()) {
				output.Copy_Fields (*fh);

				if (min_ptr->runs > 0.0) {
					freq = 60 / min_ptr->runs;
				} else {
					freq = 0.0;
				}
				output.Put_Field ("FREQ", freq);
				output.Put_Field ("AB_VOL", min_ptr->ab.ride);
				output.Put_Field ("AB_BRDA", min_ptr->ab.on_a);
				output.Put_Field ("AB_XITA", min_ptr->ab.off_a);
				output.Put_Field ("AB_BRDB", min_ptr->ab.on_b);
				output.Put_Field ("AB_XITB", min_ptr->ab.off_b);
				output.Put_Field ("BA_VOL", min_ptr->ba.ride);
				output.Put_Field ("BA_BRDA", min_ptr->ba.on_a);
				output.Put_Field ("BA_XITA", min_ptr->ba.off_a);
				output.Put_Field ("BA_BRDB", min_ptr->ba.on_b);
				output.Put_Field ("BA_XITB", min_ptr->ba.off_b);

				output.Write ();
			}

			//---- save the data to the line map ----

			if (map == 0 || min_ptr->name.Ends_With ("-")) continue;

			if (min_ptr->name [0] == '*') {
				if (access_flag) {
					vol = min_ptr->ab.ride;
					if (vol == 0) continue;

					access_key.stop = min_ptr->a;
					access_key.mode = min_ptr->mode;
					access_key.node = min_ptr->b;

					access_stat = access_map.insert (Access_Map_Data (access_key, on_off_data));

					if (period == 0) {
						access_stat.first->second.pk_off += vol;
					} else {
						access_stat.first->second.op_off += vol;
					}
					access_key.stop = min_ptr->b;
					access_key.node = min_ptr->a;

					access_stat = access_map.insert (Access_Map_Data (access_key, on_off_data));

					if (period == 0) {
						access_stat.first->second.pk_on += vol;
					} else {
						access_stat.first->second.op_on += vol;
					}
				}
				continue;
			}
			sort_key.mode = (short) min_ptr->mode;
			sort_key.name = min_ptr->name;
			sort_key.leg = (short) fh->Get_Integer ("SEQ");

			if (fh->Get_Integer ("STOP_A") == 0) {
				leg_data.a = -min_ptr->a;
			} else {
				leg_data.a = min_ptr->a;
			}
			if (fh->Get_Integer ("STOP_B") == 0) {
				leg_data.b = -min_ptr->b;
			} else {
				leg_data.b = min_ptr->b;
			}
			leg_data.time = (short) fh->Get_Integer ("TIME");
			leg_data.dist = (short) fh->Get_Integer ("DIST");

			leg_data.runs = min_ptr->runs;
			leg_data.service = min_ptr->service;
			leg_data.ab.ride = min_ptr->ab.ride;
			leg_data.ab.on = min_ptr->ab.on_a;
			leg_data.ab.off = min_ptr->ab.off_b;
			leg_data.ba.ride = min_ptr->ba.ride;
			leg_data.ba.on = min_ptr->ba.on_b;
			leg_data.ba.off = min_ptr->ba.off_a;

			map_stat = map->insert (Line_Map_Data (sort_key, leg_data));
			if (!map_stat.second) {
				Warning (String ("Duplicate Line Data %d-%s-%d") % sort_key.mode % sort_key.name % sort_key.leg);
			}
		}
	}
	End_Progress ();

	if (output.Is_Open ()) {
		output.Close ();
	}
}
