//*********************************************************
//	Read_Signals.cpp - read the signal file
//*********************************************************

#include "IntControl.hpp"

#include "Range_Data.hpp"

//---------------------------------------------------------
//	Read_Signals
//---------------------------------------------------------

void IntControl::Read_Signal (void)
{
	int signal, node, num;
	String buffer;
	Strings nodes;

	Signal_Data signal_rec;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Int_Set_Stat set_stat;
	Str_Itr str_itr;
	Int_Itr node_itr;
	Node_Data *node_ptr;

	//---- store the signal data ----

	Show_Message (String ("Reading %s -- Record") % signal_file.File_Type ());
	Set_Progress ();

	num_new = num_update = 0;

	while (signal_file.Read (false)) {
		Show_Progress ();

		//---- check the signal number ----

		signal = signal_file.Signal ();
		if (signal == 0) continue;
		
		map_itr = signal_map.find (signal);
		if (map_itr != signal_map.end ()) {
			if (update_flag) {
				set_stat = update_signal.insert (signal);
				if (!set_stat.second) {
					Warning ("Duplicate Signal Number = ") << signal;
				}
				num_update++;
				nsignal++;
			} else {
				Warning (String ("Signal %d Already Exists") % signal);
			}
			goto read_nest;
		}

		//---- gather the warrant signal data ----

		signal_rec.Clear ();

		signal_rec.Signal (signal);
		signal_rec.Group (signal_file.Group ());

		buffer = signal_file.Nodes ();
		if (buffer.empty ()) goto read_nest;

		num = buffer.Parse (nodes, "|+/");
		if (num == 0) goto read_nest;

		signal_rec.nodes.reserve (num);

		for (str_itr = nodes.begin (); str_itr != nodes.end (); str_itr++) {
			node = str_itr->Integer ();
			if (node <= 0) break;

			map_itr = node_map.find (node);
			if (map_itr == node_map.end ()) {
				Warning (String ("Signal %d Node %d was Not Found") % signal_rec.Signal () % node);
				continue;
			}
			node_ptr = &node_array [map_itr->second];

			if (node_ptr->Control () >= 0 && signal_file.Version () > 40) {
				Warning (String ("Node %d is assigned to multiple signals") % node);
				continue;
			} else if (node_ptr->Control () < -1) {
				Warning (String ("Node %d has a Sign/Signal control conflict") % node);
				continue;
			}
			signal_rec.nodes.push_back (map_itr->second);
		}
		if (signal_rec.nodes.size () == 0) goto read_nest;

		//---- insert the signal warrant ----

		num = (int) signal_array.size ();

		map_stat = signal_map.insert (Int_Map_Data (signal, num));

		if (!map_stat.second) {
			Warning ("Duplicate Signal Number = ") << signal_rec.Signal ();
			goto read_nest;
		}

		//---- set the node controls ----

		for (node_itr = signal_rec.nodes.begin (); node_itr != signal_rec.nodes.end (); node_itr++) {
			node_ptr = &node_array [*node_itr];
			node_ptr->Control (num);
		}
		signal_array.push_back (signal_rec);
		num_new++;
		nsignal++;

read_nest:
		for (num = signal_file.Times (); num > 0; num--) {
			if (!signal_file.Read (true)) {
				Error (String ("Number of Time Records for Signal %d") % signal);
			}
			Show_Progress ();
		}
	}
	End_Progress ();

	signal_file.Close ();

	Print (2, String ("Number of %s Records = %d") % signal_file.File_Type () % Progress_Count ());
	if (num_new > 0) {
		Print (1, "Number of New Signal Records = ") << num_new;
	}
	if (num_update > 0) {
		Print (1, "Number of Signal Updates = ") << num_update;
	}
}
