//*********************************************************
//	Get_Timing_Data.cpp - read the timing plan file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Initialize_Timings
//---------------------------------------------------------

void ArcNet::Initialize_Timing_Plans (Timing_File &file)
{
	Data_Service::Initialize_Timing_Plans (file);

	if (timing_db.capacity () == 0) {
		int num = file.Num_Records ();

		if (num > 0) timing_db.reserve (num);
	}
}

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

int ArcNet::Get_Timing_Data (Timing_File &file, Timing_Data &timing_rec)
{
	int signal = Data_Service::Get_Timing_Data (file, timing_rec);

	if (signal >= 0) {
		timing_db.Copy_Fields (file);

		if (!file.Nested ()) {
			if (time_flag) {
				Signal_Data *signal_ptr = &signal_array [signal];

				Signal_Time_Itr itr;
				bool keep = false;

				for (itr = signal_ptr->begin (); itr != signal_ptr->end (); itr++) {
					if (itr->Timing () == timing_rec.Timing () && 
						itr->Start () <= time && time <= itr->End ()) {
						keep = true;
						break;
					}
				}
				if (!keep) {
					timing_rec.Timing (-1);
					return (-1);
				}
			}
			return (signal);
		}
		if (timing_rec.Timing () >= 0) {
			if (!timing_db.Add_Record ()) {
				Error ("Writing Timing Plan Database");
			}
			return (signal);
		}
	}
	return (-1);
}
