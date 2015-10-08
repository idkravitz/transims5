//*********************************************************
//	Create_Signals.cpp - create the signal phasing and timing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Create_Signals
//---------------------------------------------------------

void IntControl::Create_Signals (void)
{
	int method, cycle_len, yellow, red_clear, min_phase, min_cap, max_cap, capacity;
	double general_factor, pocket_factor, shared_factor, turn_factor, permit_factor, extend_factor, max_factor;

	int p, p1, p2, index, phase, app, low, high, sum, cycle, max_rate, app_max, opp_max, protect, permit, position;
	int num, num_in, num_out, period, nperiod, signal_type, num_rings, detect_id, detect_len, app_index;
	int count, split [MAX_PHASE_NUM];
	double rate, share, bucket;
	Dtime start, end_time;
	String buffer;
	bool actuated_flag, opp_flag, left_flag, left2_flag, right_flag, right2_flag, adjust, detect_flag;

	Compass_Points direction (4);

	Int_Set_Itr set_itr;
	Signal_Itr signal_itr;
	Signal_Time_Itr time_itr;
	Signal_Time_Data time_rec;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;
	Connect_Data *connect_ptr;
	Dir_Data *dir_ptr, *dir2_ptr;
	Link_Data *link_ptr;
	Timing_Data timing_rec;
	Timing_Phase phase_rec;
	Timing_Phase_Itr phase_itr;
	Phasing_Data phasing_rec;
	Movement_Data move_rec;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;
	Int_Itr node_itr;
	Node_Data *node_ptr;
	List_Data *list;
	Group_Map_Itr group_itr;
	Signal_Group *group_ptr;
	Detector_Data detector_rec;
	Int_Map_Stat map_stat;

	typedef struct {
		int    dir_index;
		int    bearing;
		int    thru_index;
		int    right_index;
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
		char   phase;
		int    detect_len;
		int    detect_off;
		int    detect_left;
		int    detect_thru;
		int    right_connect;
	} Approach;

	Approach  approach [MAX_DIR_TOT], *app_ptr, *opp_ptr, *rt_ptr, *rt2_ptr;

	capacity = min_cap = max_cap = p1 = p2 = 0;

	if (!detector_array.empty ()) {
		detect_id = (--detector_map.end ())->first + 1;
	} else {
		detect_id = 1;
	}

	//---- create signal data ----

	Show_Message ("Creating Traffic Signals -- Record");
	Set_Progress ();

	//---- add the new signals ----

	for (signal_itr = signal_array.begin (); signal_itr != signal_array.end (); signal_itr++) {
		Show_Progress ();

		if (signal_itr->size () > 0) continue;

		if (signal_itr->nodes.size () == 0) {
			Warning (String ("Signal %s has No Network Nodes") % signal_itr->Signal ());
			continue;
		}

		//---- find the signal group ----

		group_itr = group_map.find (signal_itr->Group ());

		if (group_itr == group_map.end ()) {
			if (signal_itr->Group () != 0) {
				Warning (String ("Control Keys for Signal Group %d were Not Provided") % signal_itr->Group ());
			}
			group_itr = group_map.begin ();
			if (group_itr == group_map.end ()) continue;
		}
		group_ptr = &group_itr->second;
			
		detect_len = group_ptr->detector;

		//---- collect approach legs for each node ----

		num_in = num_out = 0;
		memset (approach, '\0', sizeof (approach));

		app_index = (int) detector_array.size ();

		for (node_itr = signal_itr->nodes.begin (); node_itr != signal_itr->nodes.end (); node_itr++) {
			list = &node_list [*node_itr];

			//---- process each approach link ----

			for (index = list->To_List (); index >= 0; index = dir_ptr->First_Turn ()) {
				dir_ptr = &dir_array [index];

				if (num_in == MAX_DIR_TOT) {
					Error (String ("Signal %d has Too Many Approach Links for Signal Creation") % signal_itr->Signal ());
				}
				app_ptr = approach + num_in++;
				app_ptr->dir_index = index;
				app_ptr->bearing = dir_ptr->Out_Bearing ();
				app_ptr->thru_index = app_ptr->right_index = -1;
				app_ptr->right_low = app_ptr->right_high = -1;
				app_ptr->left_low = app_ptr->left_high = -1;
				app_ptr->thru_low = app_ptr->thru_high = -1;

				//---- summarize each turning movement ----

				for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [index];
			
					low = connect_ptr->Low_Lane ();
					high = connect_ptr->High_Lane ();

					if (connect_ptr->Type () == RIGHT) {
						if (app_ptr->right_low < 0 || app_ptr->right_low > low) {
							app_ptr->right_low = (char) low;
						}
						if (app_ptr->right_high < 0 || app_ptr->right_high < high) {
							app_ptr->right_high = (char) high;
						}
						app_ptr->right_index = connect_ptr->To_Index ();
						app_ptr->right_connect = index;
					} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
						if (app_ptr->left_low < 0 || app_ptr->left_low > low) {
							app_ptr->left_low = (char) low;
						}
						if (app_ptr->left_high < 0 || app_ptr->left_high < high) {
							app_ptr->left_high = (char) high;
						}
					} else {
						if (app_ptr->thru_low < 0 || app_ptr->thru_low > low) {
							app_ptr->thru_low = (char) low;
						}
						if (app_ptr->thru_high < 0 || app_ptr->thru_high < high) {
							app_ptr->thru_high = (char) high;
						}
						if (connect_ptr->Type () == THRU) {
							app_ptr->thru_index = connect_ptr->To_Index ();
						}
					}
				}

				//---- calculate detector position ----

				link_ptr = &link_array [dir_ptr->Link ()];

				if (dir_ptr->Dir () == 0) {
					app_ptr->detect_off = link_ptr->Length () - link_ptr->Boffset () - detect_len; 
					if (app_ptr->detect_off < link_ptr->Aoffset ()) {
						app_ptr->detect_off = link_ptr->Aoffset ();
						app_ptr->detect_len = link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ();
						if (app_ptr->detect_len < 0) app_ptr->detect_len = 0;
					} else {
						app_ptr->detect_len = detect_len;
					}
				} else {
					app_ptr->detect_off = link_ptr->Length () - link_ptr->Aoffset () - detect_len; 
					if (app_ptr->detect_off < link_ptr->Boffset ()) {
						app_ptr->detect_off = link_ptr->Boffset ();
						app_ptr->detect_len = link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ();
						if (app_ptr->detect_len < 0) app_ptr->detect_len = 0;
					} else {
						app_ptr->detect_len = detect_len;
					}
				}
				if (app_ptr->detect_len > 0) {
					if (app_ptr->thru_low >= 0 && app_ptr->left_low >= 0) {
						app_ptr->detect_left = app_index++;
					} else {
						app_ptr->detect_left = -1;
					}
					if ((app_ptr->thru_high >= 0 || app_ptr->right_high >= 0) &&
						(app_ptr->thru_high > app_ptr->left_high || 
						app_ptr->right_high > app_ptr->left_high)) {

						app_ptr->detect_thru = app_index++;
					} else {
						app_ptr->detect_thru = -1;
					}
				} else {
					app_ptr->detect_left = app_ptr->detect_thru = -1;
				}
			}

			//---- count the number of departure links ----

			for (index = list->From_List (); index >= 0; index = dir_ptr->First_Turn (false)) {
				dir_ptr = &dir_array [index];
				num_out++;
			}
		}
		if (num_in == 0 || num_out == 0) {
			node_ptr = &node_array [*node_itr];
			Warning (String ("Signal %d Node %d does not have Entry and Exit Links") % signal_itr->Signal () % node_ptr->Node ());
			continue;
		}

		//---- add time period range ----

		nperiod = group_ptr->time_breaks.Num_Periods ();
		detect_flag = false;

		for (period = 0; period < nperiod; period++) {
			group_ptr->time_breaks.Period_Range (period, start, end_time);
		
			time_rec.Start (start);
			time_rec.End (end_time);
			time_rec.Timing (0);
			time_rec.Phasing (0);

			//---- retrieve control parameters ----

			p = period + 1;
			signal_type = group_ptr->signal_type.Best (p);
			actuated_flag = (signal_type == ACTUATED);
			if (actuated_flag) detect_flag = true;

			num_rings = group_ptr->num_rings.Best (p);
			cycle_len = group_ptr->cycle_len.Best (p);
			min_phase = group_ptr->min_phase.Best (p);
			yellow = group_ptr->yellow.Best (p);
			red_clear = group_ptr->red_clear.Best (p);

			method = group_ptr->method.Best (p);
			if (method == VOLUME_SPLITS || method == UPDATE_SPLITS) {
				if (!turn_flag && !delay_flag) method = CAPACITY_SPLITS;
			}
			pocket_factor = group_ptr->pocket_factor.Best (p);
			shared_factor = group_ptr->shared_factor.Best (p);
			turn_factor = group_ptr->turn_factor.Best (p);
			permit_factor = group_ptr->permit_factor.Best (p);
			general_factor = group_ptr->general_factor.Best (p);
			extend_factor = group_ptr->extend_factor.Best (p);
			max_factor = group_ptr->max_factor.Best (p);

			if (method == VOLUME_SPLITS || method == UPDATE_SPLITS) {
				if (!sum_periods.Period_Range (start, end_time, p1, p2)) continue;
			} else if (method == CAPACITY_SPLITS) {
				min_cap = group_ptr->min_cap.Best (p);
				max_cap = group_ptr->max_cap.Best (p);
			}

			//---- process each approach ----

			for (app=0, app_ptr = approach; app < num_in; app++, app_ptr++) {
				dir_ptr = &dir_array [app_ptr->dir_index];

				//---- initialize the time period fields ----

				app_ptr->phase = app_ptr->protect = 0;
				app_ptr->left_rate = app_ptr->right_rate = app_ptr->thru_rate = 0;
				app_ptr->left_lanes = app_ptr->right_lanes = app_ptr->thru_lanes = 0;

				//---- calculate capacity weight ----

				if (method == CAPACITY_SPLITS) {
					capacity = dir_ptr->Capacity () / dir_ptr->Lanes ();
					if (capacity < min_cap) {
						capacity = min_cap;
					} else if (capacity > max_cap) {
						capacity = max_cap;
					}
					capacity *= dir_ptr->Lanes ();
				}

				//---- summarize each turning movement ----

				for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [index];

					rate = 0;

					if (method == VOLUME_SPLITS || method == UPDATE_SPLITS) {
						for (p=p1; p <= p2; p++) {
							array_ptr = &turn_delay_array [p];
							flow_ptr = &array_ptr->at (index);
							rate += flow_ptr->Flow ();
						}
					} else if (method == CAPACITY_SPLITS) {
						rate = capacity;
					}
					if (rate < 1.0) rate = 1.0;

					if (connect_ptr->Type () == RIGHT) {
						app_ptr->right_rate += Round (rate);
					} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
						app_ptr->left_rate += Round (rate);
					} else {
						app_ptr->thru_rate += Round (rate);
					}
				}

				//---- calculate the lane weights for each movement ----

				if (app_ptr->left_rate > 0) {

					//---- check for shared lanes ----

					if (app_ptr->thru_rate > 0 && app_ptr->left_high < app_ptr->thru_low) {	

						//---- shared thru-left lanes ----

						share = shared_factor * (app_ptr->left_high - app_ptr->thru_low + 1);	
						app_ptr->thru_lanes = share;
						app_ptr->left_lanes = share + (app_ptr->thru_low - app_ptr->left_low) * pocket_factor;

					//---- check for pocket lanes ----

					} else if (dir_ptr->Left () > 0 && app_ptr->left_low < dir_ptr->Left ()) {
						share = app_ptr->left_high - dir_ptr->Left () + 1;
						app_ptr->left_lanes = share + (dir_ptr->Left () - app_ptr->left_low) * pocket_factor;
					} else {
						app_ptr->left_lanes = app_ptr->left_high - app_ptr->left_low + 1;
					}

					//---- turn slow down impact ----

					app_ptr->left_lanes *= turn_factor;
				}
				if (app_ptr->right_rate > 0) {

					//---- check for shared lanes ----

					if (app_ptr->thru_rate > 0 && app_ptr->right_low < app_ptr->thru_high) {
						share = shared_factor * (app_ptr->thru_high - app_ptr->right_low + 1);
						app_ptr->thru_lanes += share;
						app_ptr->right_lanes = share + (app_ptr->right_high - app_ptr->thru_high) + pocket_factor;

					//---- check for pocket lanes ----

					} else if (dir_ptr->Right () > 0 && app_ptr->right_high >= (dir_ptr->Left () + dir_ptr->Lanes ())) {	
						share = dir_ptr->Left () + dir_ptr->Lanes () - app_ptr->left_low;
						app_ptr->right_lanes = share + (app_ptr->right_high - (dir_ptr->Left () + dir_ptr->Lanes ()) + 1) * pocket_factor;
					} else {
						app_ptr->right_lanes = app_ptr->right_high - app_ptr->right_low + 1;
					}

					//---- turn slow down impact ----

					app_ptr->right_lanes *= turn_factor;
				}
				if (app_ptr->thru_rate > 0) {

					//---- exclude shared lanes ----

					low = MAX (app_ptr->left_high, app_ptr->thru_low);
					if (app_ptr->right_low >= 0) {
						high = MIN (app_ptr->right_low, app_ptr->thru_high);
					} else {
						high = app_ptr->thru_high;
					}
					app_ptr->thru_lanes += high - low + 1;
				}
			
				//---- normalize the approach rates ----

				if (app_ptr->thru_lanes > 0.0) {
					app_ptr->thru_rate = DTOI (app_ptr->thru_rate / app_ptr->thru_lanes);
				}
				if (app_ptr->left_lanes > 0.0) {
					app_ptr->left_rate = DTOI (app_ptr->left_rate / app_ptr->left_lanes);
				}
				if (app_ptr->right_lanes > 0.0) {
					app_ptr->right_rate = DTOI (app_ptr->right_rate / app_ptr->right_lanes);
				}
			}

			//---- initialize the timing plan ----

			num = (int) signal_itr->timing_plan.size () + 1;
			time_rec.Timing (num);
			time_rec.Phasing (num);

			timing_rec.clear ();
			timing_rec.Timing (num);
			timing_rec.Type (signal_type);
			timing_rec.Cycle (cycle_len);
			timing_rec.Notes (group_ptr->time_breaks.Range_Format (period));

			phasing_rec.Phasing (num);

			//---- assign each approach to a phase ----

			phase = position = 1;
			memset (split, '\0', sizeof (split));

			for (app=0, app_ptr = approach; app < num_in; app++, app_ptr++) {
				if (app_ptr->phase > 0) continue;

				dir_ptr = &dir_array [app_ptr->dir_index];

				//---- find the related movements ----

				opp_flag = right_flag = right2_flag = false;
				opp_ptr = rt_ptr = rt2_ptr = 0;

				link_ptr = &link_array [dir_ptr->Link ()];
				if (dir_ptr->Dir () == 1) {
					index = link_ptr->AB_Dir ();
				} else {
					index = link_ptr->BA_Dir ();
				}

				//---- find the opposing right-on-red movement ----

				if (index >= 0) {
					for (num=0, rt2_ptr = approach; num <= num_in; num++, rt2_ptr++) {
						if (rt2_ptr->right_index == index) {
							right2_flag = true;
							break;
						}
					}
				}

				//---- thru movement conflicts ----

				if (app_ptr->thru_index >= 0) {

					//---- find the opposing movement ----

					if (index >= 0) {
						for (num=0, opp_ptr = approach; num < num_in; num++, opp_ptr++) { 
							if (opp_ptr->thru_index == index) {
								opp_flag = true;
								break;
							}
						}
					}
					
					//---- find the right-on-red movement ----

					for (num=0, rt_ptr = approach; num <= num_in; num++, rt_ptr++) {
						if (rt_ptr->right_index == app_ptr->thru_index) {
							right_flag = true;
							break;
						}
					}
				}

				//---- add a new phase ----

				if (opp_flag) {

					//---- combined approach ----

					app_max = MAX (app_ptr->thru_rate, app_ptr->right_rate);
					opp_max = MAX (opp_ptr->thru_rate, opp_ptr->right_rate);
					max_rate =MAX (app_max, opp_max);
					max_rate = DTOI (max_rate / general_factor);

					dir2_ptr = &dir_array [opp_ptr->dir_index];

					if (num_rings == 1) {

						//---- protected left flags ----

						left_flag = (app_ptr->left_lanes > 0 && (app_ptr->thru_low - app_ptr->left_low >= 2 ||
							app_ptr->left_rate > max_rate * permit_factor || opp_ptr->left_lanes == 0 || opp_ptr->left_rate == 0));

						left2_flag = (opp_ptr->left_lanes > 0 && (opp_ptr->thru_low - opp_ptr->left_low >= 2 ||
							opp_ptr->left_rate > max_rate * permit_factor || app_ptr->left_lanes == 0 || app_ptr->left_rate == 0));

						phase_rec.Barrier (1);
						phase_rec.Ring (1);
						phase_rec.Yellow (0);
						phase_rec.All_Red (0);

						//---- process the protection type ----

						if (left_flag && !left2_flag) {

							//---- protect approach left only ----

							protect = app_ptr->left_rate;

							if (protect + opp_max <= app_max) {
								permit = app_max - protect;
							} else {
								protect -= DTOI (opp_max * permit_factor);
								if (protect < 0) protect = 0;
								permit = app_max - protect;
								if (permit < opp_max) permit = opp_max;
							}

							//---- add protected phase ----

							if (protect > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow ((permit > 0) ? 0 : yellow);
								phase_rec.All_Red (0);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								app_ptr->protect = (char) phase++;

								split [phase] = protect;

								//---- add phase movements ----

								for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PROTECTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add left turn detector ----

								if (actuated_flag && app_ptr->detect_left >= 0) {
									phasing_rec.detectors.push_back (app_ptr->detect_left);
								}

								//---- add right on red movement ----

								if (right2_flag) {
									move_rec.Movement (Movement_Code (rt2_ptr->bearing, RIGHT));
									move_rec.Connection (rt2_ptr->right_connect);
									move_rec.Protection (STOP_PERMIT);

									phasing_rec.push_back (move_rec);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

							//---- add permitted phase ----

							if (permit > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow (yellow);
								phase_rec.All_Red (red_clear);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								opp_ptr->phase = (char) phase;
								app_ptr->phase = (char) phase++;

								split [phase] = permit;

								//---- add phase movements ----

								for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add opposing movements ----

								for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- approach detectors ----

								if (actuated_flag) {
									if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
									if (app_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (app_ptr->detect_thru);
									if (opp_ptr->detect_left >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_left);
									if (opp_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_thru);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

						} else if (!left_flag && left2_flag) {

							//---- protect the oppossing left only ----

							protect = opp_ptr->left_rate;

							if (protect + app_max <= opp_max) {
								permit = opp_max - protect;
							} else {
								protect -= DTOI (app_max * permit_factor);
								if (protect < 0) protect = 0;
								permit = opp_max - protect;
								if (permit < app_max) permit = app_max;
							}

							//---- add protected phase ----

							if (protect > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow ((permit > 0) ? 0 : yellow);
								phase_rec.All_Red (0);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								opp_ptr->protect = (char) phase++;

								split [phase] = protect;

								//---- add opposing movements ----

								for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PROTECTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add left turn detector ----

								if (actuated_flag && opp_ptr->detect_left > 0) {
									phasing_rec.detectors.push_back (opp_ptr->detect_left);
								}

								//---- add right on red movement ----

								if (right_flag) {
									move_rec.Movement (Movement_Code (rt_ptr->bearing, RIGHT));
									move_rec.Connection (rt_ptr->right_connect);
									move_rec.Protection (STOP_PERMIT);

									phasing_rec.push_back (move_rec);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

							//---- add permitted phase ----

							if (permit > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow (yellow);
								phase_rec.All_Red (red_clear);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								app_ptr->phase = (char) phase;
								opp_ptr->phase = (char) phase++;

								split [phase] = permit;

								//---- add opposing movements ----

								for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add approach movements ----

								for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- approach detectors ----

								if (actuated_flag) {
									if (opp_ptr->detect_left >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_left);
									if (opp_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_thru);
									if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
									if (app_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (app_ptr->detect_thru);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

						} else if (left_flag && left2_flag) {

							//---- protect both directions ----

							if (app_ptr->left_rate >= opp_ptr->left_rate) {
								protect = opp_ptr->left_rate;
								permit = DTOI ((app_ptr->left_rate - protect) / permit_factor);
								if (permit < max_rate) permit = max_rate;
							} else {
								protect = app_ptr->left_rate;
								permit = DTOI ((opp_ptr->left_rate - protect) / permit_factor);
								if (permit < max_rate) permit = max_rate;
							}

							//---- add protected phase ----

							if (protect > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow ((permit > 0) ? 0 : yellow);
								phase_rec.All_Red (0);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								opp_ptr->protect = (char) phase;
								app_ptr->protect = (char) phase++;

								split [phase] = protect;

								//---- add left approach movements ----

								for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];
									if (connect_ptr->Type () != LEFT && connect_ptr->Type () != UTURN) continue;

									move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PROTECTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add left opposing movements ----

								for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];
									if (connect_ptr->Type () != LEFT && connect_ptr->Type () != UTURN) continue;

									move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PROTECTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add left turn detectors ----

								if (actuated_flag) {
									if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
									if (opp_ptr->detect_left >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_left);
								}

								//---- add right on red movement ----

								if (right_flag) {
									move_rec.Movement (Movement_Code (rt_ptr->bearing, RIGHT));
									move_rec.Connection (rt_ptr->right_connect);
									move_rec.Protection (STOP_PERMIT);

									phasing_rec.push_back (move_rec);
								}
								if (right2_flag) {
									move_rec.Movement (Movement_Code (rt2_ptr->bearing, RIGHT));
									move_rec.Connection (rt2_ptr->right_connect);
									move_rec.Protection (STOP_PERMIT);

									phasing_rec.push_back (move_rec);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

							//---- add permitted phase ----

							if (permit > 0) {
								phase_rec.Phase (phase);
								phase_rec.Position (position++);
								phase_rec.Yellow (yellow);
								phase_rec.All_Red (red_clear);

								timing_rec.push_back (phase_rec);

								phasing_rec.Phase (phase);
								phasing_rec.clear ();
								phasing_rec.detectors.clear ();

								opp_ptr->phase = (char) phase;
								app_ptr->phase = (char) phase++;

								split [phase] = permit;

								//---- add phase movements ----

								for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- add opposing movements ----

								for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
									connect_ptr = &connect_array [index];

									move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
									move_rec.Connection (index);
									move_rec.Protection (PERMITTED);

									phasing_rec.push_back (move_rec);
								}

								//---- approach detectors ----

								if (actuated_flag) {
									if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
									if (app_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (app_ptr->detect_thru);
									if (opp_ptr->detect_left >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_left);
									if (opp_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_thru);
								}
								signal_itr->phasing_plan.push_back (phasing_rec);
								nphasing++;
							}

						} else {

							//---- no left turn protection ----

							permit = max_rate;

							max_rate = DTOI (app_ptr->left_rate / permit_factor);
							if (max_rate > permit) permit = max_rate;

							max_rate = DTOI (opp_ptr->left_rate / permit_factor);
							if (max_rate > permit) permit = max_rate;

							phase_rec.Phase (phase);
							phase_rec.Position (position++);
							phase_rec.Yellow (yellow);
							phase_rec.All_Red (red_clear);

							timing_rec.push_back (phase_rec);

							phasing_rec.Phase (phase);
							phasing_rec.clear ();
							phasing_rec.detectors.clear ();

							opp_ptr->phase = (char) phase;
							app_ptr->phase = (char) phase++;

							split [phase] = permit;

							//---- add phase movements ----

							for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
								connect_ptr = &connect_array [index];

								move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
								move_rec.Connection (index);
								move_rec.Protection (PERMITTED);

								phasing_rec.push_back (move_rec);
							}

							//---- add opposing movements ----

							for (index = dir2_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
								connect_ptr = &connect_array [index];

								move_rec.Movement (Movement_Code (opp_ptr->bearing, connect_ptr->Type ()));
								move_rec.Connection (index);
								move_rec.Protection (PERMITTED);

								phasing_rec.push_back (move_rec);
							}

							//---- approach detectors ----

							if (actuated_flag) {
								if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
								if (app_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (app_ptr->detect_thru);
								if (opp_ptr->detect_left >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_left);
								if (opp_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (opp_ptr->detect_thru);
							}
							signal_itr->phasing_plan.push_back (phasing_rec);
							nphasing++;
						}

					} else {	//---- dual ring ----

					}

				} else {

					//---- exclusive approach ----

					phase_rec.Phase (phase);
					phase_rec.Barrier (1);
					phase_rec.Ring (1);
					phase_rec.Position (position++);
					phase_rec.Yellow (yellow);
					phase_rec.All_Red (red_clear);

					timing_rec.push_back (phase_rec);

					phasing_rec.Phase (phase);
					phasing_rec.clear ();
					phasing_rec.detectors.clear ();

					app_ptr->phase = (char) phase++;

					max_rate = app_ptr->thru_rate;
					if (app_ptr->left_rate > max_rate) max_rate = app_ptr->left_rate;
					if (app_ptr->right_rate > max_rate) max_rate = app_ptr->right_rate;

					split [phase] = max_rate;

					//---- add phase movements ----

					for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
						connect_ptr = &connect_array [index];

						move_rec.Movement (Movement_Code (app_ptr->bearing, connect_ptr->Type ()));
						move_rec.Connection (index);
						move_rec.Protection (PROTECTED);

						phasing_rec.push_back (move_rec);
					}
					if (actuated_flag) {
						if (app_ptr->detect_left >= 0) phasing_rec.detectors.push_back (app_ptr->detect_left);
						if (app_ptr->detect_thru >= 0) phasing_rec.detectors.push_back (app_ptr->detect_thru);
					}

					//---- add right on red movement ----

					if (right2_flag) {
						move_rec.Movement (Movement_Code (rt2_ptr->bearing, RIGHT));
						move_rec.Connection (rt2_ptr->right_connect);
						move_rec.Protection (STOP_PERMIT);

						phasing_rec.push_back (move_rec);
					}
					signal_itr->phasing_plan.push_back (phasing_rec);
					nphasing++;
				}
			}

			//---- allocate phase time ----

			sum = 0;
			cycle = cycle_len;

			for (phase_itr = timing_rec.begin (); phase_itr != timing_rec.end (); phase_itr++) {
				sum += split [phase_itr->Phase ()];
				cycle -= (phase_itr->Yellow () + phase_itr->All_Red ());
			}

			//---- adjust phasing by demand ----

			for (count=0; count < 5; count++) {
				if (sum > 0) {
					share = (double) cycle / sum;
					sum = 0;
				} else {
					share = 1.0;
				}
				bucket = 0.45;
				adjust = false;

				for (phase_itr = timing_rec.begin (); phase_itr != timing_rec.end (); phase_itr++) {
					phase = phase_itr->Phase ();

					rate = share * split [phase] + bucket;
					max_rate = (int) rate;
					bucket = rate - (double) max_rate;

					if (max_rate < min_phase) {
						max_rate = min_phase;
						adjust = true;
					}
					split [phase] = max_rate;
					sum += max_rate;
				}
				if (!adjust) break;
			}

			//---- calculate the green times ----

			for (phase_itr = timing_rec.begin (); phase_itr != timing_rec.end (); phase_itr++) {
				max_rate = split [phase_itr->Phase ()];

				if (actuated_flag) {
					high = max_rate;
					phase_itr->Max_Green (high);
					low = DTOI (high / max_factor);
					if (low < min_phase) low = min_phase;
					phase_itr->Min_Green (low);
					max_rate = DTOI (low * extend_factor);
					if (max_rate < 1) max_rate = 1;
					if (max_rate + low > high) max_rate = high - low;
					phase_itr->Extension (max_rate);
				} else {
					phase_itr->Min_Green (max_rate);
					phase_itr->Max_Green (max_rate);
					phase_itr->Extension (0);
				}
			}
			signal_itr->timing_plan.push_back (timing_rec);
			ntiming++;

			signal_itr->push_back (time_rec);
		}

		//---- add the detector records ----

		if (detect_flag) {
			for (app=0, app_ptr = approach; app < num_in; app++, app_ptr++) {
				dir_ptr = &dir_array [app_ptr->dir_index];

				if (app_ptr->detect_left >= 0) {
					low = app_ptr->left_low;
					high = app_ptr->left_high;

					detector_rec.Detector (detect_id++);
					detector_rec.Dir_Index (app_ptr->dir_index);
					detector_rec.Offset (app_ptr->detect_off);
					detector_rec.Length (app_ptr->detect_len);
					detector_rec.Low_Lane (low);
					detector_rec.High_Lane (high);
					detector_rec.Type (PRESENCE);
					detector_rec.Use (0);
					detector_rec.Notes ("Left Turn");

					map_stat = detector_map.insert (Int_Map_Data (detector_rec.Detector (), app_ptr->detect_left));
					if (!map_stat.second) {
						Warning ("Duplicate Detector Records") << detector_rec.Detector ();
					} else {
						detector_array.push_back (detector_rec);
						ndetector++;
					}
				}
				if (app_ptr->detect_thru >= 0) {
					low = (app_ptr->thru_low >= 0) ? app_ptr->thru_low : app_ptr->left_low;
					high = (app_ptr->right_high >= 0) ? app_ptr->right_high : app_ptr->thru_high;

					detector_rec.Detector (detect_id++);
					detector_rec.Dir_Index (app_ptr->dir_index);
					detector_rec.Offset (app_ptr->detect_off);
					detector_rec.Length (app_ptr->detect_len);
					detector_rec.Low_Lane (low);
					detector_rec.High_Lane (high);
					detector_rec.Type (PRESENCE);
					detector_rec.Use (0);
					detector_rec.Notes (((app_ptr->thru_low >= 0) ? "Thru Right" : "Approach"));

					map_stat = detector_map.insert (Int_Map_Data (detector_rec.Detector (), app_ptr->detect_thru));
					if (!map_stat.second) {
						Warning ("Duplicate Detector Records") << detector_rec.Detector ();
					} else {
						detector_array.push_back (detector_rec);
						ndetector++;
					}
				}
			}
		}
	}
	End_Progress ();
}
