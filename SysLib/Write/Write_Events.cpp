//*********************************************************
//	Write_Events.cpp - write a new event file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Events
//---------------------------------------------------------

void Data_Service::Write_Events (void)
{
	int count = 0;

	Event_File *file = (Event_File *) System_File_Handle (NEW_EVENT);

	Event_Map_Itr itr;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = event_map.begin (); itr != event_map.end (); itr++) {
		Show_Progress ();

		count += Put_Event_Data (*file, event_array [itr->second]);
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Event_Data
//---------------------------------------------------------

int Data_Service::Put_Event_Data (Event_File &file, Event_Data &data)
{
	file.Household (data.Household ());
	file.Person (data.Person ());
	file.Tour (data.Tour ());
	file.Trip (data.Trip ());
	file.Mode (data.Mode ());
	file.Event (data.Event ());
	file.Schedule (data.Schedule ());
	file.Actual (data.Actual ());

	if (data.Dir_Index () >= 0) {
		Dir_Data *dir_ptr = &dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

		file.Link (link_ptr->Link ());
		file.Dir (dir_ptr->Dir ());
		file.Lane (Make_Lane_ID (dir_ptr, data.Lane ()));
		file.Offset (UnRound (data.Offset ()));
	} else {
		file.Link (0);
		file.Dir (0);
		file.Lane (0);
		file.Offset (0);
	}
	if (data.Route () >= 0) {
		Line_Data *line_ptr = &line_array [data.Route ()];

		file.Route (line_ptr->Route ());
	} else {
		file.Route (0);
	}
	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
