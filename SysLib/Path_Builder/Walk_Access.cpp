//*********************************************************
//	Walk_Access.cpp - process options leaving a node
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Walk_Access
//---------------------------------------------------------

bool Path_Builder::Walk_Access (Transit_Path_Index path_index, Path_End_Array *to_ptr, bool best_flag)
{
	int from_node, to_node, link, length, path, pathx, acc, type, cost, xfer;
	int from_cost, to_cost, from_len, to_len, from_walk, to_walk, to;
	int imp, imped, index, dir;
	unsigned from_imp, to_imp, hi_imp;
	Dtime from_time, to_time, ttime, ttim;
	bool ab_flag, to_flag, exit_flag;
	double len_factor, factor;

	Path_End_Itr to_itr;
	Link_Data *link_ptr;
	Path_Array *array_ptr;
	Path_Ptr path_ptr;
	Path_Itr path_itr;
	List_Data *list_ptr;
	Transit_Sort_Itr sort_itr;
	Stop_Data *stop_ptr;
	Access_Data *access_ptr;
	List_Data *acc_ptr;

	//---- initialize the path building ----

	exit_flag = false;

	from_node = path_index.Index ();
	path = path_index.Path ();
	pathx = (path < param.max_paths) ? path : param.max_paths - 1;

	array_ptr = &node_path [path];

	path_ptr = &array_ptr->at (from_node);
	path_ptr->Status (2);

	//---- check the cumulative impedance ----

	from_imp = path_ptr->Imped ();
	if (from_imp >= max_imp) return (exit_flag);

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

	to_cost = from_cost = path_ptr->Cost ();
	xfer = path_ptr->Xfer ();
	
	//---- process access links connections from the node ----
	
	if (exe->access_link_flag) {
		to_flag = false;
		acc = exe->node_access [from_node].Next (forward_flag);

		//---- process each access link leaving the node ----

		for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
			access_ptr = &exe->access_array [acc];
			acc_ptr = &exe->next_access (forward_flag) [acc];

			ab_flag = (access_ptr->From_Type () == NODE_ID && access_ptr->From_ID () == from_node);

			type = access_ptr->Type (!ab_flag);
			index = access_ptr->ID (!ab_flag);

			if (type != NODE_ID && type != STOP_ID) continue;
			if (type == STOP_ID && xfer == param.max_xfers) continue;

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
				path_ptr = &node_path [path] [index];
				path_index.Path (path);
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

	//---- process each link leaving the node ----

	for (link = exe->walk_list.at (from_node); link >= 0; link = list_ptr->Next (ab_flag)) {
		list_ptr = &exe->walk_link.at (link);

		link_ptr = &exe->link_array [link];

		ab_flag = (link_ptr->Anode () == from_node);

		to_node = (ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

		dir = (forward_flag) ? ((ab_flag) ? 0 : 1) : ((ab_flag) ? 1 : 0);

		//---- calculate the impedance ----

		ttime = (int) (link_ptr->Length () / param.walk_speed + 0.5);

		imped = DTOI (ttime * param.value_walk);
		if (imped < 1) imped = 1;

		if (random_flag) {
			imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
		}

		//---- check for destinations ----
		
		if (link_to_flag && link_to [link] == 1) {
			to_flag = false;

			for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
				if (to_itr->Type () != LINK_ID || link != to_itr->Index ()) continue;

				//---- calculate and check the distance ----

				length = (ab_flag) ? to_itr->Offset () : link_ptr->Length () - to_itr->Offset ();

				len_factor = (double) length / link_ptr->Length ();

				//---- calculate and check the time ----

				ttim = (int) (ttime * len_factor + 0.5);
				
				to_walk = from_walk + length;

				if (walk_flag && to_walk >= param.max_walk) {
					length_flag = true;
					continue;
				}

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

				//---- check the cumulative impedance ----

				imp = (int) (imped * len_factor + 0.5);
				if (imp < 1) imp = 1;

				if (walk_flag && to_walk > param.walk_pen) {
					factor = (double) to_walk / param.walk_pen;
					factor = ((factor * factor) - 1) * param.walk_fac + 1;
					imp = DTOI (imp * factor);
				}
				to_imp = from_imp + imp;

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
				path_ptr->From (from_node);
				path_ptr->Type (NODE_ID);
				path_ptr->Path (path);
				path_ptr->Dir (dir);
				path_ptr->Xfer (xfer);
				path_ptr->Status (1);

				to_flag = true;

				if (best_flag) {
					max_imp = to_imp + imp_diff;
					exit_flag = true;;
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

		//---- check for transit stops on the link ----

		for (index = exe->link_stop [link]; index >= 0 && xfer < param.max_xfers; index = exe->next_stop [index]) {
			stop_ptr = &exe->stop_array [index];

			if (ab_flag && stop_ptr->Dir () == 0) {
				length = stop_ptr->Offset ();
			} else {
				length = link_ptr->Length () - stop_ptr->Offset ();
			}
			to_walk = from_walk + length;

			if (walk_flag && to_walk >= param.max_walk) {
				length_flag = true;
				continue;
			}
			len_factor = (double) length / link_ptr->Length ();

			//---- calculate and check the time ----

			ttim = (int) (ttime * len_factor + 0.5);

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
			
			//---- check the cumulative impedance ----

			imp = (int) (imped * len_factor + 0.5);
			if (imp < 1) imp = 1;

			if (walk_flag && to_walk > param.walk_pen) {
				factor = (double) to_walk / param.walk_pen;
				factor = ((factor * factor) - 1) * param.walk_fac + 1;
				imp = DTOI (imp * factor);
			}
			to_imp = from_imp + imp;

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
					path_ptr->From (from_node);
					path_ptr->Type (NODE_ID);
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
			path_ptr->From (from_node);
			path_ptr->Type (NODE_ID);
			path_ptr->Path (path);
			path_ptr->Dir (dir);
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
		}

		//---- check max distance ----
		
		to_walk = from_walk + link_ptr->Length ();

		if (walk_flag && to_walk >= param.max_walk) {
			length_flag = true;
			continue;
		}

		//---- check the time ----

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
		to_len = from_len + link_ptr->Length ();

		//---- check the cumulative impedance to node ----

		if (walk_flag && to_walk > param.walk_pen) {
			factor = (double) to_walk / param.walk_pen;
			factor = ((factor * factor) - 1) * param.walk_fac + 1;
			imped = DTOI (imped * factor);
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
				path_ptr->From (from_node);
				path_ptr->Type (NODE_ID);
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

		path_ptr = &array_ptr->at (to_node);

		if (to_imp >= path_ptr->Imped ()) continue;

		//---- add the new path to the queue ----
		
		path_index.Index (to_node);
		path_index.Type (NODE_ID);
		path_index.Path (path);

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
		path_ptr->From (from_node);
		path_ptr->Type (NODE_ID);
		path_ptr->Path (path);
		path_ptr->Dir (dir);
		path_ptr->Xfer (xfer);
		path_ptr->Status (1);
	}
	return (exit_flag);
}
