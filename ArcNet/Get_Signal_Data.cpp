//*********************************************************
//	Get_Signal_Data.cpp - additional signal processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Signal_Data
//---------------------------------------------------------

bool ArcNet::Get_Signal_Data (Signal_File &file, Signal_Data &signal_rec)
{
	if (Data_Service::Get_Signal_Data (file, signal_rec)) {
		if (arcview_signal.Is_Open ()) {
			arcview_signal.Copy_Fields (file);

			if (!file.Nested ()) {
				Int_Itr itr;
				Node_Data *node_ptr;
				XYZ_Point point;

				arcview_signal.clear ();

				for (itr=signal_rec.nodes.begin (); itr != signal_rec.nodes.end (); itr++) {
					node_ptr = &node_array [*itr];

					point.x = UnRound (node_ptr->X ());
					point.y = UnRound (node_ptr->Y ());
					point.z = UnRound (node_ptr->Z ());
					
					arcview_signal.push_back (point);
				}
			}
			if (file.Nested () || file.Num_Nest () == 0 || file.Version () <= 40) {
				if (time_flag && signal_rec.size () > 0) {
					Signal_Time_Itr nest_itr = --signal_rec.end ();

					if (nest_itr->Start () > time || time > nest_itr->End ()) {
						signal_rec.pop_back ();
						return (false);
					}
				}
				if (!arcview_signal.Write_Record ()) {
					Error (String ("Writing %s") % arcview_signal.File_Type ());
				}
			}
		}
		return (true);
	}
	return (false);
}
