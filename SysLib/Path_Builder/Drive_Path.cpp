//*********************************************************
//	Drive_Path.cpp - build a drive path between parking lots
//*********************************************************

#include "Path_Builder.hpp"

#include "math.h"

//---------------------------------------------------------
//	Drive_Path
//---------------------------------------------------------

int Path_Builder::Drive_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag)
{
	int link, dir, length, period, approach, index, *list, *next;
	unsigned from_imp, to_imp, hi_imp;
	Dtime from_time, to_time, ttime, delay, ttim;
	int cost, from_cost, to_cost, from_len, to_len, from_walk, to_walk, from, to, penalty, pen_imp;
	int imp, imped, best_to, op_cost, cst, group;
	int x0, y0, x1, y1, x2, y2;
	double dx, dy, dist1, dist2, max_len, max_local, len_factor, local_factor, factor;
	bool ab_flag, to_flag, local_flag;
	bool restrict_flag, restrict_in, restrict_from, restrict_to;

	Path_End_Itr from_itr, to_itr;
	Path_End *end_ptr;

	Node_Data *node_ptr;
	Link_Data *link_ptr = 0;
	Dir_Data *dir_ptr, *app_ptr;
	Turn_Pen_Data *turn_ptr = 0;
	Connect_Data *connect_ptr;
	Path_Data *path_ptr;
	Path_Itr path_itr;
	Flow_Time_Array *turn_delay_ptr;
	Flow_Time_Data *turn_delay;
	
	Int_Queue next_index;
	Imp_Sort imp_sort;
	Imp_Sort_Itr imp_itr;

	if (from_ptr->size () == 0 || to_ptr->size () == 0) {
		zero_flag = true;
		return (Set_Drive_Error ());
	}

	//---- initialize the path building ----

	restrict_flag = restrict_from = restrict_to = restrict_in = false;
	local_flag = true;
	turn_delay_ptr = 0;
	turn_delay = 0;
	x0 = y0 = x1 = y1 = 0;
	max_len = max_local = local_factor = 0.0;
	period = -1;

	if (link_path.size () == 0) {
		link_path.resize (exe->dir_array.size ());
	} else {
		link_path.Clear ();
	}
	best_to = -1;

	//---- initialize the to link list ----

	link_to.assign (exe->link_array.size (), -1);
	next_to.assign (to_ptr->size (), -1);

	for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
		if (to_itr->Type () == LINK_ID && to_itr->Index () >= 0) {
			list = &link_to [to_itr->Index ()];
			next = &next_to [to];
			*next = *list;
			*list = to;
		}
	}

	//---- process each origin ----

	for (from=0, from_itr = from_ptr->begin (); from_itr != from_ptr->end (); from_itr++, from++) {
		if (from_itr->Type () != LINK_ID) continue;

		link_ptr = &exe->link_array [from_itr->Index ()];

		if (!Use_Permission (link_ptr->Use (), param.use)) {
			use_flag = true;
			continue;
		}
		if (plan_flag) {
			restrict_from = Use_Permission (link_ptr->Use (), RESTRICTED);
		}
		path_ptr = &from_itr->back ();

		from_imp = path_ptr->Imped ();
		if (from_imp == MAX_IMPEDANCE) continue;

		from_time = path_ptr->Time ();
		from_len = path_ptr->Length ();
		from_cost = path_ptr->Cost ();
		from_walk = path_ptr->Walk();

		//---- check for from and to on the same link ----

		to_flag = false;

		for (to = link_to [from_itr->Index ()]; to >= 0; to = next_to [to]) {
			end_ptr = &to_ptr->at (to);

			//---- get direction and distance ----
			
			if (from_itr->Offset () <= end_ptr->Offset ()) {
				if (forward_flag) {
					index = link_ptr->AB_Dir ();
					ab_flag = true;
				} else {
					index = link_ptr->BA_Dir ();
					ab_flag = false;
				}
				length = end_ptr->Offset () - from_itr->Offset ();
			} else {
				if (forward_flag) {
					index = link_ptr->BA_Dir ();
					ab_flag = false;
				} else {
					index = link_ptr->AB_Dir ();
					ab_flag = true;
				}
				length = from_itr->Offset () - end_ptr->Offset ();
			}
			if (index < 0) {
				zero_flag = true;
				end_ptr->Index (-1);	
				continue;
			}

			//---- check the lane use ----

			if (!Best_Lane_Use (index, from_time, ttime, delay, cost, group)) {
				use_flag = true;
				continue;
			}

			//---- adjust the travel time ----

			if (grade_flag && link_ptr->Grade (ab_flag) > 0) {
				ttime = ttime / veh_type_ptr->Grade (link_ptr->Grade (ab_flag));
			}
			len_factor = (double) length / link_ptr->Length ();

			ttime = (int) (ttime * len_factor + 0.5) + delay;

			//---- check the time schedule ----

			if (forward_flag) {
				to_time = from_time + ttime;

				if (to_time > time_limit) {
					time_flag = true;
					continue;
				}
			} else {
				to_time = from_time - ttime;

				if (to_time < time_limit) {
					time_flag = true;
					continue;
				}
			}
			cost += DTOI (op_cost_rate * length);

			//---- calculcate the impedance ----

			imped = DTOI (ttime * param.value_time + length * param.value_dist + cost * param.value_cost);
			if (imped < 0) continue;

			if (link_ptr->Type () == FREEWAY) {
				factor = param.freeway_fac;
			} else if (link_ptr->Type () == EXPRESSWAY) {
				factor = param.express_fac;
			} else {
				factor = 1.0;
			}
			if (random_flag) {
				factor = 1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0;
			}
			if (factor != 1.0) {
				imped = DTOI (imped * factor);
			}
			if (imped < 1) {
				if (imped < 0) continue;
				imped = 1;
			}

			//---- check the cumulative impedance ----
			
			to_imp = from_imp + imped;

			if (to_imp + imp_diff >= max_imp) continue;

			path_ptr = &end_ptr->back ();

			if (to_imp >= path_ptr->Imped ()) continue;

			to_len = from_len + length;
			to_cost = from_cost + cost;
			to_walk = from_walk;

			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (0);
			path_ptr->Dir ((ab_flag) ? 0 : 1);
			path_ptr->Mode (DRIVE_MODE);
			path_ptr->Status (1);

			to_flag = true;

			if (best_flag) {
				max_imp = to_imp + imp_diff;
				best_to = to;
			}
		}

		//---- update the max imp ----

		if (best_flag) {
			if (best_to >= 0) return (best_to);
			if (zero_flag) return (Set_Drive_Error ());
		} else if (to_flag || zero_flag) {
			hi_imp = 0;

			for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
				path_ptr = &to_itr->back ();

				if (path_ptr->Imped () < MAX_IMPEDANCE) {
					to_imp = path_ptr->Imped () + imp_diff;
					if (to_imp > hi_imp) hi_imp = to_imp;
				} else {
					hi_imp = MAX_IMPEDANCE;
					break;
				}
			}
			if (hi_imp > 0 && hi_imp < max_imp) {
				max_imp = hi_imp;
			}
			best_to = 0;
		}

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {

			//---- get direction data ----

			if (dir == 0) {
				if (forward_flag) {
					index = link_ptr->AB_Dir ();
					ab_flag = true;
				} else {
					index = link_ptr->BA_Dir ();
					ab_flag = false;
				}
				length = link_ptr->Length () - from_itr->Offset ();
			} else {
				if (forward_flag) {
					index = link_ptr->BA_Dir ();
					ab_flag = false;
				} else {
					index = link_ptr->AB_Dir ();
					ab_flag = true;
				}
				length = from_itr->Offset ();
			}
			if (index < 0) continue;

			if (reroute_flag && index != from_itr->Index ()) continue;

			//---- check the lane use ----

			if (!Best_Lane_Use (index, from_time, ttime, delay, cost, group)) {
				use_flag = true;
				continue;
			}

			//---- adjust the travel time ----

			if (grade_flag && link_ptr->Grade (ab_flag) > 0) {
				ttime = ttime / veh_type_ptr->Grade (link_ptr->Grade (ab_flag));
			}
			len_factor = (double) length / link_ptr->Length ();

			ttime = (int) (ttime * len_factor + 0.5) + delay;

			//---- check the time schedule ----

			if (forward_flag) {
				to_time = from_time + ttime;

				if (to_time > time_limit) {
					time_flag = true;
					continue;
				}
			} else {
				to_time = from_time - ttime;

				if (to_time < time_limit) {
					time_flag = true;
					continue;
				}
			}
			cost += DTOI (op_cost_rate * length);

			//---- calculcate the impedance ----

			imped = DTOI (ttime * param.value_time + length * param.value_dist + cost * param.value_cost);
			if (imped < 0) continue;

			if (link_ptr->Type () == FREEWAY) {
				factor = param.freeway_fac;
			} else if (link_ptr->Type () == EXPRESSWAY) {
				factor = param.express_fac;
			} else {
				factor = 1.0;
			}
			if (random_flag) {
				factor = 1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0;
			}
			if (factor != 1.0) {
				imped = DTOI (imped * factor);
			}
			if (imped < 1) {
				if (imped < 0) continue;
				imped = 1;
			}

			//---- check the cumulative impedance ----

			to_imp = from_imp + imped;
			
			if (to_imp >= max_imp) continue;

			path_ptr = &link_path [index];

			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the record to the queue ----

			if (param.sort_method) {
			
				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (imp_itr = imp_sort.find (path_ptr->Imped ()); imp_itr != imp_sort.end (); imp_itr++) {
						if (imp_itr->second == index) {
							imp_sort.erase (imp_itr);
							break;
						} else if (imp_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				imp_sort.insert (Imp_Sort_Data (to_imp, index));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (index);
				}
			}
			to_len = from_len + length;
			to_cost = from_cost + cost;
			to_walk = from_walk;	

			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (0);
			path_ptr->Dir ((ab_flag) ? 0 : 1);
			path_ptr->Mode (DRIVE_MODE);
			path_ptr->Status (1);
		}
	}
	if (param.sort_method) {
		if (imp_sort.empty ()) return (best_to);
	} else {
		if (next_index.empty ()) return (best_to);
	}

	//---- circuity and restricted access for plan-based trips ----

	if (plan_flag) {

		//---- calculate the from-to distance ----

		if (param.distance_flag || param.local_flag) {

			//---- find the composite origin location ----

			x0 = y0 = 0;

			for (from_itr = from_ptr->begin (); from_itr != from_ptr->end (); from_itr++) {
				if (from_itr->Type () != LINK_ID || from_itr->Index () < 0) continue;

				link_ptr = &exe->link_array [from_itr->Index ()];

				len_factor = (double) from_itr->Offset () / link_ptr->Length ();

				node_ptr = &exe->node_array [link_ptr->Anode ()];

				x2 = node_ptr->X ();
				y2 = node_ptr->Y ();

				node_ptr = &exe->node_array [link_ptr->Bnode ()];

				x2 += (int) ((node_ptr->X () - x2) * len_factor + 0.5);
				y2 += (int) ((node_ptr->Y () - y2) * len_factor + 0.5);

				x0 += x2;
				y0 += y2;
			}
			x0 /= (int) from_ptr->size ();
			y0 /= (int) from_ptr->size ();

			//---- find the composite destination location ----

			x1 = y1 = 0;

			for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
				if (to_itr->Type () != LINK_ID || to_itr->Index () < 0) continue;

				link_ptr = &exe->link_array [to_itr->Index ()];

				len_factor = (double) to_itr->Offset () / link_ptr->Length ();

				node_ptr = &exe->node_array [link_ptr->Anode ()];

				x2 = node_ptr->X ();
				y2 = node_ptr->Y ();

				node_ptr = &exe->node_array [link_ptr->Bnode ()];

				x2 += (int) ((node_ptr->X () - x2) * len_factor + 0.5);
				y2 += (int) ((node_ptr->Y () - y2) * len_factor + 0.5);

				x1 += x2;
				y1 += y2;
			}

			//---- calculate the distance range ----

			x1 /= (int) to_ptr->size ();
			y1 /= (int) to_ptr->size ();

			dx = x0 - x1;
			dy = y0 - y1;

			dist1 = UnRound (sqrt (dx * dx + dy * dy));

			max_len = dist1 * (param.max_ratio - 1.0);

			if (max_len < param.min_distance) {
				max_len = param.min_distance;
			} else if (max_len > param.max_distance) {
				max_len = param.max_distance;
			}
			max_len += dist1;

			max_len = Round (max_len);
			max_len *= max_len;

			max_local = Round (param.local_distance);
			max_local *= max_local;
		}

		//---- initialize the restricted access ----

		for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
			if (to_itr->Type () == LINK_ID && to_itr->Index () >= 0) {
				link_ptr = &exe->link_array [to_itr->Index ()];

				if (Use_Permission (link_ptr->Use (), RESTRICTED)) {
					restrict_to = true;
					break;
				}
			}
		}
		restrict_flag = (restrict_from || restrict_to);
	}
	
	//---- build the path ----

	for (;;) {
		
		//---- remove the approach link from the processing queue ----
		
		if (param.sort_method) {
			if (imp_sort.empty ()) break;

			imp_itr = imp_sort.begin ();
			approach = imp_itr->second;
			imp_sort.erase (imp_itr);
		} else {
			if (next_index.empty ()) break;
			approach = next_index.front ();
			next_index.pop_front ();
		}
		path_ptr = &link_path [approach];
		path_ptr->Status (2);

		//---- check the cumulative impedance ----

		from_imp = path_ptr->Imped ();

		if (from_imp >= max_imp) continue;

		//---- check the activity schedule ----

		from_time = path_ptr->Time ();

		if (forward_flag) {
			if (from_time >= time_limit) {
				time_flag = true;
				continue;
			}
		} else {
			if (from_time <= time_limit) {
				time_flag = true;
				continue;
			}
		}
		if (param.delay_flag && param.turn_delay_flag) {
			turn_delay_ptr = exe->turn_delay_array.Period_Ptr (from_time);
		}
		from_len = path_ptr->Length ();
		from_cost = path_ptr->Cost ();
		from_walk = path_ptr->Walk ();

		app_ptr = &exe->dir_array [approach];

		//---- check the circuity, local access and restrictions ----

		if (plan_flag && (param.distance_flag || param.local_flag || restrict_flag)) {
			link_ptr = &exe->link_array [app_ptr->Link ()];

			if (restrict_flag) {
				restrict_in = Use_Permission (link_ptr->Use (), RESTRICTED);
			}
			if (param.distance_flag || param.local_flag) {
				if (app_ptr->Dir () == 0) {
					if (forward_flag) {
						node_ptr = &exe->node_array [link_ptr->Bnode ()];
					} else {
						node_ptr = &exe->node_array [link_ptr->Anode ()];
					}
				} else {
					if (forward_flag) {
						node_ptr = &exe->node_array [link_ptr->Anode ()];
					} else {
						node_ptr = &exe->node_array [link_ptr->Bnode ()];
					}
				}
				x2 = node_ptr->X ();
				y2 = node_ptr->Y ();

				dx = x0 - x2;
				dy = y0 - y2;

				dist1 = dx * dx + dy * dy;
			
				dx = x1 - x2;
				dy = y1 - y2;

				dist2 = dx * dx + dy * dy;

				if (param.distance_flag && (dist1 + dist2) > max_len) {
					dist_flag = true;
					continue;
				}
				if (param.local_flag) {
					if (dist1 < dist2) {
						local_flag = (dist1 <= max_local);
					} else {
						local_flag = (dist2 <= max_local);
						dist1 = dist2;
					}
					if (!local_flag) {
						if (param.local_factor == 0.0) {
							local_factor = 0.0;
						} else {
							dist1 = dist1 / max_local;
							local_factor = ((dist1 * dist1) - 1) * param.local_factor + 1;
							if (local_factor < 1.0) local_factor = 1.0;
							if (local_factor > 25.0) local_factor = 25.0;
						}
					}
				} else {
					local_flag = true;
				}
			}
		}

		//---- process each link leaving the approach link ----

		for (index = app_ptr->First_Connect (forward_flag); index >= 0; index = connect_ptr->Next_Index (forward_flag)) {
			connect_ptr = &exe->connect_array [index];

			if (turn_delay_ptr != 0) {
				turn_delay = turn_delay_ptr->Data_Ptr (index);
			}

			//---- check for time of day turn penalties ----

			penalty = pen_imp = 0;

			for (index = app_ptr->First_Turn (forward_flag); index >= 0; index = turn_ptr->Next_Index (forward_flag)) {
				turn_ptr = &exe->turn_pen_array [index]; 

				if (forward_flag) {
					if (turn_ptr->To_Index () != connect_ptr->To_Index ()) continue;
				} else {
					if (turn_ptr->Dir_Index () != connect_ptr->Dir_Index ()) continue;
				}
				if (turn_ptr->Start () <= from_time && from_time < turn_ptr->End ()) {
					if (param.veh_type < 0 || turn_ptr->Min_Veh_Type () < 0 || 
						(turn_ptr->Min_Veh_Type () <= param.veh_type && param.veh_type <= turn_ptr->Max_Veh_Type ())) {

						if (turn_ptr->Use () == 0 || Use_Permission (turn_ptr->Use (), param.use)) break;
					}
				}
			}
			if (index >= 0) {
				if (turn_ptr->Penalty () == 0) continue;
				penalty = turn_ptr->Penalty ();
			}

			//--- get the link ---

			if (forward_flag) {
				index = connect_ptr->To_Index ();
			} else {
				index = connect_ptr->Dir_Index ();
			}
			dir_ptr = &exe->dir_array [index];

			link = dir_ptr->Link ();
			ab_flag = (dir_ptr->Dir () == 0);

			link_ptr = &exe->link_array [link];

			//---- check the vehicle type ----

			if (!Use_Permission (link_ptr->Use (), param.use)) {
				use_flag = true;
				continue;
			}

			//---- check the lane use ----

			if (!Best_Lane_Use (index, from_time, ttime, delay, cost, group)) {
				use_flag = true;
				continue;
			}

			//---- get the travel time ----

			if (param.turn_delay_flag && period >= 0) {
				penalty += turn_delay->Time ();
			}
			if (grade_flag && link_ptr->Grade (ab_flag) > 0) {
				ttime = ttime / veh_type_ptr->Grade (link_ptr->Grade (ab_flag));
			}
			ttime += delay;
			pen_imp = DTOI (penalty * param.value_time);

			//---- add turn impedance penalty ----

			if (turn_flag) {
				if (connect_ptr->Type () == LEFT) {
					imp = param.left_imped;
				} else if (connect_ptr->Type () == RIGHT) {
					imp = param.right_imped;
				} else if (connect_ptr->Type () == UTURN) {
					imp = param.uturn_imped;
				} else {
					imp = 0;
				}
				imp = Round (imp);
				if (pen_imp <= 0) {
					pen_imp += imp;
				} else if (pen_imp < imp) {
					pen_imp = imp;
				}
			}
			cst = cost;
			op_cost = DTOI (op_cost_rate * link_ptr->Length ());
			cost += op_cost;

			//---- calculate the link impedance ----

			imped = DTOI (ttime * param.value_time + link_ptr->Length () * param.value_dist + cost * param.value_cost);
			if (imped < 0) continue;

			if (link_ptr->Type () == FREEWAY) {
				factor = param.freeway_fac;
			} else if (link_ptr->Type () == EXPRESSWAY) {
				factor = param.express_fac;
			} else {
				factor = 1.0;
			}
			if (factor != 1.0) {
				imped = DTOI (imped * factor);
			}

			//---- check the local access restrictions ----

			if (!local_flag && link_ptr->Type () >= param.local_type && link_ptr->Type () <= LOCAL) {
				if (local_factor == 0.0) {
					local_acc_flag = true;
					continue;
				}
				imped = DTOI (imped * local_factor);
			} 

			//---- random impedance ----

			imp = imped + pen_imp;
			if (imp < 1) {
				if (imp < 0 && pen_imp >= 0) continue;
				imp = 1;
			}
			if (random_flag) {
				imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
				pen_imp = DTOI (pen_imp * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
			}
			to_walk = from_walk;

			//---- check for the trip end ----

			to_flag = false;

			for (to = link_to [link]; to >= 0; to = next_to [to]) {
				end_ptr = &to_ptr->at (to);

				path_ptr = &end_ptr->back ();
				if (from_imp >= path_ptr->Imped ()) continue;

				if (forward_flag) {
					length = (ab_flag) ? end_ptr->Offset () : (link_ptr->Length () - end_ptr->Offset ());
				} else {
					length = (ab_flag) ? (link_ptr->Length () - end_ptr->Offset ()) : end_ptr->Offset ();
				}
				len_factor = (double) length / link_ptr->Length ();

				//---- calculate the time ----

				ttim = (int) (ttime * len_factor + 0.5);
				if (ttim < 0) {
					time_flag = true;
					continue;
				}
				ttim += penalty;
				if (ttim < 0 && penalty < 0) ttim = 1;

				//---- check the time schedule ----

				if (forward_flag) {
					to_time = from_time + ttim;

					if (to_time > time_limit) {
						time_flag = true;
						continue;
					}
				} else {
					to_time = from_time - ttim;

					if (to_time < time_limit) {
						time_flag = true;
						continue;
					}
				}
				to_cost = from_cost + cst + (int) (op_cost * len_factor + 0.5);

				//---- adjust the link impedance ----

				imp = (int) (imped * len_factor + 0.5) + pen_imp;
				if (imp < 1) {
					if (imp < 0 && pen_imp >= 0) continue;
					imp = 1;
				}

				//---- check the cumulative impedance ----

				to_imp = from_imp + imp;

				if ((to_imp + imp_diff) >= max_imp || to_imp >= path_ptr->Imped ()) continue;

				to_len = from_len + length;

				path_ptr->Imped (to_imp);
				path_ptr->Time (to_time);
				path_ptr->Length (to_len);
				path_ptr->Cost (to_cost);
				path_ptr->Walk (to_walk);
				path_ptr->From (approach);
				path_ptr->Type (DIR_ID);
				path_ptr->Path (group);
				path_ptr->Dir ((ab_flag) ? 0 : 1);
				path_ptr->Mode (DRIVE_MODE);
				path_ptr->Status (1);

				to_flag = true;

				if (best_flag) {
					max_imp = to_imp + imp_diff;
					best_to = to;
				}
			}

			//---- update the max cum ----

			if (to_flag && !best_flag) {
				hi_imp = 0;

				for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
					path_ptr = &to_itr->back ();

					if (path_ptr->Imped () < MAX_IMPEDANCE) {
						to_imp = path_ptr->Imped () + imp_diff;
						if (to_imp > hi_imp) hi_imp = to_imp;
					} else {
						hi_imp = MAX_IMPEDANCE;
						break;
					}
				}
				if (hi_imp > 0 && hi_imp < max_imp) max_imp = hi_imp;
				best_to = 0;
			}

			//---- check the restricted access ----

			if (plan_flag) {
				if (Use_Permission (link_ptr->Use (), RESTRICTED)) {
					if (restrict_in || restrict_to) {
						if (!restrict_in) {
							restrict_from = false;
						}
					} else {
						use_flag = true;
						continue;
					}
				} else if (restrict_in && !restrict_from) {
					use_flag = true;
					continue;
				}
			}

			//---- check the travel time ----

			ttime += penalty;

			if (ttime < 0 && penalty < 0) {
				ttime = 1;
			}
			if (forward_flag) {
				to_time = from_time + ttime;

				if (to_time >= time_limit) {
					time_flag = true;
					continue;
				}
			} else {
				to_time = from_time - ttime;

				if (to_time <= time_limit) {
					time_flag = true;
					continue;
				}
			}
			to_cost = from_cost + cost;

			//---- check the cumulative impedance at B ----

			imp = imped + pen_imp;
			if (imp < 1) {
				if (imp < 0 && pen_imp >= 0) continue;
				imp = 1;
			}
			to_imp = from_imp + imp;

			if (to_imp >= max_imp) continue;
			
			path_ptr = &link_path [index];
		
			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the new path to the queue ----

			if (param.sort_method) {

				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (imp_itr = imp_sort.find (path_ptr->Imped ()); imp_itr != imp_sort.end (); imp_itr++) {
						if (imp_itr->second == index) {
							imp_sort.erase (imp_itr);
							break;
						} else if (imp_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				imp_sort.insert (Imp_Sort_Data (to_imp, index));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (index);
				} else if (path_ptr->Status () == 2) {
					next_index.push_front (index);
				}
			}
			to_len = from_len + link_ptr->Length ();

			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->From (approach);
			path_ptr->Type (DIR_ID);
			path_ptr->Path (group);
			path_ptr->Mode (DRIVE_MODE);
			path_ptr->Status (1);
		}
	}
	return (best_to);
}
