//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransitDiff::Execute (void)
{
	//---- read the network data ----

	Data_Service::Execute ();

	//---- read the compare network file ----

	Read_Node ();
	Read_Link ();
	Read_Stop ();
	Read_Line ();
	Read_Schedule ();

	if (driver_flag) {
		Read_Driver ();
	}

	//---- compare schedules ----

	Compare_Routes ();

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TransitDiff::Page_Header (void)
{
	switch (Header_Number ()) {
		case FIRST_REPORT:		//---- First Report ----
			First_Header ();
			break;
		case SECOND_REPORT:		//---- Second Report ----
			Print (1, "Second Report Header");
			Print (1);
			break;
		default:
			break;
	}
}
