//*********************************************************
//	Write_Schedules.cpp - write a new transit schedule file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Schedules
//---------------------------------------------------------

void Data_Service::Write_Schedules (void)
{
	Schedule_File *file = (Schedule_File *) System_File_Handle (NEW_TRANSIT_SCHEDULE);

	int count = 0;
	Int_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = line_map.begin (); itr != line_map.end (); itr++) {
		Show_Progress ();

		count += Put_Schedule_Data (*file, line_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();

	line_array.Schedule_Records (count);
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Schedule_Data
//---------------------------------------------------------

int Data_Service::Put_Schedule_Data (Schedule_File &file, Line_Data &data)
{
	Int_Map_Itr itr;
	Line_Stop_Itr stop_itr;
	Line_Run *run_ptr;
	Stop_Data *stop_ptr;

	int i, j, run, runs, count;

	count = 0;

	file.Route (data.Route ());
	file.Stops ((int) data.size ());
	if (file.Stops () == 0) return (0);

	stop_itr = data.begin ();
	runs = (int) stop_itr->size ();

	for (i=0; i < runs; i += NUM_SCHEDULE_COLUMNS) {
		file.Clear_Columns ();

		//---- save the header record ----

		for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
			run = i + j;
			if (run == runs) break;
			file.Run (j, run + 1);
		}
		if (!file.Write (false)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;

		//---- save the nested stop records ----

		for (stop_itr = data.begin (); stop_itr != data.end (); stop_itr++) {
			stop_ptr = &stop_array [stop_itr->Stop ()];
			file.Stop (stop_ptr->Stop ());

			for (j=0; j < NUM_SCHEDULE_COLUMNS; j++) {
				run = i + j;
				if (run == runs) break;
				run_ptr = &(stop_itr->at (run));
				file.Time (j, run_ptr->Schedule ());
			}
			if (!file.Write (true)) {
				Error (String ("Writing %s") % file.File_Type ());
			}
			count++;
		}
	}
	return (count);
}
