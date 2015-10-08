//*********************************************************
//	Write_Selections.cpp - write a new household selection file
//*********************************************************

#include "Data_Service.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Selections
//---------------------------------------------------------

void Data_Service::Write_Selections (void)
{
	int part, num_part, count;

	Selection_File *file = (Selection_File *) System_File_Handle (NEW_SELECTION);

	Select_Data *data_ptr;
	Select_Map_Itr itr;
	Partition_Files <Selection_File> new_file_set;

	if (file->Part_Flag ()) {
		for (num_part=0, itr = select_map.begin (); itr != select_map.end (); itr++) {
			part = Partition_Index (itr->second.Partition ());
			if (part > num_part) num_part = part;
		}
		new_file_set.Initialize (file, ++num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();
	
	//---- write the selection data ----

	for (count=0, itr = select_map.begin (); itr != select_map.end (); itr++) {
		Show_Progress ();

		data_ptr = &(itr->second);
		if (data_ptr->Partition () < 0) continue;

		if (file->Part_Flag ()) {
			part = Partition_Index (data_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		count += Put_Selection_Data (*file, (Trip_Index &) itr->first, *data_ptr);
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Selection_Data
//---------------------------------------------------------

int Data_Service::Put_Selection_Data (Selection_File &file, Trip_Index &index, Select_Data &data)
{
	file.Household (index.Household ());
	file.Person (index.Person ());
	file.Tour (index.Tour ());
	file.Trip (index.Trip ());
	file.Type (data.Type ());
	file.Partition (data.Partition ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
