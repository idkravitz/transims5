//*********************************************************
//	Select_Plans.cpp - select the plans to output
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Select_Plans
//---------------------------------------------------------

void PlanSelect::Select_Plans (void)
{
	double share;
	bool select;

	Select_Map_Itr map_itr;

	num_select = (int) select_map.size ();

	Print (2, "Total Number of Trips = ") << num_trips;
	Print (1, "Number of Trips Selected = ") << num_select;
	if (num_trips > 0) {
		Print (0, String (" (%.1lf%%)") % (num_select * 100.0 / num_trips) % FINISH);
	}
	if (num_select == 0) return;

	select = percent_flag;
	share = select_percent;

	//---- check the maximum selection ----

	if (num_select * share / num_trips > max_percent_select) {
		select = true;
		share = max_percent_select * num_trips / num_select;
	}

	//---- mark the selected trips ----

	num_select = 0;

	for (map_itr = select_map.begin (); map_itr != select_map.end (); map_itr++) {
		if (select) {
			if (random.Probability () > share) {
				map_itr->second.Partition (-1);
				continue;
			}
		}
		num_select++;
	}
	Print (1, "Number of Trips Written = ") << num_select;
	Print (0, String (" (%.1lf%%)") % (num_select * 100.0 / num_trips) % FINISH);
}
