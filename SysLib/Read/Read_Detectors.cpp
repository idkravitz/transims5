//*********************************************************
//	Read_Detectors.cpp - read the detector file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Detectors
//---------------------------------------------------------

void Data_Service::Read_Detectors (void)
{
	Detector_File *file = (Detector_File *) System_File_Handle (DETECTOR);
	
	int num;
	Int_Map_Stat map_stat;
	Detector_Data detector_rec;

	//---- store the lane use data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Detectors (*file);

	num = 0;

	while (file->Read ()) {
		Show_Progress ();

		detector_rec.Clear ();

		if (Get_Detector_Data (*file, detector_rec)) {
			map_stat = detector_map.insert (Int_Map_Data (detector_rec.Detector (), (int) detector_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Detector Number = ") << detector_rec.Detector ();
				continue;
			} else {
				detector_array.push_back (detector_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) detector_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) {
		System_Data_True (DETECTOR);
	}
}

//---------------------------------------------------------
//	Initialize_Detectors
//---------------------------------------------------------

void Data_Service::Initialize_Detectors (Detector_File &file)
{
	Required_File_Check (file, LINK);
	if (file.Version () <= 40) {
		Required_File_Check (file, POCKET);
	}
	int percent = System_Data_Reserve (DETECTOR);

	if (detector_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			detector_array.reserve (num);
			if (num > (int) detector_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Detector_Data
//---------------------------------------------------------

bool Data_Service::Get_Detector_Data (Detector_File &file, Detector_Data &detector_rec)
{
	int link, dir, offset, dir_index, lanes, low, high;

	Link_Data *link_ptr;

	//---- detector id ----

	detector_rec.Detector (file.Detector ());
	if (detector_rec.Detector () == 0) return (false);

	//---- check/convert the link number ----

	link = file.Link ();
	dir = file.Dir ();
	offset = Round (file.Offset ());

	link_ptr = Set_Link_Direction (file, link, dir, offset);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
	} else {
		dir_index = link_ptr->AB_Dir ();
	}
	if (dir_index < 0) {
		Warning (String ("Detector %d Link %d Direction %s was Not Found") % detector_rec.Detector () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	detector_rec.Dir_Index (dir_index);
	detector_rec.Length (file.Length ());

	if (file.Version () <= 40) {
		detector_rec.Offset (offset - detector_rec.Length ());
	} else {
		detector_rec.Offset (offset);
	}

	//---- lane number ----

	lanes = file.Lanes ();

	if (file.Version () <= 40 && lanes > 0) {
		low = file.Low_Lane () - 1;
		high = file.High_Lane () - 1;
	} else {
		Convert_Lane_Range (dir_index, lanes, low, high);
	}
	detector_rec.Low_Lane (low);
	detector_rec.High_Lane (high);

	//----- optional fields ----

	detector_rec.Type (file.Type ());
	detector_rec.Use (file.Use ());

	detector_rec.Notes (file.Notes ());

	return (true);
}
