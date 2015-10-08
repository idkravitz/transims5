//*********************************************************
//	Write_Timing_Plans.cpp - write a new timing plan file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Timing_Plans
//---------------------------------------------------------

void Data_Service::Write_Timing_Plans (void)
{
	Timing_File *file = (Timing_File *) System_File_Handle (NEW_TIMING_PLAN);

	int count = 0;
	Int_Map_Itr map_itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (map_itr = signal_map.begin (); map_itr != signal_map.end (); map_itr++) {
		Show_Progress ();

		count += Put_Timing_Data (*file, signal_array [map_itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Timing_Data
//---------------------------------------------------------

int Data_Service::Put_Timing_Data (Timing_File &file, Signal_Data &data)
{
	Int_Map_Itr map_itr;
	Timing_Itr timing_itr;
	Timing_Phase_Itr phase_itr;

	int i, num, count;
	Timing_Phase *phase_sort [20];

	file.Signal (data.Signal ());
	count = 0;

	for (timing_itr = data.timing_plan.begin (); timing_itr != data.timing_plan.end (); timing_itr++) {
		Show_Progress ();

		file.Timing (timing_itr->Timing ());
		file.Type (timing_itr->Type ());
		file.Cycle (timing_itr->Cycle ());
		file.Offset (timing_itr->Offset ());
		file.Phases ((int) timing_itr->size ());
		file.Notes (timing_itr->Notes ());

		if (!file.Write (false)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
		num = 0;
		memset (phase_sort, '\0', sizeof (phase_sort));

		for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
			i = phase_itr->Phase ();
			if (i >= 20) continue;
			if (i > num) num = i;

			phase_sort [i] = &(*phase_itr);
		}

		for (i=1; i <= num; i++) {
			if (phase_sort [i] == 0) continue;

			file.Phase (phase_sort [i]->Phase ());
			file.Barrier (phase_sort [i]->Barrier ());
			file.Ring (phase_sort [i]->Ring ());
			file.Position (phase_sort [i]->Position ());
			file.Min_Green (phase_sort [i]->Min_Green ());
			file.Max_Green (phase_sort [i]->Max_Green ());
			file.Extension (phase_sort [i]->Extension ());
			file.Yellow (phase_sort [i]->Yellow ());
			file.All_Red (phase_sort [i]->All_Red ());

			if (!file.Write (true)) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}
	}
	return (count);
}
