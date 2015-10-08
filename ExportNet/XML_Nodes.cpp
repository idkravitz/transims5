//*********************************************************
//	XML_Nodes.cpp - convert the node data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Nodes
//---------------------------------------------------------

void ExportNet::XML_Nodes (void)
{
	int i, j, k, nlane, nout, dir, link1, link2, max_link, num_sign;
	int out, lane, pocket, length, max_len, index, num_right [20];
	String name, dir_char, to_char, flag_str;

	Int_Map_Itr map_itr;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr, *to_ptr;
	Connect_Data *connect_ptr;
	Signal_Data *signal_ptr;
	Signal_Time_Itr time_itr;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;
	Pocket_Data *pocket_ptr;
	List_Data *list_ptr, *next_ptr;
	Split_Array *split_ptr;

	fstream &fh = xml_file.File ();

	Set_Progress ();

	fh << "<NODES>" << endl;
	
	max_link = (int) link_splits.size ();

	for (map_itr = node_map.begin (); map_itr != node_map.end (); map_itr++) {
		Show_Progress ();

		node_ptr = &node_array [map_itr->second];
		if (node_ptr->Subarea () == 0) continue;
		
		list_ptr = &node_list [map_itr->second];

		fh << "<NODE NO=\"" << node_ptr->Node (); 

		name ("");

		//---- construct the intersection name ----

		link1 = link2 = -1;
		num_sign = 0;

		for (dir = list_ptr->To_List (); dir >= 0; dir = next_ptr->To_List ()) {
			dir_ptr = &dir_array [dir];
			next_ptr = &dir_list [dir];

			if (dir_ptr->Link () == link1 || dir_ptr->Link () == link2) continue;

			if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == ALL_STOP) num_sign++;

			link_ptr = &link_array [dir_ptr->Link ()];
			if (link_ptr->Name () == 0 || strlen (link_ptr->Name ()) <= 1) continue;
			if (name.Equals (link_ptr->Name ())) continue;

			if (link1 < 0) {
				link1 = link_ptr->Link ();
				name += link_ptr->Name ();
			} else if (link2 < 0) {
				link2 = link_ptr->Link ();
				name += " and ";
				name += link_ptr->Name ();
			}
		}

		if (link2 < 0) {
			for (dir = list_ptr->From_List (); dir >= 0; dir = next_ptr->From_List ()) {
				dir_ptr = &dir_array [dir];
				next_ptr = &dir_list [dir];
			
				if (dir_ptr->Link () == link1 || dir_ptr->Link () == link2) continue;

				link_ptr = &link_array [dir_ptr->Link ()];
				if (link_ptr->Name () == 0 || strlen (link_ptr->Name ()) <= 1) continue;
				if (name.Equals (link_ptr->Name ())) continue;

				if (link1 < 0) {
					link1 = link_ptr->Link ();
					name += link_ptr->Name ();
				} else {
					link2 = link_ptr->Link ();
					name += " and ";
					name += link_ptr->Name ();
					break;
				}
			}	
		}
		fh << "\" NAME=\"" << name;
		to_char ("%.1lf") % UnRound (node_ptr->X ());
		fh << "\" XCOORD=\"" << to_char;
		to_char ("%.1lf") % UnRound (node_ptr->Y ());
		fh << "\" YCOORD=\"" << to_char;
		fh << "\" CONTROLTYPE=\"";

		//---- node record ----

		signal_ptr = 0;
		if (num_sign == 0) {
			if (node_ptr->Control () >= 0) {
				fh << "Signalized";
				signal_ptr = &signal_array [node_ptr->Control ()];
			} else {
				fh << "Unknown";
			}
		} else if (num_sign > 2) {
			fh << "AllWayStop";
		} else {
			fh << "TwoWayStop";
		}
		fh << "\">" << endl;

		//---- lanes in and out of the node ----

		fh << "<LANES>" << endl;

		//---- approach lanes ----

		memset (num_right, '\0', sizeof (num_right));

		for (j=0, dir = list_ptr->To_List (); dir >= 0; dir = next_ptr->To_List (), j++) {
			dir_ptr = &dir_array [dir];
			next_ptr = &dir_list [dir];

			link_ptr = &link_array [dir_ptr->Link ()];
			max_len = (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ()) * 3 / 4;
	
			nlane = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right ();

			if (dir_ptr->Dir () == 0) {
				dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "B" : "A");
			} else {
				dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "A" : "B");
			}
			if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == ALL_STOP) {
				name (" TRAFFICSIGN=\"Stop\" ");
			} else if (dir_ptr->Sign () == YIELD_SIGN) {
				name (" TRAFFICSIGN=\"Yield\" ");
			} else {
				name (" ");
			}
			for (i=nlane, lane=1; i > 0; i--) {

				if (i > (dir_ptr->Left () + dir_ptr->Lanes ())) {

					//---- write right pocket lanes ----

					for (pocket = dir_ptr->First_Pocket (); pocket >= 0; pocket = pocket_ptr->Next_Index ()) {
						pocket_ptr = &pocket_array [pocket];

						if (pocket_ptr->Type () == RIGHT_TURN && 
							i <= (dir_ptr->Left () + dir_ptr->Lanes () + pocket_ptr->Lanes ())) {
							
							flag_str ("true");
							length = pocket_ptr->Length ();
							if (length > max_len) length = max_len; 
							to_char ("%.1lf") % UnRound (length);
							num_right [j]++;
							break;
						}
					}

				} else if (i <= dir_ptr->Left ()) {

					//---- write left pocket lanes ----

					for (pocket = dir_ptr->First_Pocket (); pocket >= 0; pocket = pocket_ptr->Next_Index ()) {
						pocket_ptr = &pocket_array [pocket];

						if (pocket_ptr->Type () == LEFT_TURN && 
							i > dir_ptr->Left () - pocket_ptr->Lanes ()) {
							
							flag_str ("true");
							length = pocket_ptr->Length ();
							if (length > max_len) length = max_len;
							to_char ("%.1lf") % UnRound (length);
							break;
						}
					}

				} else {

					//---- write thru lanes ----

					pocket = 0;
					flag_str ("false");
					to_char ("0.0");
				}
				if (pocket >= 0) {
					fh << "<LANE LINKID=\"" << link_ptr->Link () << dir_char << "\" INDEX=\"" << lane++;
					fh << "\" POCKET=\"" << flag_str << "\" POCKETLENGTH=\"" << to_char;
					fh << "\" WIDTH=\"" << ((Metric_Flag ()) ? 3.7 : 12.0);
					fh << "\"" << name << "/>" << endl;
				}
			}
		}

		//---- departure lanes ----

		for (dir = list_ptr->From_List (); dir >= 0; dir = next_ptr->From_List ()) {
			dir_ptr = &dir_array [dir];
			next_ptr = &dir_list [dir];

			link_ptr = &link_array [dir_ptr->Link ()];
			max_len = (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ()) * 3 / 4;

			nlane = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right ();

			if (dir_ptr->Dir () == 0) {
				dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "B" : "A");
			} else {
				dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "A" : "B");
			}

			for (i=nlane, lane=1; i > 0; i--) {

				if (i > (dir_ptr->Left () + dir_ptr->Lanes ())) {

					//---- write right pocket lanes ----
					
					for (pocket = dir_ptr->First_Pocket (); pocket >= 0; pocket = pocket_ptr->Next_Index ()) {
						pocket_ptr = &pocket_array [pocket];

						if (pocket_ptr->Type () == RIGHT_MERGE && 
							i <= (dir_ptr->Left () + dir_ptr->Lanes () + pocket_ptr->Lanes ())) {
							
							flag_str ("true");
							length = pocket_ptr->Length ();
							if (length > max_len) length = max_len;
							to_char ("%.1lf") % UnRound (length);
							break;
						}
					}

				} else if (i <= dir_ptr->Left ()) {

					//---- write left pocket lanes ----

					for (pocket = dir_ptr->First_Pocket (); pocket >= 0; pocket = pocket_ptr->Next_Index ()) {
						pocket_ptr = &pocket_array [pocket];

						if (pocket_ptr->Type () == LEFT_MERGE && 
							i > dir_ptr->Left () - pocket_ptr->Lanes ()) {
							
							flag_str ("true");
							length = pocket_ptr->Length ();
							if (length > max_len) length = max_len;
							to_char ("%.1lf") % UnRound (length);
							break;
						}
					}

				} else {

					//---- write thru lanes ----

					pocket = 0;
					flag_str ("false");
					to_char ("0.0");
				}
				if (pocket >= 0) {
					fh << "<LANE LINKID=\"" << link_ptr->Link () << dir_char << "\" INDEX=\"" << lane++;
					fh << "\" POCKET=\"" << flag_str << "\" POCKETLENGTH=\"" << to_char;
					fh << "\" WIDTH=\"" << ((Metric_Flag ()) ? 3.7 : 12.0);
					fh << "\" />" << endl;
				}
			}
		}
		fh << "</LANES>" << endl;

		if (node_ptr->Subarea () == 2) {
			fh << "<LANETURNS />" << endl;
		} else {

			//---- lane connectivity options ----

			fh << "<LANETURNS>" << endl;

			//---- approach lanes ----

			for (j=0, dir = list_ptr->To_List (); dir >= 0; dir = next_ptr->To_List (), j++) {
				dir_ptr = &dir_array [dir];
				next_ptr = &dir_list [dir];

				link_ptr = &link_array [dir_ptr->Link ()];
				link1 = link_ptr->Link ();
			
				nlane = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right ();
				lane = dir_ptr->Right () - num_right [j];

				if (dir_ptr->Dir () == 0) {
					dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "B" : "A");
				} else {
					dir_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "A" : "B");
				}

				for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [index];

					to_ptr = &dir_array [connect_ptr->To_Index ()];

					link_ptr = &link_array [to_ptr->Link ()];

					nout = to_ptr->Left () + to_ptr->Lanes () + to_ptr->Right ();
					out = (link_ptr->Divided () != 0) ? to_ptr->Right () : 0;

					if (link_ptr->Divided () == 0) continue;
					if (to_ptr->Link () < max_link) {
						split_ptr = &link_splits [to_ptr->Link ()];
						if (split_ptr->size () != 0) continue;
					}
					if (to_ptr->Dir () == 0) {
						to_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "B" : "A");
					} else {
						to_char ((link_ptr->Anode () > link_ptr->Bnode ()) ? "A" : "B");
					}
					name (" ");

					if (signal_ptr != 0) {
						for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
							if (time_itr->Start () <= time_of_day && time_of_day <= time_itr->End ()) {
								for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
									if (phasing_itr->Phasing () == time_itr->Phasing ()) {
										for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
											if (move_itr->Connection () == index) {
												name (" SCNO=\"%d\" SGNO=\"%d\" ") % signal_ptr->Signal () % phasing_itr->Phase ();
												break;
											}
										}
										if (name.length () > 1) break;
									}
								}
								break;
							}
						}
					}

					for (i=connect_ptr->Low_Lane (), k=connect_ptr->To_Low_Lane (); i <= connect_ptr->High_Lane (); i++, k++) {
						if (k > connect_ptr->To_High_Lane ()) k = connect_ptr->To_High_Lane ();
						fh << "<LANETURN FROMLINKID=\"" << link1 << dir_char;
						fh << "\" FROMLANEINDEX=\"" << (nlane - i - lane);
						fh << "\" TOLINKID=\"" << link_ptr->Link () << to_char;
						fh << "\" TOLANEINDEX=\"" << (nout - k - out);
						fh << "\"" << name << "/>" << endl;
					}
				}
			}

			fh << "</LANETURNS>" << endl;
		}
		fh << "<CROSSWALKS />" << endl;

		fh << "</NODE>" << endl;
	}
	fh << "</NODES>" << endl;
}
