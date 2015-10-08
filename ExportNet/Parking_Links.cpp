//*********************************************************
//	Parking_Links.cpp - insert parking links
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Parking_Links
//---------------------------------------------------------

void ExportNet::Parking_Links (void)
{
	int link, offset, off_low, off_high, offset_range, count, lnk;
	double side;

	Link_Data *link_ptr;
	Parking_Itr park_itr;
	Split_Data split_data;
	Split_Itr split_itr, split2_itr;
	Split_Array split_array, *split_ptr;
	Link_Split_Itr link_itr;

	offset_range = Round (Internal_Units (20, METERS));
	side = Internal_Units (20, METERS); 
	
	Show_Message ("Inserting Parking Links");
	Set_Progress ();

	//---- allocate link splits array ----

	link_splits.assign (link_array.size (), split_array);

	//---- set the parking offsets ----

	for (park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++) {
		if (park_itr->Type () == BOUNDARY) continue;
		Show_Progress ();

		link = park_itr->Link ();

		//---- check the selection criteria ---

		link_ptr = &link_array [link];
		if (link_ptr->Divided () == 0) continue;

		if (park_itr->Dir () == 1) {
			offset = link_ptr->Length () - park_itr->Offset ();
		} else {
			offset = park_itr->Offset ();
		}
		split_ptr = &link_splits [link];

		//---- scan existing records ----

		off_low = offset - offset_range;
		off_high = offset + offset_range;

		for (split_itr = split_ptr->begin (); split_itr != split_ptr->end (); split_itr++) {
			if (off_low < split_itr->offset && off_high > split_itr->offset) {
				if (park_itr->Dir () == 0) {
					split_itr->park_ab = -1;
				} else {
					split_itr->park_ba = -1;
				}
				break;
			}
		}
		if (split_itr != split_ptr->end ()) continue;

		//---- add a new split record ----

		memset (&split_data, '\0', sizeof (split_data));

		split_data.offset = offset;

		if (park_itr->Dir () == 0) {
			split_data.park_ab = -1;
		} else {
			split_data.park_ba = -1;
		}
		split_ptr->push_back (split_data);
	}

	//---- sort the split offsets ----

	count = lnk = 0;
	Set_Progress ();

	for (link=0, link_itr = link_splits.begin (); link_itr != link_splits.end (); link_itr++, link++) {
		if (link_itr->size () == 0) continue;
		Show_Progress ();

		count += (int) link_itr->size ();
		lnk++;

		for (split_itr = link_itr->begin (); split_itr != link_itr->end (); split_itr++) {
			for (split2_itr = split_itr + 1; split2_itr != link_itr->end (); split2_itr++) {
				if (split_itr->offset > split2_itr->offset) {
					split_data = *split2_itr;
					*split2_itr = *split_itr;
					*split_itr = split_data;
				}
			}
		}

		//---- add an end record ----

		memset (&split_data, '\0', sizeof (split_data));

		link_ptr = &link_array [link];

		split_data.offset = link_ptr->Length ();
		split_data.node = link_ptr->Bnode ();
		split_data.park_ab = split_data.link_ab = -1;
		split_data.park_ba = split_data.link_ba = -1;

		link_itr->push_back (split_data);
	}
	Print (2, "Number of Split Links = ") << lnk;
	Print (1, "Number of Link Splits = ") << count;

	//---- insert nodes and links ----

	New_Nodes ();
	New_Links ();
	New_Locations ();
	New_Connections ();

	End_Progress ();
}

