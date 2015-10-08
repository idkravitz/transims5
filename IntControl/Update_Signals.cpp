//*********************************************************
//	Update_Signals.cpp - Update signal timing plans
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Update_Signals
//---------------------------------------------------------

void IntControl::Update_Signals (void)
{
	int method, min_phase;
	double pocket_factor, shared_factor, turn_factor, permit_factor;	

	int p, p1, p2, phase, nphase, dir, tot, low, high, sum, cycle, count, time;
	int bar, nbarrier, barrier_extra, barrier_critical;
	int extra, ring, nring, ring_extra, ring_critical, min_cap, max_cap;
	int split [MAX_PHASE_NUM], demand [MAX_PHASE_NUM], critical [MAX_PHASE_NUM];
	double rate, share, bucket, factor, lanes [MAX_PHASE_NUM];
	String buffer;
	bool adjust, first;
	
	Int_Map_Itr map_itr;	
	Int_Set_Itr set_itr;
	Signal_Data *signal_ptr;
	Signal_Itr signal_itr;
	Signal_Time_Itr time_itr;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;
	Connect_Data *connect_ptr;
	Dir_Data *dir_ptr;
	Timing_Itr timing_itr;
	Timing_Phase_Itr phase_itr;
	Group_Map_Itr group_itr;
	Signal_Group *group_ptr;

	typedef struct {
		int    dir_index;
		int    left_rate;
		int    thru_rate;
		int    right_rate;
		double left_lanes;
		double thru_lanes;
		double right_lanes;
		char   left_low;
		char   left_high;
		char   thru_low;
		char   thru_high;
		char   right_low;
		char   right_high;
		char   protect;
		char   flag;
	} Approach;

	Approach total [MAX_DIR_TOT], approach [MAX_PHASE_NUM] [MAX_DIR_NUM], *app_ptr, *tot_ptr;

	min_cap = max_cap = 0;

	if (regen_flag) {
		for (signal_itr = signal_array.begin (); signal_itr != signal_array.end (); signal_itr++) {
			update_signal.insert (signal_itr->Signal ());
		}
	}

	//---- initialize the report header ----

	if (update_report) {
		Header_Number (TIMING_UPDATE);

		if (!Break_Check ((int) update_signal.size () * 6)) {
			Print (1);
			Update_Header ();
		}
	}
	first = true;

	//---- process each signal marked for updating ----

	Show_Message ("Updating Signal Timing Plans -- Record");
	Set_Progress ();

	for (set_itr = update_signal.begin (); set_itr != update_signal.end (); set_itr++) {
		Show_Progress ();
		
		map_itr = signal_map.find (*set_itr);
		if (map_itr == signal_map.end ()) continue;

		signal_ptr = &signal_array [map_itr->second];

		//---- find the signal group ----

		group_itr = group_map.find (signal_ptr->Group ());

		if (group_itr == group_map.end ()) {
			if (signal_ptr->Group () != 0) {
				Warning (String ("Control Keys for Signal Group %d were Not Provided") % signal_ptr->Group ());
			}
			group_itr = group_map.begin ();
			if (group_itr == group_map.end ()) continue;
		}
		group_ptr = &group_itr->second;

		//---- process each time period in range ----

		for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
			if (!sum_periods.Period_Range (time_itr->Start (), time_itr->End (), p1, p2)) continue;

			p = group_ptr->time_breaks.Period ((time_itr->Start () + time_itr->End ()) / 2);
			if (p++ < 0) continue;

			pocket_factor = group_ptr->pocket_factor.Best (p);
			shared_factor = group_ptr->shared_factor.Best (p);
			turn_factor = group_ptr->turn_factor.Best (p);
			permit_factor = group_ptr->permit_factor.Best (p);
			min_phase = group_ptr->min_phase.Best (p);
			method = group_ptr->method.Best (p);

			if (method == CAPACITY_SPLITS) {
				min_cap = group_ptr->min_cap.Best (p);
				max_cap = group_ptr->max_cap.Best (p);
			}

			//---- gather the phase and flow data ----

			nphase = 0;
			memset (approach, '\0', sizeof (approach));
			memset (total, '\0', sizeof (total));

			tot_ptr = total;

			for (tot=0; tot < MAX_DIR_TOT; tot++, tot_ptr++) {
				tot_ptr->dir_index = -1;
			}

			for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
				if (phasing_itr->Phasing () != time_itr->Phasing ()) continue;

				phase = phasing_itr->Phase ();
				if (phase >= MAX_PHASE_NUM) continue;
				if (phase > nphase) nphase = phase;

				app_ptr = approach [phase];

				for (dir=0; dir < MAX_DIR_NUM; dir++, app_ptr++) {
					app_ptr->dir_index = -1;
				}

				//---- process each turning movement ----

				for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
					if (move_itr->Protection () == STOP_PERMIT) continue;

					connect_ptr = &connect_array [move_itr->Connection ()];

					low = connect_ptr->Low_Lane () + 1;
					high = connect_ptr->High_Lane () + 1;

					//---- sum the time period rates ----

					rate = 0;

					if (method == VOLUME_SPLITS || method == UPDATE_SPLITS) {
						for (p=p1; p <= p2; p++) {
							array_ptr = &turn_delay_array [p];
							flow_ptr = &array_ptr->at (move_itr->Connection ());
							rate += flow_ptr->Flow ();
						}
					} else if (method == CAPACITY_SPLITS) {
						dir_ptr = &dir_array [connect_ptr->Dir_Index ()];
						rate = (double) dir_ptr->Capacity () / dir_ptr->Lanes ();
						if (rate < min_cap) {
							rate = min_cap;
						} else if (rate > max_cap) {
							rate = max_cap;
						}
						rate *= dir_ptr->Lanes ();
					}
					//if (rate < 1.0) rate = 1.0;

					//---- store the movement type data ----

					app_ptr = approach [phase];

					for (dir=0; dir < MAX_DIR_NUM; dir++, app_ptr++) {
						if (app_ptr->dir_index < 0 || app_ptr->dir_index == connect_ptr->Dir_Index ()) {
							app_ptr->dir_index = connect_ptr->Dir_Index ();

							if (connect_ptr->Type () == RIGHT) {
								app_ptr->right_rate += Round (rate);
								if (app_ptr->right_low == 0 || app_ptr->right_low > low) {
									app_ptr->right_low = (char) low;
								}
								if (app_ptr->right_high == 0 || app_ptr->right_high < high) {
									app_ptr->right_high = (char) high;
								}
							} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
								app_ptr->left_rate += Round (rate);
								if (app_ptr->left_low == 0 || app_ptr->left_low > low) {
									app_ptr->left_low = (char) low;
								}
								if (app_ptr->left_high == 0 || app_ptr->left_high < high) {
									app_ptr->left_high = (char) high;
								}
								if (move_itr->Protection () == PROTECTED) {
									app_ptr->protect = 1;
								}
							} else {
								app_ptr->thru_rate += Round (rate);
								if (app_ptr->thru_low == 0 || app_ptr->thru_low > low) {
									app_ptr->thru_low = (char) low;
								}
								if (app_ptr->thru_high == 0 || app_ptr->thru_high < high) {
									app_ptr->thru_high = (char) high;
								}
							}
							break;
						}
					}
				}

				//---- calculate the lane weights for each movement ----

				app_ptr = approach [phase];

				for (dir=0; dir < MAX_DIR_NUM; dir++, app_ptr++) {
					if (app_ptr->dir_index < 0) break;

					dir_ptr = &dir_array [app_ptr->dir_index];

					if (app_ptr->left_rate > 0) {

						//---- check for shared lanes ----

						if (app_ptr->thru_rate > 0 && app_ptr->left_high < app_ptr->thru_low) {	

							//---- shared thru-left lanes ----

							share = shared_factor * (app_ptr->left_high - app_ptr->thru_low + 1);	
							app_ptr->thru_lanes = share;
							app_ptr->left_lanes = share + (app_ptr->thru_low - app_ptr->left_low) * pocket_factor;

						//---- check for pocket lanes ----

						} else if (dir_ptr->Left () > 0 && app_ptr->left_low <= dir_ptr->Left ()) {
							share = app_ptr->left_high - dir_ptr->Left ();
							app_ptr->left_lanes = share + (dir_ptr->Left () - app_ptr->left_low + 1) * pocket_factor;
						} else {
							app_ptr->left_lanes = app_ptr->left_high - app_ptr->left_low + 1;
						}

						//---- turn slow down impact ----

						app_ptr->left_lanes *= turn_factor;

						//---- non-protected left adjustment ----

						if (app_ptr->protect == 0) {
							app_ptr->left_lanes *= permit_factor;
						}
					}
					if (app_ptr->right_rate > 0) {

						//---- check for shared lanes ----

						if (app_ptr->thru_rate > 0 && app_ptr->right_low < app_ptr->thru_high) {
							share = shared_factor * (app_ptr->thru_high - app_ptr->right_low + 1);
							app_ptr->thru_lanes += share;
							app_ptr->right_lanes = share + (app_ptr->right_high - app_ptr->thru_high) + pocket_factor;

						//---- check for pocket lanes ----

						} else if (dir_ptr->Right () > 0 && app_ptr->right_high > (dir_ptr->Left () + dir_ptr->Lanes ())) {	
							share = dir_ptr->Left () + dir_ptr->Lanes () - app_ptr->left_low + 1;
							app_ptr->right_lanes = share + (app_ptr->right_high - (dir_ptr->Left () + dir_ptr->Lanes ())) * pocket_factor;
						} else {
							app_ptr->right_lanes = app_ptr->right_high - app_ptr->right_low + 1;
						}

						//---- turn slow down impact ----

						app_ptr->right_lanes *= turn_factor;
					}
					if (app_ptr->thru_rate > 0) {

						//---- exclude shared lanes ----

						low = MAX (app_ptr->left_high, app_ptr->thru_low);
						if (app_ptr->right_low > 0) {
							high = MIN (app_ptr->right_low, app_ptr->thru_high);
						} else {
							high = app_ptr->thru_high;
						}
						app_ptr->thru_lanes += high - low + 1;
					}

					//---- add to the intersection totals ----

					tot_ptr = total;

					for (tot=0; tot < MAX_DIR_TOT; tot++, tot_ptr++) {
						if (tot_ptr->dir_index < 0 || tot_ptr->dir_index == app_ptr->dir_index) {
							tot_ptr->dir_index = app_ptr->dir_index;

							if (tot_ptr->left_rate == 0) {
								tot_ptr->left_rate = app_ptr->left_rate;
								tot_ptr->left_low = app_ptr->left_low;
								tot_ptr->left_high = app_ptr->left_high;
							}
							if (tot_ptr->right_rate == 0) {
								tot_ptr->right_rate = app_ptr->right_rate;
								tot_ptr->right_low = app_ptr->right_low;
								tot_ptr->right_high = app_ptr->right_high;
							}
							if (tot_ptr->thru_rate == 0) {
								tot_ptr->thru_rate = app_ptr->thru_rate;
								tot_ptr->thru_low = app_ptr->thru_low;
								tot_ptr->thru_high = app_ptr->thru_high;
							}
							tot_ptr->left_lanes += app_ptr->left_lanes;
							tot_ptr->right_lanes += app_ptr->right_lanes;
							tot_ptr->thru_lanes += app_ptr->thru_lanes;
							break;
						}
					}
				}
			}

			//---- adjust the phase rates ----

			sum = 0;
			memset (lanes, '\0', sizeof (lanes));
			memset (demand, '\0', sizeof (demand));
			memset (split, '\0', sizeof (split));
			memset (critical, '\0', sizeof (critical));

			for (phase=1; phase <= nphase; phase++) {
				app_ptr = approach [phase];
				if (app_ptr->dir_index < 0) continue;

				high = 0;

				for (dir=0; dir < MAX_DIR_NUM; dir++, app_ptr++) {
					if (app_ptr->dir_index < 0) break;

					tot_ptr = total;

					for (tot=0; tot < MAX_DIR_TOT; tot++, tot_ptr++) {
						if (tot_ptr->dir_index == app_ptr->dir_index) break;
					}
					if (tot == MAX_DIR_TOT) continue;

					if (app_ptr->thru_lanes > 0.0) {
						lanes [phase] += app_ptr->thru_lanes;
						rate = tot_ptr->thru_rate * app_ptr->thru_lanes / tot_ptr->thru_lanes;
						demand [phase] += DTOI (rate);
						app_ptr->thru_rate = DTOI (rate / tot_ptr->thru_lanes);
						if (app_ptr->thru_rate > high) high = app_ptr->thru_rate;
					}
					if (app_ptr->left_lanes > 0.0) {
						lanes [phase] += app_ptr->left_lanes;
						rate = tot_ptr->left_rate * app_ptr->left_lanes / tot_ptr->left_lanes;
						demand [phase] += DTOI (rate);
						app_ptr->left_rate = DTOI (rate / tot_ptr->left_lanes);
						if (app_ptr->left_rate > high) high = app_ptr->left_rate;
					}
					if (app_ptr->right_lanes > 0.0) {
						lanes [phase] += app_ptr->right_lanes;
						rate = tot_ptr->right_rate * app_ptr->right_lanes / tot_ptr->right_lanes;
						demand [phase] += DTOI (rate);
						app_ptr->right_rate = DTOI (rate / tot_ptr->right_lanes);
						if (app_ptr->right_rate > high) high = app_ptr->right_rate;
					}
				}
				critical [phase] = split [phase] = high;
				sum += high;
			}
			if (sum == 0) continue;

			//---- process the timing plan ----

			for (timing_itr = signal_ptr->timing_plan.begin (); timing_itr != signal_ptr->timing_plan.end (); timing_itr++) {
				if (timing_itr->Timing () != time_itr->Timing ()) continue;

				//---- scan for multiple rings ----

				nbarrier = nring = 0;

				for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
					if (phase_itr->Barrier () > nbarrier) nbarrier = phase_itr->Barrier ();
					if (phase_itr->Ring () > nring) nring = phase_itr->Ring ();
				}

				//---- multiple ring processing ----

				if (nbarrier > 1 || nring > 1) {
					sum = extra = 0;

					for (bar = 1; bar <= nbarrier; bar++) {
						barrier_extra = barrier_critical = 0;

						for (ring = 1; ring <= nring; ring++) {
							ring_extra = ring_critical = 0;

							for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
								if (phase_itr->Barrier () != bar || phase_itr->Ring () != ring) continue;

								ring_critical += critical [phase_itr->Phase ()];
								ring_extra += phase_itr->Yellow () + phase_itr->All_Red ();
							}
							if (ring_critical > barrier_critical) {
								barrier_critical = ring_critical;
								barrier_extra = ring_extra;
							}
						}
						sum += barrier_critical;
						extra += barrier_extra;
					}

					//---- adjust phasing by demand ----

					for (count=0; count < 5; count++) {
						cycle = timing_itr->Cycle () - extra;

						if (sum > 0) {
							factor = (double) cycle / sum;
							sum = 0;
						} else {
							factor = 1.0;
						}
						bucket = 0.45;
						adjust = false;

						sum = extra = 0;

						for (bar = 1; bar <= nbarrier; bar++) {
							barrier_extra = barrier_critical = 0;

							for (ring = 1; ring <= nring; ring++) {
								ring_extra = ring_critical = 0;

								for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
									if (phase_itr->Barrier () != bar || phase_itr->Ring () != ring) continue;
									phase = phase_itr->Phase ();

									share = factor * split [phase] + bucket;
									high = (int) share;
									bucket = share - (double) high;

									if (high < min_phase) {
										high = min_phase;
										adjust = true;
									}
									split [phase] = high;
									ring_critical += high;
									ring_extra += phase_itr->Yellow () + phase_itr->All_Red ();
								}
								if (ring_critical > barrier_critical) {
									barrier_critical = ring_critical;
									barrier_extra = ring_extra;
								}
							}
							sum += barrier_critical;
							extra += barrier_extra;

							//---- normalize the rings in each barrier ----

							for (ring = 1; ring <= nring; ring++) {
								ring_extra = ring_critical = 0;

								for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
									if (phase_itr->Barrier () != bar || phase_itr->Ring () != ring) continue;

									ring_critical += split [phase_itr->Phase ()];
									ring_extra += phase_itr->Yellow () + phase_itr->All_Red ();
								}
								if (ring_critical >= barrier_critical || ring_critical == 0) continue;

								share = (double) (barrier_critical + barrier_extra - ring_extra) / ring_critical;

								for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
									if (phase_itr->Barrier () != bar || phase_itr->Ring () != ring) continue;
									phase = phase_itr->Phase ();

									split [phase] = DTOI (split [phase] * share);
								}
							}
						}
						if (!adjust) break;
					}

				} else {		//---- single ring ----

					//---- calculate the green cycle length ----

					cycle = timing_itr->Cycle ();

					for (phase = 1; phase <= nphase; phase++) {
						for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
							if (phase_itr->Phase () == phase) break;
						}
						if (phase_itr == timing_itr->end ()) continue;
						cycle -= (phase_itr->Yellow () + phase_itr->All_Red ());
					}

					//---- adjust phasing by demand ----

					for (count=0; count < 5; count++) {
						if (sum > 0) {
							factor = (double) cycle / sum;
							sum = 0;
						} else {
							factor = 1.0;
						}
						bucket = 0.45;
						adjust = false;

						for (phase = 1; phase <= nphase; phase++) {
							share = factor * split [phase] + bucket;
							high = (int) share;
							bucket = share - (double) high;

							if (high < min_phase) {
								high = min_phase;
								adjust = true;
							}
							split [phase] = high;
							sum += high;
						}
						if (!adjust) break;
					}
				}

				//---- update the green times ----

				for (phase = 1; phase <= nphase; phase++) {
					for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
						if (phase_itr->Phase () == phase) break;
					}
					if (phase_itr == timing_itr->end ()) continue;

					if (update_report) {
						if (phase == 1) {
							if (!Break_Check (nphase + 1)) {
								if (first) {
									first = false;
								} else {
									Print (1);
								}
							}
							buffer = time_itr->Start ().Time_String () + "-" + time_itr->End ().Time_String ();

							Print (1, String ("%10d  %-11.11s %6d") % signal_ptr->Signal () % buffer % time_itr->Timing ());
						} else {
							Print (1, String ("%30c") % BLANK);
						}
						Print (0, String ("   %3d  %7d  %4.1lf  %5d   %3d  %3d  %3d") % phase % 
							Resolve (demand [phase]) % lanes [phase] % Resolve (critical [phase]) %
							phase_itr->Min_Green () % phase_itr->Max_Green () % phase_itr->Extension ());
					}

					time = MAX (phase_itr->Min_Green (), phase_itr->Max_Green ());
					if (time > 0) {
						factor = split [phase] / (double) time;
					} else {
						factor = 1.0;
					}
					time = DTOI (phase_itr->Min_Green () * factor);
					if (time < min_phase) time = min_phase;
					phase_itr->Min_Green (time);

					time = DTOI (phase_itr->Max_Green () * factor);
					phase_itr->Max_Green (time);

					time = DTOI (phase_itr->Extension () * factor);
					phase_itr->Extension (time);

					if (update_report) {
						Print (0, String ("   %3d  %3d  %3d") % phase_itr->Min_Green () % 
							phase_itr->Max_Green () % phase_itr->Extension ());
					}
				}
				ntiming++;
			}
		}
	}
	End_Progress ();
	Header_Number (0);
}

//---------------------------------------------------------
//	Update_Header
//---------------------------------------------------------

void IntControl::Update_Header (void)
{
	Print (1, "Signal Timing Updates");
	Print (2, String ("%24cTiming%17cNet Critical   Input Greens   Output Greens") % BLANK % BLANK);
	Print (1, "    Signal  Time Period   Plan  Phase  Volume Lanes   Lane   Min  Max  Ext   Min  Max  Ext");
	Print (1);
}

/*********************************************|***********************************************

	Signal Timing Updates

	                       Timing                 Net Critical   Input Greens   Output Greens
	    Signal Time Period   Plan  Phase  Volume Lanes   Lane   Min  Max  Ext   Min  Max  Ext

	dddddddddd sssssssssss dddddd   ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd
	                                ddd  ddddddd  dd.d  ddddd   ddd  ddd  ddd   ddd  ddd  ddd

**********************************************|***********************************************/ 
