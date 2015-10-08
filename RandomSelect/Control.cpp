//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void RandomSelect::Program_Control (void)
{
	String key;

	//---- create the network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	trip_flag = System_File_Flag (TRIP);
	if (trip_flag) System_File_False (HOUSEHOLD);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the number of partitions ----

	num_parts = Get_Control_Integer (NUMBER_OF_PARTITIONS);

	part_count.assign (num_parts, 0);

	//---- check the selection file ----

	if (num_parts > 1) {
		Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);

		if (!file->Part_Flag ()) {
			file->Clear_Fields ();
			file->Partition_Flag (true);
			file->Create_Fields ();
			file->Write_Header ();
		}
	}
}

