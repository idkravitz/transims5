//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ProblemSelect.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ProblemSelect::Execute (void)
{
	int num_select, max_select, max_rec;
	double share;
	bool share_flag, select_flag;

	Select_Map_Itr map_itr;

	//---- read the network ----

	Data_Service::Execute ();

	//---- select problems ----

	select_flag = System_File_Flag (SELECTION);
	max_select = (int) select_map.size ();
	max_rec = (select_flag) ? max_select : max_problem;

	if (num_problem > 0) {
		share_flag = percent_flag;
		share = select_percent;

		//---- check the maximum selection ----

		if (num_problem * share / max_rec > max_percent_select) {
			share_flag = true;
			share = max_percent_select * max_rec / num_problem;
		}

		//---- mark the selected trips ----

		num_select = 0;

		for (map_itr = select_map.begin (); map_itr != select_map.end (); map_itr++) {
			if (select_flag && map_itr->second.Type () >= 0) continue;
			if (share_flag) {
				if (random.Probability () > share) {
					map_itr->second.Partition (-1);
					continue;
				}
			}
			if (map_itr->second.Type () < 0) map_itr->second.Type (-map_itr->second.Type ());
			num_select++;
		}
	} else {
		num_select = 0;
	}

	//---- write the selection file ----

	Write_Selections ();

	//---- write summary statistics ----

	Break_Check (6);

	Write (2, "Number of Problem Records = ") << max_problem;
	Write (1, "Number of Problems Selected = ") << num_problem;
	if (max_problem > 0) Write (0, String (" (%.1lf%%)") % (100.0 * num_problem / max_problem) % FINISH);

	if (System_File_Flag (SELECTION)) {
		Write (2, "Number of Selection Records = ") << max_select;
		Write (1, "Number of Records Selected = ") << num_select;
		if (max_select > 0) Write (0, String (" (%.1lf%%)") % (100.0 * num_select / max_select) % FINISH);
	} else {
		Write (1, "Number of Selection Records = ") << num_select;
		if (num_problem > 0) Write (0, String (" (%.1lf%%)") % (100.0 * num_select / num_problem) % FINISH);
	}
	Exit_Stat (DONE);
}
