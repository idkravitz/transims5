//*********************************************************
//	Write_Performance.cpp - write a new performance file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Performance
//---------------------------------------------------------

void Data_Service::Write_Performance (bool fill_flag)
{
	int i, index, dir, count, num_periods, ratio, num;
	double length, speed;
	Dtime low, high, time;
	bool turn_flag;

	Int_Map_Itr itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Connect_Data *connect_ptr;
	Link_Perf_Data *link_perf_ptr;
	Flow_Time_Data *turn_perf_ptr;
	Link_Perf_Array *link_perf;
	Flow_Time_Array *turn_perf;
	Performance_Data perf_rec;
	Turn_Delay_Data turn_rec;

	Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	count = 0;

	num_periods = time_periods.Num_Periods ();

	turn_flag = (System_File_Flag (CONNECTION) && file->Turn_Flag () && (turn_perf_array.size () > 0));
	turn_perf = 0;

	//---- process each time period ----

	for (i=0; i < num_periods; i++) {
		time_periods.Period_Range (i, low, high);

		perf_rec.Start (low);
		perf_rec.End (high);

		link_perf = &link_perf_array [i];

		if (turn_flag) {
			turn_perf = &turn_perf_array [i];
		}

		//---- sort the links ----

		for (itr = link_map.begin (); itr != link_map.end (); itr++) {
			link_ptr = &link_array [itr->second];

			length = link_ptr->Length ();

			for (dir=0; dir < 2; dir++) {
				index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (index < 0) continue;

				Show_Progress ();

				link_perf_ptr = &link_perf->at (index);
				if (!fill_flag && link_perf_ptr->Flow () == 0) continue;

				dir_ptr = &dir_array [index];

				perf_rec.Dir_Index (index);
				perf_rec.Type (0);

				perf_rec.Flow (link_perf_ptr->Flow ());

				if (link_perf_ptr->Time () > 0) {
					speed = length / link_perf_ptr->Time ();
				} else {
					speed = length / dir_ptr->Time0 ();
				}
				if (speed < 0.1) speed = 0.1;
				perf_rec.Speed (Round (speed));

				time = (int) (length / speed + 0.5);
				if (time < 1) time = 1;

				perf_rec.Time (time);
				perf_rec.Delay (time - dir_ptr->Time0 ());

				if (dir_ptr->Time0 () > 0) {
					ratio = Round (time * 100.0 / dir_ptr->Time0 ());
				} else {
					ratio = Round (100);
				}
				perf_rec.Time_Ratio (ratio);

				num = link_perf_ptr->Occupancy ();
				if (num < 1 || Ratio_Flag ()) num = 1;

				perf_rec.Density ((link_perf_ptr->Density () + (num / 2)) / num);
				perf_rec.Max_Density (link_perf_ptr->Max_Density ());
				perf_rec.Queue ((link_perf_ptr->Queue () + (num / 2)) / num);
				perf_rec.Max_Queue (link_perf_ptr->Max_Queue ());
				perf_rec.Failure (link_perf_ptr->Failure ());

				//---- gather the turn delays ----

				if (turn_flag) {
					perf_rec.clear ();

					for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
						connect_ptr = &connect_array [index];

						turn_perf_ptr = &turn_perf->at (index);
						if (!fill_flag && turn_perf_ptr->Flow () == 0) continue;

						turn_rec.To_Index (connect_ptr->To_Index ());
						turn_rec.Flow (turn_perf_ptr->Flow ());

						time = perf_rec.Time () + turn_rec.Time ();

						if (time > 0) {
							speed = length / time;
						} else {
							speed = length / dir_ptr->Time0 ();
						}
						if (speed < 0.1) speed = 0.1;

						time = (int) (length / speed + 0.5);
						if (time < 1) time = 1;

						turn_rec.Time (time);

						perf_rec.push_back (turn_rec);
					}
				}
				count += Put_Performance_Data (*file, perf_rec);

				//---- lane use flows ----

				if (dir_ptr->Flow_Index () >= 0) {
					link_perf_ptr = &link_perf->at (dir_ptr->Flow_Index ());
					if (!fill_flag && link_perf_ptr->Flow () == 0) continue;

					perf_rec.Type (1);

					perf_rec.Flow (link_perf_ptr->Flow ());

					if (link_perf_ptr->Time () > 0) {
						speed = length / link_perf_ptr->Time ();
					} else {
						speed = length / dir_ptr->Time0 ();
					}
					if (speed < 0.1) speed = 0.1;
					perf_rec.Speed (Round (speed));

					time = (int) (length / speed + 0.5);
					if (time < 1) time = 1;

					perf_rec.Time (time);
					perf_rec.Delay (time - dir_ptr->Time0 ());

					if (dir_ptr->Time0 () > 0) {
						ratio = Round (time * 100.0 / dir_ptr->Time0 ());
					} else {
						ratio = Round (100);
					}
					perf_rec.Time_Ratio (ratio);

					num = link_perf_ptr->Occupancy ();
					if (num < 1 || Ratio_Flag ()) num = 1;

					perf_rec.Density ((link_perf_ptr->Density () + (num / 2)) / num);
					perf_rec.Max_Density (link_perf_ptr->Max_Density ());
					perf_rec.Queue ((link_perf_ptr->Queue () + (num / 2)) / num);
					perf_rec.Max_Queue (link_perf_ptr->Max_Queue ());
					perf_rec.Failure (link_perf_ptr->Failure ());

					perf_rec.clear ();
					count += Put_Performance_Data (*file, perf_rec);
				}
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Performance_Data
//---------------------------------------------------------

int Data_Service::Put_Performance_Data (Performance_File &file, Performance_Data &data)
{
	int count;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Turn_Delay_Itr turn_itr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	//---- save the link record ----

	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());

	file.Type (data.Type ());

	file.Start (data.Start ());
	file.End (data.End ());

	file.Flow (data.Flow ());
	file.Time (data.Time ());

	file.Speed (UnRound (data.Speed ()));
	file.Delay (data.Delay ());
	file.Density (UnRound (data.Density ()));
	file.Max_Density (UnRound (data.Max_Density ()));
	file.Time_Ratio (UnRound (data.Time_Ratio ()));
	file.Queue (UnRound (data.Queue ()));
	file.Max_Queue (data.Max_Queue ());
	file.Cycle_Failure (data.Failure ());

	file.Num_Nest ((int) data.size ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count = 1;

	//---- save the connections ----

	for (turn_itr = data.begin (); turn_itr != data.end (); turn_itr++) {
		dir_ptr = &dir_array [turn_itr->To_Index ()];
		link_ptr = &link_array [dir_ptr->Link ()];

		file.Out_Link (link_ptr->Link ());
		file.Out_Flow (turn_itr->Flow ());
		file.Out_Time (turn_itr->Time ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	if (data.size () == 0 && file.Nest () == FLATTEN) {
		file.Blank_Nested_Fields ();
		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
	}
	return (count);
}
