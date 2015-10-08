//*********************************************************
//	Travel_Plans.cpp - convert the plan files
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Travel_Plans 
//---------------------------------------------------------

void NewFormat::Plan_Processing::Travel_Plans (int part)
{
	int i, current_traveler, current_trip, current_origin, park_id, link, dir, node, distance, length;
	int traveler, trip, origin, tour, mode, time, tim, type, id, num, *path, *leg, imp, imped, walk_fac;
	bool drive_flag, first;
	double factor, imped_fac, time_fac;

	//enum Plan_Mode {AUTO_MODE, TRANSIT_MODE, WALK_MODE, BIKE_MODE, ACTIVITY_MODE, OTHER_MODE, MAGIC_MODE, CARPOOL_MODE, DRIVER_MODE};
	//enum Mode_Type {NO_MODE, ALL_WALK, DRIVE_ALONE, TRANSIT, PASSENGER, PNR_OUT, PNR_IN, 
	//	BICYCLE, MAGIC_MOVE, SCHOOL_BUS, CARPOOL2, CARPOOL3, CARPOOL4, MAX_MODE};

	//enum Mode_Type {NO_MODE, WALK_MODE, BIKE_MODE, DRIVE_MODE, RIDE_MODE, 
	//	TRANSIT_MODE, PNR_OUT_MODE, PNR_IN_MODE, KNR_OUT_MODE, KNR_IN_MODE,
	//	TAXI_MODE, OTHER_MODE, HOV2_MODE, HOV3_MODE, HOV4_MODE, MAX_MODE};

	int mode_map [] = { DRIVE_MODE, TRANSIT_MODE, WALK_MODE, BIKE_MODE, WAIT_MODE, OTHER_MODE, OTHER_MODE, HOV3_MODE, WAIT_MODE };

	Int_Map_Itr map_itr;
	Plan_Data plan_rec;
	Plan_Leg leg_rec;
	Parking_Data *parking_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int2_Map_Itr ab_itr;

	//---- open the file partition ----

	if (!old_plan.Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % old_plan.Filename ()); 
		END_LOCK
	}
	new_plan.Open (part);

	//---- process the plan files ----

	if (thread_flag) {
		MAIN_LOCK
		if (old_plan.Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d") % old_plan.File_Type () % old_plan.Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s") % old_plan.File_Type ());
		}
		END_LOCK
	} else {
		if (old_plan.Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d -- Plan") % old_plan.File_Type () % old_plan.Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s -- Plan") % old_plan.File_Type ());
		}
		exe->Set_Progress ();
	}

	current_traveler = current_trip = current_origin = tour = length = 0;
	first = true;
	walk_fac = (Metric_Flag ()) ? 1 : 3;

	while (old_plan.Read ()) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}
		mode = mode_map [old_plan.Mode ()];
		if (mode == WAIT_MODE) continue;
		drive_flag = (mode == DRIVE_MODE || mode == HOV3_MODE);

		traveler = old_plan.Traveler ();
		trip = old_plan.Trip ();
		origin = old_plan.Start_ID ();

		if (current_traveler != traveler || current_trip != trip) {
			if (current_traveler > 0) {
				plan_rec.Length (length);

				if (!new_plan.Write_Plan (plan_rec)) {
					MAIN_LOCK
					exe->Error (String ("Writing %s") % new_plan.File_Type ());
					END_LOCK
				}
			}
			if (current_traveler != traveler) {
				current_traveler = traveler;
				current_origin = origin;
				tour = 1;
			} else if (current_origin == origin) {
				tour++;
			}
			current_trip = trip;
			length = 0;

			plan_rec.Clear ();
			plan_rec.Household (old_plan.Household ());
			plan_rec.Person (old_plan.Person ());
			plan_rec.Tour (tour);
			plan_rec.Trip (trip);
			plan_rec.Mode (mode);
			plan_rec.Start (old_plan.Time ());
			plan_rec.Depart (old_plan.Time ());
			plan_rec.Origin (origin);
			first = true;
		}
		plan_rec.End (old_plan.Stop_Time ());
		plan_rec.Arrive (old_plan.Stop_Time ());
		plan_rec.Destination (old_plan.End_ID ());

		time = old_plan.Duration ();
		type = old_plan.End_Type ();
		id = old_plan.End_ID ();
		imped = old_plan.GCF ();

		//---- save the first leg ----

		if (first) {
			length = time * walk_fac;

			leg_rec.Mode (mode);
			leg_rec.Type (old_plan.Start_Type ());
			leg_rec.ID (old_plan.Start_ID ());
			leg_rec.Time (time);
			leg_rec.Length (length);
			leg_rec.Cost (0);
			leg_rec.Impedance (imped);

			plan_rec.push_back (leg_rec);
			first = false;

			plan_rec.Add_Walk (time);
			mode = OTHER_MODE;
			time = 0;
			imped = 0;
		}

		//---- save the path ----

		path = old_plan.Path (&num);

		if (num > 0) {

			tim = time / (num + 1);
			imp = imped / (num + 1);

			if (!old_plan.Node_Based_Flag ()) {		//---- link-based path ----

				//---- accumulate the path time and distance ----

				tim = distance = 0;

				for (i=1, leg=path; i <= num; i++, leg++) {
					link = abs (*leg);
					map_itr = exe->link_map.find (link);

					if (map_itr == exe->link_map.end ()) {
						MAIN_LOCK
						exe->Warning (String ("Plan Link %d was Not Found") % link);
						END_LOCK
						break;
					}
					link_ptr = &exe->link_array [map_itr->second];
					dir = (*leg < 0) ? 1 : 0;

					if (i == 1) {
						if (drive_flag) {
							park_id = old_plan.Start_ID ();
							map_itr = exe->parking_map.find (park_id);

							if (map_itr == exe->parking_map.end ()) {
								MAIN_LOCK
								exe->Warning (String ("Parking Lot %d was Not Found") % park_id);
								END_LOCK
								break;
							}
							parking_ptr = &exe->parking_array [map_itr->second];
							if (dir == parking_ptr->Dir ()) {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							} else {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							}
						} else {
							factor = 0.5;
						}
					} else if (i == num) {
						if (drive_flag) {
							park_id = old_plan.End_ID ();
							map_itr = exe->parking_map.find (park_id);

							if (map_itr == exe->parking_map.end ()) {
								MAIN_LOCK
								exe->Warning (String ("Parking Lot %d was Not Found") % park_id);
								END_LOCK
								break;
							}
							parking_ptr = &exe->parking_array [map_itr->second];
							if (dir == parking_ptr->Dir ()) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						} else {
							factor = 0.5;
						}
					} else {
						factor = 1.0;
					}
					distance += DTOI (link_ptr->Length () * factor);

					if (drive_flag) {
						if (dir == 1) {
							dir = link_ptr->BA_Dir ();
						} else {
							dir = link_ptr->AB_Dir ();
						}
						dir_ptr = &exe->dir_array [dir];
						tim += DTOI (dir_ptr->Time0 () * factor);
					}
				}
				if (i <= num) continue;

				//---- calculate the time and impedance factors ----

				if (time > 0) {
					imped_fac = (double) imped / time;
				} else {
					imped_fac = imped / (num + 1);
				}
				if (drive_flag) {
					if (tim > 0) {
						time_fac = (double) time / tim;
					} else {
						time_fac = time / (num + 1);
					}
				} else {
					if (distance > 0) {
						time_fac = (double) time / (distance * walk_fac);
					} else {
						time_fac = time / (num + 1);
					}
				}

				//---- apply the factors to each leg ----

				for (i=1, leg=path; i <= num; i++, leg++) {
					link = abs (*leg);
					map_itr = exe->link_map.find (link);
					link_ptr = &exe->link_array [map_itr->second];
					dir = (*leg < 0) ? 1 : 0;

					if (i == 1) {
						if (drive_flag) {
							park_id = old_plan.Start_ID ();
							map_itr = exe->parking_map.find (park_id);
							parking_ptr = &exe->parking_array [map_itr->second];
							if (dir == parking_ptr->Dir ()) {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							} else {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							}
						} else {
							factor = 0.5;
						}
					} else if (i == num) {
						if (drive_flag) {
							park_id = old_plan.End_ID ();
							map_itr = exe->parking_map.find (park_id);
							parking_ptr = &exe->parking_array [map_itr->second];
							if (dir == parking_ptr->Dir ()) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						} else {
							factor = 0.5;
						}
					} else {
						factor = 1.0;
					}
					distance = DTOI (link_ptr->Length () * factor);

					if (drive_flag) {
						if (dir == 1) {
							dir = link_ptr->BA_Dir ();
						} else {
							dir = link_ptr->AB_Dir ();
						}
						dir_ptr = &exe->dir_array [dir];
						tim = DTOI (dir_ptr->Time0 () * time_fac * factor);
					} else {
						tim = DTOI (distance * walk_fac * time_fac);
					}
					imp = DTOI (tim * imped_fac);

					leg_rec.Mode (mode);
					leg_rec.Type (LINK_ID);
					leg_rec.ID (*leg);
					leg_rec.Time (tim);
					leg_rec.Length (distance);
					leg_rec.Cost (0);
					leg_rec.Impedance (imp);
					
					length += distance;
					plan_rec.push_back (leg_rec);
				}

			} else if (!drive_flag) {		//---- node-based walk path ----

				//---- apply the factors to each leg ----

				for (i=0, leg=path; i < num; i++, leg++) {
					leg_rec.Mode (mode);
					leg_rec.Type (NODE_ID);
					leg_rec.ID (*leg);
					leg_rec.Time (tim);
					leg_rec.Length (tim * walk_fac);
					leg_rec.Cost (0);
					leg_rec.Impedance (imp);
					
					length += tim * walk_fac;
					plan_rec.push_back (leg_rec);
				}

			} else {	//---- node-based drive path ----

				if (old_plan.Start_Type () != PARKING_ID) {
					MAIN_LOCK
					exe->Warning ("Drive Path does not Start from a Parking Lot");
					END_LOCK
					continue;
				}

				park_id = old_plan.Start_ID ();
				map_itr = exe->parking_map.find (park_id);

				if (map_itr == exe->parking_map.end ()) {
					MAIN_LOCK
					exe->Warning (String ("Parking Lot %d was Not Found") % park_id);
					END_LOCK
					continue;
				}

				//---- accumulate path time ----

				tim = 0;

				parking_ptr = &exe->parking_array [map_itr->second];
				link_ptr = &exe->link_array [parking_ptr->Link ()];
				link = link_ptr->Link ();
				node = 0;

				for (i=0, leg=path; i <= num; i++, leg++) {
					if (i == num) {
						if (old_plan.End_Type () != PARKING_ID) {
							MAIN_LOCK
							exe->Warning ("Drive Path does not End at a Parking Lot");
							END_LOCK
							continue;
						}
						park_id = old_plan.End_ID ();
						map_itr = exe->parking_map.find (park_id);

						if (map_itr == exe->parking_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("Parking Lot %d was Not Found") % park_id);
							END_LOCK
							continue;
						}
						parking_ptr = &exe->parking_array [map_itr->second];
						link_ptr = &exe->link_array [parking_ptr->Link ()];
						link = link_ptr->Link ();

						if (node > 0) {
							map_itr = exe->node_map.find (node);
							if (map_itr == exe->node_map.end ()) {
								MAIN_LOCK
								exe->Warning (String ("Node %d was Not Found") % node);
								END_LOCK
								continue;
							}
							if (link_ptr->Bnode () == map_itr->second) {
								dir = link_ptr->BA_Dir ();
								if (parking_ptr->Dir () == 1) {
									factor = (double) parking_ptr->Offset () / link_ptr->Length ();
								} else {
									factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
								}
							} else {
								dir = link_ptr->AB_Dir ();
								if (parking_ptr->Dir () == 0) {
									factor = (double) parking_ptr->Offset () / link_ptr->Length ();
								} else {
									factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
								}
							}
							if (dir < 0) {
								MAIN_LOCK
								exe->Warning (String ("Link %d Direction was Not Found") % link);
								END_LOCK
								continue;
							}
							dir_ptr = &exe->dir_array [dir];
							tim += DTOI (dir_ptr->Time0 () * factor);
						}
					} else if (i == 0) {
						map_itr = exe->node_map.find (*leg);
						if (map_itr == exe->node_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("Node %d was Not Found") % *leg);
							END_LOCK
							continue;
						}
						if (link_ptr->Anode () == map_itr->second) {
							dir = link_ptr->BA_Dir ();
							if (parking_ptr->Dir () == 0) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						} else {
							dir = link_ptr->AB_Dir ();
							if (parking_ptr->Dir () == 1) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						}
						if (dir < 0) {
							MAIN_LOCK
							exe->Warning (String ("Link %d Direction was Not Found") % link);
							END_LOCK
							continue;
						}
						dir_ptr = &exe->dir_array [dir];
						tim += DTOI (dir_ptr->Time0 () * factor);
					} else {
						ab_itr = exe->ab_map.find (Int2_Key (node, *leg));
						if (ab_itr == exe->ab_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("A Link between Nodes %d and %d was Not Found") % node % *leg);
							END_LOCK
							continue;
						}
						dir_ptr = &exe->dir_array [ab_itr->second];
						link_ptr = &exe->link_array [dir_ptr->Link ()];
						link = link_ptr->Link ();
						tim += dir_ptr->Time0 ();
					}
					node = *leg;
				}

				//---- calculate the time and impedance factors ----

				if (time > 0) {
					imped_fac = (double) imped / time;
				} else {
					imped_fac = imped / (num + 1);
				}
				if (tim > 0) {
					time_fac = (double) time / tim;
				} else {
					time_fac = time / (num + 1);
				}

				//---- apply the adjustment factors ----

				park_id = old_plan.Start_ID ();
				map_itr = exe->parking_map.find (park_id);

				parking_ptr = &exe->parking_array [map_itr->second];
				link_ptr = &exe->link_array [parking_ptr->Link ()];
				link = link_ptr->Link ();
				distance = link_ptr->Length () / 2;
				node = 0;

				for (i=0, leg=path; i <= num; i++, leg++) {
					if (i == num) {
						if (old_plan.End_Type () != PARKING_ID) {
							MAIN_LOCK
							exe->Warning ("Drive Path does not End at a Parking Lot");
							END_LOCK
							continue;
						}
						park_id = old_plan.End_ID ();
						map_itr = exe->parking_map.find (park_id);

						if (map_itr == exe->parking_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("Parking Lot %d was Not Found") % park_id);
							END_LOCK
							continue;
						}
						parking_ptr = &exe->parking_array [map_itr->second];
						link_ptr = &exe->link_array [parking_ptr->Link ()];
						link = link_ptr->Link ();

						if (node > 0) {
							map_itr = exe->node_map.find (node);
							if (map_itr == exe->node_map.end ()) {
								MAIN_LOCK
								exe->Warning (String ("Node %d was Not Found") % node);
								END_LOCK
								continue;
							}
							if (link_ptr->Bnode () == map_itr->second) {
								dir = link_ptr->BA_Dir ();
								link = -link;
								if (parking_ptr->Dir () == 1) {
									factor = (double) parking_ptr->Offset () / link_ptr->Length ();
								} else {
									factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
								}
							} else {
								dir = link_ptr->AB_Dir ();
								if (parking_ptr->Dir () == 0) {
									factor = (double) parking_ptr->Offset () / link_ptr->Length ();
								} else {
									factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
								}
							}
							if (dir < 0) {
								MAIN_LOCK
								exe->Warning (String ("Link %d Direction was Not Found") % link);
								END_LOCK
								continue;
							}
							dir_ptr = &exe->dir_array [dir];
							tim = DTOI (dir_ptr->Time0 () * factor);
							distance = DTOI (link_ptr->Length () * factor);
						}
					} else if (i == 0) {
						map_itr = exe->node_map.find (*leg);
						if (map_itr == exe->node_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("Node %d was Not Found") % *leg);
							END_LOCK
							continue;
						}
						if (link_ptr->Anode () == map_itr->second) {
							dir = link_ptr->BA_Dir ();
							link = -link;
							if (parking_ptr->Dir () == 0) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						} else {
							dir = link_ptr->AB_Dir ();
							if (parking_ptr->Dir () == 1) {
								factor = (double) parking_ptr->Offset () / link_ptr->Length ();
							} else {
								factor = (double) (link_ptr->Length () - parking_ptr->Offset ()) / link_ptr->Length ();
							}
						}
						if (dir < 0) {
							MAIN_LOCK
							exe->Warning (String ("Link %d Direction was Not Found") % link);
							END_LOCK
							continue;
						}
						dir_ptr = &exe->dir_array [dir];
						tim = DTOI (dir_ptr->Time0 () * time_fac * factor);
						distance = DTOI (link_ptr->Length () * factor);
					} else {
						ab_itr = exe->ab_map.find (Int2_Key (node, *leg));
						if (ab_itr == exe->ab_map.end ()) {
							MAIN_LOCK
							exe->Warning (String ("A Link between Nodes %d and %d was Not Found") % node % *leg);
							END_LOCK
							continue;
						}
						dir_ptr = &exe->dir_array [ab_itr->second];
						link_ptr = &exe->link_array [dir_ptr->Link ()];
						link = link_ptr->Link ();
						distance = link_ptr->Length ();
						if (dir_ptr->Dir () == 1) link = -link;
						tim = DTOI (dir_ptr->Time0 () * time_fac);
					}
					imp = DTOI (tim * imped_fac);

					leg_rec.Mode (mode);
					leg_rec.Type (LINK_ID);
					leg_rec.ID (link);
					leg_rec.Time (tim);
					leg_rec.Length (distance);
					leg_rec.Cost (0);
					leg_rec.Impedance (imp);

					length += distance;
					node = *leg;

					plan_rec.push_back (leg_rec);
				}
			}
			mode = OTHER_MODE;
			tim = 0;
			imp = 0;
			distance = 0;
		} else {
			tim = time;
			imp = imped;
			distance = time * walk_fac;
		}

		//---- update the skims ----

		if (mode == WALK_MODE) {
			plan_rec.Add_Walk (time);
		} else if (drive_flag) {
			plan_rec.Add_Drive (time);
			plan_rec.Vehicle (exe->Fix_Vehicle_ID (old_plan.Vehicle ()));

			if (plan_rec.Mode () == WALK_MODE) {
				if (old_plan.Driver_Flag () == 1) {
					plan_rec.Mode (mode_map [old_plan.Mode ()]);
				} else {
					plan_rec.Mode (RIDE_MODE);
				}
			} else if (plan_rec.Mode () == TRANSIT_MODE) {
				plan_rec.Mode (PNR_IN_MODE);
			}
		} else if (mode == TRANSIT_MODE) {
			//---- approximate wait time ----
			time = tim / 10;
			tim -= time;
			plan_rec.Add_Transit (tim);
			plan_rec.Add_Wait (time);

			if (plan_rec.Mode () == WALK_MODE) {
				plan_rec.Mode (mode);
			} else if (plan_rec.Mode () == DRIVE_MODE) {
				plan_rec.Mode (PNR_OUT_MODE);
			}
			leg_rec.Mode (WAIT_MODE);
			leg_rec.Type (ROUTE_ID);
			leg_rec.ID (old_plan.Route ());
			leg_rec.Time (time);
			leg_rec.Length (0);
			leg_rec.Cost (0);
			leg_rec.Impedance (imped);

			plan_rec.push_back (leg_rec);
		} else {
			plan_rec.Add_Other (time);
		}
		plan_rec.Add_Impedance (old_plan.GCF ());

		//---- save the end leg ----

		leg_rec.Mode (mode);
		leg_rec.Type (type);
		leg_rec.ID (id);
		leg_rec.Time (tim);
		leg_rec.Length (distance);
		leg_rec.Cost (0);
		leg_rec.Impedance (imp);

		length += distance;
		plan_rec.push_back (leg_rec);
	}
	if (current_traveler > 0) {
		plan_rec.Length (length);

		if (!new_plan.Write_Plan (plan_rec)) {
			MAIN_LOCK
			exe->Error (String ("Writing %s") % new_plan.File_Type ());
			END_LOCK
		}
	}
	if (!thread_flag) exe->End_Progress ();
}
