//*********************************************************
//	Write_Turn_Vol.cpp - Write the Turn Volume File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Turn_Volume
//---------------------------------------------------------

void LinkSum::Write_Turn_Volume (void)
{
	int nrec, node, index, period, nperiod;
	Dtime low, high;
	bool select_flag;

	Ints_Map node_sort;
	Ints_Map_Itr sort_itr;
	Integers ints_rec;
	Int_Itr int_itr;
	Int_Map_Itr itr;
	Dir_Itr dir_itr;
	Dir_Data *dir_ptr, *to_dir;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Connect_Data *connect_ptr;
	Flow_Time_Array *flow_time_ptr;
	Flow_Time_Data *flow_ptr;

	Show_Message (String ("Writing %s -- Record") % turn_vol_file.File_Type ());
	Set_Progress ();

	//---- construct the node link liste ----

	select_flag = (turn_range.size () > 0);
	nperiod = time_periods.Num_Periods ();

	if ((int) turn_perf_array.size () < nperiod) {
		Write (1);
		Warning ("The Performance File does not included Turning Movements");
		return;
	}

	for (itr = node_map.begin (); itr != node_map.end (); itr++) {
		if (select_flag && !turn_range.In_Range (itr->first)) continue;

		node_sort.insert (Ints_Map_Data (itr->first, ints_rec));
	}

	//---- assign links to nodes ----

	for (index=0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, index++) {
		link_ptr = &link_array [dir_itr->Link ()];

		node = (dir_itr->Dir () == 0) ? link_ptr->Bnode () : link_ptr->Anode ();

		node_ptr = &node_array [node];

		sort_itr = node_sort.find (node_ptr->Node ());
		if (sort_itr == node_sort.end ()) continue;

		sort_itr->second.push_back (index);
	}

	//---- process each node ----

	nrec = 0;

	for (sort_itr = node_sort.begin (); sort_itr != node_sort.end (); sort_itr++) {
		Show_Progress ();

		turn_vol_file.Node (sort_itr->first);

		for (int_itr = sort_itr->second.begin (); int_itr != sort_itr->second.end (); int_itr++) {
			dir_ptr = &dir_array [*int_itr];

			link_ptr = &link_array [dir_ptr->Link ()];

			turn_vol_file.Link (link_ptr->Link ());

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				to_dir = &dir_array [connect_ptr->To_Index ()];
				link_ptr = &link_array [to_dir->Link ()];

				turn_vol_file.To_Link (link_ptr->Link ());

				for (period=0; period < nperiod; period++) {
					flow_time_ptr = &turn_perf_array [period];
					if (index >= (int) flow_time_ptr->size ()) continue;

					flow_ptr = &flow_time_ptr->at (index);

					time_periods.Period_Range (period, low, high);

					turn_vol_file.Start (low);
					turn_vol_file.End (high);
					turn_vol_file.Volume (DTOI (flow_ptr->Flow ()));

					if (!turn_vol_file.Write ()) {
						Error (String ("Writing %s") % turn_vol_file.File_Type ());
					}
					nrec++;
				}
			}
		}
	}
	End_Progress ();

	Print (2, "Number of Turn Volume Records = ") << nrec;
}
