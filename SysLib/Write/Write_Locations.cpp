//*********************************************************
//	Write_Locations.cpp - write a new location file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Locations
//---------------------------------------------------------

void Data_Service::Write_Locations (void)
{
	Location_File *file = (Location_File *) System_File_Handle (NEW_LOCATION);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = location_map.begin (); itr != location_map.end (); itr++) {
		Show_Progress ();

		count += Put_Location_Data (*file, location_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Location_Data
//---------------------------------------------------------

int Data_Service::Put_Location_Data (Location_File &file, Location_Data &data)
{
	Link_Data *link_ptr;
	Zone_Data *zone_ptr;
	int zone;

	link_ptr = &link_array [data.Link ()];

	file.Location (data.Location ());
	file.Link (link_ptr->Link ());
	file.Dir (data.Dir ());
	file.Offset (UnRound (data.Offset ()));
	file.Setback (UnRound (data.Setback ()));

	zone = data.Zone ();
	if (zone >= 0 && System_Data_Flag (ZONE)) {
		zone_ptr = &zone_array [zone];
		zone = zone_ptr->Zone ();
	} else if (zone < 0) {
		zone = 0;
	}
	file.Zone (zone);

	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}

