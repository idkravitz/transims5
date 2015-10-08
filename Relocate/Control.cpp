//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Relocate::Program_Control (void)
{
	String key;

	//---- open network and demand files ----

	Data_Service::Program_Control ();

	trip_flag = System_File_Flag (NEW_TRIP);
	if (!trip_flag) System_File_False (TRIP);

	vehicle_flag = System_File_Flag (NEW_VEHICLE);
	if (!vehicle_flag) System_File_False (VEHICLE);

	plan_flag = System_File_Flag (NEW_PLAN);
	if (!plan_flag) System_File_False (PLAN);

	select_flag = System_File_Flag (NEW_SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);

	//---- old node file ----

	key = Get_Control_String (OLD_NODE_FILE);

	if (!key.empty ()) {
		old_node_file.File_Type ("Old Node File");

		if (Check_Control_Key (OLD_NODE_FORMAT)) {
			old_node_file.Dbase_Format (Get_Control_String (OLD_NODE_FORMAT));
		}
		old_node_file.Open (Project_Filename (key));
	}

	//---- old shape file ----

	key = Get_Control_String (OLD_SHAPE_FILE);

	if (!key.empty ()) {
		old_shape_file.File_Type ("Old Shape File");

		if (Check_Control_Key (OLD_SHAPE_FORMAT)) {
			old_shape_file.Dbase_Format (Get_Control_String (OLD_SHAPE_FORMAT));
		}
		old_shape_file.Open (Project_Filename (key));
		shape_flag = true;
	}

	//---- old link file ----

	key = Get_Control_String (OLD_LINK_FILE);

	if (!key.empty ()) {
		old_link_file.File_Type ("Old Link File");

		if (Check_Control_Key (OLD_LINK_FORMAT)) {
			old_link_file.Dbase_Format (Get_Control_String (OLD_LINK_FORMAT));
		}
		old_link_file.Open (Project_Filename (key));
	}

	//---- old location file ----

	key = Get_Control_String (OLD_LOCATION_FILE);

	if (!key.empty ()) {
		old_loc_file.File_Type ("Old Location File");

		if (Check_Control_Key (OLD_LOCATION_FORMAT)) {
			old_loc_file.Dbase_Format (Get_Control_String (OLD_LOCATION_FORMAT));
		}
		old_loc_file.Open (Project_Filename (key));
	}

	//---- old parking file ----

	key = Get_Control_String (OLD_PARKING_FILE);

	if (!key.empty ()) {
		old_park_file.File_Type ("Old Parking File");

		if (Check_Control_Key (OLD_PARKING_FORMAT)) {
			old_park_file.Dbase_Format (Get_Control_String (OLD_PARKING_FORMAT));
		}
		old_park_file.Open (Project_Filename (key));
	}

	//---- old access file ----

	key = Get_Control_String (OLD_ACCESS_FILE);

	if (!key.empty ()) {
		old_acc_file.File_Type ("Old Access File");

		if (Check_Control_Key (OLD_ACCESS_FORMAT)) {
			old_acc_file.Dbase_Format (Get_Control_String (OLD_ACCESS_FORMAT));
		}
		old_acc_file.Open (Project_Filename (key));
	}

	//---- new location map file ----
	
	key = Get_Control_String (NEW_LOCATION_MAP_FILE);

	if (!key.empty ()) {
		new_loc_file.File_Type ("New Location Map File");
		Print (1);

		if (!new_loc_file.Create (Project_Filename (key))) {
			Error (String ("Creating %s") % new_loc_file.File_Type ());
		}
		loc_map_flag = true;

		new_loc_file.Add_Field ("OLD_LOC", DB_INTEGER, 10);
		new_loc_file.Add_Field ("NEW_LOC", DB_INTEGER, 10);

		new_loc_file.Write_Header ();
	}

	//---- new parking map file ----
	
	key = Get_Control_String (NEW_PARKING_MAP_FILE);

	if (!key.empty ()) {
		new_park_file.File_Type ("New Parking Map File");
		Print (1);

		if (!new_park_file.Create (Project_Filename (key))) {
			Error (String ("Creating %s") % new_park_file.File_Type ());
		}
		park_map_flag = true;

		new_park_file.Add_Field ("OLD_PARK", DB_INTEGER, 10);
		new_park_file.Add_Field ("NEW_PARK", DB_INTEGER, 10);

		new_park_file.Write_Header ();
	}
} 
