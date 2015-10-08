//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransitAccess::Execute (void)
{
	Skim_Itr skim_itr;
	Zone_Map_Itr zone_itr;
	Mode_Itr mode_itr;

	//---- read the zone file ----

	Read_Zone ();

	//---- read the node file ----

	Read_Node ();

	//---- read the extra link file ----

	if (extra_flag) {
		Read_Extra ();
	}

	//---- read the walk link file ----

	if (walk_link_flag) {
		Read_Link ();
	}

	//---- read the station file ----

	Read_Station ();

	//---- write walk access links ----

	if (zone_walk_flag) {
		Walk_Access ();
	}

	//---- read the slug file ----

	if (slug_flag) {
		Read_Slug ();
	}

	//---- read the skim files ----

	for (skim_itr = skim_array.begin (); skim_itr != skim_array.end (); skim_itr++) {
		Read_Skims (skim_itr);
	}
	
	//---- process each origin zone ----
	
	Show_Message ("Creating Auto Access Data -- Origin");
	Set_Progress ();

	for (zone_itr = zone_map.begin (); zone_itr != zone_map.end (); zone_itr++) {
		Show_Progress (zone_itr->first);

		if (!select_org_zones || org_zone_range.In_Range (zone_itr->first)) {
			Process_Origin (zone_itr);
		}
	}
	End_Progress ();

	//---- print the results ----

	if (missing_skims > 0 || num_default > 0) {
		Print (1);
		if (missing_skims > 0) {
			Warning ("Number of Missing Skims = ") << missing_skims;
		}
		if (num_default > 0) {
			Warning ("Number of Default Skims = ") << num_default;
		}
		Show_Message (1);
	}
	Print (1);

	for (mode_itr = mode_array.begin (); mode_itr != mode_array.end (); mode_itr++) {
		Print (1, String ("Mode Number %2d, Number of Records = %d") % 
			mode_itr->number % mode_itr->count);
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case ZONE_REPORT:			//---- Zone Data Report ----
				Zone_Report ();
				break;
			case STATION_REPORT:		//---- Station Data Report ----
				Station_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TransitAccess::Page_Header (void)
{
	switch (Header_Number ()) {
		case ZONE_REPORT:			//---- Zone Data Report
			Zone_Header ();
			break;
		case STATION_REPORT:		//---- Station Data Report ----
			Station_Header ();
			break;
		default:
			break;
	}
}
