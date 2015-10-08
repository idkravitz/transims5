//*********************************************************
//	XML_Stops.cpp - convert transit stop data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Stops
//---------------------------------------------------------

void ExportNet::XML_Stops (void)
{
	int link;
	double offset, len1, len2;

	Int_Map_Itr map_itr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Split_Itr split_itr;
	Split_Array *splits_array;

	fstream &fh = xml_file.File ();

	Set_Progress ();

	fh << "<PTSTOPS>" << endl;

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		Show_Progress ();

		stop_ptr = &stop_array [map_itr->second];

		link_ptr = &link_array [stop_ptr->Link ()];
		if (link_ptr->Divided () == 0) continue;

		splits_array = &link_splits [stop_ptr->Link ()];

		if (splits_array->size () == 0) {
			link = link_ptr->Link ();
			offset = (double) stop_ptr->Offset () / link_ptr->Length ();
		} else {
			link = link_ptr->Link ();
			len1 = len2 = 0;

			if (stop_ptr->Dir () == 1) {
				offset = link_ptr->Length () - stop_ptr->Offset ();
			} else {
				offset = stop_ptr->Offset ();
			}
			for (split_itr = splits_array->begin (); split_itr != splits_array->end (); split_itr++) {
				link_ptr = &link_array [split_itr->link];
				link = link_ptr->Link ();
				len2 += link_ptr->Length ();
				if (offset <= len2) break;
				len1 = len2;
			}
			if (stop_ptr->Dir () == 1) {
				offset = len2 - offset;
				len2 -= len1;
			} else {
				offset -= len1;
				len2 -= len1;
			}
			offset = offset / len2;
		}
		if (offset < 0.01) {
			offset = 0.01;
		} else if (offset > 0.99) {
			offset = 0.99;
		}
		fh << "<PTSTOP ID=\"" << stop_ptr->Stop ();
		fh << "\" NAME=\"" << ((stop_ptr->Name () != 0) ? stop_ptr->Name () : "");
		fh << "\" LINKID=\"" << link << ((stop_ptr->Dir ()) ? "B" : "A");
		fh << "\" RELPOS=\"" << offset << "\" BAY=\"0\" />" << endl;
	}
	fh << "</PTSTOPS>" << endl;
}
