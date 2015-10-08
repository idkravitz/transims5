//*********************************************************
//	New_Locations.cpp - move locations to parking links
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	New_Locations
//---------------------------------------------------------

void ExportNet::New_Locations (void)
{
	int link, offset, off_low, off_high, offset_range, park_len, count;

	Link_Data *link_ptr;
	Location_Itr loc_itr;
	Split_Itr split_itr;
	Split_Array *split_ptr;

	park_len = Round (Internal_Units (20, METERS)); 
	offset_range = Round (Internal_Units (20, METERS));

	//---- set the parking offsets ----

	count = 0;
	Set_Progress ();

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
		link = loc_itr->Link ();
		Show_Progress ();

		//---- check the selection criteria ---

		link_ptr = &link_array [link];
		if (link_ptr->Divided () == 0) continue;

		if (loc_itr->Dir () == 1) {
			offset = link_ptr->Length () - loc_itr->Offset ();
		} else {
			offset = loc_itr->Offset ();
		}
		split_ptr = &link_splits [link];

		//---- scan existing records ----

		off_low = offset - offset_range;
		off_high = offset + offset_range;

		for (split_itr = split_ptr->begin (); split_itr != split_ptr->end (); split_itr++) {
			if (off_low < split_itr->offset && off_high > split_itr->offset) {
				if (loc_itr->Dir () == 0) {
					loc_itr->Link (split_itr->link_ab);
				} else {
					loc_itr->Link (split_itr->link_ba);
				}
				loc_itr->Dir (0);
				loc_itr->Offset (park_len);
				count++;
				break;
			}
		}
	}
	Print (2, "Number of Locations on Parking Links = ") << count;
}

