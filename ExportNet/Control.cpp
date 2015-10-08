//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ExportNet::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	//----- new utdf file ----

	Print (1);
	key = Get_Control_String (NEW_UTDF_NETWORK_FILE);

	if (!key.empty ()) {
		Print (1);
		utdf_file.File_Type ("New UTDF Network File");
		utdf_file.File_ID ("UTDF");

		utdf_file.Create (Project_Filename (key));
		utdf_flag = true;

		//----- new node map file ----

		key = Get_Control_String (NEW_NODE_MAP_FILE);

		if (!key.empty ()) {
			Print (1);
			map_file.File_Type ("New Node Map File");
			map_file.File_ID ("MAP");

			map_file.Create (Project_Filename (key));
			map_flag = true;
		}
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

		//---- insert parking links flag ----

		parking_flag = Get_Control_Flag (INSERT_PARKING_LINKS);

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

	//---- check for a new route nodes file ----

	route_flag = System_File_Flag (NEW_ROUTE_NODES);

	if (route_flag) {
		Route_Nodes_File *route_nodes_file;

		//---- write dwell field ----

		Print (1);
		dwell_flag = Get_Control_Flag (WRITE_DWELL_FIELD);

		//---- write time field ----

		ttime_flag = Get_Control_Flag (WRITE_TIME_FIELD);

		//---- write speed field ----

		speed_flag = Get_Control_Flag (WRITE_SPEED_FIELD);

		//---- dwell times ----

		if (dwell_flag) {
			min_dwell = Get_Control_Time (MINIMUM_DWELL_TIME);

			max_dwell = Get_Control_Time (MAXIMUM_DWELL_TIME);
		}

		//---- get the time periods ----

		key = Get_Control_Text (TRANSIT_TIME_PERIODS);

		if (!key.empty ()) {
			schedule_periods.Add_Breaks (key);
		}
		num_periods = schedule_periods.Num_Periods ();

		//---- get the period travel time flag ----

		time_flag = Get_Control_Flag (PERIOD_TRAVEL_TIMES);

		//---- update the file header ----

		route_nodes_file = (Route_Nodes_File *) System_File_Handle (NEW_ROUTE_NODES);

		route_nodes_file->Num_Periods (num_periods);
		route_nodes_file->Dwell_Flag (dwell_flag);
		route_nodes_file->Time_Flag (ttime_flag);
		route_nodes_file->Speed_Flag (speed_flag);

		route_nodes_file->Clear_Fields ();
		route_nodes_file->Create_Fields ();
		route_nodes_file->Write_Header ();
	}
} 
