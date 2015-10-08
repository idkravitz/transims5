//*********************************************************
//	XML_Signals.cpp - convert the signal data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Signals
//---------------------------------------------------------

void ExportNet::XML_Signals (void)
{
	int seconds;
	bool flag;

	Int_Map_Itr map_itr;
	Int_Itr int_itr;
	Signal_Data *signal_ptr;
	Signal_Time_Itr time_itr;
	Timing_Data *timing_ptr;
	Timing_Phase_Itr phase_itr;
	Node_Data *node_ptr;

	fstream &fh = xml_file.File ();	

	Set_Progress ();

	fh << "<SIGNALCONTROLS>" << endl;

	for (map_itr = signal_map.begin (); map_itr != signal_map.end (); map_itr++) {
		Show_Progress ();

		signal_ptr = &signal_array [map_itr->second];

		//---- check the selection criteria ----
		
		if (select_nodes || select_subarea) {
			flag = false;

			for (int_itr = signal_ptr->nodes.begin (); int_itr != signal_ptr->nodes.end (); int_itr++) {
				node_ptr = &node_array [*int_itr];
				if (node_ptr->Subarea () == 1) {
					flag = true;
					break;
				}
			}
			if (!flag) continue;
		}

		//---- find the requested time of day ----

		for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
			if (time_itr->Start () <= time_of_day && time_of_day <= time_itr->End ()) {
				timing_ptr = &signal_ptr->timing_plan [time_itr->Timing ()];

				fh << "<SIGNALCONTROL NO=\"" << signal_ptr->Signal ();
				fh << "\" NAME=\"" << ((time_itr->Notes () != 0) ? time_itr->Notes () : " ");
				fh << "\" CYCLETIME=\"PT" << timing_ptr->Cycle () << "S";
				fh << "\" TIMEOFFSET=\"PT" << timing_ptr->Offset () << "S";
				fh << "\" SIGNALIZATIONTYPE=\"" << "FixedTime";
				fh << "\" CYCLETIMEFIXED=\"" << "true" << "\" >" << endl;

				fh << "<SIGNALGROUPS>" << endl;

				seconds = 0;

				for (phase_itr = timing_ptr->begin (); phase_itr != timing_ptr->end (); phase_itr++) {

					fh << "<SIGNALGROUP NO=\"" << phase_itr->Phase ();
					fh << "\" NAME=\"\" GTSTART=\"PT" << seconds << "S";

					seconds += phase_itr->Min_Green () + phase_itr->Yellow () + phase_itr->All_Red ();
					fh << "\" GTEND=\"PT" << seconds << "S";
					fh << "\" SIGNALGROUPTYPE=\"" << "Cycle" << "\" />" << endl;
				}
				fh << "</SIGNALGROUPS>" << endl;
				fh << "</SIGNALCONTROL>" << endl;
				break;
			}
		}
	}
	fh << "</SIGNALCONTROLS>" << endl;
}
