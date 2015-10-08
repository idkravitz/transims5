//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Relocate::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	//---- read the old network ----

	Old_Nodes ();
	if (shape_flag) {
		Old_Shapes ();
	}
	Old_Links ();
	Old_Locations ();
	Old_Parking_Lots ();
	if (access_flag) {
		Old_Access_Links ();
	}

	Map_Network ();

	//---- match activity locations ----

	Match_Locations ();

	//---- process trip files ----

	if (trip_flag) {
		Read_Trip ();
	}

	//---- process plan files ----

	if (plan_flag) {
		Read_Plan ();
	}

	//---- process the vehicle file ----

	if (vehicle_flag) {
		Read_Vehicle ();
	}

	//---- write the selection file ----

	if (select_flag) {
		Write_Selections ();
	}
	Exit_Stat (DONE);
}
