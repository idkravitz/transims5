//*********************************************************
//	Read_Stops.cpp - read the transit stop file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Stops
//---------------------------------------------------------

void Data_Service::Read_Stops (void)
{
	Stop_File *file = (Stop_File *) System_File_Handle (TRANSIT_STOP);
	
	int num;
	Stop_Data stop_rec;
	Int_Map_Stat map_stat;

	//---- store the transit stop data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Stops (*file);

	while (file->Read ()) {
		Show_Progress ();

		stop_rec.Clear ();

		if (Get_Stop_Data (*file, stop_rec)) {
			map_stat = stop_map.insert (Int_Map_Data (stop_rec.Stop (), (int) stop_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Stop Number = ") << stop_rec.Stop ();
				continue;
			} else {
				stop_array.push_back (stop_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) stop_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (TRANSIT_STOP);
}

//---------------------------------------------------------
//	Initialize_Stops
//---------------------------------------------------------

void Data_Service::Initialize_Stops (Stop_File &file)
{
	Required_File_Check (file, LINK);
	
	int percent = System_Data_Reserve (TRANSIT_STOP);

	if (stop_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			stop_array.reserve (num);
			if (num > (int) stop_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool Data_Service::Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec)
{
	int link, dir, offset, use;

	stop_rec.Stop (file.Stop ());
	if (stop_rec.Stop () == 0) return (false);

	//---- check/convert the link number ----

	link = file.Link ();
	dir = file.Dir ();
	offset = Round (file.Offset ());

	Set_Link_Direction (file, link, dir, offset);

	stop_rec.Link (link);
	stop_rec.Dir (dir);
	stop_rec.Offset (offset);

	//----- optional fields ----

	use = file.Use ();

	if (use == 0) {
		String text ("BUS/RAIL");
		use = Use_Code (text);
	}
	stop_rec.Use (use);

	if (file.Type_Flag ()) {
		stop_rec.Type (file.Type ());
	} else {
		stop_rec.Type (STOP);
	}
	stop_rec.Space (file.Space ());
	
	stop_rec.Name (file.Name ());
	stop_rec.Notes (file.Notes ());

	return (true);
}
