//*********************************************************
//	Write_UTDF.cpp - write a utdf network file
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Write_UTDF
//---------------------------------------------------------

void ExportNet::Write_UTDF (void)
{
	int next, type, up, down, node, dir, num, control, *first_ptr, *list_ptr;
	int turn_type_map [UTURN+1], low [4], high [4], share [4], turns [8][4], phasing, phase, x, y, z;

	Int_Map_Itr map_itr, in_itr;
	Node_Data *node_ptr;
	Link_Itr link_itr;
	Link_Data *link_ptr, *links [8];
	Dir_Data *dir_ptr, *dirs [8];
	Connect_Data *con_ptr;
	Pocket_Data *pocket_ptr;
	Signal_Data *signal_ptr;
	Signal_Time_Itr time_itr;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;
	Data_Range_Itr range_itr;

	Integers node_list, node_link;
	Int_Map in_out_map, out_in_map;
	Int_Map_Stat map_stat;

	compass.Set_Points (8);

	Show_Message ("Writing ") << utdf_file.File_Type ();
	Set_Progress ();

	//---- build the approach node list ----

	node_list.resize (node_array.size (), -1);
	node_link.resize (dir_array.size ());

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->Divided () == 0) continue;
		Show_Progress ();

		//---- initialize the renumber map ----
		
		node_ptr = &node_array [link_itr->Anode ()];
		node = (node_ptr->Subarea () == 1) ? node_ptr->Node () : -node_ptr->Node ();

		map_stat = in_out_map.insert (Int_Map_Data (node_ptr->Node (), node));

		node_ptr = &node_array [link_itr->Bnode ()];
		node = (node_ptr->Subarea () == 1) ? node_ptr->Node () : -node_ptr->Node ();

		map_stat = in_out_map.insert (Int_Map_Data (node_ptr->Node (), node));

		//---- create the node-link list ----

		if (link_itr->AB_Dir () >= 0) {
			dir = link_itr->AB_Dir ();
			list_ptr = &node_link [dir];
			first_ptr = &node_list [link_itr->Bnode ()];
			*list_ptr = *first_ptr;
			*first_ptr = dir;
		}
		if (link_itr->BA_Dir () >= 0) {
			dir = link_itr->BA_Dir ();
			list_ptr = &node_link [dir];
			first_ptr = &node_list [link_itr->Anode ()];
			*list_ptr = *first_ptr;
			*first_ptr = dir;
		}
	}
	Set_Progress ();

	//---- renumber large node numbers ----

	next = 1;
	for (map_itr = in_out_map.begin (); map_itr != in_out_map.end (); map_itr++) {
		node = map_itr->first;
		Show_Progress ();

		if (node >= 10000) {
			while (in_out_map.find (next) != in_out_map.end ()) {
				next++;
			}
			if (next >= 10000) {
				Warning ("Too Many UTDF Nodes");
			}
			node = next;
			if (map_itr->second < 0) {
				map_itr->second = -node;
			} else {
				map_itr->second = node;
			}
		}
		if (map_itr->second < 0) {
			out_in_map.insert (Int_Map_Data (node, -map_itr->first));
		} else {
			out_in_map.insert (Int_Map_Data (node, map_itr->first));
		}
	}
	Set_Progress ();

	//---- write the map file ----

	if (map_flag) {
		fstream &fh = map_file.File ();

		fh << "UTDF\tTRANSIMS" << endl;

		for (map_itr = out_in_map.begin (); map_itr != out_in_map.end (); map_itr++) {
			fh << map_itr->first << "\t" << abs (map_itr->second) << endl;
		}
		map_file.Close ();
	}

	//---- write the file ----

	fstream &fh = utdf_file.File ();

	fh << "[Network]" << endl;
	fh << "Network Settings" << endl;
	fh << "RECORDNAME,DATA" << endl;
	if (Metric_Flag ()) {
		fh << "Metric,1" << endl;		//---- metric ----
	} else {
		fh << "Metric,0" << endl;		//---- feet ----
	}
	fh << "yellowTime,3.5" << endl;
	fh << "allRedTime,0.0" << endl;
	fh << "Walk,5.0" << endl;
	fh << "DontWalk,15.0" << endl;
	fh << "HV,0.02" << endl;
	fh << "PHF,0.90" << endl;
	if (Metric_Flag ()) {
		fh << "DefWidth,3.5" << endl;	//---- meters ----
	} else {
		fh << "DefWidth,12" << endl;	//---- feet ----
	}
	fh << "DefFlow,1900" << endl;
	if (Metric_Flag ()) {
		fh << "vehLength,6.0" << endl;	//---- meters ----
	} else {
		fh << "vehLength,25" << endl;	//---- feet ----
	}
	fh << "growth,1.00" << endl;
	if (Metric_Flag ()) {
		fh << "PedSpeed,1.0" << endl;	//---- m/s ----
	} else {
		fh << "PedSpeed,4.0" << endl;	//---- f/s ----
	}
	fh << "LostTimeAdjust,0.0" << endl;
	fh << "ScenarioTime," << time_of_day.Time_String (HOUR_CLOCK) << endl;
	fh << endl;

	//---- node processing ----

	fh << "[Nodes]" << endl;
	fh << "Node Data" << endl;
	fh << "INTID,Type,X,Y,Z,DESCRIPTION,CBD,Inside Radius,Outside Radius,Roundabout Lanes,Circle Speed" << endl;

	for (map_itr = out_in_map.begin (); map_itr != out_in_map.end (); map_itr++) {
		Show_Progress ();

		node = map_itr->first;
		num = map_itr->second;
		in_itr = node_map.find (abs (num));
		node_ptr = &node_array [in_itr->second];

		//---- type: 0=signal, 1=external, 2=shape, 3=sign, 4=roundabout ----

		if (num < 0) {
			type = 1;
		} else if (node_ptr->Control () >= 0) {
			type = 0;
		} else {
			type = 3;
		}
		x = DTOI (UnRound (node_ptr->X ()));
		y = DTOI (UnRound (node_ptr->Y ()));
		z = DTOI (UnRound (node_ptr->Z ()));

		fh << node << "," << type << "," << x << "," << y << "," << z << ",,,,,," << endl;
	}
	fh << endl;

	//---- link processing ----

	fh << "[Links]" << endl;
	fh << "Link Data" << endl;
	fh << "RECORDNAME,INTID,NB,NE,EB,SE,SB,SW,WB,NW" << endl;

	Set_Progress ();

	for (map_itr = out_in_map.begin (); map_itr != out_in_map.end (); map_itr++) {
		Show_Progress ();

		node = map_itr->first;
		in_itr = node_map.find (abs (map_itr->second));
		num = in_itr->second;

		//---- assign the approach links to a direction type ----

		memset (links, '\0', sizeof (links));
		memset (dirs, '\0', sizeof (dirs));

		for (dir = node_list [num]; dir >= 0; dir = node_link [dir]) {
			dir_ptr = &dir_array [dir];

			type = compass.Bearing_Point (dir_ptr->Out_Bearing ());
			if (dirs [type] != 0) {
				up = (type < 7) ? type + 1 : 0;
				down = (type > 0) ? type - 1 : 7;

				if (dir_ptr->Out_Bearing () >= dirs [type]->Out_Bearing ()) {
					if (dirs [up] == 0) {
						type = up;
					} else if (dirs [down] == 0) {
						dirs [down] = dirs [type];
						links [down] = links [type];
					}
				} else if (dirs [down] == 0) {
					type = down;
				} else if (dirs [up] == 0) {
					dirs [up] = dirs [type];
					links [up] = links [type];
				}
			}
			dirs [type] = dir_ptr;
			links [type] = &link_array [dir_ptr->Link ()];
		}

		//---- up stream node numbers ----

		fh << "Up ID," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (dir_ptr->Dir () == 0) {
					num = link_ptr->Anode ();
				} else {
					num = link_ptr->Bnode ();
				}
				node_ptr = &node_array [num];
				fh << node_ptr->Node ();
			}
		}
		fh << endl;

		//---- lanes ----

		fh << "Lanes," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				fh << dir_ptr->Lanes ();
			}
		}
		fh << endl;

		//---- street names ----

		fh << "Name," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (link_ptr->Name () != 0) {
					fh << link_ptr->Name ();
				}
			}
		}
		fh << endl;

		//---- link length ----

		fh << "Distance," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				fh << Resolve (link_ptr->Length ());
			}
		}
		fh << endl;

		//---- link speed ----

		fh << "Speed," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				fh << External_Units (Resolve (dir_ptr->Speed ()), (Metric_Flag () ? KPH : MPH));
			}
		}
		fh << endl;

		//---- travel time ----

		fh << "Time," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				fh << dir_ptr->Time0 ().Seconds ();
			}
		}
		fh << endl;

		//---- grade ----

		fh << "Grade," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (dir_ptr->Dir () == 0 || link_ptr->Grade () == 0) {
					fh << UnRound (link_ptr->Grade ());
				} else {
					fh << -UnRound (link_ptr->Grade ());
				}
			}
		}
		fh << endl;

		//---- median ----

		fh << "Median," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (link_ptr->Divided ()) {
					if (Metric_Flag ()) {
						num = 3;
					} else {
						num = 12;
					}
				} else {
					num = 0;
				}
				fh << num;
			}
		}
		fh << endl;

		//---- offset ----

		fh << "Offset," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				fh << 0;
			}
		}
		fh << endl;

		//---- TWLTL ----

		fh << "TWLTL," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				fh << 0;
			}
		}
		fh << endl;

		//---- crosswalk ----

		fh << "Crosswalk Width," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (link_ptr->Type () > EXPRESSWAY && link_ptr->Type () <= LOCAL) {
					if ((dir_ptr->Dir () == 0 && link_ptr->Boffset () > 0) ||
						(dir_ptr->Dir () == 1 && link_ptr->Aoffset () > 0)) {
						if (Metric_Flag ()) {
							fh << 5;
						} else {
							fh << 16;
						}
					} else {
						fh << 0;
					}
				} else {
					fh << 0;
				}
			}
		}
		fh << endl;

		//---- Mandatory Distance ----

		fh << "Mandatory Distance," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				if (Metric_Flag ()) {
					fh << 60;
				} else {
					fh << 200;
				}
			}
		}
		fh << endl;

		//---- Mandatory Distance2 ----

		fh << "Mandatory Distance2," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				if (Metric_Flag ()) {
					fh << 400;
				} else {
					fh << 1320;
				}
			}
		}
		fh << endl;

		//---- Positioning Distance ----

		fh << "Positioning Distance," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				if (Metric_Flag ()) {
					fh << 267;
				} else {
					fh << 880;
				}
			}
		}
		fh << endl;

		//---- Positioning Distance2 ----

		fh << "Positioning Distance2," << node;	
		for (type=0; type < 8; type++) {
			fh << ",";
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				if (Metric_Flag ()) {
					fh << 533;
				} else {
					fh << 1760;
				}
			}
		}
		fh << endl;

		//---- curve points ----

		fh << "Curve Pt X," << node << ",,,,,,,," << endl;
		fh << "Curve Pt Y," << node << ",,,,,,,," << endl;
		fh << "Curve Pt Z," << node << ",,,,,,,," << endl;
	}
	fh << endl;

	//---- lane processing ----

	turn_type_map [NO_TYPE] = 2;
	turn_type_map [THRU] = 2;
	turn_type_map [R_SPLIT] = 3;
	turn_type_map [L_SPLIT] = 1;
	turn_type_map [R_MERGE] = 2;
	turn_type_map [L_MERGE] = 2;
	turn_type_map [RIGHT] = 3;
	turn_type_map [LEFT] = 1;
	turn_type_map [UTURN] = 0;

	fh << "[Lanes]" << endl;
	fh << "Lane Group Data" << endl;
	fh << "RECORDNAME,INTID,NBU,NBL,NBT,NBR,NEU,NEL,NET,NER,EBU,EBL,EBT,EBR,SEU,SEL,SET,SER,SBU,SBL,SBT,SBR,SWU,SWL,SWT,SWR,WBU,WBL,WBT,WBR,NWU,NWL,NWT,NWR" << endl;

	Set_Progress ();

	for (map_itr = out_in_map.begin (); map_itr != out_in_map.end (); map_itr++) {
		Show_Progress ();

		node = map_itr->first;
		in_itr = node_map.find (abs (map_itr->second));
		num = in_itr->second;
		node_ptr = &node_array [num];
		control = node_ptr->Control ();

		//---- assign the approach links to a direction type ----

		memset (links, '\0', sizeof (links));
		memset (dirs, '\0', sizeof (dirs));
		memset (turns, -1, sizeof (turns)); 

		for (dir = node_list [num]; dir >= 0; dir = node_link [dir]) {
			dir_ptr = &dir_array [dir];

			type = compass.Bearing_Point (dir_ptr->Out_Bearing ());
			if (dirs [type] != 0) {
				up = (type < 7) ? type + 1 : 0;
				down = (type > 0) ? type - 1 : 7;

				if (dir_ptr->Out_Bearing () >= dirs [type]->Out_Bearing ()) {
					if (dirs [up] == 0) {
						type = up;
					} else if (dirs [down] == 0) {
						dirs [down] = dirs [type];
						links [down] = links [type];
					}
				} else if (dirs [down] == 0) {
					type = down;
				} else if (dirs [up] == 0) {
					dirs [up] = dirs [type];
					links [up] = links [type];
				}
			}
			dirs [type] = dir_ptr;
			links [type] = &link_array [dir_ptr->Link ()];
		}

		//---- set the lane connections ----

		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				for (num = dir_ptr->First_Connect (); num >= 0; num = con_ptr->Next_Index ()) {
					con_ptr = &connect_array [num];
					turns [type] [turn_type_map [con_ptr->Type ()]] = num;
				}
			}
		}
		
		//---- up stream node numbers ----

		fh << "Up Node," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];
			if (dir_ptr != 0) {
				link_ptr = links [type];
				if (dir_ptr->Dir () == 0) {
					num = link_ptr->Anode ();
				} else {
					num = link_ptr->Bnode ();
				}
				node_ptr = &node_array [num];
				num = node_ptr->Node ();
			} else {
				num = 0;
			}
			for (dir=0; dir < 4; dir++) {
				fh << ",";
				if (num > 0) {
					if (turns [type][dir] >= 0) {
						fh << num;
					}
				}
			}
		}
		fh << endl;

		//---- dest node numbers ----

		fh << "Dest Node," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			for (dir=0; dir < 4; dir++) {
				fh << ",";
				if (dir_ptr > 0) {
					num = turns [type][dir];

					if (num >= 0) {
						con_ptr = &connect_array [num];
						Dir_Data *ptr = &dir_array [con_ptr->To_Index ()];
						link_ptr = &link_array [ptr->Link ()];

						if (ptr->Dir () == 0) {
							num = link_ptr->Bnode ();
						} else {
							num = link_ptr->Anode ();
						}
						node_ptr = &node_array [num];

						fh << node_ptr->Node ();
					}
				}
			}
		}
		fh << endl;

		//---- turn lanes ----

		fh << "Lanes," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			if (dir_ptr == 0) {
				fh << ",,,,";
			} else {
				memset (low, -1, sizeof (low));
				memset (high, -1, sizeof (high));

				for (dir=0; dir < 4; dir++) {
					num = turns [type][dir];

					if (num >= 0) {
						con_ptr = &connect_array [num];
						low [dir] = con_ptr->Low_Lane ();
						high [dir] = con_ptr->High_Lane ();
					}
				}
				if (high [0] >= 0 && low [1] >= 0 && high [0] >= low [1]) {
					high [0] = low [1] - 1;
				}
				if (high [1] >= 0 && low [2] >= 0 && high [1] >= low [2]) {
					high [1] = low [2] - 1;
				}
				if (high [2] >= 0 && low [3] >= 0 && high [2] >= low [3]) {
					low [3] = high [2] + 1;
				}
				if (high [2] < 0 && high [1] >= 0 && low [3] >= 0 && high [1] >= low [3]) {
					low [3] = high [1] + 1;
				}
				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (low [dir] >= 0 && high [dir] >= 0) {
						num = high [dir] - low [dir] + 1;
						if (num > 0) {
							fh << num;
						}
					}
				}
			}
		}
		fh << endl;

		//---- shared lanes ----

		fh << "Shared," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			if (dir_ptr == 0) {
				fh << ",,,,";
			} else {
				memset (low, -1, sizeof (low));
				memset (high, -1, sizeof (high));
				memset (share, -1, sizeof (share));

				for (dir=0; dir < 4; dir++) {
					num = turns [type][dir];

					if (num >= 0) {
						con_ptr = &connect_array [num];
						low [dir] = con_ptr->Low_Lane ();
						high [dir] = con_ptr->High_Lane ();
					}
				}
				if (high [0] >= 0 && low [1] >= 0 && high [0] >= low [1]) {
					share [1] = 1;
				}
				if (high [1] >= 0 && low [2] >= 0 && high [1] >= low [2]) {
					share [2] = 1;
				}
				if (high [2] >= 0 && low [3] >= 0 && high [2] >= low [3]) {
					share [2] = ((share [2] > 0) ? 3 : 2);
				}
				if (high [2] < 0 && high [1] >= 0 && low [3] >= 0 && high [1] >= low [3]) {
					share [1] = ((share [1] > 0) ? 3 : 2);
				}
				if (low [2] >= 0 && high [2] >= 0 && share [2] < 0) share [2] = 0;

				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (share [dir] >= 0) {
						fh << share [dir];
					}
				}
			}
		}
		fh << endl;

		//---- width ----

		fh << "Width," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			if (dir_ptr == 0) {
				fh << ",,,,";
			} else {
				for (dir=0; dir < 4; dir++) {
					fh << ",";
					num = turns [type][dir];

					if (num >= 0) {
						if (Metric_Flag ()) {
							fh << 3.6;
						} else {
							fh << 12;
						}
					}
				}
			}
		}
		fh << endl;

		//---- storage ----

		fh << "Storage," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			for (dir=0; dir < 4; dir++) {
				fh << ",";
				if (dir_ptr > 0 && ((dir < 2 && dir_ptr->Left () > 0) || (dir > 2 && dir_ptr->Right () > 0))) {
					if (turns [type][dir] >= 0) {
						for (num = dir_ptr->First_Pocket (); num >= 0; num = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [num];

							if (dir < 2) {
								if (pocket_ptr->Type () == LEFT_TURN) {
									fh << Resolve (pocket_ptr->Length ());
									break;
								}
							} else {
								if (pocket_ptr->Type () == RIGHT_TURN) {
									fh << Resolve (pocket_ptr->Length ());
									break;
								}
							}
						}
					}
				}
			}
		}
		fh << endl;

		//---- taper ----

		fh << "Taper," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			for (dir=0; dir < 4; dir++) {
				fh << ",";
				if (dir_ptr > 0 && ((dir < 2 && dir_ptr->Left () > 0) || (dir > 2 && dir_ptr->Right () > 0))) {
					if (turns [type][dir] >= 0) {
						for (num = dir_ptr->First_Pocket (); num >= 0; num = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [num];

							if (dir < 2) {
								if (pocket_ptr->Type () == LEFT_TURN) {
									if (Metric_Flag ()) {
										fh << 7.5;
									} else {
										fh << 25;
									}
									break;
								}
							} else {
								if (pocket_ptr->Type () == RIGHT_TURN) {
									if (Metric_Flag ()) {
										fh << 7.5;
									} else {
										fh << 25;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		fh << endl;

		//---- storage lanes ----

		fh << "StLanes," << node;	
		for (type=0; type < 8; type++) {
			dir_ptr = dirs [type];

			for (dir=0; dir < 4; dir++) {
				fh << ",";
				if (dir_ptr > 0 && ((dir < 2 && dir_ptr->Left () > 0) || (dir > 2 && dir_ptr->Right () > 0))) {
					if (turns [type][dir] >= 0) {
						for (num = dir_ptr->First_Pocket (); num >= 0; num = pocket_ptr->Next_Index ()) {
							pocket_ptr = &pocket_array [num];

							if (dir < 2) {
								if (pocket_ptr->Type () == LEFT_TURN) {
									fh << pocket_ptr->Lanes ();
									break;
								}
							} else {
								if (pocket_ptr->Type () == RIGHT_TURN) {
									fh << pocket_ptr->Lanes ();
									break;
								}
							}
						}
					}
				}
			}
		}
		fh << endl;

		//---- grade ----

		fh << "Grade," << node;	
		for (type=0; type < 8; type++) {
			fh << ",,,,";
		}
		fh << endl;

		//---- phase1 ----

		fh << "Phase1," << node;	
		if (control >= 0) {
			signal_ptr = &signal_array [control];
			phasing = 0;

			for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
				if (time_itr->Start () <= time_of_day && time_of_day <= time_itr->End ()) {
					phasing = time_itr->Phasing ();
					break;
				}
			}
			for (type=0; type < 8; type++) {
				dir_ptr = dirs [type];

				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (dir_ptr > 0) {
						num = turns [type][dir];
						phase = 0;

						if (num >= 0) {
							for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
								if (phasing_itr->Phasing () == phasing) {
									for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
										if (move_itr->Connection () == num) {
											phase = phasing_itr->Phase ();
											break;
										}
									}
								}
								if (phase > 0) break;
							}
							fh << phase;
						}
					}
				}
			}
			fh << endl;

			//---- phase 2 ----

			fh << "Phase2," << node;	

			for (type=0; type < 8; type++) {
				dir_ptr = dirs [type];

				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (dir_ptr > 0) {
						num = turns [type][dir];
						phase = up = 0;

						if (num >= 0) {
							for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
								if (phasing_itr->Phasing () == phasing) {
									for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
										if (move_itr->Connection () == num) {
											if (up == 1) {
												phase = phasing_itr->Phase ();
												break;
											} else {
												up = 1;
											}
										}
									}
								}
								if (phase > 0) break;
							}
							if (phase > 0) {
								fh << phase;
							}
						}
					}
				}
			}
			fh << endl;

			//---- permitted phase 1 ----

			fh << "PermPhase1," << node;
			for (type=0; type < 8; type++) {
				dir_ptr = dirs [type];

				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (dir_ptr > 0) {
						num = turns [type][dir];
						phase = 0;

						if (num >= 0) {
							for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
								if (phasing_itr->Phasing () == phasing) {
									for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
										if (move_itr->Connection () == num && move_itr->Protection () == PERMITTED) {
											phase = phasing_itr->Phase ();
											break;
										}
									}
								}
								if (phase > 0) break;
							}
							if (phase > 0) {
								fh << phase;
							}
						}
					}
				}
			}
		} else {
			for (type=0; type < 8; type++) {
				dir_ptr = dirs [type];

				for (dir=0; dir < 4; dir++) {
					fh << ",";
					if (dir_ptr > 0) {
						if (turns [type][dir] >= 0) {
							fh << -1;
						}
					}
				}
			}
		}
		fh << endl;

		//---- volume ----

		fh << "Volume," << node;	
		for (type=0; type < 8; type++) {
			fh << ",,,,";
		}
		fh << endl;

		//---- peds ----

		fh << "Peds," << node;	
		for (type=0; type < 8; type++) {
			fh << ",,,,";
		}
		fh << endl;
	}
	fh << endl;

	fh << "[Timeplan]" << endl;
	fh << "Timing Plan Settings" << endl;
	fh << "RECORDNAME,INTID,DATA" << endl;

	Set_Progress ();

	for (map_itr = out_in_map.begin (); map_itr != out_in_map.end (); map_itr++) {
		Show_Progress ();

		node = map_itr->first;
		in_itr = node_map.find (abs (map_itr->second));
		num = in_itr->second;
		node_ptr = &node_array [num];
		control = node_ptr->Control ();

		//---- control type ----

		fh << "Control Type," << node;	
		fh << ",0" << endl;
		
		//---- cycle length ----

		fh << "Cycle Length," << node;	
		fh << ",0" << endl;

		//---- referenced to ----

		fh << "Referenced To," << node;	
		fh << ",0" << endl;

		//---- reference phase ----

		fh << "Reference Phase," << node;	
		fh << ",0" << endl;

		//---- offset ----

		fh << "Offset," << node;	
		fh << ",0" << endl;
	}
	fh << endl;
	End_Progress ();
}

