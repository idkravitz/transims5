//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ExportPlans::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	if (select_vehicles && (!System_File_Flag (VEHICLE) || !System_File_Flag (VEHICLE_TYPE))) {
		Error ("Vehicle and Vehicle Type files are required for Vehicle Type Selection");
	}

	//----- new vissim xml file ----

	Print (1);
	key = Get_Control_String (NEW_VISSIM_XML_FILE);

	if (!key.empty ()) {
		Print (1);
		xml_file.File_Type ("New VISSIM XML File");
		xml_file.File_ID ("XML");

		xml_file.Create (Project_Filename (key));
		vissim_flag = true;

		//---- open the location zone map ----
	
		key = Get_Control_String (LOCATION_ZONE_MAP_FILE);

		if (!key.empty ()) {
			map_file.File_Type ("Location Zone Map_File");
			map_file.File_ID ("Map");

			map_file.Open (Project_Filename (key));
			map_flag = true;
		}

		//---- open the subzone weight file ----

		key = Get_Control_String (SUBZONE_WEIGHT_FILE);

		if (!key.empty ()) {
			subzone_file.File_Type ("Subzone Weight File");
			subzone_file.File_ID ("Subzone");

			subzone_file.Open (Project_Filename (key));
			subzone_flag = true;
		}

		//----- new zone location file ----

		key = Get_Control_String (NEW_ZONE_LOCATION_FILE);

		if (!key.empty ()) {
			Print (1);
			zone_loc_file.File_Type ("New Zone Location File");

			zone_loc_file.Create (Project_Filename (key));
			zone_loc_flag = true;

			zone_loc_file.File () << "ZONE\tX_COORD\tY_COORD" << endl;
		}
	}
} 
