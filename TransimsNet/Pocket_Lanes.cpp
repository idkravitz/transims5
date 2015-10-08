//*********************************************************
//	Pocket_Lanes.cpp - create pocket lanes
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Pocket_Lanes
//---------------------------------------------------------

void TransimsNet::Pocket_Lanes (void)
{
	int i, index, dir_index, to_index, turn_type, pocket_type, length, len, max_len, lanes, thru, num;
	bool left_flag, left_turn, right_turn, left_merge, right_merge, detail_lane_flag;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *to_ptr;
	Connect_Data *connect_ptr;
	Int_Map_Stat map_stat;
	Pocket_Index map_index;
	Pocket_Warrant_Map_Itr map_itr;
	Pocket_Warrant *warrant_ptr;
	Pocket_Data pocket_rec, *pocket_ptr;;
	Link_Detail_Map_Itr detail_itr;

	Show_Message (String ("Creating Pocket Lanes -- Record"));
	Set_Progress ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Type () == EXTERNAL || !Use_Permission (link_itr->Use (), CAR)) continue;

		map_index.Area_Type (link_itr->Area_Type ());

		max_len = link_itr->Length () - link_itr->Aoffset () - link_itr->Boffset ();

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			if (i == 0) {
				dir_index = link_itr->AB_Dir ();
			} else {
				dir_index = link_itr->BA_Dir ();
			}
			if (dir_index < 0) continue;

			dir_ptr = &dir_array [dir_index];
			left_turn = right_turn = left_merge = right_merge = detail_lane_flag = false;
			thru = 0;

			if (link_detail_flag) {
				detail_itr = detail_map.find (dir_index);
				if (detail_itr != detail_map.end ()) {
					thru = detail_itr->second.Left_Thru () + detail_itr->second.Thru () + detail_itr->second.Right_Thru ();
					lanes = detail_itr->second.Left () + thru + detail_itr->second.Right () + 
						detail_itr->second.Left_Merge () + detail_itr->second.Right_Merge ();
					detail_lane_flag = (lanes > 0);
				}
			}

			//---- process turn pockets ----
		
			map_index.From_Type (link_itr->Type ());

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];
			
				turn_type = connect_ptr->Type ();

				if (turn_type == THRU || turn_type == UTURN) continue;

				pocket_type = -1;
				left_flag = false;
				length = lanes = 0;

				to_index = connect_ptr->To_Index ();
				to_ptr = &dir_array [to_index];

				link_ptr = &link_array [to_ptr->Link ()];

				map_index.To_Type (link_ptr->Type ());

				map_itr = pocket_warrant_map.find (map_index);

				if (map_itr != pocket_warrant_map.end ()) {
					warrant_ptr = &pocket_warrants [map_itr->second];

					//---- check the pocket criteria ----

					switch (turn_type) {
						case LEFT:
						case L_SPLIT:
						case L_MERGE:
							left_flag = true;
							pocket_type = LEFT_TURN;
							length = warrant_ptr->Left_Length ();
							lanes = warrant_ptr->Left_Lanes ();
							break;
						case RIGHT:
						case R_SPLIT:
						case R_MERGE:
							pocket_type = RIGHT_TURN;
							length = warrant_ptr->Right_Length ();
							lanes = warrant_ptr->Right_Lanes ();
							break;
						default:
							pocket_type = AUX_LANE;
							length = lanes = 0;
							break;
					}
				} else {
					if (!detail_lane_flag) continue;
					if (length == 0) {
						length = MAX (link_itr->Aoffset (), link_itr->Boffset ()) * 3;
						if (length > link_itr->Length () / 2) length = link_itr->Length () / 2;
					}
					if (length == 0) length = link_itr->Length () / 3;
				}
				if (detail_lane_flag) {
					pocket_type = AUX_LANE;
					len = lanes = 0;

					switch (turn_type) {
						case LEFT:
						case L_SPLIT:
						case L_MERGE:
							if (detail_itr->second.Left () > 0 && 
								(dir_ptr->Lanes () <= thru || detail_itr->second.Right () > 0)) {

								left_flag = true;
								pocket_type = LEFT_TURN;
								len = detail_itr->second.LT_Length ();
								lanes = detail_itr->second.Left ();
							}
							break;
						case RIGHT:
						case R_SPLIT:
						case R_MERGE:
							if (detail_itr->second.Right () > 0 && dir_ptr->Lanes () <= thru) {
								pocket_type = RIGHT_TURN;
								len = detail_itr->second.RT_Length ();
								lanes = detail_itr->second.Right ();
							}
							break;
						default:
							break;
					}
					if (len > 0) length = len;

					if (length == 0 && lanes > 0) {
						length = MAX (link_itr->Aoffset (), link_itr->Boffset ()) * 3;
						if (length > link_itr->Length () / 2) length = link_itr->Length () / 2;
					}
					if (length == 0) length = link_itr->Length () / 3;
				}
				if (lanes == 0 || length == 0 || max_len < length / 2) continue;

				if (length >= max_len) {
					length = max_len * 9 / 10;
				}
				if (pocket_type == LEFT_TURN || pocket_type == RIGHT_TURN) {
					if (pocket_type == LEFT_TURN) {
						if (left_turn) continue;
						left_turn = true;
					} else {
						if (right_turn) continue;
						right_turn = true;
					}
					len = (dir_ptr->Dir () == 0) ? link_itr->Boffset () : link_itr->Aoffset ();
					pocket_rec.Length (length + len);
					pocket_rec.Offset (link_itr->Length () - pocket_rec.Length ());
				} else {
					pocket_rec.Length (length);
					pocket_rec.Offset (0);
				}
				num = (int) pocket_array.size ();
					
				pocket_rec.Dir_Index (dir_index);
				pocket_rec.Next_Index (dir_ptr->First_Pocket ());
				dir_ptr->First_Pocket (num);

				pocket_rec.Type (pocket_type);
				pocket_rec.Lanes (lanes);

				if (left_flag) {
					if (dir_ptr->Left () < pocket_rec.Lanes ()) {
						dir_ptr->Left (pocket_rec.Lanes ());
					}
				} else if (dir_ptr->Right () < pocket_rec.Lanes ()) {
					dir_ptr->Right (pocket_rec.Lanes ());
				}
				pocket_array.push_back (pocket_rec);
				npocket++;
			}

			//---- process merge pockets ----
		
			map_index.To_Type (link_itr->Type ());

			for (index = dir_ptr->First_Connect_From (); index >= 0; index = connect_ptr->Next_From ()) {
				connect_ptr = &connect_array [index];
			
				turn_type = connect_ptr->Type ();

				if (turn_type == THRU || turn_type == UTURN) continue;

				pocket_type = -1;
				left_flag = false;
				length = lanes = 0;

				to_index = connect_ptr->Dir_Index ();
				to_ptr = &dir_array [to_index];

				link_ptr = &link_array [to_ptr->Link ()];

				map_index.From_Type (link_ptr->Type ());

				map_itr = pocket_warrant_map.find (map_index);

				if (map_itr != pocket_warrant_map.end ()){
					warrant_ptr = &pocket_warrants [map_itr->second];

					//---- check the pocket criteria ----

					switch (turn_type) {
						case L_MERGE:
						case LEFT:
						case L_SPLIT:
							left_flag = true;
						case R_MERGE:
						case RIGHT:
						case R_SPLIT:
							pocket_type = (left_flag) ? LEFT_MERGE : RIGHT_MERGE;
							length = warrant_ptr->Merge_Length ();
							lanes = warrant_ptr->Merge_Lanes ();
							break;
						default:
							break;
					}
				} else {
					if (!detail_lane_flag) continue;
					if (length == 0) {
						length = MAX (link_itr->Aoffset (), link_itr->Boffset ()) * 3;
						if (length > link_itr->Length () / 2) length = link_itr->Length () / 2;
					}
					if (length == 0) length = link_itr->Length () / 3;
				}
				if (detail_lane_flag) {
					len = 0;

					switch (turn_type) {
						case L_MERGE:
						case LEFT:
						case L_SPLIT:
							left_flag = true;
							pocket_type = LEFT_MERGE;
							len = detail_itr->second.LM_Length ();
							lanes = detail_itr->second.Left_Merge ();
							break;
						case R_MERGE:
						case RIGHT:
						case R_SPLIT:
							pocket_type = RIGHT_MERGE;
							len = detail_itr->second.RM_Length ();
							lanes = detail_itr->second.Right_Merge ();
							break;
						default:
							len = lanes = 0;
							break;
					}
					if (len > 0) length = len;
					if (length == 0) length = max_len / 3;
				}
				if (lanes == 0 || length == 0 || max_len < length / 2) continue;

				if (length >= max_len) {
					length = max_len * 9 / 10;
				}
				if (pocket_type == LEFT_MERGE) {
					if (left_merge) continue;
					left_merge = true;
				} else {
					if (right_merge) continue;
					right_merge = true;
				}
				len = (dir_ptr->Dir () == 0) ? link_itr->Aoffset () : link_itr->Boffset ();
				pocket_rec.Offset (0);
				pocket_rec.Length (length + len);

				num = (int) pocket_array.size ();
					
				pocket_rec.Dir_Index (dir_index);
				pocket_rec.Next_Index (dir_ptr->First_Pocket ());
				dir_ptr->First_Pocket (num);

				pocket_rec.Type (pocket_type);
				pocket_rec.Lanes (lanes);

				if (left_flag) {
					if (dir_ptr->Left () < pocket_rec.Lanes ()) {
						dir_ptr->Left (pocket_rec.Lanes ());
					}
				} else if (dir_ptr->Right () < pocket_rec.Lanes ()) {
					dir_ptr->Right (pocket_rec.Lanes ());
				}
				pocket_array.push_back (pocket_rec);
				npocket++;
			}

			//---- adjust the pocket lengths ----

			len = length = 0;

			for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
					length += pocket_ptr->Length ();
				} else if (pocket_ptr->Type () == RIGHT_TURN || pocket_ptr->Type () == RIGHT_MERGE) {
					len += pocket_ptr->Length ();
				}
			}
			if (len > link_itr->Length () || length > link_itr->Length ()) {
				for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
					pocket_ptr = &pocket_array [index];

					if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
						if (length > link_itr->Length ()) {
							pocket_ptr->Length (pocket_ptr->Length () * link_itr->Length () / length);
						}
					} else if (pocket_ptr->Type () == RIGHT_TURN || pocket_ptr->Type () == RIGHT_MERGE) {
						if (len > link_itr->Length ()) {
							pocket_ptr->Length (pocket_ptr->Length () * link_itr->Length () / len);
						}
					}
				}
			}
		}
	}
	End_Progress ();
	return;
}
