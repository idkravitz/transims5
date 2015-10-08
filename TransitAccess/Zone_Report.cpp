//*********************************************************
//	Zone_Report.cpp - create a zone data report
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Zone_Report
//---------------------------------------------------------

void TransitAccess::Zone_Report (void)
{
	int zone;
	Zone_Map_Itr map_itr;
	Zone_Data *data;

	Show_Message ("Creating Zone Data Report -- Record");
	Set_Progress ();

	Header_Number (ZONE_REPORT);
	New_Page ();

	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		Show_Progress ();

		zone = map_itr->first;
		data = &map_itr->second;

		Print (1, String ("%5d %9d %9d    %4d       %2d     %2d    %3d    %7.2lf   %7.2lf     %d") % 
			zone % data->x_coord % data->y_coord % data->location % data->barrier % data->slug %
			data->walk % data->distance % data->weight % data->del_flag);
	}
	End_Progress ();

	if (Progress_Count ()) {
		Print (2, "Number of Records in the Report = ") << Progress_Count ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_Header
//---------------------------------------------------------

void TransitAccess::Zone_Header (void)
{
	Print (1, "Zone Data Report");
	Print (2, " Zone   X_Coord   Y_Coord  Location  Barrier  Slug   Walk   Distance   Weight   Flag");
	Print (1);
}

/*********************************************|***********************************************

	Zone Data Report

	 Zone   X_Coord   Y_Coord  Location  Barrier  Slug   Walk  Distance   Weight   Flag

	ddddd  dddddddd  dddddddd     ddd       dd     dd    ddd    ffff.ff   ffff.ff     d

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
