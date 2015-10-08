//*********************************************************
//	Boundary_Nodes.cpp - assign nodes to subareas boundaries
//*********************************************************

#include "SimSubareas.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Boundary_Nodes
//---------------------------------------------------------

void SimSubareas::Boundary_Nodes (void)
{
	bool first;
	int subarea, min_sub, max_sub, count;
	Node_Itr node_itr;

	Show_Message ("Subarea Boundary Processing -- Subarea");
	Set_Progress ();

	//---- initialize the subarea data ----

	subarea = max_sub = -1;
	min_sub = MAX_INTEGER;

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		node_itr->Subarea (subarea);
	}

	//---- process each boundary ----

	while (boundary_file.Read_Record ()) {
		Show_Progress ();

		subarea = boundary_file.Get_Integer (subarea_field);
		if (subarea < 0) {
			Warning (String ("Subarea Number %d is Out of Range") % subarea);
			continue;
		}
		if (subarea < min_sub) min_sub = subarea;
		if (subarea > max_sub) max_sub = subarea;

		//---- assign nodes to the subarea ----

		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			if (In_Polygon (boundary_file, UnRound (node_itr->X ()), UnRound (node_itr->Y ()))) {
				node_itr->Subarea (subarea);
			}
		}
	}
	End_Progress ();
	boundary_file.Close ();

	//---- report unassigned nodes ----

	first = true;
	count = 0;
	if (min_sub > 0) {
		subarea = --min_sub;
	} else {
		subarea = ++max_sub;
	}

	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		if (node_itr->Subarea () < 0) {
			if (first) {
				Print (2, "The Following Nodes were Assigned to Default Subarea ") << subarea << ":";
				first = false;
			}
			if ((count % 10) == 0) {
				Print (1, "\t") << node_itr->Node ();
			} else {
				Print (0, ", ") << node_itr->Node ();
			}
			count++;
			node_itr->Subarea (subarea);
		}
	}
	if (!first) {
		Warning (String ("%d Nodes were Assigned to Default Subarea %d") % count % subarea);
	}
	num_subareas = max_sub + 1;
	subarea_count.assign (num_subareas, 0);
}
