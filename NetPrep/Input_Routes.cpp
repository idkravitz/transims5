//*********************************************************
//	Input_Routes.cpp - convert the transit line data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Read_Line
//---------------------------------------------------------

void NetPrep::Input_Routes (void)
{
	int i, period;
	bool stop;
	Dtime freq;
	String record, text;

	File_Group_Itr group_itr;
	Route_Data data;
	Int_Map_Itr map_itr;

	//---- read each line file ----

	for (group_itr = file_groups.begin (); group_itr != file_groups.end (); group_itr++) {

		if (group_itr->line_file == 0) continue;

		Show_Message (String ("Reading %s -- Record") % group_itr->line_file->File_Type ());
		Set_Progress ();

		period = 1;

		data.flip = group_itr->flip;
		data.oneway = true;
		data.name.clear ();
		data.nodes.clear ();
		data.headway.assign ((num_periods + 1), 0);
		data.ttime = 0;
		data.offset = -1;
		data.mode = -1;
		data.notes = String ("Group %d") % group_itr->group;

		//---- process each record ----

		while (group_itr->line_file->Read ()) {
			Show_Progress ();

			record = group_itr->line_file->Record_String ();
			record.Clean ();

			while (!record.empty ()) {
				record.Split (text, "=, \t");

				if (text.empty () || text [0] == ';') break;

				if (text.Equals ("LINE")) {

					//---- process the existing line ----

					if (data.nodes.size () > 1 && data.mode >= LOCAL_BUS) {
						Save_Route (data);
					}
					period = 1;
					data.oneway = true;
					data.nodes.clear ();
					data.headway.assign ((num_periods + 1), 0);
					data.ttime = 0;
					data.offset = -1;
					data.mode = -1;
					data.notes = String ("Group %d") % group_itr->group;

				} else if (text.Equals ("NAME")) {

					record.Split (data.name, ",");

				} else if (text.Equals ("OWNER")) {

					record.Split (data.notes, ",");

				} else if (text.Equals ("ONEWAY")) {

					record.Split (text, ",");
					data.oneway = (text [0] == 'Y' || text [0] == 'y' || text [0] == 'T' || text [0] == 't');

				} else if (text.Equals ("MODE")) {

					record.Split (text, ",");

					data.mode = text.Integer ();

					if (data.mode > 0) {
						if (data.mode < (int) mode_map.size ()) {
							data.mode = mode_map [data.mode];

							if (data.mode == -1) {
								Warning (String ("Mode %s was Not Converted") % text);
								mode_map [data.mode] = -2;
								data.mode = -1;
							}
						} else {
							data.mode = -1;
						}
					}

				} else if (text.Starts_With ("FREQ")) {

					if (text [4] == '[') {
						text.erase (0, 5);
						period = text.Integer ();
					} else {
						period = 1;
					}
					record.Split (text, ",");

					freq.Minutes (text.Integer ());

					for (i=0; i < num_periods; i++) {
						if (group_itr->period_map [i] == period) {
							data.headway [i] = freq;
						}
					}

				} else if (text.Equals ("RUNTIME") || text.Equals ("RT")) {

					record.Split (text, ",");

					data.ttime.Minutes (text.Integer ());

				} else if (text.Equals ("OFFSET")) {

					record.Split (text, ",");

					data.offset.Minutes (text.Integer ());
					if (data.offset < 0) data.offset = -1;

				} else if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

					i = text.Integer ();
					if (i < 0) {
						i = -i;
						stop = false;
					} else {
						stop = true;
					}
					map_itr = node_map.find (i);
					if (map_itr != node_map.end ()) {
						i = map_itr->second;
						if (!stop) i = -i;
						data.nodes.push_back (i);
					} else {
						Warning (String ("Route Node %d as Not Found in the Node File") % i);
					}

				} else if (text.Equals ("NODES") || text.Equals ("N")) {

					record.Split (text, ", ");
				
					if ((text [0] >= '0' && text [0] <= '9') || text [0] == '-') {

						i = text.Integer ();
						if (i < 0) {
							i = -i;
							stop = false;
						} else {
							stop = true;
						}
						map_itr = node_map.find (i);
						if (map_itr != node_map.end ()) {
							i = map_itr->second;
							if (!stop) i = -i;
							data.nodes.push_back (i);
						} else {
							Warning (String ("Route Node %d as Not Found in the Node File") % i);
						}
					}
				}
			}
		}
		End_Progress ();

		if (data.nodes.size () > 1 && data.mode >= LOCAL_BUS) {
			Save_Route (data);
		}
		group_itr->line_file->Close ();
	}
}
