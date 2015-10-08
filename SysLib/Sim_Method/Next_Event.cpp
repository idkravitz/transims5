 //*********************************************************
//	Next_Event.cpp - next event time
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Next_Signal_Event
//---------------------------------------------------------

int Sim_Method::Next_Signal_Event (int dir_index)
{
	int node;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Sim_Signal_Data *control_ptr;

	if (dir_index < 0) return (0);

	dir_ptr = &exe->dir_array [dir_index];
	link_ptr = &exe->link_array [dir_ptr->Link ()];

	if (dir_ptr->Dir () == 1) {
		node = link_ptr->Anode ();
	} else {
		node = link_ptr->Bnode ();
	}
	node_ptr = &exe->node_array [node];
	if (node_ptr->Control () < 0 || node_ptr->Partition () != partition) return (0);

	control_ptr = &exe->sim_signal_array [node_ptr->Control ()];

	return (control_ptr->Check_Time ());
}
