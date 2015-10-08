//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcNet::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	arcview_node.Close ();
	arcview_zone.Close ();
	arcview_lane_use.Close ();
	arcview_location.Close ();
	arcview_parking.Close ();
	arcview_access.Close ();
	arcview_connect.Close ();
	arcview_turn.Close ();
	arcview_sign.Close ();
	arcview_signal.Close ();
	arcview_phasing.Close ();
	arcview_detector.Close ();
	arcview_stop.Close ();
	arcview_route_nodes.Close ();

	if (System_File_Flag (LINK) && (arcview_link.Is_Open () || arcview_center.Is_Open ())) {
		Draw_Links ();
	}
	if (System_File_Flag (POCKET) && arcview_pocket.Is_Open ()) {
		Draw_Pockets ();
	}
	if (System_File_Flag (TIMING_PLAN) && arcview_timing.Is_Open ()) {
		Draw_Timing_Plans ();
	}
	//if (subzone_flag) {
	//	Write_Subzone ();
	//}
	if (route_flag && arcview_route.Is_Open ()) {
		Draw_Route ();
	}
	if (driver_flag && arcview_driver.Is_Open ()) {
		Draw_Driver ();
	}
	Exit_Stat (DONE);
}

