//*********************************************************
//	Write_Match.cpp - write the trip match file
//*********************************************************

#include "PlanCompare.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Match
//---------------------------------------------------------

void PlanCompare::Write_Match (void)
{
	int part, num_part, count;

	Trip_Index trip_index;
	Selection_File *file;
	Select_Data *data_ptr;
	Select_Map_Itr itr;
	Partition_Files <Selection_File> new_file_set;

	file = &match_file;

	if (match_file.Part_Flag ()) {
		for (num_part=0, itr = matched.begin (); itr != matched.end (); itr++) {
			part = Partition_Index (itr->second.Partition ());
			if (part > num_part) num_part = part;
		}
		new_file_set.Initialize (&match_file, ++num_part);
		Show_Message (String ("Writing %ss -- Record") % match_file.File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % match_file.File_Type ());
	}
	Set_Progress ();
	
	//---- write the selection data ----

	for (count=0, itr = matched.begin (); itr != matched.end (); itr++) {
		Show_Progress ();

		data_ptr = &(itr->second);
		if (data_ptr->Partition () < 0) continue;

		if (match_file.Part_Flag ()) {
			part = Partition_Index (data_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		trip_index = itr->first;

		file->Household (trip_index.Household ());
		file->Person (trip_index.Person ());
		file->Tour (trip_index.Tour ());
		file->Trip (trip_index.Trip ());
		file->Type (data_ptr->Type ());
		file->Partition (data_ptr->Partition ());

		if (!file->Write ()) {
			Error (String ("Writing %s") % file->File_Type ());
		}
		count++;
	}
	End_Progress ();

	match_file.Close ();
	
	Print (2, String ("%s Records = %d") % match_file.File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}
