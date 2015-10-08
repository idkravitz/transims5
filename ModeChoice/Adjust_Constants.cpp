//*********************************************************
//	Adjust_Constants - perform mode choice calibration
//*********************************************************

#include "ModeChoice.hpp"

#include <math.h>

//---------------------------------------------------------
//	Adjust_Constatns
//---------------------------------------------------------

bool ModeChoice::Adjust_Constants (int iteration, bool save_flag)
{
	int n, seg, s1, s2, s, count, tab, mode, m;
	double target, trips, total_target, total_trips, constant, prev_trips, prev_const, total;
	double target_sq, trip_sq, target_trip, error, error_sq, per_error, factor, diff, rmse;
	bool flag, distribute_flag, log_flag;

	Market_Seg current_const;
	Integers *nest_ptr;
	Int_Itr mode_itr;
	String text;

	flag = true;
	count = 0;
	error_sq = total = 0;

	if (calib_seg_flag) {
		s1 = 1;
		s2 = num_market;
		distribute_flag = false;
	} else {
		s1 = s2 = 0;
		distribute_flag = segment_flag;
	}
	current_const = seg_constant;

	//---- process each segment ----

	for (seg=s1; seg <= s2; seg++) {

		//---- get the trip totals ----

		tab = num_tables;
		total_target = total_trips = 0.0;

		for (mode=0; mode < num_modes; mode++) {
			if (mode_nest [mode] < 0) {
				total_trips += market_seg [seg] [mode] [tab];
				total_target += calib_target [seg] [mode] [tab];
			}
		}
		
		//---- update the mode constants ----

		for (mode=0; mode < num_modes; mode++) {
			constant = current_const [seg] [mode] [tab];

			target = calib_target [seg] [mode] [tab];
			if (target < 0) continue;
			if (target == 0) {
				seg_constant [seg] [mode] [tab] = min_const [seg] [mode];
				continue;
			}
			trips = market_seg [seg] [mode] [tab];
			diff = trips - target;
			if (diff != 0) flag = false;

			//---- write the calibration data ----
				
			if (data_flag) {
				per_error = 100.0 * diff / target;

				if (seg > 0) {
					data_file.File () << seg << "\t";
				}
				if (calib_tab_flag) {
					data_file.File () << "Total\t";
				}
				text ("%s\t%.0lf\t%.0lf\t%.0lf\t%.1lf%%") % mode_names [mode] % target % trips % diff % per_error % FINISH;

				data_file.File () << text << (String ("\t%.6lf") % constant) << endl;
			}

			//---- accumulate the convergence data ----

			if (mode_nested [mode] < 0) {
				count++;
				error_sq += diff * diff;
				total += target;
			}
			log_flag = true;

			//---- try to interpolate the constant ----

			if (iteration > 1) {
				prev_trips = calib_trips [seg] [mode] [tab];			

				if (prev_trips <= target && target <= trips) {
					prev_const = calib_const [seg] [mode] [tab];

					if (prev_const != 0.0 || constant != 0.0) {
						factor = trips - prev_trips;

						if (factor != 0.0) {
							factor = (target - prev_trips) / factor;
							if (factor < 0.5 ) {
								factor = 0.5;
							} else if (factor > 1.5) {
								factor = 1.5;
							}
							constant = prev_const + factor * (constant - prev_const);
							log_flag = false;
						}
					}
				} else if (trips <= target && target <= prev_trips) {
					prev_const = calib_const [seg] [mode] [tab];
						
					if (prev_const != 0.0 || constant != 0.0) {
						factor = prev_trips - trips;

						if (factor != 0.0) {
							factor = (prev_trips - target) / factor;
							if (factor < 0.5 ) {
								factor = 0.5;
							} else if (factor > 1.5) {
								factor = 1.5;
							}
							constant = prev_const - factor * (constant - prev_const);
							log_flag = false;
						}
					}
				}
			}

			//---- apply log-based extrapolation ----

			if (log_flag && trips > 0.0) {
				factor = (target / total_target) / (trips / total_trips);
				constant += scale_fac * log (factor);
			}
			seg_constant [seg] [mode] [tab] = constant;
		}

		//---- normalize to the zero mode constant ----

		for (n=0; n < num_nests; n++) {
			nest_ptr = &nested_modes [n];
			target = factor = 0;

			for (mode_itr = nest_ptr->begin (); mode_itr != nest_ptr->end (); mode_itr++) {
				mode = *mode_itr;
				target += trips = (calib_target [seg] [mode] [tab] + market_seg [seg] [mode] [tab]) / 2;
				factor += trips * seg_constant [seg] [mode] [tab];
			}
			if (target == 0) continue;
			factor /= target;

			for (mode_itr = nest_ptr->begin (); mode_itr != nest_ptr->end (); mode_itr++) {
				mode = *mode_itr;
				constant = seg_constant [seg] [mode] [tab] - factor;

				if (constant < min_const [seg] [mode]) {
					constant = min_const [seg] [mode];
				} else if (constant > max_const [seg] [mode]) {
					constant = max_const [seg] [mode];
				}
				seg_constant [seg] [mode] [tab] = constant;

				//---- distribute the mode constants ----

				if (distribute_flag) {
					for (s=1; s <= num_market; s++) {
						seg_constant [s] [mode] [tab] = constant;
					}
				}
			}
		}
		
		//---- update the table constants ----

		if (calib_tab_flag) {
			for (tab=0; tab < num_tables; tab++) {

				//---- get the total target ----

				total_target = 0.0;

				for (mode=0; mode < num_modes; mode++) {
					if (mode_nest [mode] < 0) {
						total_target += calib_target [seg] [mode] [tab];
					}
				}

				//----- adjust the constant for each mode ----

				for (mode=0; mode < num_modes; mode++) {
					if (mode_nested [mode] >= 0) continue;

					target = calib_target [seg] [mode] [num_tables];
					if (target < 0) continue;

					total_trips = market_seg [seg] [mode] [num_tables];
					error = total_trips - target;

					constant = current_const [seg] [mode] [tab];

					target = calib_target [seg] [mode] [tab];
					if (target < 0) continue;

					trips = market_seg [seg] [mode] [tab];
					diff = trips - target;
					error_sq += diff * diff;
					count++;

					if (data_flag) {
						per_error = 100.0 * diff / target;

						if (seg > 0) {
							data_file.File () << seg << "\t";
						}
						data_file.File () << trip_file->Field (tab)->Name () << "\t";

						text ("%s\t%.0lf\t%.0lf\t%.0lf\t%.1lf%%") % mode_names [mode] % target % trips % diff % per_error % FINISH;

						data_file.File () << text << (String ("\t%.6lf") % constant) << endl;
					}
					diff -= error * trips / total_trips;
					log_flag = true;

					//---- try to interpolate the constant ----

					if (iteration > 1) {
						prev_trips = calib_trips [seg] [mode] [tab];
						trips -= diff * 0.5;

						if (prev_trips <= target && target <= trips) {
							prev_const = calib_const [seg] [mode] [tab];

							if (prev_const != 0.0 || constant != 0.0) {
								factor = trips - prev_trips;

								if (factor != 0.0) {
									factor = (target - prev_trips) / factor;
									if (factor < 0.5 ) {
										factor = 0.5;
									} else if (factor > 1.5) {
										factor = 1.5;
									}
									constant = prev_const + factor * (constant - prev_const);
									log_flag = false;
								}
							}
						} else if (trips <= target && target <= prev_trips) {
							prev_const = calib_const [seg] [mode] [tab];
						
							if (prev_const != 0.0 || constant != 0.0) {
								factor = prev_trips - trips;

								if (factor != 0.0) {
									factor = (prev_trips - target) / factor;
									if (factor < 0.5 ) {
										factor = 0.5;
									} else if (factor > 1.5) {
										factor = 1.5;
									}
									constant = prev_const - factor * (constant - prev_const);
									log_flag = false;
								}
							}
						}
					}

					//---- apply log-based extrapolation ----

					if (log_flag && total_target > 0.0) {
						factor = 1 - diff / total_target;
						constant += scale_fac * log (factor);
					}
					if (constant < min_const [seg] [mode]) {
						constant = min_const [seg] [mode];
					} else if (constant > max_const [seg] [mode]) {
						constant = max_const [seg] [mode];
					}
					seg_constant [seg] [mode] [tab] = constant;

					if (distribute_flag) {
						for (s=1; s <= num_market; s++) {
							seg_constant [s] [mode] [tab] = constant;
						}
					}
				}
			}
		}
	}
	calib_trips = market_seg;
	calib_const = current_const;

	//---- calculate the rmse ----

	factor = (double) count;
	rmse = 100.0 * sqrt (error_sq / factor) * factor / total;

	text ("%d  RMSE=%.1lf%%") % zones % rmse % FINISH;
	End_Progress (text);
	
	if (rmse <= exit_rmse) flag = true;

	//---- write the calibration data file ----

	if (output_flag && save_flag) {
		for (seg=s1; seg <= s2; seg++) {
			for (mode=0; mode < num_modes; mode++) {
				n = 0;
				if (calib_seg_flag) {
					calib_file.Put_Field (n++, seg);
				}
				calib_file.Put_Field (n++, mode_names [mode]);
				calib_file.Put_Field (n++, calib_const [seg] [mode] [num_tables]);

				for (tab=0; tab < num_tables; tab++) {
					calib_file.Put_Field (n++, calib_const [seg] [mode] [tab]);
				}
				calib_file.Write ();
			}
		}
	}

	//---- write the calibration report ----

	if (calib_report) {
		header_value = iteration;
		Header_Number (CALIB_REPORT);

		if (!Break_Check (num_modes + 8)) {
			Print (1);
			Calib_Header ();
		}
		total_target = total_trips = target_sq = trip_sq = target_trip = error = per_error = 0.0;

		for (seg=s1; seg <= s2; seg++) {
			for (mode=0; mode < num_modes; mode++) {
				trips = calib_trips [seg] [mode] [num_tables];
				target = calib_target [seg] [mode] [num_tables];

				diff = trips - target;
				if (target > 0) {
					factor = diff * 100.0 / target;
				} else {
					factor = 0.0;
				}
				Print (1, "");
				if (calib_seg_flag) {
					Print (0, String ("%4d    ") % seg);
				}
				Print (0, String ("%-20s  %8.0lf  %8.0lf    %8.0lf  %6.1lf%%") % 
					mode_names [mode] % target % trips % diff % factor % FINISH);

				constant = calib_const [seg] [mode] [num_tables];
				
				Print (0, String ("  %10.6lf") % constant);

				if (mode_nested [mode] < 0) {
					for (m = mode_nest [mode]; m >= 0; m = mode_nest [m]) {
						constant = constant * nest_coef [mode_nested [m]] + calib_const [seg] [m] [num_tables];
						constant = log (exp (constant));
					}
					constant /= -time_value;
					Print (0, String ("  %6.1lf") % constant);
				}

				//---- bottom line statistics ----

				if (mode_nested [mode] < 0) {
					total_trips += trips;
					trip_sq += trips * trips;
					total_target += target;
					target_sq += target * target;
					target_trip += target * trips;
					diff = fabs (diff);
					error += diff;
					per_error += diff / target;
				}
			}
		}

		//---- write statistics ----

		diff = error * 100.0 / total_target;
		text ("%.1lf%%") % diff % FINISH;

		Print (2, String ("Abs.Error = %-6s") % text);

		factor = (double) count;
		diff = per_error * 100.0 / factor;
		text ("%.1lf%%") % diff % FINISH;

		Print (0, String ("  Avg.Error = %-7s") % text);

		text ("%.1lf%%") % rmse % FINISH;
		Print (0, String ("  RMSE = %-6s") % text);

		diff = (trip_sq - total_trips * total_trips / factor) * (target_sq - total_target * total_target / factor);

		if (diff != 0.0) {
			diff = (target_trip - total_target * total_trips / factor) / sqrt (diff);
			diff *= diff;
		} else {
			diff = 0.0;
		}
		Print (0, String ("  R-Squared = %5.3lf") % diff);

		Header_Number (0);
	}
	return (flag);
}

//---------------------------------------------------------
//	Calib_Header
//---------------------------------------------------------

void ModeChoice::Calib_Header (void)
{
	Print (1, "Calibration Report for Iteration #") << header_value << " -- " << purpose_label;
	Print (2, "");
	if (calib_seg_flag) {
		Print (0, "Segment ");
	}
	Print (0, "Trips by Mode           Target     Trips  Difference  Percent    Constant  Minutes");
	Print (1);
}

/*********************************************|***********************************************

	Calibration Report for Iteration #dd -- sssssssssssss

	Trips by Mode           Target     Trips  Difference  Percent    Constant  Minutes

	ssssssss20ssssssssss  ffffffff  ffffffff    ffffffff  ffff.f%  fff.ffffff  ffff.f

or
	Segment  Trips by Mode           Target     Trips  Difference  Percent    Constant  Minutes

	 dddd    ssssssss20ssssssssss  ffffffff  ffffffff    ffffffff  ffff.f%  fff.ffffff  ffff.f

	Abs.Error = fff.f%  Avg.Error = ffff.f%  RMSE = fff.f%  R-Squared = f.fff

**********************************************|***********************************************/ 
