//*********************************************************
//	Lane_Ranges.cpp - build the lane connectivity
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Lane_Ranges
//---------------------------------------------------------

void TransimsNet::Lane_Ranges (void)
{
	int index, to_index, bear_in, bear_out, change, type, dir_index;
	int num_left, num_lmerge, num_lsplit, num_thru, num_rmerge, num_rsplit, num_right;
	int lns_left, lns_lmerge, lns_lsplit, lns_thru, lns_rmerge, lns_rsplit, lns_right;
	int in_left1, in_left2, in_right1, in_right2, in_thru1, in_thru2, left_lanes, right_lanes;
	int out_left1, out_left2, out_right1, out_right2, out_thru1, out_thru2;
	int in_low, in_high, out_low, out_high, lanes;
	bool detail_lane_flag = false;

	Dir_Data *to_ptr;
	Dir_Itr dir_itr;
	Int_Map bear_map;
	Int_Map_Itr map_itr;
	Connect_Data *connect_ptr;
	Pocket_Data *pocket_ptr;
	Link_Detail_Map_Itr detail_itr;

	Show_Message ("Building Lane Connectivity -- Record");
	Set_Progress ();

	//---- create connection records between links ----

	for (dir_index = 0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, dir_index++) {
		Show_Progress ();

		//---- approach lane type ranges ----

		in_thru1 = dir_itr->Left ();
		in_thru2 = dir_itr->Left () + dir_itr->Lanes () - 1;

		in_left1 = in_left2 = in_right1 = in_right2 = -1;

		for (index = dir_itr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
			pocket_ptr = &pocket_array [index];

			if (pocket_ptr->Type () == LEFT_TURN) {
				in_left2 = dir_itr->Left () - 1;
				in_left1 = dir_itr->Left () - pocket_ptr->Lanes ();
			} else if (pocket_ptr->Type () == RIGHT_TURN) {
				in_right1 = dir_itr->Left () + dir_itr->Lanes ();
				in_right2 = in_right1 + pocket_ptr->Lanes () - 1;
			}
		}

		//---- get link details for shared lanes ----

		if (link_detail_flag) {
			detail_itr = detail_map.find (dir_index);
			if (detail_itr != detail_map.end ()) {
				lanes = detail_itr->second.Left () + detail_itr->second.Left_Thru () + detail_itr->second.Thru () + detail_itr->second.Right_Thru () + 
					detail_itr->second.Right () + detail_itr->second.Left_Merge () + detail_itr->second.Right_Merge ();
				detail_lane_flag = (lanes > 0);
			} else {
				detail_lane_flag = false;
			}
		}

		//---- sort the link connections ----

		bear_in = dir_itr->Out_Bearing ();
		bear_map.clear ();

		for (index = dir_itr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
			connect_ptr = &connect_array [index];
			to_index = connect_ptr->To_Index ();
			
			to_ptr = &dir_array [to_index];
			bear_out = to_ptr->In_Bearing ();

			change = compass.Change (bear_in, bear_out);
			bear_map.insert (Int_Map_Data (change, index));
		}

		//---- identify the movement groups ----

		num_left = num_lmerge = num_lsplit = num_thru = num_rsplit = num_rmerge = num_right = 0;
		lns_left = lns_lmerge = lns_lsplit = lns_thru = lns_rsplit = lns_rmerge = lns_right = 0;

		for (map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++) {
			index = map_itr->second;
			connect_ptr = &connect_array [index];

			to_index = connect_ptr->To_Index ();
			to_ptr = &dir_array [to_index];

			left_lanes = right_lanes = 0;

			for (index = to_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_MERGE) {
					if (pocket_ptr->Lanes () > left_lanes) left_lanes = pocket_ptr->Lanes ();
				} else if (pocket_ptr->Type () == RIGHT_MERGE) {
					if (pocket_ptr->Lanes () > right_lanes) right_lanes = pocket_ptr->Lanes ();
				}
			}
			type = connect_ptr->Type ();

			switch (type) {
				case THRU:
					num_thru++;
					lns_thru += to_ptr->Lanes ();
					break;
				case LEFT:
					num_left++;
					lns_left += to_ptr->Lanes () + left_lanes;
					break;
				case L_SPLIT:
					num_lsplit++;
					lns_lsplit += to_ptr->Lanes () + right_lanes;
					break;
				case RIGHT:
					num_right++;
					lns_right += to_ptr->Lanes () + right_lanes;
					break;
				case R_SPLIT:
					num_rsplit++;
					lns_rsplit += to_ptr->Lanes () + left_lanes;
					break;
				case L_MERGE:
					num_lmerge++;
					lns_lmerge += to_ptr->Lanes () + left_lanes;
					break;
				case R_MERGE:
					num_rmerge++;
					lns_rmerge += to_ptr->Lanes () + right_lanes;
					break;
				default:
					break;
			}
		}

		//---- allocate lanes ----

		left_lanes = right_lanes = 0;

		for (map_itr = bear_map.begin (); map_itr != bear_map.end (); map_itr++) {
			index = map_itr->second;
			connect_ptr = &connect_array [index];

			to_index = connect_ptr->To_Index ();
			to_ptr = &dir_array [to_index];

			//---- departure lane type ranges ----

			out_thru1 = to_ptr->Left ();
			out_thru2 = to_ptr->Left () + to_ptr->Lanes () - 1;

			out_left1 = out_left2 = out_right1 = out_right2 = -1;

			for (index = to_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_MERGE) {
					out_left2 = to_ptr->Left () - 1;
					out_left1 = to_ptr->Left () - pocket_ptr->Lanes ();
				} else if (pocket_ptr->Type () == RIGHT_MERGE) {
					out_right1 = to_ptr->Left () + to_ptr->Lanes ();
					out_right2 = out_right1 + pocket_ptr->Lanes () - 1;
				}
			}
			type = connect_ptr->Type ();

			switch (type) {
				case THRU:
					in_low = in_thru1;
					in_high = in_thru2;
					out_low = out_thru1;
					out_high = out_thru2;
					if (detail_lane_flag && detail_itr->second.Thru () < (in_high - in_low + 1)) {
						if (detail_itr->second.Left () > 0) {
							if (in_left2 < 0) {
								in_low = MIN ((in_thru1 + detail_itr->second.Left ()), in_thru2);
							} else if (detail_itr->second.Left () > (in_left2 - in_left1 + 1)) {
								in_low = MIN ((in_thru1 + detail_itr->second.Left () - (in_left2 - in_left1 + 1)), in_thru2);
							}
						}
						if (detail_itr->second.Thru () < (in_high - in_low + 1)) {
							if (detail_itr->second.Right () > 0) {
								if (in_right2 < 0) {
									in_high = MAX ((in_thru2 - detail_itr->second.Right ()), in_thru1);
								} else if (detail_itr->second.Right () > (in_right2 - in_right1 + 1)) {
									in_high = MAX ((in_thru2 - detail_itr->second.Right () - (in_right2 - in_right1 + 1)), in_thru1);
								}
							}
						}
					}
					break;
				case LEFT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (++left_lanes > 1) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru1;
							in_high= MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_low = in_high = in_thru1;
						}
						if (lns_thru < dir_itr->Lanes ()) {
							in_thru1 = MIN ((in_thru1 + 1), in_thru2);
						}
					} else if (in_left1 >= 0) {
						in_low = in_left1;

						if (detail_lane_flag && (num_thru > 0 || detail_itr->second.Thru () == 0)) {
							if (detail_itr->second.Left_Thru () > 0) {
								in_high = in_thru1;
							} else {
								in_high = in_left2;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_high = in_thru1;
						} else {
							in_high = in_left2;
						}
					} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
						in_low = in_thru1;

						if (detail_lane_flag && (detail_itr->second.Left_Thru () > 0 || 
							(num_thru == 0 && detail_itr->second.Thru () > 0))) {
							in_high = MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_high = in_low;
						}
					} else {
						in_low = in_high = in_thru1;
					}
					break;
				case RIGHT:
					if (out_right1 >= 0) {
						out_low = out_right1;
						out_high = out_right1;
					} else {
						out_low = MAX ((out_thru2 - 1), out_thru1);
						out_high = out_thru2;
					}
					if (++right_lanes < num_right) {
						in_high = in_thru2;

						if (detail_lane_flag && (num_thru > 0 || detail_itr->second.Thru () == 0)) {
							if (detail_itr->second.Right_Thru () > 0) {
								in_low = MAX ((in_thru2 - 1), in_thru1);
							} else {
								in_low = in_thru2;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = MAX ((in_thru2 - 1), in_thru1);
						} else {
							in_low = in_thru2;
						}
					} else if (in_right1 >= 0) {
						in_high = in_right2;

						if (detail_lane_flag && (num_thru > 0 || detail_itr->second.Thru () == 0)) {
							if (detail_itr->second.Right_Thru () > 0) {
								in_low = in_thru2;
							} else {
								in_low = in_right1;
							}
						} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru2;
						} else if (lns_rsplit > 1) {
							in_low = in_thru2;
						} else {
							in_low = in_right1;
						}
					} else if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
						in_high = in_thru2;

						if (detail_lane_flag && (num_thru > 0 || detail_itr->second.Thru () == 0)) {
							if (detail_itr->second.Right_Thru () > 0 && detail_itr->second.Right () > 0) {
								in_low = MAX ((in_thru2 - 1), in_thru1);
							} else {
								in_low = in_thru2;
							}
						} else {
							in_low = MAX ((in_thru2 - 1), in_thru1);
						}
					} else {
						in_low = in_high = in_thru2;
					}
					break;
				case L_SPLIT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (num_left > 0) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = in_thru1;
							in_high = MIN ((in_thru1 + 1), in_thru2);
						} else {
							in_low = in_high = in_thru1;
						}
					} else if (in_left1 >= 0) {
						in_low = in_left1;
						if (lns_lsplit > 1) {
							in_high = in_thru1;
						} else {
							in_high = in_left2;
						}
					} else {
						in_low = in_high = in_thru1;
					}
					break;
				case R_SPLIT:
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					if (num_right > 0) {
						if ((out_high - out_low) > 0 && lns_thru < dir_itr->Lanes ()) {
							in_low = MAX ((in_thru2 - 1), in_thru1);
							in_high = in_thru2;
						} else {
							in_low = in_high = in_thru2;
						}
					} else if (in_right1 >= 0) {
						if (lns_rsplit > 1) {
							in_low = in_thru2;
						} else {
							in_low = in_right1;
						}
						in_high = in_right2;
					} else if (lns_rsplit > 1 && lns_thru < dir_itr->Lanes ()) {
						in_low = MAX ((in_thru2 - 1), in_thru1);
						in_high = in_thru2;
					} else {
						in_low = in_high = in_thru2;
					}
					break;
				case L_MERGE:
					if (num_thru > 0) {
						if (in_left1 >= 0) {
							in_low = in_left1;
							in_high = in_left2;
						} else {
							in_low = in_high = in_thru1;
						}
					} else {
						if (in_left1 >= 0) {
							in_low = in_left1;
						} else {
							in_low = in_thru1;
						}
						if (in_right1 >= 0) {
							in_high = in_right2;
						} else {
							in_high = in_thru2;
						}
					}
					if (out_left1 >= 0) {
						out_low = out_left1;
						if (lns_lmerge > 2) {
							out_high = out_thru1;
						} else {
							out_high = out_left2;
						}
					} else {
						out_low = out_thru1;
						if (lns_lmerge > 2) {
							out_high = MIN ((out_thru1 + 1), out_thru2);
						} else {
							out_high = out_thru1;
						}
					}
					break;
				case R_MERGE:
					if (num_thru > 0) {
						if (in_left1 >= 0) {
							in_low = in_left1;
							in_high = in_left2;
						} else {
							in_low = in_high = in_thru1;
						}
					} else {
						if (in_left1 >= 0) {
							in_low = in_left1;
						} else {
							in_low = in_thru1;
						}
						if (in_right1 >= 0) {
							in_high = in_right2;
						} else {
							in_high = in_thru2;
						}
					}
					if (out_right1 >= 0) {
						if (lns_rmerge > 2) {
							out_low = out_thru2;
						} else {
							out_low = out_right1;
						}
						out_high = out_right1;
					} else {
						if (lns_rmerge > 2) {
							out_low = MAX ((out_thru2 - 1), out_thru1);
						} else {
							out_low = out_thru2;
						}
						out_high = out_thru2;
					}
					break;
				case UTURN:
					if (in_left1 >= 0) {
						in_low = in_left1;
						in_high = in_left2;
					} else {
						in_low = in_high = in_thru1;
					}
					if (out_left1 >= 0) {
						out_low = out_left1;
						out_high = out_thru1;
					} else {
						out_low = out_thru1;
						out_high = MIN ((out_thru1 + 1), out_thru2);
					}
					break;
				default:
					in_low = in_high = out_low = out_high = 0;
					break;
			}
			connect_ptr->Low_Lane (in_low);
			connect_ptr->High_Lane (in_high);
			connect_ptr->To_Low_Lane (out_low);
			connect_ptr->To_High_Lane (out_high);
		}
	}
	End_Progress ();
}
