//*********************************************************
//	Read_Locations.cpp - read the location file
//*********************************************************

#include "Data_Service.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Read_Locations
//---------------------------------------------------------

void Data_Service::Read_Locations (void)
{
	Location_File *file = (Location_File *) System_File_Handle (LOCATION);

	int num;
	bool zone_flag = System_Data_Flag (ZONE);

	Int_Map_Stat map_stat;
	Location_Data location_rec;

	//---- store the location data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Locations (*file);

	while (file->Read ()) {
		Show_Progress ();

		location_rec.Clear ();

		if (Get_Location_Data (*file, location_rec)) {
			map_stat = location_map.insert (Int_Map_Data (location_rec.Location (), (int) location_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Location Number = ") << location_rec.Location ();
				continue;
			} else {
				if (!file->Setback_Flag () && location_rec.Setback () == 0) {
					location_rec.Setback (Internal_Units (100.0, FEET));
				}
				location_array.push_back (location_rec);

				if (!zone_flag && location_rec.Zone () > Max_Zone_Number ()) {
					Max_Zone_Number (location_rec.Zone ());
				}
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) location_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (LOCATION);
}

//---------------------------------------------------------
//	Initialize_Locations
//---------------------------------------------------------

void Data_Service::Initialize_Locations (Location_File &file)
{
	Required_File_Check (file, LINK);
	
	int percent = System_Data_Reserve (LOCATION);

	if (location_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			location_array.reserve (num);
			if (num > (int) location_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool Data_Service::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	int link, dir, offset;
	double setback;
	Link_Data *link_ptr;

	//---- read and save the data ----

	location_rec.Location (file.Location ());

	if (location_rec.Location () == 0) return (false);

	//---- check/convert the link number and direction  ----
	
	link = file.Link ();
	dir = file.Dir ();
	offset = Round (file.Offset ());

	link_ptr = Set_Link_Direction (file, link, dir, offset);

	location_rec.Link (link);
	location_rec.Dir (dir);
	location_rec.Offset (offset);

	setback = file.Setback ();
	location_rec.Setback (setback);

	if (Location_XY_Flag ()) {
		Points points;

		Link_Shape (link_ptr, dir, points, UnRound (offset), 0.0, setback);

		if (points.size () > 0) {
			location_rec.X (points [0].x);
			location_rec.Y (points [0].y);
		} else {
			Node_Data *node_ptr = &node_array [link_ptr->Anode ()];
		
			location_rec.X (node_ptr->X ());
			location_rec.Y (node_ptr->Y ());
		}
	}
	location_rec.Zone (file.Zone ());

	if (location_rec.Zone () > 0 && System_Data_Flag (ZONE)) {
		Int_Map_Itr map_itr = zone_map.find (location_rec.Zone ());
		if (map_itr == zone_map.end ()) {
			Warning (String ("Location %d Zone %d was Not Found") % location_rec.Location () % location_rec.Zone ());
			return (false);
		}
		location_rec.Zone (map_itr->second);
	} else if (location_rec.Zone () == 0) {
		location_rec.Zone (-1);

		//---- delete extra activity location used for transit stops ----

		if (file.Version () <= 40) return (false);
	}
	location_rec.Notes (file.Notes ());

	return (true);
}

