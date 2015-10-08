//*********************************************************
//	Read_Travelers.cpp - Read the Traveler File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Travelers
//---------------------------------------------------------

void Data_Service::Read_Travelers (void)
{
	Traveler_File *file = (Traveler_File *) System_File_Handle (TRAVELER);
	
	int num;
	Traveler_Data traveler_rec;
	Traveler_Index traveler_index;
	Traveler_Map_Stat map_stat;

	//---- store the Traveler data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Travelers (*file);

	while (file->Read ()) {
		Show_Progress ();

		traveler_rec.Clear ();

		if (Get_Traveler_Data (*file, traveler_rec)) {
			traveler_rec.Get_Traveler_Index (traveler_index);

			map_stat = traveler_map.insert (Traveler_Map_Data (traveler_index, (int) traveler_array.size ()));

			if (!map_stat.second) {
				Warning (String ("Duplicate Traveler Record = %d-%d-%s") % 
					traveler_index.Household () % traveler_index.Person () % traveler_index.Time ().Time_String ());
				continue;
			} else {
				traveler_array.push_back (traveler_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) traveler_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %d Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (EVENT);
}

//---------------------------------------------------------
//	Initialize_Travelers
//---------------------------------------------------------

void Data_Service::Initialize_Travelers (Traveler_File &file)
{
	int percent = System_Data_Reserve (EVENT);

	if (traveler_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			traveler_array.reserve (num);
			if (num > (int) traveler_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Traveler_Data
//---------------------------------------------------------

bool Data_Service::Get_Traveler_Data (Traveler_File &file, Traveler_Data &traveler_rec)
{
	int hhold, lvalue, link, dir, offset, dir_index;

	hhold = file.Household ();
	if (hhold < 1) return (false);

	traveler_rec.Household (hhold);

	traveler_rec.Person (file.Person ());
	traveler_rec.Tour (MAX (file.Tour (), 1));
	traveler_rec.Trip (file.Trip ());

	if (file.Version () <= 40) {
		traveler_rec.Mode (Trip_Mode_Map (file.Mode ()));
	} else {
		traveler_rec.Mode (file.Mode ());
	}
	traveler_rec.Time (file.Time ());
	traveler_rec.Distance (file.Distance ());
	traveler_rec.Speed (file.Speed ());

	//---- check/convert the link number and direction ----

	link = file.Link ();

	if (link > 0) {
		dir = file.Dir ();
		offset = Round (file.Offset ());
	
		Link_Data *link_ptr = Set_Link_Direction (file, link, dir, offset);

		if (link_ptr == 0) return (false);

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
		} else {
			dir_index = link_ptr->AB_Dir ();
		}
		if (dir_index < 0) {
			Warning (String ("Traveler %d Link %d Direction %s was Not Found") % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
		}
		traveler_rec.Dir_Index (dir_index);
		traveler_rec.Offset (offset);
		traveler_rec.Lane (file.Lane ());
	}

	lvalue = file.Route ();

	if (lvalue > 0) {
		if (System_File_Flag (TRANSIT_ROUTE)) {
			Int_Map_Itr map_itr = line_map.find (lvalue);

			if (map_itr == line_map.end ()) {
				Warning (String ("Traveler %d Route %d was Not Found") % Progress_Count () % lvalue);
				return (false);
			}
			traveler_rec.Route (map_itr->second);
		} else {
			traveler_rec.Route (lvalue);
		}
	}
	return (true);
}
