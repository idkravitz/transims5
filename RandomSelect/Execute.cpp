//*********************************************************
//	RandomSelect.cpp - main execution procedure
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void RandomSelect::Execute (void)
{
	int last_hhold, last_part, part;
	double share, total, prob;
	Select_Map_Itr map_itr;
	Trip_Index trip_index;
	Int_Itr int_itr;

	//---- read the network data ----

	Data_Service::Execute ();

	//---- select a partition ----

	last_hhold = last_part = 0;
	share = 1.0 / num_parts;

	for (map_itr = select_map.begin (); map_itr != select_map.end (); map_itr++) {
		trip_index = map_itr->first;

		if (last_hhold != trip_index.Household ()) {
			last_hhold = trip_index.Household ();

			prob = random.Probability ();
			total = share;

			for (part=0; part < num_parts; part++, total += share) {
				if (total > prob) break;
			}
			last_part = part;
		}
		map_itr->second.Partition (last_part);
		part_count [last_part]++;
	}

	//---- write the selection file ----

	Write_Selections ();

	//---- report the records per partition ----

	total = select_map.size () / 100.0;
	if (total == 0.0) total = 1.0;

	Break_Check (num_parts + 3);
	Write (2, "Partition Distribution");

	for (part=0, int_itr = part_count.begin (); int_itr != part_count.end (); int_itr++, part++) {
		Write (1, String ("%5d %10d (%5.1lf%%)") % part % *int_itr % (*int_itr / total) % FINISH);
	}
	Write (1, String ("Total %10d") % (int) select_map.size ());

	//---- end the program ----

	Exit_Stat (DONE);
}
