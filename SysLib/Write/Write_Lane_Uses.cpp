//*********************************************************
//	Write_Lane_Uses.cpp - write a new lane use file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Lane_Use
//---------------------------------------------------------

void Data_Service::Write_Lane_Uses (void)
{
	Lane_Use_File *file = (Lane_Use_File *) System_File_Handle (NEW_LANE_USE);

	int i, num, dir, index, period, count;

	Lane_Use_Data *lane_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr itr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *index_ptr;
	Int_Set use_sort;
	Int_Set_Itr sort_itr;

	count = 0;

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		link_ptr = &link_array [itr->second];

		for (dir=0; dir < 2; dir++) {
			index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
			if (index < 0) continue;

			Show_Progress ();

			dir_ptr = &dir_array [index];

			//---- sort the lane use indices from each time period ----

			period = dir_ptr->First_Lane_Use ();
			if (period < 0) continue;

			use_sort.clear ();

			for (;; period++) {
				period_ptr = &use_period_array [period];

				num = period_ptr->Records ();
				index = period_ptr->Index ();

				for (i=0; i < num; i++, index++) {
					index_ptr = &use_period_index [index];
					use_sort.insert (index_ptr->Link ());
				}
				if (period_ptr->Periods () == 0) break;
			}

			//---- write the lane use records in sorted order -----

			for (sort_itr = use_sort.begin (); sort_itr != use_sort.end (); sort_itr++) {
				lane_ptr = &lane_use_array [*sort_itr];

				count += Put_Lane_Use_Data (*file, *lane_ptr);
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Lane_Use_Data
//---------------------------------------------------------

int Data_Service::Put_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data)
{
	double rate, toll;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Veh_Type_Data *veh_type_ptr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());

	file.Lanes (Make_Lane_Range (dir_ptr, data.Low_Lane (), data.High_Lane ()));
	file.Use (data.Use ());
	file.Type (data.Type ());

	if (data.Min_Veh_Type () < 0) {
		file.Min_Veh_Type (0);
	} else if (veh_type_array.size () > 0) {
		veh_type_ptr = &veh_type_array [data.Min_Veh_Type ()];
		file.Min_Veh_Type (veh_type_ptr->Type ());
	} else {
		file.Min_Veh_Type (data.Min_Veh_Type ());
	}
	if (data.Max_Veh_Type () < 0) {
		file.Max_Veh_Type (0);
	} else if (veh_type_array.size () > 0) {
		veh_type_ptr = &veh_type_array [data.Max_Veh_Type ()];
		file.Max_Veh_Type (veh_type_ptr->Type ());
	} else {
		file.Max_Veh_Type (data.Max_Veh_Type ());
	}
	file.Min_Traveler (MAX (data.Min_Traveler (), 0)); 
	file.Max_Traveler (MAX (data.Max_Traveler (), 0));

	file.Start (data.Start ());
	file.End (data.End ());
	file.Length (UnRound (data.Length ()));
	file.Offset (UnRound (data.Offset ()));

	toll = UnRound (data.Toll ());
	rate = UnRound (data.Toll_Rate ());

	if (rate > 0) {
		if (Metric_Flag ()) {
			rate /= 1000.0;
		} else {
			rate /= MILETOFEET;
		}
		toll -= DTOI (rate * link_ptr->Length ());
	}
	file.Toll (DTOI (toll));
	file.Toll_Rate (rate);

	file.Min_Delay (UnRound (data.Min_Delay ()));
	file.Max_Delay (UnRound (data.Max_Delay ()));
	file.Notes (data.Notes ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}
