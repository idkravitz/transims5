//*********************************************************
//	Write_Detectors.cpp - write a new detector file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Detectors
//---------------------------------------------------------

void Data_Service::Write_Detectors (void)
{
	Detector_File *file = (Detector_File *) System_File_Handle (NEW_DETECTOR);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = detector_map.begin (); itr != detector_map.end (); itr++) {
		Show_Progress ();

		count += Put_Detector_Data (*file, detector_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Detector_Data
//---------------------------------------------------------

int Data_Service::Put_Detector_Data (Detector_File &file, Detector_Data &data)
{
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	file.Detector (data.Detector ());
	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());
	file.Offset (UnRound (data.Offset ()));
	file.Length (UnRound (data.Length ()));
	file.Lanes (Make_Lane_Range (dir_ptr, data.Low_Lane (), data.High_Lane ()));
	file.Type (data.Type ());
	file.Use (data.Use ());
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}

