//*********************************************************
//	Select_Plans.cpp - select the plans to output
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Select_Plans
//---------------------------------------------------------

void PlanCompare::Select_Plans (void)
{
	int index;
	double share, total;
	bool select;

	Select_Map_Itr map_itr;
	NInt_Map sort_map;
	NInt_Map_RItr sort_ritr;
	Integers selection_set;

	num_select = (int) selected.size ();

	if (Num_Threads () > 1) Write (1); else Print (1);

	Write (1, "Total Number of Trips = ") << num_trips;
	Write (1, "Number of Trips Selected = ") << num_select;
	if (num_trips > 0) {
		Write (0, String (" (%.1lf%%)") % (num_select * 100.0 / num_trips) % FINISH);
	}
	if (num_select == 0) return;

	select = percent_flag;
	share = select_percent;

	//---- check the maximum selection ----

	if (num_select * share / num_trips > max_percent_select) {
		select = true;
		share = max_percent_select * num_trips / num_select;
		num_select = DTOI (num_trips * max_percent_select);
	} else if (share < 1.0) {
		select = true;
		num_select = DTOI (num_select * share);
	}

	//---- selection method ----

	if (method == PERCENT_METHOD && select) {

		//---- sort by percent difference ----

		for (index = 0, map_itr = selected.begin (); map_itr != selected.end (); map_itr++, index++) {
			sort_map.insert (NInt_Map_Data (map_itr->second.Type (), index));
		}
		index = 0;
		selection_set.assign (selected.size (), 0);

		for (sort_ritr = sort_map.rbegin (); sort_ritr != sort_map.rend (); sort_ritr++, num_select--) {
			if (num_select > 0) {
				index = sort_ritr->second;
				selection_set [index] = 1;
			} else {
				break;
			}
		}
		num_select = 0;

		for (index = 0, map_itr = selected.begin (); map_itr != selected.end (); map_itr++, index++) {
			if (selection_set [index] == 0) {
				map_itr->second.Partition (-1);
			} else {
				num_select++;
			}
		}

	} else if (method == GAP_METHOD && select) {

		//---- sort by gap value ----

		for (index = 0, map_itr = selected.begin (); map_itr != selected.end (); map_itr++, index++) {
			sort_map.insert (NInt_Map_Data (map_itr->second.Type (), index));
		}
		index = num_select;

		for (sort_ritr = sort_map.rbegin (); sort_ritr != sort_map.rend (); sort_ritr++, index--) {
			if (index == 0) {
				share = sort_ritr->first;
				break;
			}
		}
		if (share == 0) share = 0.001;

		//---- calculate the total gap probabilities ----

		for (index = 0, map_itr = selected.begin (); map_itr != selected.end (); map_itr++) {
			if (index < num_select) {
				total = map_itr->second.Type ();

				if (total >= share) {
					total = (total / share) + (random.Probability () - 0.5);
					select = (total > 1.0);
				} else if (total > 0) {
					total = (share / total) - (random.Probability () - 0.5);
					select = (total < 1.0);
				} else {
					select = false;
				}
				if (select) {
					index++;
				} else {
					map_itr->second.Partition (-1);
				}
			} else {
				map_itr->second.Partition (-1);
			}
		}

	} else if (select) {

		//---- mark the selected trips ----

		for (index = 0, map_itr = selected.begin (); map_itr != selected.end (); map_itr++) {
			if (index < num_select) {
				if (random.Probability () > share) {
					map_itr->second.Partition (-1);
				} else {
					index++;
				}
			} else {
				map_itr->second.Partition (-1);
			}
		}
	}
	Write (1, "Number of Trips Written = ") << num_select;
	Write (0, String (" (%.1lf%%)") % (num_select * 100.0 / num_trips) % FINISH);
	Show_Message (1);
}
