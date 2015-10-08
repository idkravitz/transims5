//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanTrips::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	//---- process plan files ----

	Read_Plan ();

	//---- write the trip file ----

	Write_Trips ();

	Exit_Stat (DONE);
}
