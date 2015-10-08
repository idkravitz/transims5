//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ZoneData::Execute (void)
{
	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}

	//---- read the network data ----
		
	System_File_False (ZONE);

	Data_Service::Execute ();

	//---- read each data file ----

	Show_Message (1);

	if (data_flag) {
		Read_Data ();
	}

	//---- read boundary polygon files ----

	if (polygon_flag) {
		Read_Polygons ();
	}

	//---- read the zone file ----

	Read_Zones ();

	//---- sum zone data ----

	if (sum_flag) {
		Sum_Zone ();
	}

	//---- process and write zone data ----

	Write_Zone ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ZoneData::Page_Header (void)
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
