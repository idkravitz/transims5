//*********************************************************
//	Read_Vehicle.cpp - Read the Vehicle File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Vehicle
//---------------------------------------------------------

void Relocate::Read_Vehicle (void)
{
	int part, num_rec, num_update, parking, num_error;

	Vehicle_File *vehicle_file, *new_file;
	Int_Map_Itr park_itr;
		
	vehicle_file = (Vehicle_File *) System_File_Handle (VEHICLE);
	new_file = (Vehicle_File *) System_File_Handle (NEW_VEHICLE);

	num_rec = num_update = num_error = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!vehicle_file->Open (part)) break;
			new_file->Open (part);
		}
		if (vehicle_file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % vehicle_file->File_Type () % vehicle_file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % vehicle_file->File_Type ());
		}
		Set_Progress ();

		while (vehicle_file->Read ()) {
			Show_Progress ();

			new_file->Copy_Fields (*vehicle_file);

			parking = new_file->Parking ();
			park_itr = old_new_park.find (parking);
			if (park_itr != old_new_park.end ()) {
				new_file->Parking (park_itr->second);
				if (parking != park_itr->second) num_update++;
			} else {
				num_error++;
			}
			new_file->Write ();
			num_rec++;
		}
	}
	vehicle_file->Close ();
	new_file->Close ();
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % vehicle_file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	Print (1, "Number of Parking Lots Updated = ") << num_update;
	if (num_error > 0) {
		Print (1, "Number of Parking Lot Errors = ") << num_error;
	}
}
