//*********************************************************
//	Read_Parking_Lots.cpp - read the parking file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Parking_Lots
//---------------------------------------------------------

void Data_Service::Read_Parking_Lots (void)
{
	Parking_File *file = (Parking_File *) System_File_Handle (PARKING);
	
	int i, num;
	bool keep_flag;
	Int_Map_Stat map_stat;
	Parking_Data parking_rec;

	//---- store the parking data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Parking_Lots (*file);

	while (file->Read (false)) {
		Show_Progress ();

		parking_rec.Clear ();

		keep_flag = Get_Parking_Data (*file, parking_rec);

		num = file->Num_Nest ();
		if (num > 0) parking_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Nested Records for Parking %d") % file->Parking ());
			}
			Show_Progress ();

			Get_Parking_Data (*file, parking_rec);
		}
		if (keep_flag) {
			map_stat = parking_map.insert (Int_Map_Data (parking_rec.Parking (), (int) parking_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Parking Number = ") << parking_rec.Parking ();
			} else {
				parking_array.push_back (parking_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) parking_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (PARKING);
}

//---------------------------------------------------------
//	Initialize_Parking_Lots
//---------------------------------------------------------

void Data_Service::Initialize_Parking_Lots (Parking_File &file)
{
	Required_File_Check (file, LINK);

	int percent = System_Data_Reserve (PARKING);

	if (parking_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 1.5);
		}
		if (num > 1) {
			parking_array.reserve (num);
			if (num > (int) parking_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool Data_Service::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	int link, dir, offset;

	bool flag = (file.Version () <= 40);

	//---- process a header line ----

	if (!file.Nested ()) {
		parking_rec.Parking (file.Parking ());
		if (parking_rec.Parking () == 0) return (false);

		//---- check/convert the link number and direction ----
		
		link = file.Link ();
		dir = file.Dir ();
		offset = Round (file.Offset ());

		Set_Link_Direction (file, link, dir, offset);

		parking_rec.Link (link);
		parking_rec.Dir (dir);
		parking_rec.Offset (offset);

		//---- set the parking type ----
		
		parking_rec.Type (file.Type ());
		parking_rec.Notes (file.Notes ());

		if (!flag) return (true);
	}

	//---- process a nested record ----

	Parking_Nest nest_rec;

	nest_rec.Use (file.Use ());

	nest_rec.Start (file.Start ());
	nest_rec.End (file.End ());
	if (nest_rec.End () == 0) nest_rec.End (Model_End_Time ());

	nest_rec.Space (file.Space ());
	nest_rec.Time_In (file.Time_In ());
	nest_rec.Time_Out (file.Time_Out ());
	nest_rec.Hourly (file.Hourly ());
	nest_rec.Daily (file.Daily ());

	if (nest_rec.Start () > 0 || nest_rec.End () < Model_End_Time () || nest_rec.Space () > 0 ||
		nest_rec.Time_In () > 0 || nest_rec.Time_Out () > 0 || nest_rec.Hourly () > 0 || nest_rec.Daily () > 0) {

		if (flag) {
			Int_Map_Itr map_itr = parking_map.find (parking_rec.Parking ());

			if (map_itr != parking_map.end ()) {
				Parking_Data *parking_ptr = &parking_array [map_itr->second];
				parking_ptr->push_back (nest_rec);
				return (false);
			}
		}
	} else {
		return (flag);
	}
	parking_rec.push_back (nest_rec);
	return (true);
}
