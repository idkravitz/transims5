//*********************************************************
//	Read_Events.cpp - Read the Event File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Events
//---------------------------------------------------------

void Data_Service::Read_Events (void)
{
	Event_File *file = (Event_File *) System_File_Handle (EVENT);
	
	int num;
	Event_Data event_rec;
	Event_Index event_index;
	Event_Map_Stat map_stat;

	//---- store the event data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Events (*file);

	while (file->Read ()) {
		Show_Progress ();

		event_rec.Clear ();

		if (Get_Event_Data (*file, event_rec)) {
			event_rec.Get_Event_Index (event_index);

			map_stat = event_map.insert (Event_Map_Data (event_index, (int) event_array.size ()));

			if (!map_stat.second) {
				Warning (String ("Duplicate Event Record = %d-%d-%d-%d-%d") % 
					event_index.Household () % event_index.Person () % event_index.Tour () % 
					event_index.Trip () % event_index.Event ());

				continue;
			} else {
				event_array.push_back (event_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) event_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %d Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (EVENT);
}

//---------------------------------------------------------
//	Initialize_Events
//---------------------------------------------------------

void Data_Service::Initialize_Events (Event_File &file)
{
	int percent = System_Data_Reserve (EVENT);

	if (event_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			event_array.reserve (num);
			if (num > (int) event_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Event_Data
//---------------------------------------------------------

bool Data_Service::Get_Event_Data (Event_File &file, Event_Data &event_rec)
{
	int hhold, lvalue, link, dir, offset, dir_index;

	hhold = file.Household ();
	if (hhold < 1) return (false);

	event_rec.Household (hhold);

	event_rec.Person (file.Person ());
	event_rec.Tour (MAX (file.Tour (), 1));
	event_rec.Trip (file.Trip ());
	event_rec.Event (file.Event ());

	if (file.Version () <= 40) {
		event_rec.Mode (Trip_Mode_Map (file.Mode ()));
	} else {
		event_rec.Mode (file.Mode ());
	}
	event_rec.Schedule (file.Schedule ());
	event_rec.Actual (file.Actual ());

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
			Warning (String ("Event %d Link %d Direction %s was Not Found") % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
		}
		event_rec.Dir_Index (dir_index);
		event_rec.Offset (offset);
		event_rec.Lane (file.Lane ());
	}

	lvalue = file.Route ();

	if (lvalue > 0) {
		if (System_File_Flag (TRANSIT_ROUTE)) {
			Int_Map_Itr map_itr = line_map.find (lvalue);

			if (map_itr == line_map.end ()) {
				Warning (String ("Event %d Route %d was Not Found") % Progress_Count () % lvalue);
				return (false);
			}
			event_rec.Route (map_itr->second);
		} else {
			event_rec.Route (lvalue);
		}
	}
	return (true);
}
