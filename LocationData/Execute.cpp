//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LocationData::Execute (void)
{
	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}
	Show_Message (1);

	//---- read the network data ----
		
	System_File_False (LOCATION);

	Data_Service::Execute ();

	//---- read each data file ----

	if (data_flag) {
		Read_Data ();
	}

	//---- read zone boundary file ----

	if (boundary_flag) {
		Read_Boundary ();
	}

	//---- read the activity location file ----

	Read_Locations ();

	//---- walk access to transit ----

	if (walk_access_flag) {
		Walk_Access ();
	}

	//---- read each subzone data file ----

	if (subzone_map_flag) {
		Read_Subzone_Map ();
	}
	if (subzone_flag) {
		Read_Subzone ();
	}
	if (subzone_map_flag) {
		Subzone_Map_Weights ();
	} else if (subzone_flag) {
		Subzone_Weights ();
	}

	//---- read boundary polygon files ----

	if (polygon_flag) {
		Read_Polygons ();
	}

	//---- process and write location data ----

	Write_Location ();

	//---- write the zone location file ----

	if (zone_loc_flag) {
		Zone_Locations ();

		zone_loc_map.Write ();
	}

	//---- print the zone coverage report ----

	if (Report_Flag (ZONE_CHECK)) {
		Check_Zones ();
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void LocationData::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
