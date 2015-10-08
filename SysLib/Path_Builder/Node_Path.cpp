//*********************************************************
//	Node_Path.cpp - build an all-walk or bike path
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Node_Path
//---------------------------------------------------------

int Path_Builder::Node_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag)
{
	int from_node, to_node, link, dir, length, max_len, pen_len, mode;
	int from_cost, to_cost, from_len, to_len, from_walk, to_walk, from, to;
	int imp, imped, best_to;
	unsigned from_imp, to_imp, hi_imp;
	Dtime from_time, to_time, ttime, ttim;
	bool ab_flag, to_flag, flag;
	double speed, len_factor, value, pen_fac, factor;
	
	Path_End_Itr from_itr, to_itr;
	Link_Data *link_ptr;
	Path_Array *array_ptr;
	Path_Data *path_ptr;
	Path_Itr path_itr;
	List_Data *list_ptr;
	Integers *mode_list_ptr;
	List_Array *mode_link_ptr;

	Int_Queue next_index;
	Imp_Sort imp_sort;
	Imp_Sort_Itr imp_itr;

	//---- initialize the mode ----

	if (param.use == WALK) {
		if (!exe->walk_path_flag) return (-1);
		mode = WALK_MODE;
		flag = walk_flag;
		max_len = param.max_walk;
		pen_len = param.walk_pen;
		pen_fac = param.walk_fac;
		value = param.value_walk;
		speed = param.walk_speed;
		mode_list_ptr = &exe->walk_list;
		mode_link_ptr = &exe->walk_link;
	} else {
		if (!exe->bike_path_flag) return (-1);
		mode = BIKE_MODE;
		flag = bike_flag;
		max_len = param.max_bike;
		pen_len = param.bike_pen;
		pen_fac = param.bike_fac;
		value = param.value_bike;
		speed = param.bike_speed;
		mode_list_ptr = &exe->bike_list;
		mode_link_ptr = &exe->bike_link;
	}

	//---- initialize the path building ----

	array_ptr = &node_path [0];

	if (array_ptr->size () == 0) {
		array_ptr->resize (exe->node_array.size ());
	} else {
		array_ptr->Clear ();
	}
	best_to = -1;
	to_walk = 0;

	//---- calculate the access link impedance ----

	for (from=0, from_itr = from_ptr->begin (); from_itr != from_ptr->end (); from_itr++, from++) {

		//---- process node access ----

		if (from_itr->Type () == NODE_ID) {

			to_node = from_itr->Index ();
			path_ptr = &from_itr->back ();

			to_imp = path_ptr->Imped ();
			to_time = path_ptr->Time ();
			to_len = path_ptr->Length ();
			to_cost = path_ptr->Cost ();
			to_walk = path_ptr->Walk ();

			path_ptr = &array_ptr->at (to_node);

			//---- add the record to the queue ----

			if (param.sort_method) {
			
				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (imp_itr = imp_sort.find (path_ptr->Imped ()); imp_itr != imp_sort.end (); imp_itr++) {
						if (imp_itr->second == to_node) {
							imp_sort.erase (imp_itr);
							break;
						} else if (imp_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				imp_sort.insert (Imp_Sort_Data (to_imp, to_node));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (to_node);
				}
			}
			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (mode);
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (0);
			path_ptr->Dir (0);
			path_ptr->Status (1);
			continue;
		}

		//---- get the link data ----

		if (from_itr->Type () != LINK_ID) continue;

		link_ptr = &exe->link_array [from_itr->Index ()];

		if (!Use_Permission (link_ptr->Use (), param.use)) {
			use_flag = true;
			continue;
		}

		//---- initialize the from_node values ----

		path_ptr = &from_itr->back ();

		from_imp = path_ptr->Imped ();
		from_time = path_ptr->Time ();
		from_len = path_ptr->Length ();
		from_cost = path_ptr->Cost ();
		from_walk = path_ptr->Walk ();

		if (param.use != WALK) {
			to_walk = from_walk;
		}
		to_cost = from_cost;

		//---- check for from and to on the same link ----
		
		if (link_to_flag && link_to [from_itr->Index ()] == 1) {
			to_flag = false;

			for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
				if (to_itr->Type () != LINK_ID || from_itr->Index () != to_itr->Index ()) continue;

				//---- calculate and check the distance ----

				length = abs (to_itr->Offset () - from_itr->Offset ());

				to_len = from_len + length;

				if (flag && to_len >= max_len) {
					length_flag = true;
					continue;
				}

				//---- calculate and check the time ----

				ttime = (int) (length / speed + 0.5);

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
				if (param.use == WALK) {
					to_walk = from_walk + length;
				}

				//---- calculate and check the impedance ----

				imped = DTOI (ttime * value);
				if (imped < 1) imped = 1;

				if (flag && to_len > pen_len) {
					factor = (double) to_len / pen_len;
					factor = ((factor * factor) - 1) * pen_fac + 1;
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
				path_ptr->Mode (mode);
				path_ptr->From (from);
				path_ptr->Type (FROM_ID);
				path_ptr->Path (0);
				path_ptr->Dir ((to_itr->Offset () > from_itr->Offset ()) ? 0 : 1);
				path_ptr->Status (1);

				to_flag = true;

				if (best_flag) {
					max_imp = to_imp + imp_diff;
					best_to = to;
				}
			}

			//---- update the max imp ----

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
		}

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {

			//---- calculate and check the distance ----

			if (dir == 0) {
				to_node = link_ptr->Bnode ();
				length = link_ptr->Length () - from_itr->Offset ();
			} else {
				to_node = link_ptr->Anode ();
				length = from_itr->Offset ();
			}
			to_len = from_len + length;

			if (flag && to_len >= max_len) {
				length_flag = true;
				continue;
			}

			//---- calculate and check the time ----

			ttime = (int) (length / speed + 0.5);

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
			if (param.use == WALK) {
				to_walk = from_walk + length;
			}

			//---- calculate and check the impedance ----

			imped = DTOI (ttime * value);
			if (imped < 1) imped = 1;

			if (flag && to_len > pen_len) {
				factor = (double) to_len / pen_len;
				factor = ((factor * factor) - 1) * pen_fac + 1;
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
					path_ptr->Mode (mode);
					path_ptr->From (from);
					path_ptr->Type (FROM_ID);
					path_ptr->Path (0);
					path_ptr->Dir ((forward_flag) ? dir : (1 - dir));
					path_ptr->Status (1);

					to_flag = true;

					if (best_flag) {
						max_imp = to_imp + imp_diff;
						best_to = to;
					}
				}

				//---- update the max imp ----

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
					continue;
				}
			}

			//---- check the node impedance ----

			path_ptr = &array_ptr->at (to_node);

			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the record to the queue ----

			if (param.sort_method) {
			
				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (imp_itr = imp_sort.find (path_ptr->Imped ()); imp_itr != imp_sort.end (); imp_itr++) {
						if (imp_itr->second == to_node) {
							imp_sort.erase (imp_itr);
							break;
						} else if (imp_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				imp_sort.insert (Imp_Sort_Data (to_imp, to_node));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (to_node);
				}
			}
			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (mode);
			path_ptr->From (from);
			path_ptr->Type (FROM_ID);
			path_ptr->Path (0);
			path_ptr->Dir ((forward_flag) ? dir : (1 - dir));
			path_ptr->Status (1);
		}
	}
	if (param.sort_method) {
		if (imp_sort.empty ()) return (best_to);
	} else {
		if (next_index.empty ()) {
			return (best_to);
		}
	}

	//---- build the path ----

	for (;;) {
		
		//---- remove the approach link from the processing queue ----

		if (param.sort_method) {
			if (imp_sort.empty ()) break;
			imp_itr = imp_sort.begin ();
			from_node = imp_itr->second;
			imp_sort.erase (imp_itr);
		} else {
			if (next_index.empty ()) break;
			from_node = next_index.front ();
			next_index.pop_front ();
		}
		path_ptr = &array_ptr->at (from_node);
		path_ptr->Status (2);

		//---- check the cumulative impedance ----

		from_imp = path_ptr->Imped ();

		if (from_imp >= max_imp) continue;

		//---- check the time limits ----
			
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
		from_walk = path_ptr->Walk ();

		//---- check max length ----

		from_len = path_ptr->Length ();

		if (flag && from_len >= max_len) {
			length_flag = true;
			continue;
		}

		to_cost = from_cost = path_ptr->Cost ();

		if (param.use != WALK) {
			to_walk = from_walk;
		}

		//---- process each link leaving the node ----

		for (link = mode_list_ptr->at (from_node); link >= 0; link = list_ptr->Next (ab_flag)) {
			list_ptr = &mode_link_ptr->at (link);

			link_ptr = &exe->link_array [link];

			ab_flag = (link_ptr->Anode () == from_node);
			dir = (ab_flag) ? 0 : 1;
			if (!forward_flag) dir = 1 - dir;

			to_node = (ab_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

			//---- calculate the impedance ----

			ttime = (int) (link_ptr->Length () / speed + 0.5);

			imped = DTOI (ttime * value);
			if (imped < 1) imped = 1;

			if (random_flag) {
				imped = DTOI (imped * (1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0));
			}

			//---- check for the trip end ----

			if (link_to_flag && link_to [link] == 1) {

				to_flag = false;

				for (to=0, to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++, to++) {
					if (to_itr->Type () != LINK_ID || link != to_itr->Index ()) continue;

					//---- calculate and check the distance ----

					length = (ab_flag) ? to_itr->Offset () : link_ptr->Length () - to_itr->Offset ();

					to_len = from_len + length;

					if (flag && to_len >= max_len) {
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
					if (param.use == WALK) {
						to_walk = from_walk + ttim;
					}
				
					//---- check the cumulative impedance ----

					imp = (int) (imped * len_factor + 0.5);
					if (imp < 1) imp = 1;

					if (flag && to_len > pen_len) {
						factor = (double) to_len / pen_len;
						factor = ((factor * factor) - 1) * pen_fac + 1;
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
					path_ptr->Mode (mode);
					path_ptr->From (from_node);
					path_ptr->Type (NODE_ID);
					path_ptr->Path (0);
					path_ptr->Dir (dir);
					path_ptr->Status (1);

					to_flag = true;

					if (best_flag) {
						max_imp = to_imp + imp_diff;
						best_to = to;
					}
				}

				//---- update the max imp ----

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
			}

			//---- check max distance ----

			to_len = from_len + link_ptr->Length ();

			if (flag && to_len >= max_len) {
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
			if (param.use == WALK) {
				to_walk = from_walk + ttime;
			}

			//---- check the cumulative impedance at B ----

			if (flag && to_len > pen_len) {
				factor = (double) to_len / pen_len;
				factor = ((factor * factor) - 1) * pen_fac + 1;
				imped = DTOI (imped * factor);
			}
			to_imp = from_imp + imped;

			if (to_imp + imp_diff >= max_imp) continue;

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
					path_ptr->Mode (mode);
					path_ptr->From (from_node);
					path_ptr->Type (NODE_ID);
					path_ptr->Path (0);
					path_ptr->Dir (0);
					path_ptr->Status (1);

					to_flag = true;

					if (best_flag) {
						max_imp = to_imp + imp_diff;
						best_to = to;
					}
				}

				//---- update the max imp ----

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
					continue;
				}
			}

			//---- check the node impedance ----

			path_ptr = &array_ptr->at (to_node);

			if (to_imp >= path_ptr->Imped ()) continue;

			//---- add the new path to the queue ----

			if (param.sort_method) {

				//---- remove the previous path from the queue ----

				if (path_ptr->Status () == 1) {
					for (imp_itr = imp_sort.find (path_ptr->Imped ()); imp_itr != imp_sort.end (); imp_itr++) {
						if (imp_itr->second == to_node) {
							imp_sort.erase (imp_itr);
							break;
						} else if (imp_itr->first > path_ptr->Imped ()) {
							break;
						}
					}
				}
				imp_sort.insert (Imp_Sort_Data (to_imp, to_node));
			} else {
				if (path_ptr->Status () == 0) {
					next_index.push_back (to_node);
				} else if (path_ptr->Status () == 2) {
					next_index.push_front (to_node);
				}
			}
			path_ptr->Imped (to_imp);
			path_ptr->Time (to_time);
			path_ptr->Length (to_len);
			path_ptr->Cost (to_cost);
			path_ptr->Walk (to_walk);
			path_ptr->Mode (mode);
			path_ptr->From (from_node);
			path_ptr->Type (NODE_ID);
			path_ptr->Path (0);
			path_ptr->Dir (0);
			path_ptr->Status (1);
		}
	}
	return (best_to);
}
