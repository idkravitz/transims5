//*********************************************************
//	Read_Plan.cpp - Read Each Plan File
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void ArcPlan::Read_Plan (void)
{
	Dtime time;
	int link, dir, index, distance, cost, imped, num_leg, acc, type, prev_stop;
	int org_field, start_field, des_field, ttime_field, distance_field;	
	int leg_mode_fld, leg_id_fld, leg_time_fld, leg_dist_fld, leg_cost_fld, leg_imp_fld;
	double offset, length, off, side, near_offset;
	bool select_flag;
		
	Plan_Data plan;
	Plan_Leg_Itr leg_itr, prev_itr;
	Select_Map_Itr sel_itr;
	Location_Data *loc_ptr;
	Parking_Data *parking_ptr;
	Location_Data *location_ptr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Driver_Itr driver_itr;
	Access_Data *access_ptr;

	XYZ_Point point;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr int_itr;
	Point_Map_Itr pt_itr;
	Trip_Index trip_index;

	org_field = arcview_access.Field_Number ("ORIGIN");
	start_field = arcview_access.Field_Number ("START");
	des_field = arcview_access.Field_Number ("LOCATION");
	ttime_field = arcview_access.Field_Number ("TTIME");
	distance_field = arcview_access.Field_Number ("DISTANCE");

	leg_mode_fld = arcview_plan.Field_Number ("LEG_MODE");
	leg_id_fld = arcview_plan.Field_Number ("LEG_ID");
	leg_time_fld = arcview_plan.Field_Number ("LEG_TIME");
	leg_dist_fld = arcview_plan.Field_Number ("LEG_LENGTH");
	leg_cost_fld = arcview_plan.Field_Number ("LEG_COST");
	leg_imp_fld = arcview_plan.Field_Number ("LEG_IMPED");
	
	near_offset = Internal_Units (10.0, FEET);	

	//---- set the filename and extension ----

	if (plan_file->Part_Flag ()) {
		Show_Message (String ("Reading %s %d -- Record") % plan_file->File_Type () % plan_file->Part_Number ());
	} else {
		Show_Message (String ("Reading %s -- Record") % plan_file->File_Type ());
	}
	Set_Progress ();

	//---- read each plan record ----

	select_flag = System_File_Flag (SELECTION);

	while (plan_file->Read_Plan (plan)) {
		Show_Progress ();

		//---- check the selection criteria ----

		if (select_households && !hhold_range.In_Range (plan.Household ())) continue;
		if (plan.Mode () < MAX_MODE && !select_mode [plan.Mode ()]) continue;
		if (select_purposes && !purpose_range.In_Range (plan.Purpose ())) continue;
		if (select_travelers && !traveler_range.In_Range (plan.Type ())) continue;
		if (select_start_times && !start_range.In_Range (plan.Start ())) continue;
		if (select_end_times && !end_range.In_Range (plan.End ())) continue;
		if (select_origins && !org_range.In_Range (plan.Origin ())) continue;
		if (select_destinations && !des_range.In_Range (plan.Destination ())) continue;
		
		if (select_org_zones) {
			loc_ptr = &location_array [plan.Origin ()];
			if (!org_zone_range.In_Range (loc_ptr->Zone ())) continue;
		}
		if (select_des_zones) {
			loc_ptr = &location_array [plan.Destination ()];
			if (!des_zone_range.In_Range (loc_ptr->Zone ())) continue;
		}

		//---- check the problem map ----

		if (problem_flag) {
			trip_index.Set (plan.Household (), plan.Person (), plan.Tour (), plan.Trip ());

			if (problem_map.find (trip_index) == problem_map.end ()) continue;
		}

		if (select_links && !Select_Plan_Links (plan)) continue;
		if (select_nodes && !Select_Plan_Nodes (plan)) continue;
		if (select_subarea && !Select_Plan_Subarea (plan)) continue;

		//---- check the selection records ----

		if (select_flag) {
			sel_itr = select_map.Best (plan.Household (), plan.Person (), 
				plan.Tour (), plan.Trip ());
			if (sel_itr == select_map.end ()) continue;
		}
		if (percent_flag && random.Probability () > select_percent) continue;

		//---- write accessibility data ----

		if (access_flag) {
			pt_itr = location_pt.find (plan.Destination ());
			if (pt_itr != location_pt.end ()) {
				arcview_access.Put_Field (org_field, plan.Origin ());
				arcview_access.Put_Field (start_field, plan.Start ());
				arcview_access.Put_Field (des_field, plan.Destination ());
				arcview_access.Put_Field (ttime_field, plan.Total_Time ());
				arcview_access.Put_Field (distance_field, UnRound (plan.Length ()));

				arcview_access.assign (1, pt_itr->second);

				if (!arcview_access.Write_Record ()) {
					Error (String ("Writing %s") % arcview_access.File_Type ());
				}
				num_access++;
			}
		}

		//---- store the data fields ----

		if (path_flag) {
			arcview_plan.clear ();
			arcview_plan.Copy_Fields (*plan_file);

			//---- process the path legs ----

			time = 0;
			distance = cost = imped = num_leg = 0;
			offset = length = -1.0;
			side = 0.0;

			for (leg_itr = prev_itr = plan.begin (); leg_itr != plan.end (); prev_itr = leg_itr++) {

				//---- first leg ----

				if (prev_itr == leg_itr) {

					//---- set the origin point and offset ----

					pt_itr = location_pt.find (plan.Origin ());
					arcview_plan.push_back (pt_itr->second);

					int_itr = location_map.find (plan.Origin ());

					if (int_itr != location_map.end ()) {
						location_ptr = &location_array [int_itr->second];

						if (leg_itr->Access_Type ()) {

							//---- find the end of the access link ----

							acc = leg_itr->Access_ID ();
							dir = leg_itr->Access_Dir ();

							int_itr = access_map.find (acc);

							if (int_itr != access_map.end ()) {
								access_ptr = &access_array [int_itr->second];

								if (dir == 1) {
									index = access_ptr->From_ID ();
									type = access_ptr->From_Type ();
								} else {
									index = access_ptr->To_ID ();
									type = access_ptr->To_Type ();
								}
								if (type == LOCATION_ID) {
									location_ptr = &location_array [index];
									pt_itr = location_pt.find (location_ptr->Location ());
								} else if (type == PARKING_ID) {
									parking_ptr = &parking_array [index];
									pt_itr = parking_pt.find (parking_ptr->Parking ());

									if (parking_ptr->Dir () == 0) {
										offset = UnRound (parking_ptr->Offset ());
									} else {
										link_ptr = &link_array [parking_ptr->Link ()];
										offset = UnRound (link_ptr->Length () - parking_ptr->Offset ());
									}
								} else if (type == STOP_ID) {
									stop_ptr = &stop_array [index];
									pt_itr = stop_pt.find (stop_ptr->Stop ());

									if (stop_ptr->Dir () == 0) {
										offset = UnRound (stop_ptr->Offset ());
									} else {
										link_ptr = &link_array [stop_ptr->Link ()];
										offset = UnRound (link_ptr->Length () - stop_ptr->Offset ());
									}
								} else {
									continue;
								}
								arcview_plan.push_back (pt_itr->second);
						
								arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
							}
						} else if (location_ptr->Dir () == 0) {
							offset = UnRound (location_ptr->Offset ());
						} else {
							link_ptr = &link_array [location_ptr->Link ()];
							offset = UnRound (link_ptr->Length () - location_ptr->Offset ());
						}
					}

				} else if (leg_itr->Type () == LOCATION_ID) {

					if (prev_itr->Mode () == WALK_MODE && prev_itr->Link_Type ()) {
						link = prev_itr->Link_ID ();
						dir = prev_itr->Link_Dir ();

						int_itr = link_map.find (link);

						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							int_itr = location_map.find (leg_itr->ID ());

							if (int_itr != location_map.end ()) {
								location_ptr = &location_array [int_itr->second];

								if (location_ptr->Dir () == dir) {
									length = UnRound (location_ptr->Offset ());
								} else {
									length = UnRound (link_ptr->Length () - location_ptr->Offset ());
								}
								if (offset > -1) length -= offset;

								Link_Shape (link_ptr, dir, points, offset, length, side);
								
								arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());

								pt_itr = location_pt.find (leg_itr->ID ());

								arcview_plan.push_back (pt_itr->second);

								arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
							}
						}
					} else {
						pt_itr = location_pt.find (leg_itr->ID ());
						arcview_plan.push_back (pt_itr->second);
					}

				} else if (leg_itr->Type () == PARKING_ID) {

					if (prev_itr->Mode () == DRIVE_MODE && prev_itr->Link_Type ()) {
						link = prev_itr->Link_ID ();
						dir = prev_itr->Link_Dir ();

						int_itr = link_map.find (link);
						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							int_itr = parking_map.find (leg_itr->ID ());
							if (int_itr != parking_map.end ()) {
								parking_ptr = &parking_array [int_itr->second];

								if (parking_ptr->Dir () == dir) {
									length = UnRound (parking_ptr->Offset ());
								} else {
									length = UnRound (link_ptr->Length () - parking_ptr->Offset ());
								}
								if (offset > -1) length -= offset;

								Link_Shape (link_ptr, dir, points, offset, length, side);
								
								arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());

								pt_itr = parking_pt.find (leg_itr->ID ());

								arcview_plan.push_back (pt_itr->second);

								//---- add the parking attributes to the drive leg ----

								num_leg++;
								time += leg_itr->Time ();
								distance += leg_itr->Length ();
								cost += leg_itr->Cost ();
								imped += leg_itr->Impedance ();

								//---- write the shape record ----

								arcview_plan.Put_Field (leg_mode_fld, DRIVE_MODE);
								arcview_plan.Put_Field (leg_id_fld, num_leg);
								arcview_plan.Put_Field (leg_time_fld, time);
								arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
								arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
								arcview_plan.Put_Field (leg_imp_fld, imped);

								if (!arcview_plan.Write_Record ()) {
									Error (String ("Writing %s") % arcview_plan.File_Type ());
								}
								num_out++;
								arcview_plan.clear ();
								time = 0;
								distance = cost = imped = num_leg = 0;

								arcview_plan.push_back (pt_itr->second);

								arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);

								if (parking_ptr->Dir () == 0) {
									offset = UnRound (parking_ptr->Offset ());
								} else {
									offset = UnRound (link_ptr->Length () - parking_ptr->Offset ());
								}
							}
						}
						continue;

					} else if (prev_itr->Mode () == WALK_MODE && prev_itr->Link_Type ()) {
						link = prev_itr->Link_ID ();
						dir = prev_itr->Link_Dir ();

						int_itr = link_map.find (link);

						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							int_itr = parking_map.find (leg_itr->ID ());

							if (int_itr != parking_map.end ()) {
								parking_ptr = &parking_array [int_itr->second];

								if (parking_ptr->Dir () == dir) {
									length = UnRound (parking_ptr->Offset ());
								} else {
									length = UnRound (link_ptr->Length () - parking_ptr->Offset ());
								}
								if (offset > -1) length -= offset;
								Link_Shape (link_ptr, dir, points, offset, length, side);
								
								arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());

								pt_itr = parking_pt.find (leg_itr->ID ());

								arcview_plan.push_back (pt_itr->second);

								//---- write the shape record ----

								arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
								arcview_plan.Put_Field (leg_id_fld, num_leg);
								arcview_plan.Put_Field (leg_time_fld, time);
								arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
								arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
								arcview_plan.Put_Field (leg_imp_fld, imped);

								if (!arcview_plan.Write_Record ()) {
									Error (String ("Writing %s") % arcview_plan.File_Type ());
								}
								num_out++;

								//---- start a new leg ---

								arcview_plan.clear ();
								time = 0;
								distance = cost = imped = num_leg = 0;

								arcview_plan.push_back (pt_itr->second);

								if (parking_ptr->Dir () == 0) {
									offset = UnRound (parking_ptr->Offset ());
								} else {
									offset = UnRound (link_ptr->Length () - parking_ptr->Offset ());
								}
							}
						}
					} else if (prev_itr->Type () == LOCATION_ID) {
						pt_itr = parking_pt.find (leg_itr->ID ());
						arcview_plan.push_back (pt_itr->second);

						//---- write the shape record ----

						arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
						arcview_plan.Put_Field (leg_id_fld, num_leg);
						arcview_plan.Put_Field (leg_time_fld, time);
						arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
						arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
						arcview_plan.Put_Field (leg_imp_fld, imped);

						if (!arcview_plan.Write_Record ()) {
							Error (String ("Writing %s") % arcview_plan.File_Type ());
						}
						num_out++;

						//---- start a new leg ---

						arcview_plan.clear ();
						time = 0;
						distance = cost = imped = num_leg = 0;

						arcview_plan.push_back (pt_itr->second);

						int_itr = parking_map.find (leg_itr->ID ());

						if (int_itr != parking_map.end ()) {
							parking_ptr = &parking_array [int_itr->second];

							if (parking_ptr->Dir () == 0) {
								offset = UnRound (parking_ptr->Offset ());
							} else {
								link_ptr = &link_array [parking_ptr->Link ()];
								offset = UnRound (link_ptr->Length () - parking_ptr->Offset ());
							}
						}
					}

				} else if (leg_itr->Mode () == DRIVE_MODE) {

					if (prev_itr->Mode () == DRIVE_MODE) {
						length = -1;
						link = prev_itr->ID ();
						if (link < 0) {
							link = -link;
							dir = 1;
						} else {
							dir = 0;
						}
						int_itr = link_map.find (link);
						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							Link_Shape (link_ptr, dir, points, offset, length, side);

							arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());
						}
						offset = -1.0;
					}

				} else if (leg_itr->Mode () == WALK_MODE && leg_itr->Link_Type ()) {

					if (prev_itr->Mode () == WALK_MODE && prev_itr->Link_Type ()) {
						link = prev_itr->Link_ID ();
						dir = prev_itr->Link_Dir ();
						length = -1;

						int_itr = link_map.find (link);
						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							Link_Shape (link_ptr, dir, points, offset, length, side);

							arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());
						}
						offset = -1.0;
					}

				} else if (leg_itr->Type () == STOP_ID) {

					if (leg_itr->Mode () == TRANSIT_MODE) {

						//---- trace the route ----

						int_itr = line_map.find (prev_itr->ID ());
						if (int_itr != line_map.end ()) {
							line_ptr = &line_array [int_itr->second];

							//---- find the boarding link ----

							int_itr = stop_map.find (prev_stop);
							if (int_itr != stop_map.end ()) {
								stop_ptr = &stop_array [int_itr->second];

								link_ptr = &link_array [stop_ptr->Link ()];

								if (stop_ptr->Dir ()) {
									index = link_ptr->BA_Dir ();
									offset = UnRound (link_ptr->Length () - stop_ptr->Offset ());
								} else {
									index = link_ptr->AB_Dir ();
									offset = UnRound (stop_ptr->Offset ());
								}
								for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
									if (*driver_itr == index) break;
								}

								//---- continue to the alighting link ----
							
								int_itr = stop_map.find (leg_itr->ID ());
								if (int_itr != stop_map.end ()) {
									stop_ptr = &stop_array [int_itr->second];

									link_ptr = &link_array [stop_ptr->Link ()];

									if (stop_ptr->Dir ()) {
										index = link_ptr->BA_Dir ();
									} else {
										index = link_ptr->AB_Dir ();
									}
									for (; driver_itr != line_ptr->driver_array.end (); driver_itr++) {
										dir_ptr = &dir_array [*driver_itr];
										link_ptr = &link_array [dir_ptr->Link ()];

										if (*driver_itr == index) {
											length = UnRound (stop_ptr->Offset ());
										} else {
											length = UnRound (link_ptr->Length ());
										}
										if (offset > -1) length -= offset;

										if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
											side = link_offset;
										} else {
											side = 0.0;
										}
										Link_Shape (link_ptr, dir_ptr->Dir (), points, offset, length, side);

										arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());
										offset = -1;
										if (*driver_itr == index) break;
									}
								}
							}
						}

						//---- add the stop point and save the leg ----

						pt_itr = stop_pt.find (leg_itr->ID ());
						arcview_plan.push_back (pt_itr->second);

						num_leg++;
						time += leg_itr->Time ();
						distance += leg_itr->Length ();
						cost += leg_itr->Cost ();
						imped += leg_itr->Impedance ();

						//---- write the shape record ----

						arcview_plan.Put_Field (leg_mode_fld, TRANSIT_MODE);
						arcview_plan.Put_Field (leg_id_fld, num_leg);
						arcview_plan.Put_Field (leg_time_fld, time);
						arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
						arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
						arcview_plan.Put_Field (leg_imp_fld, imped);

						if (!arcview_plan.Write_Record ()) {
							Error (String ("Writing %s") % arcview_plan.File_Type ());
						}
						num_out++;

						//---- start a new leg ---

						arcview_plan.clear ();
						time = 0;
						distance = cost = imped = num_leg = 0;

						arcview_plan.push_back (pt_itr->second);

						int_itr = stop_map.find (leg_itr->ID ());

						if (int_itr != stop_map.end ()) {
							stop_ptr = &stop_array [int_itr->second];

							if (stop_ptr->Dir () == 0) {
								offset = UnRound (stop_ptr->Offset ());
							} else {
								link_ptr = &link_array [stop_ptr->Link ()];
								offset = UnRound (link_ptr->Length () - stop_ptr->Offset ());
							}
						}
						continue;

					} else if (leg_itr->Mode () == WALK_MODE) {

						prev_stop = leg_itr->ID ();

						int_itr = stop_map.find (prev_stop);

						if (int_itr != stop_map.end ()) {
							stop_ptr = &stop_array [int_itr->second];
							link_ptr = &link_array [stop_ptr->Link ()];

							if (stop_ptr->Dir () == 0) {
								off = length = UnRound (stop_ptr->Offset ());
							} else {
								off = length = UnRound (link_ptr->Length () - stop_ptr->Offset ());
							}
							if (abs ((int) (length - offset)) > near_offset) {
								if (prev_itr->Type () == LOCATION_ID) {
									if (length > offset) {
										dir = 0;
									} else {
										dir = 1;
									}
								} else if (prev_itr->Link_Type ()) {
									dir = prev_itr->Link_Dir ();
								} else {
									dir = stop_ptr->Dir ();
								}
								if (dir == 1) {
									length = UnRound (link_ptr->Length ()) - off;
									if (offset > -1) {
										offset = UnRound (link_ptr->Length ()) - offset;
									}
								}
								if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
									side = link_offset;
								} else {
									side = 0.0;
								}
								if (offset > -1) length -= offset;
								Link_Shape (link_ptr, dir, points, offset, length, side);

								arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());
							}
							offset = off;
						}

						//---- add the stop point and save the leg ----

						pt_itr = stop_pt.find (prev_stop);
						arcview_plan.push_back (pt_itr->second);

						num_leg++;
						time += leg_itr->Time ();
						distance += leg_itr->Length ();
						cost += leg_itr->Cost ();
						imped += leg_itr->Impedance ();

						//---- write the shape record ----

						arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
						arcview_plan.Put_Field (leg_id_fld, num_leg);
						arcview_plan.Put_Field (leg_time_fld, time);
						arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
						arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
						arcview_plan.Put_Field (leg_imp_fld, imped);

						if (!arcview_plan.Write_Record ()) {
							Error (String ("Writing %s") % arcview_plan.File_Type ());
						}
						num_out++;

						//---- start a new leg ---

						arcview_plan.clear ();
						time = 0;
						distance = cost = imped = num_leg = 0;

						arcview_plan.push_back (pt_itr->second);
						continue;
					}

				} else if (leg_itr->Access_Type ()) {

					if (prev_itr->Link_Type ()) {
						link = prev_itr->Link_ID ();
						dir = prev_itr->Link_Dir ();

						int_itr = link_map.find (link);

						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];

							if (dir == 1 && offset > -1) {
								offset = UnRound (link_ptr->Length ()) - offset;
							}
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = link_offset;
							} else {
								side = 0.0;
							}
							length = UnRound (link_ptr->Length ());
							if (offset > -1) length -= offset;

							Link_Shape (link_ptr, dir, points, offset, length, side);
								
							arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());

							//---- find the end of the access link ----

							acc = leg_itr->ID ();
							if (acc < 0) {
								acc = -acc;
								dir = 1;
							} else {
								dir = 0;
							}
							int_itr = access_map.find (acc);

							if (int_itr != access_map.end ()) {
								access_ptr = &access_array [int_itr->second];

								if (dir == 1) {
									index = access_ptr->From_ID ();
									type = access_ptr->From_Type ();
								} else {
									index = access_ptr->To_ID ();
									type = access_ptr->To_Type ();
								}
								if (type == LOCATION_ID) {
									location_ptr = &location_array [index];
									pt_itr = location_pt.find (location_ptr->Location ());
								} else if (type == PARKING_ID) {
									parking_ptr = &parking_array [index];
									pt_itr = parking_pt.find (parking_ptr->Parking ());
								} else if (type == STOP_ID) {
									stop_ptr = &stop_array [index];
									pt_itr = stop_pt.find (stop_ptr->Stop ());
								} else {
									continue;
								}
								arcview_plan.push_back (pt_itr->second);
							}
						}
					}
				}
				num_leg++;
				time += leg_itr->Time ();
				distance += leg_itr->Length ();
				cost += leg_itr->Cost ();
				imped += leg_itr->Impedance ();
			}

			//---- complete the path to the destination ----

			if (prev_itr->Type () != LOCATION_ID) {
				if (prev_itr->Link_Type ()) {
					link = prev_itr->Link_ID ();
					dir = prev_itr->Link_Dir ();

					int_itr = link_map.find (link);

					if (int_itr != link_map.end ()) {
						link_ptr = &link_array [int_itr->second];

						if (dir == 1 && offset > -1) {
							offset = UnRound (link_ptr->Length ()) - offset;
						}
						if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
							side = link_offset;
						} else {
							side = 0.0;
						}
						int_itr = location_map.find (plan.Destination ());

						if (int_itr != location_map.end ()) {
							location_ptr = &location_array [int_itr->second];

							if (location_ptr->Dir () == dir) {
								length = UnRound (location_ptr->Offset ());
							} else {
								length = UnRound (link_ptr->Length () - location_ptr->Offset ());
							}
							if (offset > -1) length -= offset;
							Link_Shape (link_ptr, dir, points, offset, length, side);
								
							arcview_plan.insert (arcview_plan.end (), points.begin (), points.end ());

							pt_itr = location_pt.find (plan.Destination ());

							arcview_plan.push_back (pt_itr->second);

							arcview_plan.Put_Field (leg_mode_fld, WALK_MODE);
						}
					}
				}
			}

			//---- write the last leg ----

			if (arcview_plan.size () > 0) {
				arcview_plan.Put_Field (leg_id_fld, num_leg);
				arcview_plan.Put_Field (leg_time_fld, time);
				arcview_plan.Put_Field (leg_dist_fld, UnRound (distance));
				arcview_plan.Put_Field (leg_cost_fld, UnRound (cost));
				arcview_plan.Put_Field (leg_imp_fld, imped);

				if (!arcview_plan.Write_Record ()) {
					Error (String ("Writing %s") % arcview_plan.File_Type ());
				}
				num_out++;
			}
		}

		//---- summarized path data ----

		if (width_flag || time_flag || distance_flag || parking_flag || rider_flag || on_off_flag) {
			for (leg_itr = prev_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
				if (width_flag && leg_itr->Mode () == DRIVE_MODE) {
					if (leg_itr->Link_Type ()) {
						link = leg_itr->Link_ID ();
						dir = leg_itr->Link_Dir ();

						int_itr = link_map.find (link);
						if (int_itr != link_map.end ()) {
							link_ptr = &link_array [int_itr->second];
							if (dir) {
								index = link_ptr->BA_Dir ();
							} else {
								index = link_ptr->AB_Dir ();
							}
							width_data [index]++;
						}
					}
				} else if (parking_flag && leg_itr->Type () == PARKING_ID) {
					int_itr = parking_map.find (leg_itr->ID ());
					if (int_itr != parking_map.end ()) {
						if (prev_itr->Mode () != DRIVE_MODE) {
							parking_out [int_itr->second]++;
						} else {
							parking_in [int_itr->second]++;
						}
					}
				} else if ((rider_flag || on_off_flag) && leg_itr->Type () == STOP_ID) {
					int_itr = stop_map.find (leg_itr->ID ());
					if (int_itr != stop_map.end ()) {
						if (prev_itr->Type () != ROUTE_ID) {
							alight_data [int_itr->second]++;
						} else {
							board_data [int_itr->second]++;
						}
						if (rider_flag) {
							//---- driver path ----
							//load_data [index]++;
						}
					}
				} else if (time_flag || distance_flag) {
					//	Write_Contour (org_ptr);
				}
				prev_itr = leg_itr;
			}
		}
	}
	End_Progress ();

	plan_file->Close ();
}
