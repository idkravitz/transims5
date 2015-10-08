//*********************************************************
//	Read_Ridership.cpp - read the Transit Ridership file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Ridership
//---------------------------------------------------------

void Data_Service::Read_Ridership (void)
{
	int num_rec, stop, stops, stop2, run, runs, time, time1, time2, sched, sched1, sched2;
	Dtime delta;

	Ridership_Data rider_rec;
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr, best_itr;
	Line_Data *line_ptr;
	Line_Run *run_ptr, *best_ptr;

	Ridership_File *file = (Ridership_File *) System_File_Handle (RIDERSHIP);

	//---- store the location data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	num_rec = 0;
	Initialize_Ridership (*file);

	delta = Dtime (3, MINUTES);

	while (file->Read ()) {
		Show_Progress ();

		rider_rec.Clear ();

		if (Get_Ridership_Data (*file, rider_rec)) {
			line_ptr = &line_array [rider_rec.Route ()];

			//---- find the stop index ----

			stop = rider_rec.Stop ();
			run = rider_rec.Run () - 1;
			sched = rider_rec.Schedule ();
			sched1 = sched - delta;		//---- three minutes ----
			sched2 = sched + delta;

			best_ptr = 0;

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				if (stop_itr->Stop () == stop) {
					run_ptr = &(stop_itr->at (run));

					sched = run_ptr->Schedule ();
					if (sched1 <= sched && sched <= sched2) {
						best_ptr = run_ptr;
						break;
					} else if (best_ptr == 0) {
						best_ptr = run_ptr;
					}
				}
			}
			if (best_ptr != 0) {
				num_rec++;
				best_ptr->Time (rider_rec.Time ());
				best_ptr->Board (rider_rec.Board ());
				best_ptr->Alight (rider_rec.Alight ());
				best_ptr->Load (rider_rec.Load ());
				best_ptr->Factor (rider_rec.Factor ());
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (num_rec && num_rec != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num_rec);
	}
	if (num_rec > 0) System_Data_True (RIDERSHIP);

	//---- interpolate the loaded time data ----

	sched1 = time1 = 0;

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {

		stops = (int) line_itr->size ();
		runs = (int) line_itr->begin ()->size ();

		for (run=0; run < runs; run++) {
			for (stop=0; stop < stops; stop++) {
				run_ptr = &((*line_itr) [stop] [run]);

				if (run_ptr->Time () == 0) {
					if (stop == 0) {
						run_ptr->Time (run_ptr->Schedule ());
					} else {
						for (stop2 = stop + 1; stop2 < stops; stop2++) {
							run_ptr = &((*line_itr) [stop2] [run]);
							if (run_ptr->Time () > 0) break;
						}
						if (stop2 < stops) {
							sched2 = run_ptr->Schedule () - sched1;
							if (sched2 == 0) sched2 = 1;

							time2 = run_ptr->Time () - time1;

							for (; stop <= stop2; stop++) {
								run_ptr = &((*line_itr) [stop] [run]);
								if (stop == stop2) break;

								sched = run_ptr->Schedule () - sched1;

								time = time1 + (int) ((double) sched * time2 / sched2);

								run_ptr->Time (Dtime (time).Round_Seconds ());
							}
						} else {
							time2 = time1 - sched1;

							for (; stop < stops; stop++) {
								run_ptr = &((*line_itr) [stop] [run]);

								time = run_ptr->Schedule () + time2;
								if (time < 0) time = 0;
								run_ptr->Time (time);
							}
						}
					}
				}
				time1 = run_ptr->Time ();
				sched1 = run_ptr->Schedule ();
			}
		}
	}
}

//---------------------------------------------------------
//	Initialize_Ridership
//---------------------------------------------------------

void Data_Service::Initialize_Ridership (Ridership_File &file)
{
	Required_File_Check (file, TRANSIT_ROUTE);
	Required_File_Check (file, TRANSIT_STOP);
	Required_File_Check (file, TRANSIT_SCHEDULE);
}

//---------------------------------------------------------
//	Get_Ridership_Data
//---------------------------------------------------------

bool Data_Service::Get_Ridership_Data (Ridership_File &file, Ridership_Data &rider_rec)
{
	int route, stop;

	Int_Map_Itr map_itr;

	//---- get the route id ----

	route = file.Route ();
	if (route == 0) return (false);

	map_itr = line_map.find (route);
	if (map_itr == line_map.end ()) {
		Warning (String ("Ridership Route %d was Not Found") % route);
		return (false);
	}
	rider_rec.Route (map_itr->second);

	rider_rec.Mode (file.Mode ());
	rider_rec.Run (file.Run ());

	//---- get the stop number ----

	stop = file.Stop ();

	map_itr = stop_map.find (stop);
	if (map_itr == stop_map.end ()) {
		Warning (String ("Ridership Stop %d on Route %d was Not Found") % stop % route);
		return (false);
	}
	rider_rec.Stop (map_itr->second);

	//---- get the time ----

	rider_rec.Schedule (file.Schedule ());
	rider_rec.Time (file.Time ());

	//--- get the ridership ----

	rider_rec.Board (file.Board ());
	rider_rec.Alight (file.Alight ());
	rider_rec.Load (file.Load ());
	rider_rec.Factor (file.Factor ());

	return (true);
}
