//*********************************************************
//	Zone_Summary.cpp - summarize activity locations on links
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Zone_Summary
//---------------------------------------------------------

void LinkSum::Zone_Summary (void)
{
	int i, j, z, count, link, nrec, nzone, index, flow_index;
	double factor, vmt, vht, flow;
	Dtime time;

	Link_Data *link_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Data flow_data;

	Link_Loc_Map_Itr loc_itr, first_itr;
	Doubles dbl;
	Doubles_Itr itr;
	Int_Map_Itr map_itr;

	Show_Message ("Summarize Activity Locations on Links -- Record");
	Set_Progress ();

	link = count = nrec = 0;
	link_ptr = 0;

	for (loc_itr = first_itr = link_location.begin (); loc_itr != link_location.end (); loc_itr++) {
		Show_Progress ();

		if (loc_itr->second.link == link) {
			count++;
		} else {
			if (link > 0) {
				link_ptr = &link_array [link];

				if (!select_flag || link_ptr->Use () != 0) {
					first_itr->second.count = count;
				}
			}
			first_itr = loc_itr;
			link = loc_itr->second.link;
			count = 1;
		}
	}
	Set_Progress ();

	//---- set the count data for all records ----

	link = 0;

	for (loc_itr = link_location.begin (); loc_itr != link_location.end (); loc_itr++) {
		Show_Progress ();

		if (loc_itr->second.link == link) {
			loc_itr->second.count = count;
		} else {
			link = loc_itr->second.link;
			count = loc_itr->second.count;
		}
		if (count > 0) {
			nrec += count;
			zone_list.insert (Int_Map_Data (loc_itr->second.zone, (int) zone_list.size ()));
		}
	}
	nzone = (int) zone_list.size ();

	Break_Check (4);

	Print (2, "Number of Link Locations = ") << nrec;
	Print (1, "Number of Selected Zones = ") << nzone;

	//---- allocate space for zone summaries ----

	zone_vmt.assign (num_inc, dbl);
	zone_vht.assign (num_inc, dbl);

	for (itr = zone_vmt.begin (); itr != zone_vmt.end (); itr++) {
		itr->assign (nzone, 0.0);
	}
	for (itr = zone_vht.begin (); itr != zone_vht.end (); itr++) {
		itr->assign (nzone, 0.0);
	}

	//---- summarizes link data by zone ----

	link = 0;

	for (loc_itr = link_location.begin (); loc_itr != link_location.end (); loc_itr++) {
		Show_Progress ();

		if (loc_itr->second.count == 0) continue;
		
		if (loc_itr->second.link != link) {
			link = loc_itr->second.link;
			link_ptr = &link_array [link];
		}
		map_itr = zone_list.find (loc_itr->second.zone);
		if (map_itr == zone_list.end ()) continue;

		z = map_itr->second;

		factor = UnRound (link_ptr->Length ()) / loc_itr->second.count; 

		//---- process each direction ----

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_ptr->Use () == -1) continue;
				index = link_ptr->BA_Dir ();
			} else {
				if (link_ptr->Use () == -2) continue;
				index = link_ptr->AB_Dir ();
			}
			if (index < 0) continue;
			flow_index = dir_array [index].Flow_Index ();

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				flow = flow_data.Flow ();
				time = flow_data.Time ();

				if (flow > 0) {
					vmt = flow * factor;
					vht = flow * time / loc_itr->second.count;

					zone_vmt [j] [z] += vmt;
					zone_vht [j] [z] += vht;
				}
			}
		}
	}
	End_Progress ();
}
