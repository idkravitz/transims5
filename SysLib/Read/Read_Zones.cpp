//*********************************************************
//	Read_Zones.cpp - read the zone file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Zones
//---------------------------------------------------------

void Data_Service::Read_Zones (void)
{
	Zone_File *file = (Zone_File *) System_File_Handle (ZONE);
	
	Zone_Data zone_rec;
	Int_Map_Stat map_stat;

	//---- store the zone data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Zones (*file);

	while (file->Read ()) {
		Show_Progress ();

		zone_rec.Clear ();

		if (Get_Zone_Data (*file, zone_rec)) {
			map_stat = zone_map.insert (Int_Map_Data (zone_rec.Zone (), (int) zone_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Zone Number = ") << zone_rec.Zone ();
				continue;
			} else {
				zone_array.push_back (zone_rec);

				if (zone_rec.Zone () > Max_Zone_Number ()) {
					Max_Zone_Number (zone_rec.Zone ());
				}
			}
		}
	}
	End_Progress ();
	file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	int num = (int) zone_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num != Max_Zone_Number ()) Print (1, "Highest Zone Number = ") << Max_Zone_Number ();

	if (num > 0) System_Data_True (ZONE);
}

//---------------------------------------------------------
//	Initialize_Zones
//---------------------------------------------------------

void Data_Service::Initialize_Zones (Zone_File &file)
{
	int percent = System_Data_Reserve (ZONE);

	if (zone_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			zone_array.reserve (num);
			if (num > (int) zone_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool Data_Service::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	//---- read and save the data ----

	zone_rec.Zone (file.Zone ());
	zone_rec.X (file.X ());
	zone_rec.Y (file.Y ());
	zone_rec.Z (file.Z ());
	zone_rec.Area_Type (file.Area_Type ());
	zone_rec.Notes (file.Notes ());

	return (true);
}
