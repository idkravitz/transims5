//*********************************************************
//	Read_Signals.cpp - read the signal file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Signals
//---------------------------------------------------------

void Data_Service::Read_Signals (void)
{
	Signal_File *file = (Signal_File *) System_File_Handle (SIGNAL);

	int i, num, count;
	bool keep_flag;
	Signal_Data signal_rec;
	Int_Map_Stat map_stat;
	Int_Itr node_itr;
	Node_Data *node_ptr;

	//---- store the signal data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Signals (*file);
	count = 0;

	while (file->Read (false)) {
		Show_Progress ();
		signal_rec.Clear ();

		keep_flag = Get_Signal_Data (*file, signal_rec);

		num = file->Num_Nest ();
		if (num > 0) signal_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Time Records for Signal %d") % file->Signal ());
			}
			Show_Progress ();

			Get_Signal_Data (*file, signal_rec);
		}
		if (keep_flag) {
			map_stat = signal_map.insert (Int_Map_Data (signal_rec.Signal (), (int) signal_array.size ()));

			if (file->Version () <= 40) {
				int num, index;
				Timing40_Map_Stat timing40_stat;
				Timing40_Map_Itr timing40_itr;
				Timing40_Data timing40_rec;
				Signal_Time_Itr time_itr, tim_itr;
				Signal_Data *signal_ptr = 0;
				
				time_itr = signal_rec.begin ();
				num = 1;

				if (!map_stat.second) {
					index = map_stat.first->second;
					signal_ptr = &signal_array [index];

					tim_itr = --signal_ptr->end ();

					if (tim_itr->Start () < time_itr->Start () && tim_itr->End () == Model_End_Time ()) {
						tim_itr->End (time_itr->Start ());
					}
					for (tim_itr = signal_ptr->begin (); tim_itr != signal_ptr->end (); tim_itr++) {
						if (tim_itr->Timing () >= num) {
							num = tim_itr->Timing () + 1;
						}
					}
					timing40_itr = timing40_map.find (time_itr->Timing ());
					if (timing40_itr != timing40_map.end ()) {
						if (timing40_itr->second.Signal () == index) {
							num = timing40_itr->second.Timing ();
						}
					}
				} else {
					index = (int) signal_array.size ();
				}
				timing40_rec.Signal (index);
				timing40_rec.Timing (num);
				timing40_rec.Type (file->Type ());
				timing40_rec.Offset (file->Offset ());

				timing40_stat = timing40_map.insert (Timing40_Map_Data (time_itr->Timing (), timing40_rec));

				if (!timing40_stat.second && timing40_stat.first->second.Signal () != index) {
					Warning ("Duplicate Signal Timing Number = ") << time_itr->Timing ();
					continue;
				}
				time_itr->Timing (num);
				time_itr->Phasing (num);

				if (!map_stat.second) {
					signal_ptr->push_back (*time_itr);
					count++;
					continue;
				}
			}
			if (!map_stat.second) {
				Warning ("Duplicate Signal Number = ") << signal_rec.Signal ();
			} else {

				//---- set the node controls ----

				num = (int) signal_array.size ();

				for (node_itr = signal_rec.nodes.begin (); node_itr != signal_rec.nodes.end (); node_itr++) {
					node_ptr = &node_array [*node_itr];
					node_ptr->Control (num);
				}
				signal_array.push_back (signal_rec);
				count += (int) signal_rec.size () + 1;
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % count);
	}
	if (count > 0) System_Data_True (SIGNAL);
}

//---------------------------------------------------------
//	Initialize_Signals
//---------------------------------------------------------

void Data_Service::Initialize_Signals (Signal_File &file)
{
	Required_File_Check (file, NODE);

	int percent = System_Data_Reserve (SIGNAL);
	
	if (signal_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else {
			num /= 4;
		}
		if (num > 1) {
			signal_array.reserve (num);
			if (num > (int) signal_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Signal_Data
//---------------------------------------------------------

bool Data_Service::Get_Signal_Data (Signal_File &file, Signal_Data &signal_rec)
{
	//---- process the header record ----
	
	if (!file.Nested ()) {
		int node, num;
		String svalue;
		Strings nodes;
		Int_Map_Itr map_itr;
		Str_Itr str_itr;
		Node_Data *node_ptr;

		signal_rec.Signal (file.Signal ());
		if (signal_rec.Signal () == 0) return (false);

		signal_rec.Group (file.Group ());

		svalue = file.Nodes ();
		if (svalue.empty ()) return (false);

		num = svalue.Parse (nodes, "|+/");
		if (num == 0) return (false);

		signal_rec.nodes.reserve (num);

		for (str_itr = nodes.begin (); str_itr != nodes.end (); str_itr++) {
			node = str_itr->Integer ();
			if (node <= 0) break;

			map_itr = node_map.find (node);
			if (map_itr == node_map.end ()) {
				Warning (String ("Signal %d Node %d was Not Found") % signal_rec.Signal () % node);
				return (false);
			}
			node_ptr = &node_array [map_itr->second];

			if (node_ptr->Control () >= 0 && file.Version () > 40) {
				Warning (String ("Node %d is assigned to multiple signals") % node);
				continue;
			} else if (node_ptr->Control () < -1) {
				Warning (String ("Node %d has a Sign/Signal control conflict") % node);
				continue;
			}
			signal_rec.nodes.push_back (map_itr->second);
		}
		if (file.Version () > 40) return (signal_rec.nodes.size () != 0);
	}
	Signal_Time_Data time_rec;

	time_rec.Start (file.Start ());
	time_rec.End (file.End ());
	if (time_rec.End () == 0) time_rec.End (Model_End_Time ());

	time_rec.Timing (file.Timing ());
	time_rec.Phasing (file.Phasing ());
	time_rec.Notes (file.Notes ());

	//---- check the previous end time ----

	if (signal_rec.size () > 0) {
		Signal_Time_Itr itr = --signal_rec.end ();

		if (itr->Start () < time_rec.Start () && itr->End () == Model_End_Time ()) {
			itr->End (time_rec.Start ());
		}
	}
	signal_rec.push_back (time_rec);

	return (true);
}
