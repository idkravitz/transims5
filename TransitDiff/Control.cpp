//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "TransitDiff.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransitDiff::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());
	Print (2, "Input Compare Network Files:");

	//---- open the compare node file ----
	
	key = Get_Control_String (COMPARE_NODE_FILE);

	if (key.empty ()) goto control_error;

	compare_node_file.File_Type ("Compare Node File");

	if (Check_Control_Key (COMPARE_NODE_FORMAT)) {
		compare_node_file.Dbase_Format (Get_Control_String (COMPARE_NODE_FORMAT));
	}
	compare_node_file.Open (Project_Filename (key));

	//---- open the compare link file ----
	
	key = Get_Control_String (COMPARE_LINK_FILE);

	if (key.empty ()) goto control_error;

	compare_link_file.File_Type ("Compare Link File");

	if (Check_Control_Key (COMPARE_LINK_FORMAT)) {
		compare_link_file.Dbase_Format (Get_Control_String (COMPARE_LINK_FORMAT));
	}
	compare_link_file.Open (Project_Filename (key));

	//---- open the compare transit stop file ----
	
	key = Get_Control_String (COMPARE_TRANSIT_STOP_FILE);

	if (key.empty ()) goto control_error;

	compare_stop_file.File_Type ("Compare Transit Stop File");

	if (Check_Control_Key (COMPARE_TRANSIT_STOP_FORMAT)) {
		compare_stop_file.Dbase_Format (Get_Control_String (COMPARE_TRANSIT_STOP_FORMAT));
	}
	compare_stop_file.Open (Project_Filename (key));

	//---- open the compare transit route file ----
	
	key = Get_Control_String (COMPARE_TRANSIT_ROUTE_FILE);

	if (key.empty ()) goto control_error;

	compare_line_file.File_Type ("Compare Transit Route File");

	if (Check_Control_Key (COMPARE_TRANSIT_ROUTE_FORMAT)) {
		compare_line_file.Dbase_Format (Get_Control_String (COMPARE_TRANSIT_ROUTE_FORMAT));
	}
	compare_line_file.Open (Project_Filename (key));

	//---- open the compare transit schedule file ----
	
	key = Get_Control_String (COMPARE_TRANSIT_SCHEDULE_FILE);

	if (key.empty ()) goto control_error;

	compare_schedule_file.File_Type ("Compare Transit Schedule File");

	if (Check_Control_Key (COMPARE_TRANSIT_SCHEDULE_FORMAT)) {
		compare_schedule_file.Dbase_Format (Get_Control_String (COMPARE_TRANSIT_SCHEDULE_FORMAT));
	}
	compare_schedule_file.Open (Project_Filename (key));

	//---- open the compare transit driver file ----

	if (System_File_Flag (TRANSIT_DRIVER)) {
		key = Get_Control_String (COMPARE_TRANSIT_DRIVER_FILE);
	
		if (!key.empty ()) {
			compare_driver_file.File_Type ("Compare Transit Driver File");
			driver_flag = true;

			//---- get the file format ----

			if (Check_Control_Key (COMPARE_TRANSIT_DRIVER_FORMAT)) {
				compare_driver_file.Dbase_Format (Get_Control_String (COMPARE_TRANSIT_DRIVER_FORMAT));
			}
			compare_driver_file.Open (Project_Filename (key));
		}
	}

	//---- open the new transit difference file ----
	
	key = Get_Control_String (NEW_TRANSIT_DIFFERENCE_FILE);

	if (key.empty ()) goto control_error;

	diff_file.File_Type ("New Transit Difference File");

	if (Check_Control_Key (NEW_TRANSIT_DIFFERENCE_FORMAT)) {
		diff_file.Dbase_Format (Get_Control_String (NEW_TRANSIT_DIFFERENCE_FORMAT));
	}
	diff_file.Create (Project_Filename (key));

	diff_file.Add_Field ("Route", DB_INTEGER, 10);
	diff_file.Add_Field ("Base_Stops", DB_INTEGER, 10);
	diff_file.Add_Field ("Base_Runs", DB_INTEGER, 10);
	diff_file.Add_Field ("Base_Time", DB_INTEGER, 10, SECONDS);
	if (driver_flag) {
		diff_file.Add_Field ("Base_Links", DB_INTEGER, 10);
	}
	diff_file.Add_Field ("Compare_Stops", DB_INTEGER, 10);
	diff_file.Add_Field ("Compare_Runs", DB_INTEGER, 10);
	diff_file.Add_Field ("Compare_Time", DB_INTEGER, 10, SECONDS);
	if (driver_flag) {
		diff_file.Add_Field ("Compare_Links", DB_INTEGER, 10);
	}
	diff_file.Add_Field ("Diff_Stops", DB_INTEGER, 10);
	diff_file.Add_Field ("Diff_Runs", DB_INTEGER, 10);
	diff_file.Add_Field ("Diff_Time", DB_INTEGER, 10, SECONDS);
	if (driver_flag) {
		diff_file.Add_Field ("Diff_Links", DB_INTEGER, 10);
	}
	diff_file.Write_Header ();

	return;

control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());
}

