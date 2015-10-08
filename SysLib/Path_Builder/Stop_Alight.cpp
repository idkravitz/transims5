//*********************************************************
//	Stop_Alight.cpp - process options leaving a stop
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Stop_Alight
//---------------------------------------------------------

bool Path_Builder::Stop_Alight (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag)
{
	int stop, to_node, link, dir, offset, length, acc, type, cost;
	int from_cost, to_cost, from_len, to_len, from_walk, to_walk, to;
	int imped, index, path, path1, pathx, xfer;
	unsigned from_imp, to_imp, hi_imp;
	Dtime from_time, to_time, ttime, ttim;
	bool ab_flag, to_flag, exit_flag;
	double factor;

	Path_End_Itr to_itr;
	Link_Data *link_ptr;
	Path_Ptr path_ptr;
	Path_Itr path_itr;
	Transit_Sort_Itr sort_itr;
	Stop_Data *stop_ptr;
	Access_Data *access_ptr;
	List_Data *acc_ptr;

	//---- initialize the path building ----

	exit_flag = false;

	stop = path_index.Index ();
	path = path_index.Path ();
	path1 = path + 1;
	if (path1 > param.max_paths) path1 = param.max_paths;
	pathx = (path1 == param.max_paths) ? path : path1;

	//---- get the current path data ----

	path_ptr = &alight_path [path] [stop];
	path_ptr->Status (2);

	from_imp = path_ptr->Imped ();
	if (from_imp >= max_imp) return (exit_flag);

	index = path_ptr->From ();

	//---- check the time limits ----
		
	from_time = path_ptr->Time ();

	if (forward_flag) {
		if (from_time >= time_limit) {
			time_flag = true;
			return (exit_flag);
		}
	} else {
		if (from_time <= time_limit) {
			time_flag = true;
			return (exit_flag);
		}
	}

	//---- check max walk ----

	from_walk = path_ptr->Walk ();

	if (walk_flag && from_walk >= param.max_walk) {
		length_flag = true;
		return (exit_flag);
	}
	from_len = path_ptr->Length ();

	from_cost = path_ptr->Cost ();
	xfer = path_ptr->Xfer ();

	//---- consider adding the stop back ----

	if (xfer < param.max_xfers) {
		path_ptr = &board_path [pathx] [stop];
					
		if (path_ptr->Imped () > from_imp) {

			to_imp = from_imp;
			to_time = from_time;
			to_len = from_len;
			to_cost = from_cost;
			to_walk = from_walk;

			//---- add the new path to the queue ----

			path_index.Index (stop);
			path_index.Type (STOP_ID);
			path_index.Path (pathx);

			if (param.sort_method) {

				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (sort_itr = transit_sort.find (path_ptr->Imped ()); sort_itr != transit_sort.end (); sort_itr++) {
						if (sort_itr->second == path_index) {
							transit_sort.erase (sort_itr);
							break;
						} else if (sort_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				transit_sort.insert (Transit_Sort_Data (to_imp, path_index));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (path_index);
				} else if (path_ptr->Status () == 2) {
					next_index.push_front (path_index);
				}
			}
			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (WALK_MODE);
			path_ptr->From (stop);
			path_ptr->Type (STOP_ID);
			path_ptr->Path (path);
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
		}
	}
	
	//---- process access link connections from the stop ----
	
	to_flag = false;

	if (exe->access_link_flag) {
		acc = exe->stop_access [stop].Next (forward_flag);

		//---- process each access link leaving the stop ----

		for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
			access_ptr = &exe->access_array [acc];
			acc_ptr = &exe->next_access (forward_flag) [acc];

			ab_flag = (access_ptr->From_Type () == STOP_ID && access_ptr->From_ID () == stop);

			type = access_ptr->Type (!ab_flag);
			index = access_ptr->ID (!ab_flag);

			if (type != STOP_ID && type != NODE_ID) continue;
			if (type == STOP_ID && xfer >= param.max_xfers) continue;

			ttime = access_ptr->Time ();
			cost = access_ptr->Cost ();

			length = (int) (ttime * param.walk_speed + 0.5);

			to_walk = from_walk + length;

			if (to_walk >= param.max_walk) {
				length_flag = true;
				continue;
			}
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
			to_len = from_len + length;
			to_cost = from_cost + cost;

			//---- calculate and check the impedance ----

			imped = DTOI (ttime * param.value_walk + cost * param.value_cost);
			if (imped < 1) imped = 1;

			if (walk_flag && to_walk > param.walk_pen) {
				factor = (double) to_walk / param.walk_pen;
				factor = ((factor * factor) - 1) * param.walk_fac + 1;
				imped = DTOI (imped * factor);
			} else if (random_flag) {
				imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
			}
			to_imp = from_imp + imped;

			if (to_imp >= max_imp) continue;

			//---- check for a destintion ----

			if ((type == NODE_ID && node_to_flag && node_to [index] == 1) ||
				(type == STOP_ID && stop_to_flag && stop_to [index] == 1)) {

				to_flag = false;

				for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
					if (to_itr->Type () != type || index != to_itr->Index ()) continue;

					if (to_imp + imp_diff >= max_imp) continue;
			
					path_ptr = &to_itr->back ();
			
					if (to_imp >= path_ptr->Imped ()) continue;

					//---- save the destination totals ----

					path_ptr->Imped (to_imp);
					path_ptr->Time (to_time);
					path_ptr->Length (to_len);
					path_ptr->Cost (to_cost);
					path_ptr->Walk (to_walk);
					path_ptr->Mode (WALK_MODE);
					path_ptr->From (acc);
					path_ptr->Type (ACCESS_ID);
					path_ptr->Path (path);
					path_ptr->Dir ((ab_flag) ? 0 : 1);
					path_ptr->Xfer (xfer);
					path_ptr->Status (1);

					to_flag = true;

					if (best_flag) {
						max_imp = to_imp + imp_diff;
						exit_flag = true;
					}
				}

				//---- update the max impedance ----

				if (to_flag) {
					if (best_flag) continue;
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
					exit_flag = true;
				}
			}

			//---- consider adding the node or stop to the processing list ----

			if (type == STOP_ID) {
				path_ptr = &board_path [pathx] [index];
				path_index.Path (pathx);
			} else if (type == NODE_ID) {
				path_ptr = &node_path [path1] [index];
				path_index.Path (path1);
			}
			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the new path to the queue ----

			path_index.Index (index);
			path_index.Type (type);

			if (param.sort_method) {

				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (sort_itr = transit_sort.find (path_ptr->Imped ()); sort_itr != transit_sort.end (); sort_itr++) {
						if (sort_itr->second == path_index) {
							transit_sort.erase (sort_itr);
							break;
						} else if (sort_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				transit_sort.insert (Transit_Sort_Data (to_imp, path_index));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (path_index);
				} else if (path_ptr->Status () == 2) {
					next_index.push_front (path_index);
				}
			}
			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (WALK_MODE);
			path_ptr->From (acc);
			path_ptr->Type (ACCESS_ID);
			path_ptr->Path (path);
			path_ptr->Dir ((ab_flag) ? 0 : 1);
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
		}
	}

	//---- get the link offset ----

	stop_ptr = &exe->stop_array [stop];

	link = stop_ptr->Link ();

	link_ptr = &exe->link_array [link];

	if (!Use_Permission (link_ptr->Use (), WALK)) {
		use_flag = true;
		return (exit_flag);
	}
	if (stop_ptr->Dir () == 0) {
		offset = stop_ptr->Offset ();
	} else {
		offset = link_ptr->Length () - stop_ptr->Offset ();
	}

	//---- check for destinations on the same link ----
	
	if (link_to_flag && link_to [link] == 1) {
		to_flag = false;

		for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
			if (to_itr->Type () == LINK_ID && link != to_itr->Index ()) continue;

			//---- calculate and check the length ----

			length = to_itr->Offset () - offset;

			ab_flag = (length >= 0);
			if (!ab_flag) length = -length;
			
			dir = (forward_flag) ? ((ab_flag) ? 0 : 1) : ((ab_flag) ? 1 : 0);

			to_walk = from_walk + length;

			if (to_walk >= param.max_walk) {
				length_flag = true;
				continue;
			}

			//---- calculate and check the time ----

			ttime = (int) (length / param.walk_speed + 0.5);

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
			to_len = from_len + length;
			to_cost = from_cost;

			//---- calculate and check the impedance ----

			imped = DTOI (ttime * param.value_walk);
			if (imped < 1) imped = 1;

			if (walk_flag && to_walk > param.walk_pen) {
				factor = (double) to_walk / param.walk_pen;
				factor = ((factor * factor) - 1) * param.walk_fac + 1;
				imped = DTOI (imped * factor);
			} else if (random_flag) {
				imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
			}
			to_imp = from_imp + imped;

			if (to_imp + imp_diff >= max_imp) continue;

			path_ptr = &to_itr->back ();

			if (to_imp >= path_ptr->Imped ()) continue;

			//---- save the destination totals ----

			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (WALK_MODE);
			path_ptr->From (stop);
			path_ptr->Type (STOP_ID);
			path_ptr->Path (path);
			path_ptr->Dir (dir);
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);

			to_flag = true;

			if (best_flag) {
				max_imp = to_imp + imp_diff;
				exit_flag = true;
			}
		}

		//---- update the max impedance ----

		if (to_flag) {
			if (best_flag) return (exit_flag);
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
			exit_flag = true;
		}
	}

	//---- check for transit stops on the link ----

	for (index = exe->link_stop [link]; index >= 0 && xfer < param.max_xfers; index = exe->next_stop [index]) {
		if (index == stop) continue;

		stop_ptr = &exe->stop_array [index];

		if (stop_ptr->Dir () == 0) {
			length = stop_ptr->Offset () - offset;
		} else {
			length = link_ptr->Length () - stop_ptr->Offset () - offset;
		}
		ab_flag = (length >= 0);
		if (!ab_flag) length = -length;
		
		dir = (forward_flag) ? ((ab_flag) ? 0 : 1) : ((ab_flag) ? 1 : 0);

		//---- calculate and check the length ----

		to_walk = from_walk + length;

		if (walk_flag && to_walk >= param.max_walk) {
			length_flag = true;
			continue;
		}

		//---- calculate and check the time ----

		ttim = (int) (length / param.walk_speed + 0.5);

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
		to_len = from_len + length;
		to_cost = from_cost;
		
		//---- check the cumulative impedance ----

		imped = DTOI (ttim * param.value_walk);
		if (imped < 1) imped = 1;

		if (walk_flag && to_walk > param.walk_pen) {
			factor = (double) to_walk / param.walk_pen;
			factor = ((factor * factor) - 1) * param.walk_fac + 1;
			imped = DTOI (imped * factor);
		}
		to_imp = from_imp + imped;

		if (to_imp >= max_imp) continue;
			
		//---- check for a destintion stop ----

		if (stop_to_flag && stop_to [index] == 1) {
			to_flag = false;

			for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
				if (to_itr->Type () != STOP_ID || index != to_itr->Index ()) continue;

				if (to_imp + imp_diff >= max_imp) continue;
		
				path_ptr = &to_itr->back ();
		
				if (to_imp >= path_ptr->Imped ()) continue;

				//---- save the destination totals ----

				path_ptr->Imped (to_imp);
				path_ptr->Time (to_time);
				path_ptr->Length (to_len);
				path_ptr->Cost (to_cost);
				path_ptr->Walk (to_walk);
				path_ptr->Mode (WALK_MODE);
				path_ptr->From (stop);
				path_ptr->Type (STOP_ID);
				path_ptr->Path (path);
				path_ptr->Dir (dir);
				path_ptr->Xfer (xfer);
				path_ptr->Status (1);

				to_flag = true;

				if (best_flag) {
					max_imp = to_imp + imp_diff;
					exit_flag = true;
				}
			}

			//---- update the max impedance ----

			if (to_flag) {
				if (best_flag) continue;
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
				exit_flag = true;
			}
		}

		//---- check the current path ----

		path_ptr = &board_path [pathx] [index];
		
		if (to_imp >= path_ptr->Imped ()) continue;

		//---- add the new path to the queue ----

		path_index.Index (index);
		path_index.Type (STOP_ID);
		path_index.Path (pathx);

		if (param.sort_method) {

			//---- remove the previous path from the queue ----

			if (path_ptr->Status () == 1) {
				for (sort_itr = transit_sort.find (path_ptr->Imped ()); sort_itr != transit_sort.end (); sort_itr++) {
					if (sort_itr->second == path_index) {
						transit_sort.erase (sort_itr);
						break;
					} else if (sort_itr->first > path_ptr->Imped ()) {
						break;
					}
				}
			}
			transit_sort.insert (Transit_Sort_Data (to_imp, path_index));
		} else {
			if (path_ptr->Status () == 0) {
				next_index.push_back (path_index);
			} else if (path_ptr->Status () == 2) {
				next_index.push_front (path_index);
			}
		}
		path_ptr->Imped (to_imp);
		path_ptr->Time (to_time);
		path_ptr->Length (to_len);
		path_ptr->Cost (to_cost);
		path_ptr->Walk (to_walk);
		path_ptr->Mode (WALK_MODE);
		path_ptr->From (stop);
		path_ptr->Type (STOP_ID);
		path_ptr->Path (path);
		path_ptr->Dir (dir);
		path_ptr->Xfer (xfer);
		path_ptr->Status (1);
	}

	//---- process each direction ----

	for (dir=0; dir < 2; dir++) {

		//---- calculate and check the distance ----

		if (dir == 0) {
			to_node = link_ptr->Bnode ();
			length = link_ptr->Length () - offset;
		} else {
			to_node = link_ptr->Anode ();
			length = offset;
		}
		to_walk = from_walk + length;

		if (to_walk >= param.max_walk) {
			length_flag = true;
			continue;
		}

		//---- calculate and check the time ----

		ttime = (int) (length / param.walk_speed + 0.5);

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
		to_len = from_len + length;
		to_cost = from_cost;

		//---- calculate and check the impedance ----

		imped = DTOI (ttime * param.value_walk);
		if (imped < 1) imped = 1;

		if (walk_flag && to_walk > param.walk_pen) {
			factor = (double) to_walk / param.walk_pen;
			factor = ((factor * factor) - 1) * param.walk_fac + 1;
			imped = DTOI (imped * factor);
		} else if (random_flag) {
			imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
		}
		to_imp = from_imp + imped;

		if (to_imp >= max_imp) continue;
				
		//---- check for a destintion node ----

		if (node_to_flag && node_to [to_node] == 1) {
			to_flag = false;

			for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
				if (to_itr->Type () != NODE_ID || to_node != to_itr->Index ()) continue;

				if (to_imp + imp_diff >= max_imp) continue;
		
				path_ptr = &to_itr->back ();
		
				if (to_imp >= path_ptr->Imped ()) continue;

				//---- save the destination totals ----

				path_ptr->Imped (to_imp);
				path_ptr->Time (to_time);
				path_ptr->Length (to_len);
				path_ptr->Cost (to_cost);
				path_ptr->Walk (to_walk);
				path_ptr->Mode (WALK_MODE);
				path_ptr->From (stop);
				path_ptr->Type (STOP_ID);
				path_ptr->Path (path);
				path_ptr->Dir ((forward_flag) ? dir : (1 - dir));
				path_ptr->Xfer (xfer);
				path_ptr->Status (1);

				to_flag = true;

				if (best_flag) {
					max_imp = to_imp + imp_diff;
					exit_flag = true;
				}
			}

			//---- update the max impedance ----

			if (to_flag) {
				if (best_flag) continue;
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
				exit_flag = true;
			}
		}

		//---- check the current path ----

		path_ptr = &node_path [path1] [to_node];

		if (to_imp >= path_ptr->Imped ()) continue;

		//---- add the record to the queue ----
		
		path_index.Index (to_node);
		path_index.Type (NODE_ID);
		path_index.Path (path1);

		if (param.sort_method) {

			//---- remove the previous path from the queue ----

			if (path_ptr->Status () == 1) {
				for (sort_itr = transit_sort.find (path_ptr->Imped ()); sort_itr != transit_sort.end (); sort_itr++) {
					if (sort_itr->second == path_index) {
						transit_sort.erase (sort_itr);
						break;
					} else if (sort_itr->first > path_ptr->Imped ()) {
						break;
					}
				}
			}
			transit_sort.insert (Transit_Sort_Data (to_imp, path_index));
		} else {
			if (path_ptr->Status () == 0) {
				next_index.push_back (path_index);
			} else if (path_ptr->Status () == 2) {
				next_index.push_front (path_index);
			}
		}
		path_ptr->Imped (to_imp);
		path_ptr->Time (to_time);
		path_ptr->Length (to_len);
		path_ptr->Cost (to_cost);
		path_ptr->Walk (to_walk);
		path_ptr->Mode (WALK_MODE);
		path_ptr->From (stop);
		path_ptr->Type (STOP_ID);
		path_ptr->Dir ((forward_flag) ? dir : (1 - dir));
		path_ptr->Path (path);
		path_ptr->Xfer (xfer);
		path_ptr->Status (1);
	}
	return (exit_flag);
}
