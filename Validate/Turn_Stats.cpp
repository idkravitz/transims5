//*********************************************************
//	Turn_Stats.cpp - Turning Movement Statistics
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Turn_Stats
//---------------------------------------------------------

void Validate::Turn_Stats (void)
{
	int i, num, link, dir, node, index, in_index, in_bearing;
	int out_link, out_index;
	double total, volume;
	double left_obs, thru_obs, right_obs, uturn_obs;
	double left_est, thru_est, right_est, uturn_est;
	double per_left_est, per_thru_est, per_right_est, per_uturn_est, factor;
	double per_left_obs, per_thru_obs, per_right_obs, per_uturn_obs;
	String label, bearing;
	bool count_flag;

	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Dir_Itr dir_itr;
	Connect_Data *connect_ptr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;

	//---- print the page header ----
	
	Header_Number (TURN_MOVEMENT);

	if (!Break_Check ((int) connect_array.size () + 9)) {
		Print (1);
		Turn_Header ();
	}
	link = dir = node = in_index = in_bearing = out_link = out_index = 0;
	count_flag = false;

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
		left_obs = thru_obs = right_obs = uturn_obs = 0;
		left_est = thru_est = right_est = uturn_est = 0;
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

			//---- sum the movement type ----

			if (connect_ptr->Type () == THRU || 
				connect_ptr->Type () == L_MERGE || connect_ptr->Type () == R_MERGE) {
				thru_est += volume;
				thru_obs += total;
			} else if (connect_ptr->Type () == UTURN) {
				uturn_est += volume;
				uturn_obs += total;
			} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == L_SPLIT) {
				out_index = connect_ptr->To_Index ();
				left_est += volume;
				left_obs += total;
			} else if (connect_ptr->Type () == RIGHT || connect_ptr->Type () == R_SPLIT) {
				out_index = connect_ptr->To_Index ();
				right_est += volume;
				right_obs += total;
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

		Page_Check (5);
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
		
		//---- estimate ----

		Print (0, String ("%-20.20s  Estimate   %6d %6d %6d %6d") % label %
			left_est % thru_est % right_est % uturn_est);

		total = left_est + thru_est + right_est + uturn_est;
		if (total < 1) total = 1;
		factor = 100.0 / total;

		per_left_est = left_est * factor;
		per_thru_est = thru_est * factor;
		per_right_est = right_est * factor;
		per_uturn_est = uturn_est * factor;

		Print (0, String ("  %6.1lf %6.1lf %6.1lf %6.1lf") % 
			per_left_est % per_thru_est % per_right_est % per_uturn_est);

		//---- observed ----

		Print (0, String ("\n%52cObserved   %6d %6d %6d %6d") % BLANK %
			left_obs % thru_obs % right_obs % uturn_obs);

		total = left_obs + thru_obs + right_obs + uturn_obs;
		if (total < 1) total = 1;
		factor = 100.0 / total;

		per_left_obs = left_obs * factor;
		per_thru_obs = thru_obs * factor;
		per_right_obs = right_obs * factor;
		per_uturn_obs = uturn_obs * factor;

		Print (0, String ("  %6.1lf %6.1lf %6.1lf %6.1lf") % 
			per_left_obs % per_thru_obs % per_right_obs % per_uturn_obs);

		//---- difference ----

		left_est -= left_obs;
		thru_est -= thru_obs;
		right_est -= right_obs;
		uturn_est -= uturn_obs;

		Print (0, String ("\n%52cDifference %6d %6d %6d %6d") % BLANK % 
			left_est % thru_est % right_est % uturn_est);

		per_left_est -= per_left_obs;
		per_thru_est -= per_thru_obs;
		per_right_est -= per_right_obs;
		per_uturn_est -= per_uturn_obs;

		Print (0, String ("  %6.1lf %6.1lf %6.1lf %6.1lf") % 
			per_left_est % per_thru_est % per_right_est % per_uturn_est);

		//---- percent error ----

		if (per_left_obs != 0.0) {
			per_left_est = 100.0 * per_left_est / per_left_obs;
			if (per_left_est > 999.9) {
				per_left_est = 999.9;
			} else if (per_left_est < -999.9) {
				per_left_est = -999.9;
			}
		}
		if (per_thru_obs != 0.0) {
			per_thru_est = 100.0 * per_thru_est / per_thru_obs;
			if (per_thru_est > 999.9) {
				per_thru_est = 999.9;
			} else if (per_thru_est < -999.9) {
				per_thru_est = -999.9;
			}
		}
		if (per_right_obs != 0.0) {
			per_right_est = 100.0 * per_right_est / per_right_obs;
			if (per_right_est > 999.9) {
				per_right_est = 999.9;
			} else if (per_right_est < -999.9) {
				per_right_est = -999.9;
			}
		}
		if (per_uturn_obs != 0.0) {
			per_uturn_est = 100.0 * per_uturn_est / per_uturn_obs;
			if (per_uturn_est > 999.9) {
				per_uturn_est = 999.9;
			} else if (per_uturn_est < -999.9) {
				per_uturn_est = -999.9;
			}
		}

		//---- volume error ----

		per_left_obs = 100.0 * left_est / ((left_obs) ? left_obs : 1);
		if (per_left_obs > 999.9) {
			per_left_obs = 999.9;
		} else if (per_left_obs < -999.9) {
			per_left_obs = -999.9;
		}
		per_thru_obs = 100.0 * thru_est / ((thru_obs) ? thru_obs : 1);
		if (per_thru_obs > 999.9) {
			per_thru_obs = 999.9;
		} else if (per_thru_obs < -999.9) {
			per_thru_obs = -999.9;
		}
		per_right_obs = 100.0 * right_est / ((right_obs) ? right_obs : 1);
		if (per_right_obs > 999.9) {
			per_right_obs = 999.9;
		} else if (per_right_obs < -999.9) {
			per_right_obs = -999.9;
		}
		per_uturn_obs = 100.0 * uturn_est / ((uturn_obs) ? uturn_obs : 1);
		if (per_uturn_obs > 999.9) {
			per_uturn_obs = 999.9;
		} else if (per_uturn_obs < -999.9) {
			per_uturn_obs = -999.9;
		}

		Print (0, String ("\n%52c%% Error    %6.1lf %6.1lf %6.1lf %6.1lf") % BLANK % 
			per_left_obs % per_thru_obs % per_right_obs % per_uturn_obs);

		Print (0, String ("  %6.1lf %6.1lf %6.1lf %6.1lf\n") % 
			per_left_est % per_thru_est % per_right_est % per_uturn_est);

	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Turn_Header
//---------------------------------------------------------

void Validate::Turn_Header (void)
{
	Page_Check (5);
	Print (0, String ("\n%47cTurning Movement Validation\n") % BLANK);
	Print (0, String ("\n%63c-----------Volume---------- -----------Percent----------") % BLANK);
	Print (0, "\nStreet/Link              Dir  Cross Street/Link     Source     ");
	Print (0, "  Left   Thru  Right  Uturn    Left   Thru  Right  Uturn\n");
}

/**********************************************************|***********************************************************

                                               Turning Movement Validation
									
                                                               -----------Volume---------- -----------Percent----------
Street/Link              Dir  Cross Street/Link     Source       Left   Thru  Right  Uturn    Left   Thru  Right  Uturn

sssssssssssssssssssssssss NB  ssssssssssssssssssss  Estimate   dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
                                                    Observed   dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
                                                    Difference dddddd dddddd dddddd dddddd  ffff.f ffff.f ffff.f ffff.f
													% Error    ffff.f ffff.f ffff.f ffff.f  ffff.f ffff.f ffff.f ffff.f


***********************************************************|***********************************************************/ 
