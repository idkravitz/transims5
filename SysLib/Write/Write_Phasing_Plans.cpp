//*********************************************************
//	Write_Phasing_Plans.cpp - write a new phasing plan file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Phasing_Plans
//---------------------------------------------------------

void Data_Service::Write_Phasing_Plans (void)
{
	Phasing_File *file = (Phasing_File *) System_File_Handle (NEW_PHASING_PLAN);

	int count = 0;	
	Int_Map_Itr map_itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	for (map_itr = signal_map.begin (); map_itr != signal_map.end (); map_itr++) {
		Show_Progress ();

		count += Put_Phasing_Data (*file, signal_array [map_itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Phasing_Data
//---------------------------------------------------------

int Data_Service::Put_Phasing_Data (Phasing_File &file, Signal_Data &data)
{
	Int_Map_Itr map_itr;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;
	Connect_Data *connect_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	String detectors;
	Int_Itr detect_itr;
	Detector_Data *detect_ptr;
	
	file.Signal (data.Signal ());
	int count = 0;

	for (phasing_itr = data.phasing_plan.begin (); phasing_itr != data.phasing_plan.end (); phasing_itr++) {
		file.Phasing (phasing_itr->Phasing ());
		file.Phase (phasing_itr->Phase ());

		detectors.erase ();

		for (detect_itr = phasing_itr->detectors.begin (); detect_itr != phasing_itr->detectors.end (); detect_itr++) {
			detect_ptr = &detector_array [*detect_itr];
			if (detectors.empty ()) {
				detectors = String ("%d") % detect_ptr->Detector ();
			} else {
				detectors += String ("|%d") % detect_ptr->Detector ();
			}
		}
		file.Detectors (detectors);
		file.Movements ((int) phasing_itr->size ());

		if (!file.Write (false)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;

		for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
			file.Movement (move_itr->Movement ());

			connect_ptr = &connect_array [move_itr->Connection ()];

			dir_ptr = &dir_array [connect_ptr->Dir_Index ()];
			link_ptr = &link_array [dir_ptr->Link ()];

			file.Link (link_ptr->Link ());
			file.Dir (dir_ptr->Dir ());

			dir_ptr = &dir_array [connect_ptr->To_Index ()];
			link_ptr = &link_array [dir_ptr->Link ()];
			
			file.To_Link (link_ptr->Link ());

			file.Protection (move_itr->Protection ());

			if (!file.Write (true)) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}
	}
	return (count);
}
