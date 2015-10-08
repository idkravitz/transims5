//*********************************************************
//	Read_Turn.cpp - Read the Turn Movement File
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Read_Turn
//---------------------------------------------------------

void IntControl::Read_Turn (void)
{
	int i, link, n, node, dir, periods, num;
	Dtime start, end;

	Link_Data *link_ptr;
	Flow_Time_Array array_rec, *array_ptr;
	Flow_Time_Data data_rec, *flow_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Int2_Key key;

	periods = sum_periods.Num_Periods ();

	if (turn_delay_array.size () == 0) {
		num = (int) connect_array.size ();

		if (num > 0) {
			turn_delay_array.assign (periods, array_rec);

			for (i=0; i < periods; i++) {
				array_ptr = &turn_delay_array [i];
				array_ptr->assign (num, data_rec);
			}
		}
	}

	//---- read the volume file ----

	Show_Message (String ("Reading %s -- Record") % turn_file.File_Type ());
	Set_Progress ();

	while (turn_file.Read ()) {
		Show_Progress ();

		node = turn_file.Node ();
		if (node == 0) continue;

		map_itr = node_map.find (node);
		if (map_itr == node_map.end ()) {
			Error (String ("Turn Node %d was Not Found in the Node File") % node);
		}
		n = node;
		node = map_itr->second;

		//---- identify the approach link ----

		link = turn_file.Link ();

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Error (String ("Turn Link %d was Not Found in the Link File") % link);
		}
		link_ptr = &link_array [map_itr->second];

		if (link_ptr->Anode () == node) {
			dir = link_ptr->BA_Dir ();
		} else if (link_ptr->Bnode () == node) {
			dir = link_ptr->AB_Dir ();
		} else {
			dir = -1;
		}
		if (dir < 0) {
			Error (String ("Turn Link %d does Not Approach Node %d") % link_ptr->Link () % n);
		}
		key.first = dir;

		//---- identify the departure link ----

		link = turn_file.To_Link ();

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Error (String ("Turn Link %d was Not Found in the Link File") % link);
		}
		link_ptr = &link_array [map_itr->second];

		if (link_ptr->Anode () == node) {
			dir = link_ptr->AB_Dir ();
		} else if (link_ptr->Bnode () == node) {
			dir = link_ptr->BA_Dir ();
		} else {
			dir = -1;
		}
		if (dir < 0) {
			Error (String ("Turn Link %d does Not Depart from Node %d") % link_ptr->Link () % n);
		}
		key.second = dir;

		//---- get the connection record ----

		map2_itr = connect_map.find (key);
		if (map2_itr == connect_map.end ()) {
			Error (String ("No Connection between Link %d to %d") % turn_file.Link () % link);
		}

		//---- save the volume data ----

		start = turn_file.Start ();
		end = turn_file.End ();

		i = sum_periods.Period ((start + end) / 2);
		array_ptr = &turn_delay_array [i];
		flow_ptr = &array_ptr->at (map2_itr->second);
		flow_ptr->Add_Flow (turn_file.Volume ());
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % turn_file.File_Type () % Progress_Count ());

	turn_file.Close ();
}

