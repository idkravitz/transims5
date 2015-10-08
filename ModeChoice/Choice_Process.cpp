//*********************************************************
//	Choice_Process - perform mode choice analysis
//*********************************************************

#include "ModeChoice.hpp"

#include <math.h>

//---------------------------------------------------------
//	Choice_Process
//---------------------------------------------------------

void ModeChoice::Choice_Process (int iteration, bool save_flag)
{
	int i, j, k, org, des, o1, d1, n, nest, table, org_map, des_map, segment, seg, group;
	double **data, *data_ptr, **output, trips, access, sum, value, cost, zero_sum, level;
	double can_walk_market, can_walk_share, must_drive_market, must_drive_share, auto_exp;
	bool org_flag, initial_flag, sum_flag, prod_sum_flag, attr_sum_flag, save_summit_flag;
	Doubles zero_tab, constant, utility, nest_sum;
	Dbls_Array mode_sum;
	Db_Base *mode_file;
	Db_Mat_Itr mat_itr;
	Integers *market_ptr, *nest_ptr;
	Int_Itr mode_itr;
	Int2_Map_Itr map_itr;
	Dbls_Array values;

	if (calib_flag) {
		Show_Message (String ("Processing Iteration %d Origin Zone") % iteration);;
	} else {
		Show_Message ("Processing Origin Zone");
	}
	Set_Progress ();

	trip_file->Put_Field ("Purpose", purpose);
	trip_file->Put_Field ("Period", period);

	seg = segment = group = 0;
	zero_sum = -700;
	zero_tab.assign (num_tables, zero_sum);
	constant.assign (num_modes, 0.0);

	output = new_file->Data ();
	
	initial_flag = (iteration == 1 && calib_flag && !constant_flag);
	sum_flag = (save_flag && (mode_value_flag || seg_value_flag));
	prod_sum_flag = (save_flag && prod_flag);
	attr_sum_flag = (save_flag && attr_flag);
	save_summit_flag = (save_flag && summit_flag);

	//---- process each origin zone ----

	for (org=1; org <= zones; org++) {
		Show_Progress ();

		if (select_org_zones && !org_zone_range.In_Range (org)) continue;

		o1 = org - 1;
		org_flag = false;

		//---- read trip matrix ----

		if (!trip_file->Read_Record (org)) {
			Error (String ("Reading %s") % trip_file->File_Type ());
		}
		data = trip_file->Data ();

		//---- scan for trips ----

		if (!org_flag) {
			for (i=0; i < num_tables && !org_flag; i++) {
				data_ptr = data [table_map [i]];

				for (des=0; des < zones; des++, data_ptr++) {
					if (*data_ptr != 0.0) {
						org_flag = true;
						break;
					}
				}
			}
		}
		if (!org_flag) continue;

		//---- read the origin zone and skim data ----

		org_db.Read_Record (org);

		if (org_map_field >= 0) {
			org_map = org_db.Get_Integer (org_map_field);
		}
		for (mat_itr = skim_files.begin (); mat_itr != skim_files.end (); mat_itr++) {
			if (!(*mat_itr)->Read_Record (org)) {
				Error (String ("Reading %s") % (*mat_itr)->File_Type ());
			}
		}
		new_file->Zero_Data ();

		//---- process each destination zone ----
			
		for (des=1; des <= zones; des++) {
			if (select_des_zones && !des_zone_range.In_Range (des)) continue;
			d1 = des - 1;

			for (i=0; i < num_tables; i++) {
				if (data [table_map [i]] [d1] != 0.0) break;
			}
			if (i == num_tables) continue;

			des_db.Read_Record (des);

			if (segment_flag && des_map_field >= 0) {
				des_map = des_db.Get_Integer (des_map_field);

				map_itr = segment_map.find (Int2_Key (org_map, des_map));

				if (map_itr != segment_map.end ()) {
					seg = segment = map_itr->second;
					trip_file->Put_Field (segment_field, segment);

					if (initial_flag && !calib_seg_flag) {
						seg = 0;
					}
				} else {
					seg = segment = 0;
				}
			}
			trip_file->Set_Fields (des);

			for (mat_itr = skim_files.begin (); mat_itr != skim_files.end (); mat_itr++) {
				(*mat_itr)->Set_Fields (des);
			}

			//---- initialize constants ----

			for (i=0; i < num_modes; i++) {
				mode_file = data_rec [i];
				mode_file->Reset_Record ();

				if (constant_flag || calib_flag) {
					value = seg_constant [segment] [i] [num_tables];
				} else {
					value = 0;
				}
				mode_file->Put_Field (const_field, value);

				for (j=0; j < num_tables; j++) {
					if (constant_flag || calib_flag) {
						value = seg_constant [segment] [i] [j];
					} else {
						value = 0;
					}
					mode_file->Put_Field (tab_field + j, value);
				}
			}

			//---- execute the user program ----

			if (program.Execute () == 0) continue;

			//---- calculate mode utilities ----
	
			mode_sum.assign (num_modes, zero_tab);

			if (sum_flag) {
				values.assign (num_modes, zero_values);
			}
			for (i=0; i < num_modes; i++) {
				mode_file = data_rec [i];

				constant [i] = value = mode_file->Get_Double (const_field);

				if (initial_flag && value != 0.0) {
					seg_constant [seg] [i] [num_tables] = value;
				}
				value = mode_file->Get_Double (time_field);
				if (value <= 0.0) continue;

				sum = value * time_value;
				if (sum_flag) {
					values [i] [time_field] = value;
					values [i] [const_field] = 1;
				}
				value = mode_file->Get_Double (walk_field);
				if (value > 0.0) sum += value * walk_value;
				if (sum_flag) values [i] [walk_field] = value;

				value = mode_file->Get_Double (auto_field);
				if (value > 0.0) sum += value * drive_value;
				if (sum_flag) values [i] [auto_field] = value;

				value = mode_file->Get_Double (wait_field);
				if (value > 0.0) sum += value * wait_value;
				if (sum_flag) values [i] [wait_field] = value;

				value = mode_file->Get_Double (lwait_field);
				if (value > 0.0) sum += value * lwait_value;
				if (sum_flag) values [i] [lwait_field] = value;

				value = mode_file->Get_Double (xwait_field);
				if (value > 0.0) sum += value * xwait_value;
				if (sum_flag) values [i] [xwait_field] = value;

				value = mode_file->Get_Double (tpen_field);
				if (value > 0.0) sum += value * tpen_value;
				if (sum_flag) values [i] [tpen_field] = value;

				value = mode_file->Get_Double (term_field);
				if (value > 0.0) sum += value * term_value;
				if (sum_flag) values [i] [term_field] = value;

				value = mode_file->Get_Double (xfer_field);
				if (value > 0.0) sum += value * xfer_value;
				if (sum_flag) values [i] [xfer_field] = value;

				value = mode_file->Get_Double (bias_field);
				sum += value;
				if (sum_flag) values [i] [bias_field] = value;

				value = mode_file->Get_Double (pef_field);
				sum += value;
				if (sum_flag) values [i] [pef_field] = value;

				value = mode_file->Get_Double (cbd_field);
				sum += value;
				if (sum_flag) values [i] [cbd_field] = value;

				cost = mode_file->Get_Double (cost_field);
				if (sum_flag) values [i] [cost_field] = cost;

				level = nest_levels [i];

				for (j=0; j < num_tables; j++) {
					value = mode_file->Get_Double (tab_field + j);

					if (initial_flag && value != 0.0) {
						seg_constant [seg] [i] [j] = value;
					}
					if (cost > 0.0) value += cost * cost_values [j];

					mode_sum [i] [j] = (sum + value) / level + constant [i];
				}
			}

			//---- split to each access market ----

			for (i=0; i < num_tables; i++) {
				trips = trip_file->Get_Double (table_map [i]);
				if (trips == 0.0) continue;

				can_walk_market = can_walk_share = must_drive_market = must_drive_share = auto_exp = 0.0;

				for (j=0; j < num_access; j++) {
					access = trips * trip_file->Get_Double (market_field + j);
					if (access == 0.0) continue;

					if (save_summit_flag) {
						group = market_group [j];
						if (group == 1) {
							can_walk_market += access;
						} else if (group == 2) {
							must_drive_market += access;
						}
					}

					//---- calculate the mode utilities ----

					utility.assign (num_modes, 0.0);
					market_ptr = &access_markets [j];

					for (mode_itr = market_ptr->begin (); mode_itr != market_ptr->end (); mode_itr++) {
						sum = mode_sum [*mode_itr] [i];
						if (sum <= zero_sum) continue;

						utility [*mode_itr] = exp (sum);
					}

					//---- calculate the nest sum ----

					nest_sum.assign (num_nests, 0.0);

					for (n = num_nests - 1; n >= 0; n--) {
						sum = 0;
						nest = nest_mode [n];
						nest_ptr = &nested_modes [n];

						for (mode_itr = nest_ptr->begin (); mode_itr != nest_ptr->end (); mode_itr++) {
							sum += utility [*mode_itr];
						}
						nest_sum [n] = sum;
						if (nest >= 0 && sum > 0) {
							sum = log (sum) * nest_coef [n] + constant [nest];
							utility [nest] = exp (sum);
						}
					}
					auto_exp = utility [0];

					//---- calculate the probabilities ----

					for (n = 0; n < num_nests; n++) {
						sum = nest_sum [n];
						if (sum <= 0.0) continue;

						nest = nest_mode [n];
						nest_ptr = &nested_modes [n];

						if (nest < 0) {
							sum = access / sum;
						} else {
							sum = utility [nest] / sum;
						}
						for (mode_itr = nest_ptr->begin (); mode_itr != nest_ptr->end (); mode_itr++) {
							value = utility [*mode_itr] *= sum;

							if (save_flag) {
								table = output_table [*mode_itr] [i];

								if (table >= 0) {
									output [table] [d1] += value * trip_factor;
								}
								if (sum_flag) {
									for (k=0; k <= const_field; k++) {
										cost = values [*mode_itr] [k] * value;

										if (mode_value_flag) {
											value_sum [0] [*mode_itr] [k] += cost;
										}
										if (seg_value_flag) {
											value_sum [segment] [*mode_itr] [k] += cost;
										}
									}
								}
							}
							if (summary_flag) {
								market_seg [0] [*mode_itr] [i] += value;
								market_seg [0] [*mode_itr] [num_tables] += value;
							}
							if (market_flag && segment > 0) {
								market_seg [segment] [*mode_itr] [i] += value;
								market_seg [segment] [*mode_itr] [num_tables] += value;
							}
							if (save_summit_flag && nest < 0 && *mode_itr == 1 && group > 0) {
								if (group == 1) {
									can_walk_share += value;
								} else if (group == 2) {
									must_drive_share += value;
								}
							}
							if (prod_sum_flag) {
								prod_share [*mode_itr] [o1] += value;
							}
							if (attr_sum_flag) {
								attr_share [*mode_itr] [d1] += value;
							}
						}
					}

					//---- store total trips ----

					if (summary_flag) {
						market_seg [0] [num_modes] [i] += access;
						market_seg [0] [num_modes] [num_tables] += access;
					}
					if (market_flag && segment > 0) {
						market_seg [segment] [num_modes] [i] += access;
						market_seg [segment] [num_modes] [num_tables] += access;
					}
					if (prod_sum_flag) {
						prod_share [num_modes] [o1] += access;
					}
					if (attr_sum_flag) {
						attr_share [num_modes] [d1] += access;
					}
				}

				//---- write the summit record ----

				if (save_summit_flag) {
					summit_file.Put_Field (summit_org, org);
					summit_file.Put_Field (summit_des, des);
					summit_file.Put_Field (summit_market, i+1);
					summit_file.Put_Field (summit_total_trips, trips);
					summit_file.Put_Field (summit_motor_trips, trips);
					summit_file.Put_Field (summit_auto_exp, auto_exp);

					if (can_walk_market <= 0.0) {
						can_walk_market = can_walk_share = 0.0;
					} else {
						can_walk_share /= can_walk_market;
						can_walk_market /= trips;
					}
					summit_file.Put_Field (summit_walk_market, can_walk_market);
					summit_file.Put_Field (summit_walk_share, can_walk_share);

					if (must_drive_market <= 0.0) {
						must_drive_market = must_drive_share = 0.0;
					} else {
						must_drive_share /= must_drive_market;
						must_drive_market /= trips;
					}
					summit_file.Put_Field (summit_drive_market, must_drive_market);
					summit_file.Put_Field (summit_drive_share, must_drive_share);

					summit_file.Write (true);
				}
			}
		}
		if (save_flag) {
			if (!new_file->Write_Record (org)) {
				Error ("Writing Tables");
			}
		}
	}
	if (!calib_flag) End_Progress ();
}
