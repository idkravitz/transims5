//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SimSubareas::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Node_File *file = (Node_File *) System_File_Handle (NEW_NODE);
	file->Subarea_Flag (true);
	file->Clear_Fields ();
	file->Create_Fields ();
	file->Write_Header ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the subarea boundary file ----

	key = exe->Get_Control_String (SUBAREA_BOUNDARY_FILE);

	if (!key.empty ()) {

		boundary_file.File_Type ("Select Subarea Polygon");

		boundary_file.Open (exe->Project_Filename (key));

		subarea_field = boundary_file.Required_Field ("SUBAREA", "ID", "NUMBER", "PARTITION", "SUB");

		boundary_flag = true;

	} else {

		//---- get the number of subareas ----

		num_subareas = Get_Control_Integer (NUMBER_OF_SUBAREAS);

		subarea_count.assign (num_subareas, 0);

		//---- get the partitions per subarea ----

		num_parts = Get_Control_Integer (PARTITIONS_PER_SUBAREA);

		//---- get the center node number ---

		center = Get_Control_Integer (CENTER_NODE_NUMBER);
	}
}
