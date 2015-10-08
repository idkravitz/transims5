//*********************************************************
//	Write_VISSIM.cpp - write a vissim xml file
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Write_VISSIM
//---------------------------------------------------------

void ExportNet::Write_VISSIM (void)
{
	int link, index, hour, minute, second, max_link;
	String start, end;

	List_Data list_rec, *list_a, *list_b, *next_list;
	Link_Itr link_itr;
	Data_Range_Itr range_itr;
	Split_Array *split_ptr;

	//---- insert parking links ----

	if (parking_flag) {
		Parking_Links ();
	}

	Show_Message ("Writing ") << xml_file.File_Type ();
	Set_Progress ();

	//---- allocate node list ----

	node_list.assign (node_array.size (), list_rec);
	dir_list.assign (dir_array.size (), list_rec);
	max_link = (int) link_splits.size ();

	//---- add link directions to the node list ----

	for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		if (link_itr->Divided () == 0) continue;
		Show_Progress ();

		if (link < max_link) {
			split_ptr = &link_splits [link];
			if (split_ptr->size () != 0) continue;
		}

		//---- add the link to the list ----

		list_a = &node_list [link_itr->Anode ()];
		list_b = &node_list [link_itr->Bnode ()];

		if (link_itr->AB_Dir () >= 0) {
			index = link_itr->AB_Dir ();
			next_list = &dir_list [index];

			next_list->From_List (list_a->From_List ());
			list_a->From_List (index);

			next_list->To_List (list_b->To_List ());
			list_b->To_List (index);
		}
		if (link_itr->BA_Dir () >= 0) {
			index = link_itr->BA_Dir ();
			next_list = &dir_list [index];

			next_list->From_List (list_b->From_List ());
			list_b->From_List (index);

			next_list->To_List (list_a->To_List ());
			list_a->To_List (index);
		}
	}

	//---- write the file ----

	fstream &fh = xml_file.File ();

	fh << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl;

	if (sum_periods.Num_Periods () > 0) {
		Dtime time = sum_periods.Start ();

		second = DTOI (time.Seconds ());
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		start ("%02d:%02d:%02d") % hour % minute % second;

		time = sum_periods.End ();

		second = DTOI (time.Seconds ());
		hour = second / 3600;
		second = second - hour * 3600;
		minute = second / 60;
		second = second - minute * 60;

		end ("%02d:%02d:%02d") % hour % minute % second;
	} else {
		start ("00:00:00");
		end ("23:59:59");
	}
	fh << "<ABSTRACTNETWORKMODEL VERSNO=\"1.0\" FROMTIME=\"" << start << "\" TOTIME=\"" << end << "\" NAME=\"" << Title () << "\">" << endl;
	fh << "<NETWORK LEFTHANDTRAFFIC=\"0\">" << endl;


	//---- write vehicle type records ----

	XML_Vehicle_Types ();

	//---- write node records ----

	XML_Nodes ();

	//---- write zone records ----

	XML_Zones ();

	//---- write link records ----

	XML_Links ();

	//---- write transit stop records ----

	if (stop_array.size () > 0) {
		XML_Stops ();
	}

	//---- write transit line records ----

	if (line_array.size () > 0) {
		XML_Routes ();
	}

	//---- write signal records ----

	XML_Signals ();

	//---- end the program ----

	fh << "</NETWORK>" << endl;
	fh << "</ABSTRACTNETWORKMODEL>" << endl;
	
	End_Progress ();
}

