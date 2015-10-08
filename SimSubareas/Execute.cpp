//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SimSubareas::Execute (void)
{
	int i, num;
	Int_Itr int_itr;

	//---- read the network data ----

	Data_Service::Execute ();

	//---- distribute nodes to subareas ----

	if (boundary_flag) {
		Boundary_Nodes ();
	} else {
		Distribute_Nodes ();
	}

	//---- summarize the subarea counts ----

	for (Node_Itr node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () >= num_subareas) {
			Warning ("Subarea ") << (node_itr->Subarea () + 1) << " > " << num_subareas;
			continue;
		}
		if (node_itr->Subarea () >= 0) subarea_count [node_itr->Subarea ()]++;
	}
	Write (1);
	num = (int) node_array.size ();

	for (i=0, int_itr = subarea_count.begin (); int_itr != subarea_count.end (); int_itr++, i++) {
		Write (1, String ("Subarea %d Node Count = %d  (%.1lf%%)") % i % *int_itr % (*int_itr * 100.0 / num) % FINISH);
	}
	Show_Message (1);

	//---- distribute subarea nodes to partitions ----

	if (num_parts > 1) {
		for (i=0; i < num_subareas; i++) {
			Partition_Nodes (i);
		}
	}

	//---- write the node file ----

	Write_Nodes ();

	//---- end the program ----

	Exit_Stat (DONE);
}
