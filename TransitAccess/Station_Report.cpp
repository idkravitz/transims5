//*********************************************************
//	Station_Report.cpp - create a station data report
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Station_Report
//---------------------------------------------------------

void TransitAccess::Station_Report (void)
{
	Station_Itr itr;

	Show_Message ("Creating Station Data Report -- Record");
	Set_Progress ();

	Header_Number (STATION_REPORT);
	New_Page ();

	for (itr = station_array.begin (); itr != station_array.end (); itr++) {
		Show_Progress ();

		Print (1, String ("  %c   %2d   %d   %d  %5d%c%5d %7d %7d %7d %8d %8d %s") % 
			(char) itr->mode % itr->type % itr->pnr % itr->use % itr->skim % ((itr->missing) ? '*' : ' ') %
			itr->zone % itr->stop % itr->park % itr->node % itr->x_coord % itr->y_coord % itr->name);
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Station_Header
//---------------------------------------------------------

void TransitAccess::Station_Header (void)
{
	Print (1, "Station Data Report");
	Print (2, "Mode Type PNR USE  Skim  Zone    Stop Parking    Node  X_Coord  Y_Coord Name");
	Print (1);
}

/*********************************************|***********************************************

	Station Data Report

	Mode Type PNR USE  Skim  Zone    Stop Parking    Node  X_Coord  Y_Coord Name

	  c   dd   b   b  ddddd ddddd ddddddd ddddddd ddddddd dddddddd dddddddd ssssssssssssssssss

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
