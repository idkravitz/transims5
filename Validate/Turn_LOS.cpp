//*********************************************************
//	Turn_LOS.cpp - Turning Movement LOS Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Turn_LOS
//---------------------------------------------------------

void Validate::Turn_LOS (void)
{
	//int i, j, num, in_link_dir, link, dir, node, index, in_index, in_bearing;
	//int out_link, out_index, total, volume, diff, tod;
	//int timing, record, green, cycle, yellow, rings;
	//double cap, lanes, share;

	//bool left_lane [20];
	//int left_est_vol, left_obs_vol, left_diff_vol;
	//double left_est_vc, left_obs_vc, left_diff_per, left_green;
	//char left_est_los, left_obs_los, left_diff_los [8];

	//bool thru_lane [20];
	//int thru_est_vol, thru_obs_vol, thru_diff_vol;
	//double thru_est_vc, thru_obs_vc, thru_diff_per, thru_green;
	//char thru_est_los, thru_obs_los, thru_diff_los [8];

	//string label, bearing;
	//bool count_flag, new_line, left_flag, pass_flag;

	int i, j, num, link, dir, node, index, in_index, in_bearing;
	int out_link, out_index;
	double cap, lanes, total, volume;

	bool left_lane [20];
	double left_est_vol, left_obs_vol, left_diff_vol;
	double left_est_vc, left_obs_vc, left_diff_per, left_green;
	char left_est_los, left_obs_los, left_diff_los [8];

	bool thru_lane [20];
	double thru_est_vol, thru_obs_vol, thru_diff_vol;
	double thru_est_vc, thru_obs_vc, thru_diff_per, thru_green;
	char thru_est_los, thru_obs_los, thru_diff_los [8];

	string label, bearing;
	bool count_flag, new_line, left_flag;

	double los_ratio [5] = {0.55, 0.66, 0.775, 0.885, 0.985};

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Dir_Itr dir_itr;
	Connect_Data *connect_ptr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;

	//Signal_Data *signal_ptr;
	//Timing_Data *timing_ptr, timing_rec;
	//Phasing_Data *phasing_ptr;

	////---- estimate percent greens by phase ----

	//timing = rings = 0;
	//pass_flag = false;

	//for (timing_ptr = timing_data.First_Key (); timing_ptr; timing_ptr = timing_data.Next_Key ()) {
	//	if (timing_ptr->Timing () != timing) {
	//		if (timing > 0) {
	//			if (!pass_flag) {
	//				timing_ptr = timing_data [record];
	//				pass_flag = true;

	//				//---- estimate the cycle length ----

	//				green = MAX (timing_rec.Min_Green (), timing_rec.Max_Green ());
	//				yellow = timing_rec.Yellow () + timing_rec.Red_Clear ();

	//				if (rings > 1) {
	//					green = (green + rings / 2) / rings;
	//					yellow = (yellow + rings / 2) / rings;
	//				}
	//				cycle = green + yellow;

	//				if (cycle < 60) {
	//					share = 1.0 - (yellow / 60.0);
	//				} else {
	//					share = 1.0 - ((double) yellow / cycle);
	//				}
	//				if (share < 0.9) {
	//					share = 0.9;
	//				}
	//				cycle = (int) (green / share + 0.5);

	//				//---- calculate percent green ----

	//				green = MAX (timing_ptr->Min_Green (), timing_ptr->Max_Green ());
	//				timing_ptr->Max_Green ((green * 100 + cycle / 2) / cycle);
	//				continue;
	//			} else {
	//				pass_flag = false;
	//			}
	//		}
	//		timing = timing_ptr->Timing ();
	//		rings = timing_ptr->Ring ();
	//		record = timing_data.Record_Index ();
	//		memcpy (&timing_rec, timing_ptr, sizeof (timing_rec));
	//	} else {
	//		if (pass_flag) {
	//			green = MAX (timing_ptr->Min_Green (), timing_ptr->Max_Green ());
	//			timing_ptr->Max_Green ((green * 100 + cycle / 2) / cycle);				
	//		} else {
	//			timing_rec.Min_Green (timing_rec.Min_Green () + timing_ptr->Min_Green ());
	//			timing_rec.Max_Green (timing_rec.Max_Green () + timing_ptr->Max_Green ());
	//			timing_rec.Yellow (timing_rec.Yellow () + timing_ptr->Yellow ());
	//			timing_rec.Red_Clear (timing_rec.Red_Clear () + timing_ptr->Red_Clear ());
	//			if (timing_ptr->Ring () > rings) rings = timing_ptr->Ring ();
	//		}
	//	}
	//}

	////---- initialize the percent green ----

	//for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
	//	dir_ptr->Thru (0);
	//	dir_ptr->Left (0);
	//}

	////---- apply percent green to each approach link ----

	//tod = sum_periods.First ()->Low ();

	//for (phasing_ptr = phasing_data.First (); phasing_ptr; phasing_ptr = phasing_data.Next ()) {

	//	//---- check the time of day restrictions ----

	//	signal_ptr = signal_data.Get_LE (phasing_ptr->Node (), tod);
	//	
	//	if (signal_ptr == NULL || signal_ptr->Timing () != phasing_ptr->Timing ()) continue;

	//	timing_rec.Timing (phasing_ptr->Timing ());
	//	timing_rec.Phase (phasing_ptr->Phase ());

	//	timing_ptr = timing_data.Get (&timing_rec);
	//	if (timing_ptr == NULL) continue;

	//	connect_ptr = connect_time.Get (phasing_ptr->In_Link_Dir (), phasing_ptr->Out_Link_Dir ());
	//	if (connect_ptr == NULL) continue;

	//	green = timing_ptr->Max_Green ();
	//	if (phasing_ptr->Protection () != PROTECTED) {
	//		green = green / 2;
	//	}

	//	//---- get the movement type ----

	//	link_ptr = link_data.Get (connect_ptr->In_Link ());
	//	if (link_ptr == NULL) continue;

	//	if (!connect_ptr->In_Dir ()) {
	//		dir_ptr = dir_data [link_ptr->AB_Dir ()];
	//	} else {
	//		dir_ptr = dir_data [link_ptr->BA_Dir ()];
	//	}
	//	if (dir_ptr == NULL) continue;

	//	if (connect_ptr->Type () == LEFT || connect_ptr->Type () == UTURN) {
	//		dir_ptr->Left (dir_ptr->Left () + green);
	//	} else {
	//		dir_ptr->Thru (dir_ptr->Thru () + green);
	//	}
	//}

	//---- print the page header ----
	
	Header_Number (TURN_LOS);

	if (!Break_Check ((int) connect_array.size () + 9)) {
		Print (1);
		Turn_Header ();
	}

	//---- print each approach link ----

	link = dir = node = in_index = in_bearing = out_link = out_index = 0;
	count_flag = left_flag = false;

	num = sum_periods.Num_Periods ();

	//---- process each approach link ----

	for (dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++) {

		//---- initialize the approach ----

		in_bearing = dir_itr->Out_Bearing ();
		in_index = dir_itr->Link ();

		link_ptr = &link_array [in_index];
		link = link_ptr->Link ();
		dir = dir_itr->Dir ();

		if (dir == 0) {
			node = link_ptr->Bnode ();
		} else {
			node = link_ptr->Anode ();
		}
		out_index = -1;
		//left_obs = thru_obs = right_obs = uturn_obs = 0;
		//left_est = thru_est = right_est = uturn_est = 0;

		//left_green = dir_ptr->Left () / 100.0;
		left_green = 0;
		memset (left_lane, '\0', sizeof (left_lane));

		left_est_vol = left_obs_vol = left_diff_vol = 0;
		left_est_vc = left_obs_vc = left_diff_per = 0.0;
		left_est_los = left_obs_los = 'A';
		left_diff_los [0] = '\0';

		//thru_green = dir_ptr->Thru () / 100.0;
		thru_green = 0;
		memset (thru_lane, '\0', sizeof (thru_lane));

		thru_est_vol = thru_obs_vol = thru_diff_vol = 0;
		thru_est_vc = thru_obs_vc = thru_diff_per = 0.0;
		thru_est_los = thru_obs_los = 'A';
		thru_diff_los [0] = '\0';

		count_flag = false;

		//---- process each link connection ----

		for (index = dir_itr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
			connect_ptr = &connect_array [index];

			//---- sum the time period data ----

			total = volume = 0;

			for (i=0; i < num; i++) {
				array_ptr = &turn_count_array [i];
				flow_ptr = &array_ptr->at (index);
				total += flow_ptr->Flow ();

				array_ptr = &turn_delay_array [i];
				flow_ptr = &array_ptr->at (index);
				volume += flow_ptr->Flow ();
			}
			if (volume == 0 && total == 0) continue;

			if (total > 0) count_flag = true;

			//---- find the movement type ----

			if (connect_ptr->Type () == THRU || 
				connect_ptr->Type () == L_MERGE || connect_ptr->Type () == R_MERGE) {
				left_flag = false;
			} else if (connect_ptr->Type () == UTURN) {
				left_flag = true;
			} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == L_SPLIT) {
				out_index = connect_ptr->To_Index ();
				left_flag = true;
			} else if (connect_ptr->Type () == RIGHT || connect_ptr->Type () == R_SPLIT) {
				out_index = connect_ptr->To_Index ();
				left_flag = false;
			}
			if (left_flag) {
				left_est_vol += volume;
				left_obs_vol += total;

				for (i=connect_ptr->Low_Lane (); i <= connect_ptr->High_Lane (); i++) {
					left_lane [i] = true;
				}
			} else {
				thru_est_vol += volume;
				thru_obs_vol += total;

				for (i=connect_ptr->Low_Lane (); i <= connect_ptr->High_Lane (); i++) {
					thru_lane [i] = true;
				}
			}
		}
		if (!count_flag) continue;

		//---- get the link name ----

		label = link_ptr->Name ();
		if (label.empty ()) {
			label = String ("Link %d %s") % link_ptr->Link () % ((dir == 0) ? "AB" : "BA");
		}

		//---- get the link direction ----
		
		bearing = compass.Point_Text (dir_itr->Out_Bearing ());
		if (bearing.empty ()) {
			bearing = "B";
		}

		//---- print the street name ----

		Page_Check (2);
		Print (0, String ("\n%-25.25s %-2.2s  ") % label % bearing);

		//---- get the cross street ----

		if (out_index >= 0) {
			dir_ptr = &dir_array [out_index];
			link_ptr = &link_array [dir_ptr->Link ()];
			label = link_ptr->Name ();
		} else {
			label.clear ();
		}
		if (label.empty ()) {
			node_ptr = &node_array [node];
			label = String ("Node %d") % node_ptr->Node ();
		}
		Print (0, String ("%-20.20s") % label);

		//---- left ----

		if (left_est_vol > 0 || left_obs_vol > 0) {
			lanes = 0;

			for (i=0; i < 20; i++) {
				if (left_lane [i]) {
					if (thru_lane [i]) {
						lanes += 0.5;
					} else {
						lanes += 1.0;
					}
				}
			}
			if (lanes > 1.0) {
				cap = hours * 1740 * (1.0 + (0.8 * (lanes - 1.0)));
			} else {
				cap = hours * 1740;
			}
			left_est_vc = left_est_vol * left_green / cap;
			left_obs_vc = left_obs_vol * left_green / cap;

			for (i=0; i < 5; i++) {
				if (left_est_vc <= los_ratio [i]) break;
			}
			left_est_los = (char) ('A' + i);

			for (j=0; j < 5; j++) {
				if (left_obs_vc <= los_ratio [j]) break;
			}
			left_obs_los = (char) ('A' + j);

			left_diff_vol = left_est_vol - left_obs_vol;

			if (left_obs_vol > 0) {
				left_diff_per = left_diff_vol * 100.0 / left_obs_vol;
			} else {
				left_diff_per = 100.0;
			}
			j = abs (i - j);
			for (i=0; i < j; i++) {
				left_diff_los [i] = '*';
			}
			left_diff_los [i] = '\0';

			Print (0, String ("   Left    %6.0lf %5.2lf  %c   %6.0lf %5.2lf  %c   %6.0lf  %6.1lf  %s") % 
				left_est_vol % left_est_vc % left_est_los %
				left_obs_vol % left_obs_vc % left_obs_los %
				left_diff_vol % left_diff_per % left_diff_los);
			new_line = true;
		} else {
			new_line = false;
		}

		//---- thru ----

		if (thru_est_vol > 0 || thru_obs_vol > 0) {
			if (new_line) Print (0, String ("\n%50c") % BLANK);

			lanes = 0;

			for (i=0; i < 20; i++) {
				if (thru_lane [i]) {
					if (left_lane [i]) {
						lanes += 0.5;
					} else {
						lanes += 1.0;
					}
				}
			}
			cap = hours * 1810 * lanes;

			thru_est_vc = thru_est_vol * thru_green / cap;
			thru_obs_vc = thru_obs_vol * thru_green / cap;

			for (i=0; i < 5; i++) {
				if (thru_est_vc <= los_ratio [i]) break;
			}
			thru_est_los = (char) ('A' + i);

			for (j=0; j < 5; j++) {
				if (thru_obs_vc <= los_ratio [j]) break;
			}
			thru_obs_los = (char) ('A' + j);

			thru_diff_vol = thru_est_vol - thru_obs_vol;

			if (thru_obs_vol > 0) {
				thru_diff_per = thru_diff_vol * 100.0 / thru_obs_vol;
			} else {
				thru_diff_per = 100.0;
			}
			j = abs (i - j);
			for (i=0; i < j; i++) {
				thru_diff_los [i] = '*';
			}
			thru_diff_los [i] = '\0';

			Print (0, String ("   Thru    %6.0lf %5.2lf  %c   %6.0lf %5.2lf  %c   %6.0lf  %6.1lf  %s") % 
				thru_est_vol % thru_est_vc % thru_est_los %
				thru_obs_vol % thru_obs_vc % thru_obs_los %
				thru_diff_vol % thru_diff_per % thru_diff_los);
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Turn_LOS_Header
//---------------------------------------------------------

void Validate::Turn_LOS_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%45cTurn Level of Service Validation\n") % BLANK);
	Print (0, String ("\n%61c----Estimate----  ----Observed----  ------Difference------") % BLANK);
	Print (0, "\nStreet/Link              Dir  Cross Street/Link    Movement  ");
	Print (0, "Volume  V/C  LOS  Volume  V/C  LOS  Volume Percent  LOS\n");
}

/**********************************************************|***********************************************************

                                             Turn Level of Service Validation

													         ----Estimate----  ----Observed----  ------Difference------
Street/Link              Dir  Cross Street/Link    Movement  Volume  V/C  LOS  Volume  V/C  LOS  Volume Percent  LOS

sssssssssssssssssssssssss NB  ssssssssssssssssssss   Left    dddddd ff.ff  c   dddddd ff.ff  c   dddddd  ffff.f  ssssss
                                                     Thru    dddddd ff.ff  c   dddddd ff.ff  c   dddddd  ffff.f  ssssss



***********************************************************|***********************************************************/ 
