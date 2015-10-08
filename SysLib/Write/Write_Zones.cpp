//*********************************************************
//	Write_Zones.cpp - write a new zone file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Zones
//---------------------------------------------------------

void Data_Service::Write_Zones (void)
{
	Zone_File *file = (Zone_File *) System_File_Handle (NEW_ZONE);

	int num, count, max_zone;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	count = max_zone = 0;

	for (itr = zone_map.begin (); itr != zone_map.end (); itr++) {
		Show_Progress ();

		num = Put_Zone_Data (*file, zone_array [itr->second]);

		if (num > 0) {
			count += num;
			num = file->Zone ();
			if (num > max_zone) max_zone = num;
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	Max_Zone_Number (max_zone);
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (count != max_zone) Print (1, "Highest Zone Number = ") << max_zone;
}

//---------------------------------------------------------
//	Put_Zone_Data
//---------------------------------------------------------

int Data_Service::Put_Zone_Data (Zone_File &file, Zone_Data &data)
{
	file.Zone (data.Zone ());
	file.X (UnRound (data.X ()));
	file.Y (UnRound (data.Y ()));
	file.Z (UnRound (data.Z ()));
	file.Area_Type (data.Area_Type ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
