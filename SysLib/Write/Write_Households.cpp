//*********************************************************
//	Write_Households.cpp - write a new household file
//*********************************************************

#include "Data_Service.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Households
//---------------------------------------------------------

void Data_Service::Write_Households (void)
{
	int part, num_part, count;

	Household_File *file = (Household_File *) System_File_Handle (NEW_HOUSEHOLD);

	Int_Map_Itr itr;
	Household_Data *hhold_ptr;
	Household_Itr hhold_itr;
	Partition_Files <Household_File> new_file_set;

	if (file->Part_Flag ()) {
		for (num_part=0, hhold_itr = hhold_array.begin (); hhold_itr != hhold_array.end (); hhold_itr++) {
			part = Partition_Index (hhold_itr->Partition ());
			if (part > num_part) num_part = part;
		}
		new_file_set.Initialize (file, ++num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();

	for (count=0, itr = hhold_map.begin (); itr != hhold_map.end (); itr++) {
		Show_Progress ();

		hhold_ptr = &hhold_array [itr->second];
		if (hhold_ptr->Partition () < 0) continue;

		if (file->Part_Flag ()) {
			part = Partition_Index (hhold_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		count += Put_Household_Data (*file, *hhold_ptr);
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Household_Data
//---------------------------------------------------------

int Data_Service::Put_Household_Data (Household_File &file, Household_Data &data)
{
	Person_Itr person_itr;
	Location_Data *loc_ptr;

	int count = 0;

	loc_ptr = &location_array [data.Location ()];

	file.Household (data.Household ());
	file.Location (loc_ptr->Location ());
	file.Persons (data.Persons ());
	file.Workers (data.Workers ());
	file.Vehicles (data.Vehicles ());
	file.Type (data.Type ());
	file.Partition (data.Partition ());
	file.Num_Nest ((int) data.size ());
	//file.Notes (data.Notes ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count++;

	for (person_itr = data.begin (); person_itr != data.end (); person_itr++) {
		file.Person (person_itr->Person ());
		file.Age (person_itr->Age ());
		file.Relate (person_itr->Relate ());
		file.Gender (person_itr->Gender ());
		file.Work (person_itr->Work ());
		file.Drive (person_itr->Drive ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	return (count);
}
