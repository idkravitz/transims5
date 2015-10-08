//*********************************************************
//	Transit_Access.cpp - initialize transit path building
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Transit_Access
//---------------------------------------------------------

bool Path_Builder::Transit_Access (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag)
{
	int index, imped, length, offset, path, dir, xfer;
	int from, from_type, from_index, from_cost, from_len, from_walk;
	int to, to_type, to_index, to_cost, to_len, to_walk;
	unsigned from_imp, to_imp, hi_imp;
	Dtime from_time, to_time, ttime, ttim;
	bool ab_flag, to_flag, exit_flag;
	double factor;

	Path_End_Itr from_itr, to_itr;
	Link_Data *link_ptr;
	Path_Data path_data;
	Path_Ptr path_ptr;
	Path_Itr path_itr;
	Transit_Sort_Itr sort_itr;
	Transit_Path_Index path_index;
	Stop_Data *stop_ptr;

	//---- initialize the path building ----

	exit_flag = false;
	path = 0;
	xfer = -1;

	for (from=0, from_itr = from_ptr->begin (); from_itr != from_ptr->end (); from_itr++, from++) {

		if (from_itr->Index () < 0) continue;

		from_type = from_itr->Type ();
		from_index = from_itr->Index ();

		path_ptr = &from_itr->back ();

		from_imp = path_ptr->Imped ();
		from_time = path_ptr->Time ();
		from_len = path_ptr->Length ();
		from_cost = path_ptr->Cost ();
		from_walk = path_ptr->Walk();

		//---- process access points ----

		if (from_type == NODE_ID || from_type == STOP_ID) {
			to_type = from_type;
			to_index = from_index;
			to_imp = from_imp;
			to_time = from_time;
			to_len = from_len;
			to_cost = from_cost;
			to_walk = from_walk;
				
			//---- check for a destintion ----

			if ((to_type == NODE_ID && node_to_flag && node_to [to_index] == 1) ||
				(to_type == STOP_ID && stop_to_flag && stop_to [to_index] == 1)) {

				to_flag = false;

				for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
					if (to_itr->Type () != to_type || to_index != to_itr->Index ()) continue;

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
					path_ptr->From (from);
					path_ptr->Type (FROM_ID);
					path_ptr->Path (path);
					path_ptr->Dir (0);
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

			if (to_type == NODE_ID) {
				path_ptr = &node_path [path] [to_index];
			} else  {
				path_ptr = &board_path [path] [to_index];
			}
			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the new path to the queue ----

			path_index.Index (to_index);
			path_index.Type (to_type);
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
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (path);
			path_ptr->Dir (0);
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
			continue;
		}

		//---- process the link offset ----

		if (from_type != LINK_ID) continue;

		link_ptr = &exe->link_array [from_index];

		if (!Use_Permission (link_ptr->Use (), WALK)) {
			use_flag = true;
			continue;
		}
		offset = from_itr->Offset ();

		//---- check for destinations on the same link ----
		
		if (link_to_flag && link_to [from_index] == 1) {
			to_flag = false;

			for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
				if (to_itr->Type () != from_type || from_index != to_itr->Index ()) continue;

				//---- calculate and check the distance ----

				if (forward_flag) {
					length = to_itr->Offset () - offset;
				} else {
					length = offset - to_itr->Offset ();
				}
				ab_flag = (length >= 0);
				if (!ab_flag) length = -length;

				to_len = from_len + length;

				if (walk_flag && to_len >= param.max_walk) {
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
				to_walk = from_walk + length;

				//---- calculate and check the impedance ----

				imped = DTOI (ttime * param.value_walk);
				if (imped < 1) imped = 1;

				if (walk_flag && to_len > param.walk_pen) {
					factor = (double) to_len / param.walk_pen;
					factor = ((factor * factor) - 1) * param.walk_fac + 1;
					imped = DTOI (imped * factor);
				} else if (random_flag) {
					imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
				}
				to_imp = from_imp + imped;

				if (to_imp + imp_diff >= max_imp) continue;
			
				path_ptr = &to_itr->back ();
		
				if (to_imp >= path_ptr->Imped ()) continue;
				to_cost = from_cost;

				//---- save the destination totals ----

				path_ptr->Imped (to_imp);
				path_ptr->Time (to_time);
				path_ptr->Length (to_len);
				path_ptr->Cost (to_cost);
				path_ptr->Walk (to_walk);
				path_ptr->Mode (WALK_MODE);
				path_ptr->From (from);
				path_ptr->Type (FROM_ID);
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

		//---- check for transit stops on the link ----

		for (index = exe->link_stop [from_index]; index >= 0; index = exe->next_stop [index]) {

			stop_ptr = &exe->stop_array [index];

			if (stop_ptr->Dir () == 0) {
				length = stop_ptr->Offset () - offset;
			} else {
				length = link_ptr->Length () - stop_ptr->Offset () - offset;
			}
			if (!forward_flag) length = -length;

			ab_flag = (length >= 0);
			if (!ab_flag) length = -length;

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
		
			//---- check the impedance ----

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
					path_ptr->From (from);
					path_ptr->Type (FROM_ID);
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

			//---- check the current path ----

			path_ptr = &board_path [path] [index];
	
			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the new path to the queue ----

			path_index.Index (index);
			path_index.Type (STOP_ID);
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
			to_cost = from_cost;

			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (WALK_MODE);
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (path);
			path_ptr->Dir ((ab_flag) ? 0 : 1);

			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
		}

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {

			//---- calculate and check the distance ----

			if (dir == 0) {
				to_index = link_ptr->Bnode ();
				length = link_ptr->Length () - offset;
			} else {
				to_index = link_ptr->Anode ();
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

			if (node_to_flag && node_to [to_index] == 1) {
				to_flag = false;

				for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
					if (to_itr->Type () != NODE_ID || to_index != to_itr->Index ()) continue;

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
					path_ptr->From (from);
					path_ptr->Type (FROM_ID);
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

			path_ptr = &node_path [path] [to_index];

			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the record to the queue ----
	
			path_index.Index (to_index);
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
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (path);
			path_ptr->Dir ((forward_flag) ? dir : (1 - dir));
			path_ptr->Xfer (xfer);
			path_ptr->Status (1);
		}
	}
	return (exit_flag);
}

