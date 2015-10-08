//*********************************************************
//	Write_Vehicles.cpp - write a new vehicle file
//*********************************************************

#include "Data_Service.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Write_Vehicles
//---------------------------------------------------------

void Data_Service::Write_Vehicles (void)
{
	int part, num_part, count;

	Vehicle_File *file = (Vehicle_File *) System_File_Handle (NEW_VEHICLE);

	Vehicle_Map_Itr itr;
	Vehicle_Itr veh_itr;
	Vehicle_Data *veh_ptr;
	Partition_Files <Vehicle_File> new_file_set;

	if (file->Part_Flag ()) {
		for (num_part=0, veh_itr = vehicle_array.begin (); veh_itr != vehicle_array.end (); veh_itr++) {
			part = Partition_Index (veh_itr->Partition ());
			if (part > num_part) num_part = part;
		}
		new_file_set.Initialize (file, ++num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();

	for (count=0, itr = vehicle_map.begin (); itr != vehicle_map.end (); itr++) {
		Show_Progress ();

		veh_ptr = &vehicle_array [itr->second];
		if (veh_ptr->Partition () < 0) continue;

		if (file->Part_Flag ()) {
			part = Partition_Index (veh_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		count += Put_Vehicle_Data (*file, *veh_ptr);
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Vehicle_Data
//---------------------------------------------------------

int Data_Service::Put_Vehicle_Data (Vehicle_File &file, Vehicle_Data &data)
{
	Veh_Type_Data *veh_type_ptr;
	Parking_Data *parking_ptr;

	file.Household (data.Household ());
	file.Vehicle (data.Vehicle ());
	
	if (veh_type_array.size () > 0) {
		veh_type_ptr = &veh_type_array [data.Type ()];
		file.Type (veh_type_ptr->Type ());
	} else {
		file.Type (data.Type ());
	}
	if (parking_array.size () > 0) {
		parking_ptr = &parking_array [data.Parking ()];
		file.Parking (parking_ptr->Parking ());
	} else {
		file.Parking (data.Parking ());
	}

	//file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
